#include <Windows.h>
#include <stdio.h>
#include <list>
#include <sstream>

#include "logger.h"

#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMProcessing.h>

#include "VRiCommPort.h"
#include "BaseAircraft.h"
#include "LaminarB738.h"
#include "ZiboB738.h"
#include "LaminarB744.h"
#include "EadtX737.h"
#include "SsgB748.h"

static char *g_ident1 = "VrII";
static char *g_ident2 = "v0.5";

void scanForVRiDevices();
void scanUserPlane();

static std::list<VRiCommPort*> g_allPorts;
static std::list<BaseDeviceHandler*> g_vriDeviceHandlers;
static bool g_enabled = false;

static XPLMDataRef g_refAcfDescription = NULL;
static XPLMDataRef g_refAcfAuthor = NULL;

static BaseAircraft *g_plane = nullptr;

extern "C" 
{
	float mainloopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon);
}

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
	strcpy(outName, "X Plane 11 VrInsight Interface");
	strcpy(outSig, "vortex.xvrii.interface");
	strcpy(outDesc, "Plug-in to interface directly with the VRInsight MCP Combo panel");

	vlogger_setPluginName("vortex.xvrii");
	VLLog("Started");

	g_refAcfDescription = XPLMFindDataRef("sim/aircraft/view/acf_descrip");
	g_refAcfAuthor = XPLMFindDataRef("sim/aircraft/view/acf_author");
	return 1;
}

PLUGIN_API void XPluginStop(void)
{
	VLLog("Stopped");
}

PLUGIN_API int XPluginEnable(void)
{
	VLLog("Enabled Plugin");

	if (!g_enabled)
	{
		g_enabled = true;

		// Find hardware
		scanForVRiDevices();

		// Find out which is the current user plane
		scanUserPlane();

		// Start the main loop
		XPLMRegisterFlightLoopCallback(mainloopCallback, 1.0, nullptr);
	}

	return 1;
}

PLUGIN_API void XPluginDisable(void)
{
	if (g_enabled)
	{
		XPLMUnregisterFlightLoopCallback(mainloopCallback, nullptr);

		// close commports and kill poll threads
		for (auto it = g_vriDeviceHandlers.begin(); it != g_vriDeviceHandlers.end(); it++)
			delete *it;

		g_vriDeviceHandlers.clear();

		if (g_plane != nullptr)
		{
			delete g_plane;
			g_plane = nullptr;
		}

		g_enabled = false;
	}
	VLLog("Disabled Plugin");
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam)
{
	if (inFrom == XPLM_PLUGIN_XPLANE && inMsg == XPLM_MSG_LIVERY_LOADED && inParam == 0)
	{
		// Find out which is the current user plane
		scanUserPlane();
	}
}

extern "C" float mainloopCallback(float elapsedMe, float elapsedSim, int counter, void * refcon)
{
	// get the currently queued commands (taking ownership of the returned queued)
	if (g_plane != nullptr)
	{
		CommandQueue *commands = g_plane->queuedCommands();
		if (commands != nullptr)
		{
			do
			{
				XPLMCommandOnce(commands->front());
				commands->pop();
			} while (!commands->empty());
			delete commands;
		}
		g_plane->updateDisplays(g_vriDeviceHandlers);
	}

	return -5;
}

void scanForVRiDevices()
{
	DWORD index = 0;
	HKEY commPorts;

	char allPorts[1024] = {0};

	// Find the existing COMM ports in the registry
	LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"), 0, KEY_READ, &commPorts);
	while (res == ERROR_SUCCESS)
	{
		char szKey[256];
		char szData[1024];
		DWORD keyLength = 256;
		DWORD dataLength = 1024;
		res = RegEnumValueA(commPorts, index++, szKey, &keyLength, nullptr, nullptr, (LPBYTE)szData, &dataLength);
		if (res == ERROR_SUCCESS)
		{
			g_allPorts.push_back(new VRiCommPort(szData));
			strcat(allPorts, szData);
			strcat(allPorts, " ");
		}
	}
	RegCloseKey(commPorts);
	VLLog("Found Serial Ports: %s, scanning them now.", allPorts);

	// Wait untill all ports have finished scanning (should go to a thread)
	bool scanning = true;
	while (scanning)
	{
		scanning = false;
		for (auto it = g_allPorts.begin(); it != g_allPorts.end(); it++)
		{
			if ((*it)->status() == VRiCommPort::Scanning)
			{
				scanning = true;
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				continue;
			}
		}
	}

	VLLog("All ports scanned.", allPorts);

	// Free the unsuccessfull ports, keep the ones where a VRi device was found
	for (auto it = g_allPorts.begin(); it != g_allPorts.end(); it++)
	{
		if ((*it)->status() == VRiCommPort::Found)
		{
			(*it)->parser()->displayIdent1(g_ident1);
			(*it)->parser()->displayIdent2(g_ident2);
			g_vriDeviceHandlers.push_back((*it)->parser());
		}
		else
		{
			VLLog("Giving up on %s", (*it)->portName());
			delete *it;
		}
	}
	g_allPorts.clear();
}

void scanUserPlane()
{
	VLLog("Looking for current User Plane");

	VLLog("Loaded B738: %d B744 %d Zibo737: %d EADT737L %d", LaminarB738::isLoaded(), LaminarB744::isLoaded(), ZiboB738::isLoaded(), EadtX737::isLoaded());
	if (g_plane != nullptr)
	{
		delete g_plane;
		g_plane = nullptr;
	}

	if (ZiboB738::isLoaded())
	{
		VLLog("Aircraft is supported (Zibo B738)");
		for (auto it = g_vriDeviceHandlers.begin(); it != g_vriDeviceHandlers.end(); it++)
		{
			(*it)->displayIdent1("ZIBO");
			(*it)->displayIdent2("B738");
		}

		g_plane = new ZiboB738();
	}
	else if (LaminarB738::isLoaded())
	{
		VLLog("Aircraft is supported (Laminar B738)");
		for (auto it = g_vriDeviceHandlers.begin(); it != g_vriDeviceHandlers.end(); it++)
		{
			(*it)->displayIdent1("LAMI");
			(*it)->displayIdent2("B738");
		}

		g_plane = new LaminarB738();
	}
	else if (LaminarB744::isLoaded())
	{
		VLLog("Aircraft is supported (Laminar B744)");
		for (auto it = g_vriDeviceHandlers.begin(); it != g_vriDeviceHandlers.end(); it++)
		{
			(*it)->displayIdent1("LAMI");
			(*it)->displayIdent2("B744");
		}

		g_plane = new LaminarB744();
	}
	else if (EadtX737::isLoaded())
	{
		VLLog("Aircraft is not yet supported (Eadt x737)");
		for (auto it = g_vriDeviceHandlers.begin(); it != g_vriDeviceHandlers.end(); it++)
		{
			(*it)->displayIdent1("EADT");
			(*it)->displayIdent2("x737");
		}

		g_plane = new EadtX737();
	}
	else if (SsgB748::isLoaded())
	{
		VLLog("Aircraft is supported (SSG B748)");
		for (auto it = g_vriDeviceHandlers.begin(); it != g_vriDeviceHandlers.end(); it++)
		{
			(*it)->displayIdent1("SSG ");
			(*it)->displayIdent2("B748");
		}

		g_plane = new SsgB748();
	}

	if (g_plane != nullptr)
	{
		for (auto it = g_vriDeviceHandlers.begin(); it != g_vriDeviceHandlers.end(); it++)
			(*it)->setAircraft(g_plane);
	}
}

// PluginHost.cpp : Defines the exported functions for the DLL application.
//

#include <Windows.h>
#include <mutex>

#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMUtilities.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
static HMODULE hPluginDLL = NULL;
static char szPayloadPath[512];

typedef int (CALLBACK* LPPLUGINSTART)(char *, char*, char*);
typedef void (CALLBACK* LPPLUGINSTOP)(void);
typedef int (CALLBACK* LPPLUGINENABLE)(void);
typedef void (CALLBACK* LPPLUGINDISABLE)(void);
typedef void (CALLBACK* LPPLUGINRECEIVEMESSAGE)(XPLMPluginID inFrom, int inMsg, void * inParam);

bool loadActualPlugin();
void releaseActualPlugin();

LPPLUGINSTART pluginXPluginStart = nullptr;
LPPLUGINSTOP pluginXPluginStop = nullptr;
LPPLUGINENABLE pluginXPluginEnable = nullptr;
LPPLUGINDISABLE pluginXPluginDisable = nullptr;
LPPLUGINRECEIVEMESSAGE pluginXPluginReceiveMessage = nullptr;

static std::recursive_mutex gPluginDllMutex;

bool gPluginStarted = false;

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
	XPLMDebugString("*** Host Start Called\n");

	GetModuleFileName((HMODULE)&__ImageBase, szPayloadPath, 512);
	XPLMExtractFileAndPath(szPayloadPath);
	strcat(szPayloadPath, "\\payload.dll");

	std::lock_guard<std::recursive_mutex> guard(gPluginDllMutex);
	if (!loadActualPlugin())
	{
		releaseActualPlugin();
		return 0;
	}

	gPluginStarted = true;

	return pluginXPluginStart(outName, outSig, outDesc);
}

PLUGIN_API void XPluginStop(void)
{
	XPLMDebugString("*** Host Stop Called\n");
	std::lock_guard<std::recursive_mutex> guard(gPluginDllMutex);

	if (pluginXPluginStop)
		pluginXPluginStop();

	gPluginStarted = false;

	releaseActualPlugin();
}

PLUGIN_API int XPluginEnable(void)
{
	XPLMDebugString("*** Host Enable Called\n");
	std::lock_guard<std::recursive_mutex> guard(gPluginDllMutex);

	if (!loadActualPlugin())
	{
		releaseActualPlugin();
		return 0;
	}
	else
	{
		char devNull[256];
		if (!gPluginStarted)
		{
			pluginXPluginStart(devNull, devNull, devNull);
			gPluginStarted = true;
		}
		return pluginXPluginEnable();
	}
}

PLUGIN_API void XPluginDisable(void)
{
	XPLMDebugString("*** Host Disable Called\n");
	std::lock_guard<std::recursive_mutex> guard(gPluginDllMutex);

	if (pluginXPluginDisable)
		pluginXPluginDisable();

	if (gPluginStarted)
	{
		if (pluginXPluginStop != nullptr)
			pluginXPluginStop();
		gPluginStarted = false;
	}

	releaseActualPlugin();
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam)
{
	std::lock_guard<std::recursive_mutex> guard(gPluginDllMutex);
	if (pluginXPluginReceiveMessage != nullptr)
		pluginXPluginReceiveMessage(inFrom, inMsg, inParam);
}

bool loadActualPlugin()
{
	std::lock_guard<std::recursive_mutex> guard(gPluginDllMutex);
	if (hPluginDLL != NULL)
		return true;

	XPLMDebugString("Loading Actual Plugin...\n");
	bool failed = false;
	hPluginDLL = LoadLibrary(szPayloadPath);
	if (hPluginDLL == NULL)
	{
		DWORD le = GetLastError();
		char error[1024];
		sprintf(error, "Failed to load actual plugin DLL, code=%d\n", le);
		XPLMDebugString(error);
		return false;
	}

	pluginXPluginStart = (LPPLUGINSTART)GetProcAddress(hPluginDLL, "XPluginStart");
	if (pluginXPluginStart == nullptr)
	{
		XPLMDebugString("Failed to find <XPluginStart> in actual plugin DLL\n");
		failed = true;
	}

	pluginXPluginStop = (LPPLUGINSTOP)GetProcAddress(hPluginDLL, "XPluginStop");
	if (pluginXPluginStart == nullptr)
	{
		XPLMDebugString("Failed to find <XPluginStop> in actual plugin DLL\n");
		failed = true;
	}

	pluginXPluginEnable = (LPPLUGINENABLE)GetProcAddress(hPluginDLL, "XPluginEnable");
	if (pluginXPluginStart == nullptr)
	{
		XPLMDebugString("Failed to find <XPluginEnable> in actual plugin DLL\n");
		failed = true;
	}

	pluginXPluginDisable = (LPPLUGINDISABLE)GetProcAddress(hPluginDLL, "XPluginDisable");
	if (pluginXPluginStart == nullptr)
	{
		XPLMDebugString("Failed to find <XPluginDisable> in actual plugin DLL\n");
		failed = true;
	}

	// nullptr is okay for this one
	pluginXPluginReceiveMessage = (LPPLUGINRECEIVEMESSAGE)GetProcAddress(hPluginDLL, "XPluginReceiveMessage");

	XPLMDebugString("*** Actual Plugin Loaded...\n");
	return !failed;
}

void releaseActualPlugin()
{
	std::lock_guard<std::recursive_mutex> guard(gPluginDllMutex);

	XPLMDebugString("Releasing Actual Plugin...\n");
	if (hPluginDLL != NULL)
	{
		do
		{
			FreeLibrary(hPluginDLL);
		} 
		while (GetModuleHandle(szPayloadPath));
		hPluginDLL = NULL;
	}

	pluginXPluginStart = nullptr;
	pluginXPluginStop = nullptr;
	pluginXPluginEnable = nullptr;
	pluginXPluginDisable = nullptr;
	pluginXPluginReceiveMessage = nullptr;
	XPLMDebugString("Actual Plugin Released...\n");
}


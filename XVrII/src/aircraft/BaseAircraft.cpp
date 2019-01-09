#include "BaseAircraft.h"
#include "logger.h"

#include <mutex>
#include <list>
#include "VRiCommPort.h"

#define COMMAND(a) case BaseDeviceHandler::##a: scheduleCommand(m_ref##a); return true;

static std::mutex commandMutex;

const char *BaseAircraft::SIM_COCKPIT2_AUTOPILOT = "sim/cockpit2/autopilot/";
const char *BaseAircraft::SIM_COCKPIT2_RADIOS = "sim/cockpit2/radios/";
const char *BaseAircraft::SIM_INSTRUMENTS = "sim/instruments/";

BaseAircraft::BaseAircraft()
	: m_xpCommands(nullptr)
	, m_efisModeStepTracker(0)
	, m_efisZoomStepTracker(0)
	, m_altitude(-1.0)
	, m_speed(-1.0)
	, m_heading(-1.0)
	, m_hardwareAltitude(-1.0)
	, m_hardwareSpeed(-1.0)
	, m_hardwareHeading(-1.0)
	, m_mode(None)
	, m_typeResetCounter(5)
{
	m_refApDisplayedAltitude = findDataRef(SIM_COCKPIT2_AUTOPILOT, "altitude_dial_ft");
	m_refApDisplayedSpeed = findDataRef(SIM_COCKPIT2_AUTOPILOT, "airspeed_dial_kts_mach");
	m_refApDisplayedHeading = findDataRef(SIM_COCKPIT2_AUTOPILOT, "heading_dial_deg_mag_pilot");

	m_refEfisZoomIn = findCommandRef(SIM_INSTRUMENTS, "map_zoom_in");
	m_refEfisZoomOut = findCommandRef(SIM_INSTRUMENTS, "map_zoom_out");
	m_refEfisModeDown = findCommandRef(SIM_INSTRUMENTS, "EFIS_mode_dn");
	m_refEfisModeUp = findCommandRef(SIM_INSTRUMENTS, "EFIS_mode_up");

	m_refEfisBaroUp = findCommandRef("sim/instruments/barometer_up");
	m_refEfisBaroDown = findCommandRef("sim/instruments/barometer_down");
	m_refEfisBaroReset = findCommandRef("sim/instruments/barometer_2992");

	m_refEfisMinsUp = findCommandRef("sim/instruments/dh_ref_up");
	m_refEfisMinsDown = findCommandRef("sim/instruments/dh_ref_down");

	m_refAptToGa = findCommandRef("sim/engines/TOGA_power");

	m_refObsUp = findCommandRef("sim/radios/obs1_up");
	m_refObsDown = findCommandRef("sim/radios/obs1_down");
	m_refAptVsUp = findCommandRef("sim/autopilot/vertical_speed_up");
	m_refAptVsDown = findCommandRef("sim/autopilot/vertical_speed_down");

	m_refComm1 = findDataRef("sim/cockpit/radios/com1_freq_hz");
	m_refComm1Stdby = findDataRef("sim/cockpit/radios/com1_stdby_freq_hz");
	m_refComm2 = findDataRef("sim/cockpit/radios/com2_freq_hz");
	m_refComm2Stdby = findDataRef("sim/cockpit/radios/com2_stdby_freq_hz");

	m_refNav1 = findDataRef("sim/cockpit/radios/nav1_freq_hz");
	m_refNav1Stdby = findDataRef("sim/cockpit/radios/nav1_stdby_freq_hz");
	m_refNav2 = findDataRef("sim/cockpit/radios/nav2_freq_hz");
	m_refNav2Stdby = findDataRef("sim/cockpit/radios/nav2_stdby_freq_hz");

	m_refDme1Dist = findDataRef(SIM_COCKPIT2_RADIOS, "indicators/nav1_dme_distance_nm");
	m_refDme1Ident = findDataRef(SIM_COCKPIT2_RADIOS, "indicators/nav1_nav_id");
	m_refDme1Speed = findDataRef(SIM_COCKPIT2_RADIOS, "indicators/nav1_dme_speed_kts");
	m_refDme1Course = findDataRef("sim/cockpit/radios/nav1_obs_degm");

	m_refDme2Dist = findDataRef(SIM_COCKPIT2_RADIOS, "indicators/nav2_dme_distance_nm");
	m_refDme2Ident = findDataRef(SIM_COCKPIT2_RADIOS, "indicators/nav2_nav_id");
	m_refDme2Speed = findDataRef(SIM_COCKPIT2_RADIOS, "indicators/nav2_dme_speed_kts");
	m_refDme2Course = findDataRef("sim/cockpit/radios/nav2_obs_degm");

	m_refTransponderCode = findDataRef(SIM_COCKPIT2_RADIOS, "actuators/transponder_code");
	m_refTransponderMode = findDataRef(SIM_COCKPIT2_RADIOS, "actuators/transponder_mode");

	m_refTransponderModeUp = findCommandRef("sim/transponder/transponder_up");
	m_refTransponderModeDown =  findCommandRef("sim/transponder/transponder_dn");
}

BaseAircraft::~BaseAircraft()
{
}

XPLMDataRef BaseAircraft::findDataRef(const char *prefix, char *reference)
{
	char szFullRef[512];
	strcpy(szFullRef, prefix);
	if (reference != nullptr)
		strcat(szFullRef, reference);
	XPLMDataRef ref = XPLMFindDataRef(szFullRef);
	if (ref == nullptr)
	{
		VLLog("Could not find data reference: <%s>\n", szFullRef);
	}
	return ref;
}

XPLMCommandRef BaseAircraft::findCommandRef(const char *prefix, char *command)
{
	char szFullRef[512];
	strcpy(szFullRef, prefix);
	if (command != nullptr)
	strcat(szFullRef, command);
	XPLMCommandRef ref = XPLMFindCommand(szFullRef);
	if (ref == nullptr)
	{
		VLLog("Could not find command reference: <%s>\n", szFullRef);
	}
	return ref;
}

void BaseAircraft::updateDisplays(const std::list<VRiCommPort*> &devices)
{
	if (m_typeResetCounter > 0)
	{
		m_typeResetCounter--;
		if (m_typeResetCounter == 0)
		{
			for (auto it = devices.begin(); it != devices.end(); it++)
			{
				(*it)->sendIdent1("    ");
				(*it)->sendIdent2("    ");
			}
		}
	}

	updateAltitude(devices);
	updateHeading(devices);

	if (m_hardwareSpeed != m_speed)
	{
		XPLMSetDataf(m_refApDisplayedSpeed, m_hardwareSpeed);
		m_speed = m_hardwareSpeed;
	}
	else if (hardwareDisplayUpdateAllowed())
	{
		float displayed = XPLMGetDataf(m_refApDisplayedSpeed);
		if (displayed != m_speed)
		{
			char command[16];
			sprintf(command, "SPD%03.0f\0\0", displayed);
			for (auto &it = devices.begin(); it != devices.end(); it++)
			{
				(*it)->send(command);
			}
			m_speed = displayed;
			m_hardwareSpeed = displayed;
		}
	}
	
	switch (m_mode)
	{
	case Comm1:
		syncNavDisplay(m_hardwareComm1Stdby, m_comm1Stdby, m_refComm1Stdby, devices, "COMs%04d");
		syncNavDisplay(m_hardwareComm1, m_comm1, m_refComm1, devices, "COMx%04d");
		break;
	case Comm2:
		syncNavDisplay(m_hardwareComm2Stdby, m_comm2Stdby, m_refComm2Stdby, devices, "COMS%04d");
		syncNavDisplay(m_hardwareComm2, m_comm2, m_refComm2, devices, "COMX%04d");
		break;
	case Nav1:
		syncNavDisplay(m_hardwareNav1Stdby, m_nav1Stdby, m_refNav1Stdby, devices, "NAVs%04d");
		syncNavDisplay(m_hardwareNav1, m_nav1, m_refNav1, devices, "NAVx%04d");
		break;
	case Nav2:
		syncNavDisplay(m_hardwareNav2Stdby, m_nav2Stdby, m_refNav2Stdby, devices, "NAVS%04d");
		syncNavDisplay(m_hardwareNav2, m_nav2, m_refNav2, devices, "NAVX%04d");
		break;
	case Trans:
		syncNavDisplay(m_hardwareTransponderCode, m_transponderCode, m_refTransponderCode, devices, "TRN%04d");
		break;
	case Dme1:
		{
			int sim = (int)(XPLMGetDataf(m_refDme1Dist) * 10);
			if (m_dme1Dist != sim)
			{
				char command[9];
				sprintf(command, "DMEd%04u", sim);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				m_dme1Dist = sim;
				break;
			}

			sim = (int)(XPLMGetDataf(m_refDme1Speed));
			if (m_dme1Speed != sim)
			{
				char command[9];
				sprintf(command, "DMEs%03u", sim);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				m_dme1Speed = sim;
				break;
			}

			sim = (int)(XPLMGetDataf(m_refDme1Course) + 360.5) % 360;
			if (m_dme1Course != sim)
			{
				char command[9];
				sprintf(command, "DMc%03u ", sim);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				m_dme1Course = sim;
				break;
			}


			char ident[16];
			int ret = XPLMGetDatab(m_refDme1Ident, ident, 0, 16);
			if (strcmp(ident, m_dme1Ident))
			{
				if (strlen(ident) == 0)
				{
					strcpy(ident, "-----");
				}

				char command[16];
				strcpy(command, "DMi");
				strncat(command, ident, 5);
				strncat(command, "        ", 8);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				strncpy(m_dme1Ident, ident, 8);
			}
		}
		break;

	case Dme2:
		{
			int sim = (int)(XPLMGetDataf(m_refDme2Dist) * 10);
			if (m_dme2Dist != sim)
			{
				char command[9];
				sprintf(command, "DMED%04u", sim);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				m_dme2Dist = sim;
				break;
			}

			sim = (int)(XPLMGetDataf(m_refDme2Speed));
			if (m_dme2Speed != sim)
			{
				char command[9];
				sprintf(command, "DMES%03u", sim);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				m_dme2Speed = sim;
				break;
			}

			sim = (int)(XPLMGetDataf(m_refDme2Course));
			if (m_dme2Course != sim)
			{
				char command[8];
				sprintf(command, "DMC%03u", sim);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				m_dme2Course = sim;
				break;
			}

			char ident[16];
			int ret = XPLMGetDatab(m_refDme2Ident, ident, 0, 16);
			if (strcmp(ident, m_dme2Ident))
			{
				if (strlen(ident) == 0)
				{
					strcpy(ident, "-----");
				}
				char command[16];
				strcpy(command, "DMI");
				strncat(command, ident, 5);
				strncat(command, "        ", 8);
				for (auto &it = devices.begin(); it != devices.end(); it++)
				{
					(*it)->send(command);
				}
				strncpy(m_dme2Ident, ident, 8);
			}
		}
		break;

	default:
		break;
	}
}

bool BaseAircraft::hardwareDisplayUpdateAllowed()
{
	return true;
}

void BaseAircraft::updateAltitude(const std::list<VRiCommPort*> &devices)
{
	if (m_hardwareAltitude != m_altitude)
	{
		XPLMSetDataf(m_refApDisplayedAltitude, m_hardwareAltitude);
		m_altitude = m_hardwareAltitude;
	}
	else if (hardwareDisplayUpdateAllowed())
	{
		float displayed = XPLMGetDataf(m_refApDisplayedAltitude);
		if (displayed != m_altitude)
		{
			char command[16];
			sprintf(command, "ALT%03.0f\0\0", (displayed / 100));
			for (auto &it = devices.begin(); it != devices.end(); it++)
			{
				(*it)->send(command);
			}
			m_altitude = displayed;
			m_hardwareAltitude = displayed;
		}
	}
}

void BaseAircraft::updateHeading(const std::list<VRiCommPort*> &devices)
{
	if (m_hardwareHeading != m_heading)
	{
		XPLMSetDataf(m_refApDisplayedHeading, m_hardwareHeading);
		m_heading = m_hardwareHeading;
	}
	else if (hardwareDisplayUpdateAllowed())
	{
		float displayed = XPLMGetDataf(m_refApDisplayedHeading);
		if (displayed != m_heading)
		{
			char command[16];
			sprintf(command, "HDG%03.0f", displayed);
			for (auto &it = devices.begin(); it != devices.end(); it++)
			{
				(*it)->send(command);
			}
			m_heading = displayed;
			m_hardwareHeading = displayed;
		}
	}
}

void BaseAircraft::syncNavDisplay(int &hardware, int &synced, XPLMDataRef ref, const std::list<VRiCommPort*> &devices, char *format)
{
	if (synced != hardware)
	{
		XPLMSetDatai(ref, hardware);
		synced = hardware;
	}
	else
	{
		int simulated = XPLMGetDatai(ref);
		if (synced != simulated)
		{
			char command[16];
			sprintf(command, format, simulated % 10000);
			for (auto &it = devices.begin(); it != devices.end(); it++)
			{
				(*it)->send(command);
			}
			synced = simulated;
			hardware = simulated;
		}
	}
}

void BaseAircraft::simulateKey(int keyNumber)
{
	INPUT key;
	key.type = INPUT_KEYBOARD;
	key.ki.wVk = keyNumber + 0x7c;  // F13
	key.ki.wScan = 0;
	key.ki.dwFlags = 0;
	key.ki.time = 0;
	key.ki.dwExtraInfo = GetMessageExtraInfo();
	SendInput(1, &key, sizeof(INPUT));
	key.ki.dwFlags = 2;
	SendInput(1, &key, sizeof(INPUT));
}



bool BaseAircraft::handleCommand(BaseDeviceHandler::VriCommand command, float value, bool boost)
{
	switch (command)
	{
	case BaseDeviceHandler::CtlBn0:
		simulateKey(0);
		return true;
	case BaseDeviceHandler::CtlBn1:
		simulateKey(1);
		return true;
	case BaseDeviceHandler::CtlBn2:
		simulateKey(2);
		return true;
	case BaseDeviceHandler::CtlBn3:
		simulateKey(3);
		return true;
	case BaseDeviceHandler::CtlBn4:
		simulateKey(4);
		return true;
	case BaseDeviceHandler::CtlBn5:
		simulateKey(5);
		return true;
	case BaseDeviceHandler::CtlBn6:
		simulateKey(6);
		return true;
	case BaseDeviceHandler::CtlBn7:
		simulateKey(7);
		return true;
	case BaseDeviceHandler::ComSel1:
		m_comm1 = -1;
		m_comm1Stdby = -1;
		m_hardwareComm1 = -1;
		m_hardwareComm1Stdby = -1;
		m_mode = Comm1;
		return true;
	case BaseDeviceHandler::ComSel2:
		m_comm2 = -1;
		m_comm2Stdby = -1;
		m_hardwareComm2 = -1;
		m_hardwareComm2Stdby = -1;
		m_mode = Comm2;
		return true;
	case BaseDeviceHandler::Com1SNNN:
		m_hardwareComm1Stdby = (int)value;
		return true;
	case BaseDeviceHandler::Com1XNNN:
		m_hardwareComm1Stdby = m_hardwareComm1;
		m_hardwareComm1 = (int)value;
		return true;
	case BaseDeviceHandler::Com2SNNN:
		m_hardwareComm2Stdby = (int)value;
		return true;
	case BaseDeviceHandler::Com2XNNN:
		m_hardwareComm2Stdby = m_hardwareComm2;
		m_hardwareComm2 = (int)value;
		return true;

	case BaseDeviceHandler::NavSel1:
		m_nav1 = -1;
		m_nav1Stdby = -1;
		m_hardwareNav1 = -1;
		m_hardwareNav1Stdby = -1;
		m_mode = Nav1;
		return true;
	case BaseDeviceHandler::NavSel2:
		m_nav2 = -1;
		m_nav2Stdby = -1;
		m_hardwareNav2 = -1;
		m_hardwareNav2Stdby = -1;
		m_mode = Nav2;
		return true;
	case BaseDeviceHandler::Nav1SNNN:
		m_hardwareNav1Stdby = (int)value;
		return true;
	case BaseDeviceHandler::Nav1XNNN:
		m_hardwareNav1Stdby = m_hardwareNav1;
		m_hardwareNav1 = (int)value;
		return true;
	case BaseDeviceHandler::Nav2SNNN:
		m_hardwareNav2Stdby = (int)value;
		return true;
	case BaseDeviceHandler::Nav2XNNN:
		m_hardwareNav2Stdby = m_hardwareNav2;
		m_hardwareNav2 = (int)value;
		return true;

	case BaseDeviceHandler::DmeSel1:
		m_dme1Dist = -1;
		m_dme1Ident[0] = '\0';
		m_dme1Speed = -1;
		m_dme1Course = -1;
		m_mode = Dme1;
		return true;
	case BaseDeviceHandler::DmeSel2:
		m_dme2Dist = -1;
		m_dme2Ident[0] = '\0';
		m_dme2Speed = -1;
		m_dme2Course = -1;
		m_mode = Dme2;
		return true;

	case BaseDeviceHandler::TrnSel:
		m_hardwareTransponderCode = -1;
		m_transponderCode = -1;
		m_mode = Trans;
		return true;
	case BaseDeviceHandler::TrnSNNN:
//	case VRiCmdParser::TrnXNNN:
		m_hardwareTransponderCode = (int)value;
		return true;

	case BaseDeviceHandler::TrnAux:
		scheduleCommand(m_refTransponderModeUp);
		return true;

	case BaseDeviceHandler::TrnXNNN:
		scheduleCommand(m_refTransponderModeDown);
		return true;

	case BaseDeviceHandler::AltNNNup:
	case BaseDeviceHandler::AltNNNdn:
		m_hardwareAltitude = value;
		return true;
	case BaseDeviceHandler::SpdNNN:
	case BaseDeviceHandler::SpdNNNup:
	case BaseDeviceHandler::SpdNNNdn:
		m_hardwareSpeed = value;
		return true;
	case BaseDeviceHandler::HdgNNNup:
	case BaseDeviceHandler::HdgNNNdn:
		m_hardwareHeading = value;
		return true;
	case BaseDeviceHandler::EfisZoomIn:
		efisZoomIn();
		return true;
	case BaseDeviceHandler::EfisZoomOut:
		efisZoomOut();
		return true;
	case BaseDeviceHandler::EfisModeUp:
		efisModeUp();
		return true;
	case BaseDeviceHandler::EfisModeDown:
		efisModeDown();
		return true;
	COMMAND(EfisMinsUp);
	COMMAND(EfisMinsDown);
	COMMAND(EfisBaroUp);
	COMMAND(EfisBaroDown);
	COMMAND(EfisBaroReset);
	COMMAND(AptToGa);
	COMMAND(ObsUp);
	COMMAND(ObsDown);
	COMMAND(AptVsUp);
	COMMAND(AptVsDown);
	default:
		return false;
	}
}

void BaseAircraft::scheduleCommand(XPLMCommandRef command, int repeat)
{
	if (command != 0)
	{
		std::lock_guard<std::mutex> guard(commandMutex);
		if (m_xpCommands == nullptr)
			m_xpCommands = new std::queue<XPLMCommandRef>();

		m_xpCommands->push(command);
		if (repeat > 1)
		{
			for (int r=0;r<repeat-1;r++)
				m_xpCommands->push(command);
		}
	}
	else
	{
		VLLog("Not scheduling empty command reference\n");
	}
}

CommandQueue *BaseAircraft::queuedCommands()
{
	std::queue<XPLMCommandRef> *returnedCommands;
	commandMutex.lock();
	returnedCommands = m_xpCommands;
	m_xpCommands = nullptr;
	commandMutex.unlock();
	return returnedCommands;
}


void BaseAircraft::efisZoomIn()
{
	m_efisZoomStepTracker--;
	if (m_efisZoomStepTracker == -2)
	{
		m_efisZoomStepTracker = 0;
		scheduleCommand(m_refEfisZoomIn);
	}
}

void BaseAircraft::efisZoomOut()
{
	m_efisZoomStepTracker++;
	if (m_efisZoomStepTracker == 2)
	{
		m_efisZoomStepTracker = 0;
		scheduleCommand(m_refEfisZoomOut);
	}
}

void BaseAircraft::efisModeUp()
{
	m_efisModeStepTracker++;
	handleEfisMode(true);
}

void BaseAircraft::efisModeDown()
{
	m_efisModeStepTracker--;
	handleEfisMode(false);
}

void BaseAircraft::handleEfisMode(bool up)
{
	if (m_efisModeStepTracker < 0)
	{
		m_efisModeStepTracker = 0;
	}
	else if (m_efisModeStepTracker > 8)
	{
		m_efisModeStepTracker = 8;
	}
	else
	{
		switch (m_efisModeStepTracker)
		{
		case 0:
			scheduleCommand(m_refEfisModeDown);
			break;
		case 2:
			scheduleCommand(up ? m_refEfisModeUp : m_refEfisModeDown);
			break;
		case 6:
			scheduleCommand(up ? m_refEfisModeUp : m_refEfisModeDown);
			break;
		case 8:
			scheduleCommand(m_refEfisModeUp);
			break;
		}
	}
}
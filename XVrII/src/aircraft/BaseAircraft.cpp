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
	, m_radioMode(None)
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

void BaseAircraft::updateDisplays(const std::list<BaseDeviceHandler*> &devices)
{
	if (m_typeResetCounter > 0)
	{
		m_typeResetCounter--;
		if (m_typeResetCounter == 0)
		{
			for (auto it = devices.begin(); it != devices.end(); it++)
			{
				(*it)->displayIdent1("    ");
				(*it)->displayIdent2("    ");
			}
		}
	}

	updateAltitude(devices);
	updateHeading(devices);
	updateSpeed(devices);
	updateRadios(devices);
}

bool BaseAircraft::hardwareDisplayUpdateAllowed()
{
	return true;
}

void BaseAircraft::updateSpeed(const std::list<BaseDeviceHandler*> &devices)
{
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
			for (auto &it = devices.begin(); it != devices.end(); it++)
				(*it)->displayMcpSpeed(displayed);

			m_speed = displayed;
			m_hardwareSpeed = displayed;
		}
	}
}

void BaseAircraft::updateHeading(const std::list<BaseDeviceHandler*> &devices)
{
	if (m_hardwareHeading != m_heading)
	{
		XPLMSetDataf(m_refApDisplayedSpeed, m_hardwareHeading);
		m_heading = m_hardwareHeading;
	}
	else if (hardwareDisplayUpdateAllowed())
	{
		float displayed = XPLMGetDataf(m_refApDisplayedSpeed);
		if (displayed != m_heading)
		{
			for (auto &it = devices.begin(); it != devices.end(); it++)
				(*it)->displayMcpSpeed(displayed);

			m_heading = displayed;
			m_hardwareHeading = displayed;
		}
	}
}

void BaseAircraft::updateAltitude(const std::list<BaseDeviceHandler*> &devices)
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
			for (auto &it = devices.begin(); it != devices.end(); it++)
			{
				(*it)->displayMcpAltitude(displayed);
			}
			m_altitude = displayed;
			m_hardwareAltitude = displayed;
		}
	}
}

void BaseAircraft::updateRadios(const std::list<BaseDeviceHandler*> &devices)
{
	switch (m_radioMode)
	{
	case Comm1:
		syncNavDisplay(BaseDeviceHandler::Com1Standby, m_hardwareComm1Stdby, m_comm1Stdby, m_refComm1Stdby, devices);
		syncNavDisplay(BaseDeviceHandler::Com1, m_hardwareComm1, m_comm1, m_refComm1, devices);
		break;
	case Comm2:
		syncNavDisplay(BaseDeviceHandler::Com2Standby, m_hardwareComm2Stdby, m_comm2Stdby, m_refComm2Stdby, devices);
		syncNavDisplay(BaseDeviceHandler::Com2, m_hardwareComm2, m_comm2, m_refComm2, devices);
		break;
	case Nav1:
		syncNavDisplay(BaseDeviceHandler::Nav1Standby, m_hardwareNav1Stdby, m_nav1Stdby, m_refNav1Stdby, devices);
		syncNavDisplay(BaseDeviceHandler::Nav1, m_hardwareNav1, m_nav1, m_refNav1, devices);
		break;
	case Nav2:
		syncNavDisplay(BaseDeviceHandler::Nav2Standby, m_hardwareNav2Stdby, m_nav2Stdby, m_refNav2Stdby, devices);
		syncNavDisplay(BaseDeviceHandler::Nav2, m_hardwareNav2, m_nav2, m_refNav2, devices);
		break;
	case Trans:
		syncNavDisplay(BaseDeviceHandler::Transponder, m_hardwareTransponderCode, m_transponderCode, m_refTransponderCode, devices);
		break;
	case Dme1:
	{
		float distance = XPLMGetDataf(m_refDme1Dist);
		float speed = XPLMGetDataf(m_refDme1Speed);
		float course = XPLMGetDataf(m_refDme1Course);
		char ident[16];
		XPLMGetDatab(m_refDme1Ident, ident, 0, 16);
		for (auto &it = devices.begin(); it != devices.end(); it++)
			(*it)->displayDme1(distance, speed, course, ident);
		break;
	}
	case Dme2:
	{
		float distance = XPLMGetDataf(m_refDme2Dist);
		float speed = XPLMGetDataf(m_refDme2Speed);
		float course = XPLMGetDataf(m_refDme2Course);
		char ident[16];
		XPLMGetDatab(m_refDme2Ident, ident, 0, 16);
		for (auto &it = devices.begin(); it != devices.end(); it++)
			(*it)->displayDme2(distance, speed, course, ident);
		break;
	}

	default:
		break;
	}
}

void BaseAircraft::syncNavDisplay(BaseDeviceHandler::VriRadioDisplay radio, int &hardware, int &synced, XPLMDataRef ref, const std::list<BaseDeviceHandler*> &devices)
{
	if (synced != hardware)
	{
		XPLMSetDatai(ref, hardware);
		synced = hardware;
	}
	else
	{
		int simulated = XPLMGetDatai(ref) % 10000;
		if (synced != simulated)
		{
			for (auto &it = devices.begin(); it != devices.end(); it++)
				(*it)->displayRadio(radio, simulated);

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
	key.ki.time = 0;
	key.ki.dwExtraInfo = GetMessageExtraInfo();

	// Key Down
	key.ki.dwFlags = 0;
	SendInput(1, &key, sizeof(INPUT));

	// Key Up
	key.ki.dwFlags = 2;
	SendInput(1, &key, sizeof(INPUT));
}



bool BaseAircraft::handleCommand(BaseDeviceHandler::VriCommandParameters command)
{
	switch (command.m_command)
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
		m_radioMode = Comm1;
		return true;
	case BaseDeviceHandler::ComSel2:
		m_comm2 = -1;
		m_comm2Stdby = -1;
		m_hardwareComm2 = -1;
		m_hardwareComm2Stdby = -1;
		m_radioMode = Comm2;
		return true;
	case BaseDeviceHandler::Com1SNNN:
		m_hardwareComm1Stdby = (int)command.m_value;
		return true;
	case BaseDeviceHandler::Com1XNNN:
		m_hardwareComm1Stdby = m_hardwareComm1;
		m_hardwareComm1 = (int)command.m_value;
		return true;
	case BaseDeviceHandler::Com2SNNN:
		m_hardwareComm2Stdby = (int)command.m_value;
		return true;
	case BaseDeviceHandler::Com2XNNN:
		m_hardwareComm2Stdby = m_hardwareComm2;
		m_hardwareComm2 = (int)command.m_value;
		return true;

	case BaseDeviceHandler::NavSel1:
		m_nav1 = -1;
		m_nav1Stdby = -1;
		m_hardwareNav1 = -1;
		m_hardwareNav1Stdby = -1;
		m_radioMode = Nav1;
		return true;
	case BaseDeviceHandler::NavSel2:
		m_nav2 = -1;
		m_nav2Stdby = -1;
		m_hardwareNav2 = -1;
		m_hardwareNav2Stdby = -1;
		m_radioMode = Nav2;
		return true;
	case BaseDeviceHandler::Nav1SNNN:
		m_hardwareNav1Stdby = (int)command.m_value;
		return true;
	case BaseDeviceHandler::Nav1XNNN:
		m_hardwareNav1Stdby = m_hardwareNav1;
		m_hardwareNav1 = (int)command.m_value;
		return true;
	case BaseDeviceHandler::Nav2SNNN:
		m_hardwareNav2Stdby = (int)command.m_value;
		return true;
	case BaseDeviceHandler::Nav2XNNN:
		m_hardwareNav2Stdby = m_hardwareNav2;
		m_hardwareNav2 = (int)command.m_value;
		return true;

	case BaseDeviceHandler::DmeSel1:
		m_dme1Dist = -1;
		m_dme1Ident[0] = '\0';
		m_dme1Speed = -1;
		m_dme1Course = -1;
		m_radioMode = Dme1;
		return true;
	case BaseDeviceHandler::DmeSel2:
		m_dme2Dist = -1;
		m_dme2Ident[0] = '\0';
		m_dme2Speed = -1;
		m_dme2Course = -1;
		m_radioMode = Dme2;
		return true;

	case BaseDeviceHandler::TrnSel:
		m_hardwareTransponderCode = -1;
		m_transponderCode = -1;
		m_radioMode = Trans;
		return true;
	case BaseDeviceHandler::TrnSNNN:
//	case VRiCmdParser::TrnXNNN:
		m_hardwareTransponderCode = (int)command.m_value;
		return true;

	case BaseDeviceHandler::TrnAux:
		scheduleCommand(m_refTransponderModeUp);
		return true;

	case BaseDeviceHandler::TrnXNNN:
		scheduleCommand(m_refTransponderModeDown);
		return true;

	case BaseDeviceHandler::AltNNNup:
	case BaseDeviceHandler::AltNNNdn:
		m_hardwareAltitude = command.m_value;
		return true;
	case BaseDeviceHandler::SpdNNN:
	case BaseDeviceHandler::SpdNNNup:
	case BaseDeviceHandler::SpdNNNdn:
		m_hardwareSpeed = command.m_value;
		return true;
	case BaseDeviceHandler::HdgNNNup:
	case BaseDeviceHandler::HdgNNNdn:
		m_hardwareHeading = command.m_value;
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
#include "LaminarB744.h"

#include "logger.h"
#include <list>
#include "VRiCommPort.h"

const char *LaminarB744::LAM_B747_ND = "laminar/B747/nd/";
const char *LaminarB744::LAM_B747_EFIS = "laminar/B747/efis/";
const char *LaminarB744::LAM_B747_TOGGLE = "laminar/B747/toggle_switch/";

LaminarB744::LaminarB744()
	: BaseAircraft()
{
	m_refAltNNNup = findCommandRef("sim/autopilot/altitude_up");
	m_refAltNNNdn = findCommandRef("sim/autopilot/altitude_down");

	m_refHdgNNNup = findCommandRef("sim/autopilot/heading_up");
	m_refHdgNNNdn = findCommandRef("sim/autopilot/heading_down");

	// Overrule the normal ref to make sure correct hdg is displayed in heading hold mode
	m_refApDisplayedHeading = XPLMFindDataRef("laminar/B747/autopilot/heading/degrees");

	m_refToggleSwitchPositions = XPLMFindDataRef("laminar/B747/toggle_switch/position");
	m_refApButtonSwitchPositions = XPLMFindDataRef("laminar/B747/autopilot/button_switch/position");

	m_refFlightDirectorToggle = findCommandRef(LAM_B747_TOGGLE, "flight_dir_L");
	m_refAutoThrottleToggle = findCommandRef(LAM_B747_TOGGLE, "autothrottle");

	m_refEfisVor1Up = findCommandRef(LAM_B747_TOGGLE, "vor_adf_pilot_L_up");
	m_refEfisVor1Down = findCommandRef(LAM_B747_TOGGLE, "vor_adf_pilot_L_down");
	m_refEfisVor2Up = findCommandRef(LAM_B747_TOGGLE, "vor_adf_pilot_R_up");
	m_refEfisVor2Down = findCommandRef(LAM_B747_TOGGLE, "vor_adf_pilot_R_down");

	m_refEfisMinsReset = findCommandRef(LAM_B747_EFIS, "dh/capt/reset_switch");
	m_refEfisMeters = findCommandRef(LAM_B747_EFIS, "meters/capt/switch");
	m_refEfisFpv = findCommandRef(LAM_B747_EFIS, "fpv/capt/switch");

	m_refEfisBaroUp = findCommandRef(LAM_B747_EFIS, "baro_set/capt/sel_dial_up");
	m_refEfisBaroDown = findCommandRef(LAM_B747_EFIS, "baro_set/capt/sel_dial_dn");
	m_refEfisBaroReset = findCommandRef(LAM_B747_EFIS, "baro_std/capt/switch");

	m_refEfisWx = findCommandRef(LAM_B747_ND, "wxr/capt/switch");
	m_refEfisSta = findCommandRef(LAM_B747_ND, "sta/capt/switch");
	m_refEfisWpt = findCommandRef(LAM_B747_ND, "wpt/capt/switch");
	m_refEfisArpt = findCommandRef(LAM_B747_ND, "arpt/capt/switch");
	m_refEfisData = findCommandRef(LAM_B747_ND, "data/capt/switch");
	m_refEfisPos = findCommandRef(LAM_B747_ND, "pos/capt/switch");
	m_refEfisTerr = findCommandRef(LAM_B747_ND, "terr/capt/switch");

	m_refEfisZoomIn = findCommandRef(LAM_B747_ND, "range/capt/sel_dial_dn");
	m_refEfisZoomOut = findCommandRef(LAM_B747_ND, "range/capt/sel_dial_up");
	m_refEfisModeDown = findCommandRef(LAM_B747_ND, "mode/capt/sel_dial_dn");
	m_refEfisModeUp = findCommandRef(LAM_B747_ND, "mode/capt/sel_dial_up");

	m_refEfisCtr = findCommandRef(LAM_B747_ND, "center/capt/switch");
	m_refEfisTfc = findCommandRef(LAM_B747_ND, "traffic/capt/switch");

	m_refAptN1 = findCommandRef("laminar/B747/autopilot/button_switch/thrust_mode");
	m_refAptSpd = findCommandRef("laminar/B747/autopilot/button_switch/speed_mode");
	m_refAptFlch = findCommandRef("laminar/B747/autopilot/button_switch/flch_mode");
	/// Disabled 11.30r1: m_refAptHdgSel = findCommandRef("laminar/B747/autopilot/button_switch/hdg_sel_mode");	
	m_refAptHdgHold = findCommandRef("sim/autopilot/heading_hold");

	m_refAptAltHold = findCommandRef("laminar/B747/autopilot/button_switch/alt_hold_mode");
	m_refAptVvsHold = findCommandRef("laminar/B747/autopilot/button_switch/vs_mode");

	m_refAptVNav = findCommandRef("sim/autopilot/FMS");   ///
	/// Disabled 11.30r1: m_refAptLNav = findCommandRef("laminar/B747/autopilot/button_switch/lnav_mode");   ///
	m_refAptCmdA = findCommandRef("laminar/B747/autopilot/button_switch/cmd_L");
	m_refAptCmdC = findCommandRef("laminar/B747/autopilot/button_switch/cmd_C");
	m_refAptCmdB = findCommandRef("laminar/B747/autopilot/button_switch/cmd_R");

	m_refAptLoc = findCommandRef("laminar/B747/autopilot/button_switch/loc_mode");
	m_refAptApp = findCommandRef("sim/autopilot/approach");

	m_refApMaster = findCommandRef("laminar/B747/autopilot/slide_switch/disengage_bar");

	// Override command from base class
	/// Disabled 11.30r1: m_refAptToGa = findCommandRef("laminar/B747/engine/TOGA_switch_L");
}

void LaminarB744::updateAltitude(const std::list<VRiCommPort*> &devices)
{
	// Only sync from sim to hardware, changing is done through up/down command
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

void LaminarB744::updateHeading(const std::list<VRiCommPort*> &devices)
{
	// Only sync from sim to hardware, changing is done through up/down command
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


#define COMMAND(a) case BaseDeviceHandler::##a: scheduleCommand(m_ref##a); return true;

#define NUM_TOGGLE_SW (38)
#define TOGGLE_AP_DISENGAGE_BAR (14)
#define TOGGLE_FD_LEFT (23)
#define TOGGLE_AT (29)

#define NUM_APBUTTON_SW (15)
#define BUTTONAP_DISENGAGE_BAR (14)

static int toggles[NUM_TOGGLE_SW];
static int apbuttons[NUM_APBUTTON_SW];

bool LaminarB744::handleCommand(BaseDeviceHandler::VriCommandParameters command)
{
	XPLMGetDatavi(m_refToggleSwitchPositions, toggles, 0, NUM_TOGGLE_SW);
	XPLMGetDatavi(m_refApButtonSwitchPositions, apbuttons, 0, NUM_APBUTTON_SW);

	switch (command.m_command)
	{
		COMMAND(AltNNNup);
		COMMAND(AltNNNdn);
		COMMAND(HdgNNNup);
		COMMAND(HdgNNNdn);

	case BaseDeviceHandler::AptAtArm:
		if (toggles[TOGGLE_AT] == 0)
			scheduleCommand(m_refAutoThrottleToggle);
		return true;
	case BaseDeviceHandler::AptAtDisarm:
		if (toggles[TOGGLE_AT] != 0)
			scheduleCommand(m_refAutoThrottleToggle);
		return true;
	case BaseDeviceHandler::AptFdArm:
		if (toggles[TOGGLE_FD_LEFT] == 0)
			scheduleCommand(m_refFlightDirectorToggle);
		return true;
	case BaseDeviceHandler::AptFdDisarm:
		if (toggles[TOGGLE_FD_LEFT] != 0)
			scheduleCommand(m_refFlightDirectorToggle);
		return true;
	case BaseDeviceHandler::AptMasterConnect:
		if (apbuttons[BUTTONAP_DISENGAGE_BAR] != 0)
			scheduleCommand(m_refApMaster);
		return true;
	case BaseDeviceHandler::AptMasterDisconnect:
		if (apbuttons[BUTTONAP_DISENGAGE_BAR] == 0)
			scheduleCommand(m_refApMaster);
		return true;

	COMMAND(EfisVor1Up);
	COMMAND(EfisVor1Down);
	COMMAND(EfisVor2Up);
	COMMAND(EfisVor2Down);
	COMMAND(EfisMinsReset);
	COMMAND(EfisMeters);
	COMMAND(EfisFpv);
	COMMAND(EfisWx);
	COMMAND(EfisSta);
	COMMAND(EfisWpt);
	COMMAND(EfisArpt);
	COMMAND(EfisData);
	COMMAND(EfisPos);
	COMMAND(EfisTerr);
	COMMAND(EfisCtr);
	COMMAND(EfisTfc);
	COMMAND(AptN1);
	COMMAND(AptFlch);
	COMMAND(AptSpd);
	COMMAND(AptHdgSel);
	COMMAND(AptHdgHold);
	COMMAND(AptAltHold);
	COMMAND(AptVvsHold);
	COMMAND(AptVNav);
	COMMAND(AptLNav);
	COMMAND(AptCmdA);
	COMMAND(AptCmdB);
	COMMAND(AptCmdC);
	COMMAND(AptLoc);
	COMMAND(AptApp);

	default:
		return BaseAircraft::handleCommand(command);
	}
}

bool LaminarB744::isLoaded()
{
	return (XPLMFindDataRef("laminar/B747/toggle_switch/position") != nullptr);
}

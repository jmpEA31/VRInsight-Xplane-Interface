#include "LaminarB738.h"
#include "logger.h"

LaminarB738::LaminarB738()
	: BaseAircraft()
{
	m_refEfisVor1Up = findCommandRef("laminar/B738/EFIS_control/capt/vor1_off_up");
	m_refEfisVor1Down = findCommandRef("laminar/B738/EFIS_control/capt/vor1_off_dn");
	m_refEfisVor2Up = findCommandRef("laminar/B738/EFIS_control/capt/vor2_off_up");
	m_refEfisVor2Down = findCommandRef("laminar/B738/EFIS_control/capt/vor2_off_dn");

	m_refEfisMinsReset = findCommandRef("laminar/B738/EFIS_control/capt/push_button/rst_press");

	m_refEfisMeters = findCommandRef("laminar/B738/EFIS_control/capt/push_button/mtrs_press");
	m_refEfisFpv = findCommandRef("laminar/B738/EFIS_control/capt/push_button/fpv_press");

	m_refEfisWx = findCommandRef("laminar/B738/EFIS_control/capt/push_button/wxr_press");
	m_refEfisSta = findCommandRef("laminar/B738/EFIS_control/capt/push_button/sta_press");
	m_refEfisWpt = findCommandRef("laminar/B738/EFIS_control/capt/push_button/wpt_press");
	m_refEfisArpt = findCommandRef("laminar/B738/EFIS_control/capt/push_button/arpt_press");
	m_refEfisData = findCommandRef("laminar/B738/EFIS_control/capt/push_button/data_press");
	m_refEfisPos = findCommandRef("laminar/B738/EFIS_control/capt/push_button/pos_press");
	m_refEfisTerr = findCommandRef("laminar/B738/EFIS_control/capt/push_button/terr_press");
	m_refEfisCtr = findCommandRef("laminar/B738/EFIS_control/capt/push_button/ctr_press");
	m_refEfisTfc = findCommandRef("laminar/B738/EFIS_control/capt/push_button/tfc_press");

	m_refAutoThrottleToggle = findCommandRef("laminar/B738/autopilot/autothrottle_arm_toggle");
	m_refAutoThrottleStatus = findDataRef("laminar/B738/autopilot/autothrottle_arm_pos");
	m_refFDToggle = findCommandRef("laminar/B738/autopilot/flight_director_toggle");
	m_refFDStatus = findDataRef("laminar/B738/autopilot/flight_director_pos");

	m_refAptN1 = findCommandRef("laminar/B738/autopilot/n1_press");
	m_refAptSpd = findCommandRef("laminar/B738/autopilot/speed_press");
	m_refAptFlch = findCommandRef("laminar/B738/autopilot/lvl_chg_press");
	m_refAptHdgSel = findCommandRef("laminar/B738/autopilot/hdg_sel_press");
	m_refAptAltHold = findCommandRef("laminar/B738/autopilot/alt_hld_press");
	m_refAptVvsHold = findCommandRef("laminar/B738/autopilot/vs_press");

	m_refAptVNav = findCommandRef("laminar/B738/autopilot/vnav_press");
	m_refAptLNav = findCommandRef("laminar/B738/autopilot/lnav_press");
	m_refAptCmdA = findCommandRef("laminar/B738/autopilot/cmd_a_press");
	m_refAptCmdB = findCommandRef("laminar/B738/autopilot/cmd_b_press");
	m_refAptCwsA = findCommandRef("laminar/B738/autopilot/cws_a_press");
	m_refAptCwsB = findCommandRef("laminar/B738/autopilot/cws_b_press");
	m_refAptLoc = findCommandRef("laminar/B738/autopilot/vorloc_press");
	m_refAptApp = findCommandRef("laminar/B738/autopilot/app_press");

	m_refApMaster = findCommandRef("laminar/B738/autopilot/disconnect_toggle");
	m_refApMasterPos = findDataRef("laminar/B738/autopilot/disconnect_pos");

	// Override command from base class
	m_refTransponderModeUp = findCommandRef("laminar/B738/knob/transponder_mode_up");
	m_refTransponderModeDown =  findCommandRef("laminar/B738/knob/transponder_mode_dn");
}

#define COMMAND(a) case BaseDeviceHandler::##a: scheduleCommand(m_ref##a); return true;

bool LaminarB738::handleCommand(BaseDeviceHandler::VriCommandParameters command)
{
	switch (command.m_command)
	{
	case BaseDeviceHandler::AptAtArm:
		if (XPLMGetDatai(m_refAutoThrottleStatus) != 1)
			scheduleCommand(m_refAutoThrottleToggle);
		return true;
	case BaseDeviceHandler::AptAtDisarm:
		if (XPLMGetDatai(m_refAutoThrottleStatus) != 0)
			scheduleCommand(m_refAutoThrottleToggle);
		return true;
	case BaseDeviceHandler::AptFdArm:
		if (XPLMGetDatai(m_refFDStatus) != 1)
			scheduleCommand(m_refFDToggle);
		return true;
	case BaseDeviceHandler::AptFdDisarm:
		if (XPLMGetDatai(m_refFDStatus) != 0)
			scheduleCommand(m_refFDToggle);
		return true;
	case BaseDeviceHandler::AptMasterConnect:
		if (XPLMGetDatai(m_refApMasterPos) != 0)
			scheduleCommand(m_refApMaster);
		return true;
	case BaseDeviceHandler::AptMasterDisconnect:
		if (XPLMGetDatai(m_refApMasterPos) != 1)
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
	COMMAND(AptAltHold);
	COMMAND(AptVvsHold);
	COMMAND(AptVNav);
	COMMAND(AptLNav);
	COMMAND(AptCmdA);
	COMMAND(AptCmdB);
	COMMAND(AptCwsA);
	COMMAND(AptCwsB);
	COMMAND(AptLoc);
	COMMAND(AptApp);

	default:
		return BaseAircraft::handleCommand(command);
	}
}

bool LaminarB738::isLoaded()
{
	return (XPLMFindDataRef("laminar/B738/autopilot/autothrottle_arm_pos") != nullptr);
}

#include "EadtX737.h"


EadtX737::EadtX737()
	: BaseAircraft()
{
	//m_refEfisVor1Up = XPLMFindCommand("laminar/B738/EFIS_control/capt/vor1_off_up");
	//m_refEfisVor1Down = XPLMFindCommand("laminar/B738/EFIS_control/capt/vor1_off_dn");
	//m_refEfisVor2Up = XPLMFindCommand("laminar/B738/EFIS_control/capt/vor2_off_up");
	//m_refEfisVor2Down = XPLMFindCommand("laminar/B738/EFIS_control/capt/vor2_off_dn");

	//m_refEfisMinsReset = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/rst_press");

	//m_refEfisMeters = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/mtrs_press");
	//m_refEfisFpv = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/fpv_press");

	//m_refEfisWx = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/wxr_press");
	//m_refEfisSta = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/sta_press");
	//m_refEfisWpt = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/wpt_press");
	//m_refEfisArpt = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/arpt_press");
	//m_refEfisData = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/data_press");
	//m_refEfisPos = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/pos_press");
	//m_refEfisTerr = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/terr_press");
	//m_refEfisCtr = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/ctr_press");
	//m_refEfisTfc = XPLMFindCommand("laminar/B738/EFIS_control/capt/push_button/tfc_press");

	//m_refAutoThrottleToggle = XPLMFindCommand("laminar/B738/autopilot/autothrottle_arm_toggle");
	//m_refAutoThrottleStatus = XPLMFindDataRef("laminar/B738/autopilot/autothrottle_arm_pos");
	//m_refFDToggle = XPLMFindCommand("laminar/B738/autopilot/flight_director_toggle");
	//m_refFDStatus = XPLMFindDataRef("laminar/B738/autopilot/flight_director_pos");

	//m_refAptN1 = XPLMFindCommand("laminar/B738/autopilot/n1_press");
	//m_refAptSpd = XPLMFindCommand("laminar/B738/autopilot/speed_press");
	//m_refAptFlch = XPLMFindCommand("laminar/B738/autopilot/lvl_chg_press");
	//m_refAptHdgSel = XPLMFindCommand("laminar/B738/autopilot/hdg_sel_press");
	//m_refAptAltHold = XPLMFindCommand("laminar/B738/autopilot/alt_hld_press");
	//m_refAptVvsHold = XPLMFindCommand("laminar/B738/autopilot/vs_press");

	//m_refAptVNav = XPLMFindCommand("laminar/B738/autopilot/vnav_press");
	//m_refAptLNav = XPLMFindCommand("laminar/B738/autopilot/lnav_press");
	//m_refAptCmdA = XPLMFindCommand("laminar/B738/autopilot/cmd_a_press");
	//m_refAptCmdB = XPLMFindCommand("laminar/B738/autopilot/cmd_b_press");
	//m_refAptCwsA = XPLMFindCommand("laminar/B738/autopilot/cws_a_press");
	//m_refAptCwsB = XPLMFindCommand("laminar/B738/autopilot/cws_b_press");
	//m_refAptLoc = XPLMFindCommand("laminar/B738/autopilot/vorloc_press");
	//m_refAptApp = XPLMFindCommand("laminar/B738/autopilot/app_press");

	//m_refApMaster = XPLMFindCommand("laminar/B738/autopilot/disconnect_toggle");
	//m_refApMasterPos = XPLMFindDataRef("laminar/B738/autopilot/disconnect_pos");

	//// Override command from base class
	//m_refTransponderModeUp = XPLMFindCommand("laminar/B738/knob/transponder_mode_up");
	//m_refTransponderModeDown =  XPLMFindCommand("laminar/B738/knob/transponder_mode_dn");
}

#define COMMAND(a) case BaseDeviceHandler::##a: scheduleCommand(m_ref##a); return true;

bool EadtX737::handleCommand(BaseDeviceHandler::VriCommand command, float value, bool boost)
{
	return false;

	switch (command)
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
		return BaseAircraft::handleCommand(command, value, boost);
	}
}

bool EadtX737::isLoaded()
{
	return (XPLMFindDataRef("x737/engine/thrustrev_pos_1") != nullptr);
}

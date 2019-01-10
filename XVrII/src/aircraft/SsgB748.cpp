#include "SsgB748.h"

#include <XPLM/XPLMProcessing.h>
#include "logger.h"
#include <list>
#include "VRiCommPort.h"

//const char *SsgB748::LAM_B747_ND = "laminar/B747/nd/";
//const char *SsgB748::LAM_B747_EFIS = "laminar/B747/efis/";
//const char *SsgB748::LAM_B747_TOGGLE = "laminar/B747/toggle_switch/";

SsgB748::SsgB748()
	: BaseAircraft()
	, m_displayCommandsScheduled(0)
	, m_displayCommandCooldown(0.0)
{
	m_refAltNNNup = findCommandRef("SSG/UFMC/Alt_UP");
	XPLMRegisterCommandHandler(m_refAltNNNup, ssgCommandCallback, 0, this);
	m_refAltNNNdn = findCommandRef("SSG/UFMC/Alt_Down");
	XPLMRegisterCommandHandler(m_refAltNNNdn, ssgCommandCallback, 0, this);

	m_refHdgNNNup = findCommandRef("SSG/UFMC/HDG_UP");
	XPLMRegisterCommandHandler(m_refHdgNNNup, ssgCommandCallback, 0, this);
	m_refHdgNNNdn = findCommandRef("SSG/UFMC/HDG_Down");
	XPLMRegisterCommandHandler(m_refHdgNNNdn, ssgCommandCallback, 0, this);

	m_refSpdNNNup = findCommandRef("SSG/UFMC/Speed_UP");
	XPLMRegisterCommandHandler(m_refSpdNNNup, ssgCommandCallback, 0, this);
	m_refSpdNNNdn = findCommandRef("SSG/UFMC/Speed_Down");
	XPLMRegisterCommandHandler(m_refSpdNNNdn, ssgCommandCallback, 0, this);

	m_refFlightDirectorTogglePt = findCommandRef("SSG/UFMC/FD_Pilot_SW");
	m_refFlightDirectorToggleCp = findCommandRef("SSG/UFMC/FD_Copilot_SW");
	m_refAutoThrottleToggle = findCommandRef("SSG/UFMC/AP_ARM_AT_Switch");

	m_refAptN1 = findCommandRef("SSG/UFMC/AP_N1_Button");
	m_refAptSpd = findCommandRef("SSG/UFMC/AP_SPD_Button");
	m_refAptFlch = findCommandRef("SSG/UFMC/AP_LVLCHG_Button");
	m_refAptHdgSel = findCommandRef("SSG/UFMC/AP_HDG_Button");
	m_refAptHdgHold = findCommandRef("SSG/UFMC/AP_HDGHOLD_Button");

	m_refAptSpdSel = findCommandRef("SSG/UFMC/AP_SPD_Intervention_Button");
	m_refAptAltSel = findCommandRef("SSG/UFMC/AP_Altitude_Intervention_Button");
	m_refAptAltHold = findCommandRef("SSG/UFMC/AP_ALTHOLD_Button");
	m_refAptVvsHold = findCommandRef("SSG/UFMC/AP_VS_Button");

	m_refAptVNav = findCommandRef("SSG/UFMC/AP_VNAV_Button");
	m_refAptLNav = findCommandRef("SSG/UFMC/AP_LNAV_Button");
	m_refAptCmdA = findCommandRef("SSG/UFMC/AP_CMDA_Button");
	m_refAptCmdC = findCommandRef("SSG/UFMC/AP_CMDB_Button");
	m_refAptCmdB = findCommandRef("SSG/UFMC/AP_CMDC_Button");

	m_refAptLoc = findCommandRef("SSG/UFMC/AP_VORLOC_Button");
	m_refAptApp = findCommandRef("SSG/UFMC/AP_APP_Button");

	m_refApMaster = findCommandRef("SSG/UFMC/AP_discon_Button");

	m_refAptToGa = findCommandRef("SSG/UFMC/TOGA_Button");

	m_refNdMode = findDataRef("ssg/B748/ND/mode_pilot");
	m_ndMode = XPLMGetDataf(m_refNdMode);
	m_refNdRange = findDataRef("ssg/B748/ND/range_pilot");
	m_ndRange = XPLMGetDataf(m_refNdRange);

	m_refShowTCAS = findDataRef("ssg/B748/ND/show_TCAS_pilot");
	m_showTCAS = XPLMGetDataf(m_refShowTCAS);

	m_refShowWx = findDataRef("ssg/B748/ND/show_wheather_pilot");
	m_showWx = XPLMGetDataf(m_refShowWx);
	m_refShowSta = findDataRef("ssg/B748/ND/show_VOR_pilot");
	m_showSta = XPLMGetDataf(m_refShowSta);
	m_refShowWaypoint = findDataRef("ssg/B748/ND/show_waypoint_pilot");
	m_showWaypoint = XPLMGetDataf(m_refShowWaypoint);
	m_refShowAirport = findDataRef("ssg/B748/ND/show_airport_pilot");
	m_showAirport = XPLMGetDataf(m_refShowAirport);
	m_refShowData = findDataRef("ssg/B748/ND/show_NDB_pilot");
	m_showData = XPLMGetDataf(m_refShowData);
	m_refShowPos = findDataRef("ssg/B748/ND/show_POS_pilot");
	m_showPos = XPLMGetDataf(m_refShowPos);
	m_refShowTerr = findDataRef("ssg/B748/ND/show_Terr_pilot");
	m_showTerr = XPLMGetDataf(m_refShowTerr);

	m_refTriggerCtr = findDataRef("ssg/B748/ND/CRT_pilot");

	m_refVorAdf1 = findDataRef("ssg/B748/MCP/ap_vor_adf1");
	m_vorAdf1 = XPLMGetDataf(m_refVorAdf1);
	m_refVorAdf2 = findDataRef("ssg/B748/MCP/ap_vor_adf2");
	m_vorAdf2 = XPLMGetDataf(m_refVorAdf2);

	m_refTemp = findDataRef("ssg/B748/MCP/ap_vor_adf1");
	float m_val = XPLMGetDataf(m_refTemp);
	if (m_refTemp == nullptr)
	{
		VLLog("Not Found");
	}
	else
	{
		XPLMDataTypeID id = XPLMGetDataRefTypes(m_refTemp);
		VLLog("Type %u Can write: %d = %f", id,  XPLMCanWriteDataRef(m_refTemp), m_val);
	}
}

SsgB748::~SsgB748()
{
	VLLog("Deconstruct");
	XPLMUnregisterCommandHandler(m_refAltNNNup, ssgCommandCallback, 0, this);
	XPLMUnregisterCommandHandler(m_refAltNNNdn, ssgCommandCallback, 0, this);
	XPLMUnregisterCommandHandler(m_refSpdNNNup, ssgCommandCallback, 0, this);
	XPLMUnregisterCommandHandler(m_refSpdNNNdn, ssgCommandCallback, 0, this);
	XPLMUnregisterCommandHandler(m_refHdgNNNup, ssgCommandCallback, 0, this);
	XPLMUnregisterCommandHandler(m_refHdgNNNdn, ssgCommandCallback, 0, this);
}

int SsgB748::ssgCommandCallback(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
	SsgB748 *plane = static_cast<SsgB748*>(inRefcon);
	if (plane == nullptr)
		return 1;

	std::lock_guard<std::mutex> guard(plane->m_commandCounterMutex);
	if (/*(inCommand == plane->m_refAltNNNup || 
		 inCommand == plane->m_refAltNNNdn ||
		 inCommand == plane->m_refHdgNNNup ||
		 inCommand == plane->m_refHdgNNNdn ||
		 inCommand == plane->m_refSpdNNNup ||
		 inCommand == plane->m_refSpdNNNdn) &&*/ plane->m_displayCommandsScheduled > 0)
	{
		plane->m_displayCommandsScheduled--;
		if (plane->m_displayCommandsScheduled == 0)
			plane->m_displayCommandCooldown = (XPLMGetElapsedTime() + 1.5f);
	}

	return 1;
}

bool SsgB748::hardwareDisplayUpdateAllowed()
{
	std::lock_guard<std::mutex> guard(m_commandCounterMutex);
	return ((m_displayCommandsScheduled == 0) && (m_displayCommandCooldown < XPLMGetElapsedTime()));
}

void SsgB748::updateAltitude(const std::list<BaseDeviceHandler*> &devices)
{
	if (!hardwareDisplayUpdateAllowed())
		return;

	// Only sync from sim to hardware, changing is done through up/down command
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

void SsgB748::updateHeading(const std::list<BaseDeviceHandler*> &devices)
{
	if (!hardwareDisplayUpdateAllowed())
		return;

	// Only sync from sim to hardware, changing is done through up/down command
	float displayed = XPLMGetDataf(m_refApDisplayedHeading);
	if (displayed != m_heading)
	{
		for (auto &it = devices.begin(); it != devices.end(); it++)
		{
			(*it)->displayMcpHeading(displayed);
		}
		m_heading = displayed;
		m_hardwareHeading = displayed;
	}
}


#define COMMAND(a) case BaseDeviceHandler::##a: scheduleCommand(m_ref##a); return true;
#define DISPLAYCOMMAND(a,b) case BaseDeviceHandler::##a: { std::lock_guard<std::mutex> guard(m_commandCounterMutex); m_displayCommandsScheduled+=b; scheduleCommand(m_ref##a, b); return true; }

void SsgB748::relayToggle(XPLMDataRef ref, float &value)
{
	if (value < 1.0)
		value = 1.0;
	else
		value = 0.0;
	XPLMSetDataf(ref, value);
}

bool SsgB748::handleCommand(BaseDeviceHandler::VriCommandParameters command)
{
	switch (command.m_command)
	{
		DISPLAYCOMMAND(AltNNNup, 1);
		DISPLAYCOMMAND(AltNNNdn, 1);
		COMMAND(AptAltSel);

		DISPLAYCOMMAND(HdgNNNup, 2);
		DISPLAYCOMMAND(HdgNNNdn, 2);

		DISPLAYCOMMAND(SpdNNNup, 2);
		DISPLAYCOMMAND(SpdNNNdn, 2);
		COMMAND(AptSpdSel);

	case BaseDeviceHandler::EfisTfc:
		relayToggle(m_refShowTCAS, m_showTCAS);
		return true;

	case BaseDeviceHandler::EfisCtr:
		XPLMSetDataf(m_refTriggerCtr, 1.0);
		return true;

	case BaseDeviceHandler::EfisWx:
		relayToggle(m_refShowWx, m_showWx);
		return true;

	case BaseDeviceHandler::EfisSta:
		relayToggle(m_refShowSta, m_showSta);
		return true;

	case BaseDeviceHandler::EfisWpt:
		relayToggle(m_refShowWaypoint, m_showWaypoint);
		return true;

	case BaseDeviceHandler::EfisTerr:
		relayToggle(m_refShowTerr, m_showTerr);
		return true;

	case BaseDeviceHandler::EfisData:
		relayToggle(m_refShowData, m_showData);
		return true;

	case BaseDeviceHandler::EfisPos:
		relayToggle(m_refShowPos, m_showPos);
		return true;

	case BaseDeviceHandler::EfisArpt:
		relayToggle(m_refShowAirport, m_showAirport);
		return true;

	case BaseDeviceHandler::EfisModeDown:
		if (m_ndMode > 0.0)
		{
			m_ndMode -= 1.0;
			XPLMSetDataf(m_refNdMode, m_ndMode);
		}
		return true;

	case BaseDeviceHandler::EfisModeUp:
		if (m_ndMode < 3.0)
		{
			m_ndMode += 1.0;
			XPLMSetDataf(m_refNdMode, m_ndMode);
		}
		return true;

	case BaseDeviceHandler::EfisZoomIn:
		if (m_ndRange > -3.0)
		{
			m_ndRange -= 1.0;
			XPLMSetDataf(m_refNdRange, m_ndRange);
		}
		return true;

	case BaseDeviceHandler::EfisZoomOut:
		if (m_ndRange < 7.0)
		{
			m_ndRange += 1.0;
			XPLMSetDataf(m_refNdRange, m_ndRange);
		}
		return true;

	case BaseDeviceHandler::EfisVor1Up:
		if (m_vorAdf1 < 1.0)
		{
			m_vorAdf1++;
			XPLMSetDataf(m_refVorAdf1, m_vorAdf1);
		}
		return true;

	case BaseDeviceHandler::EfisVor1Down:
		if (m_vorAdf1 > -1.0)
		{
			m_vorAdf1--;
			XPLMSetDataf(m_refVorAdf1, m_vorAdf1);
		}
		return true;

	case BaseDeviceHandler::EfisVor2Up:
		if (m_vorAdf2 < 1.0)
		{
			m_vorAdf2++;
			XPLMSetDataf(m_refVorAdf2, m_vorAdf2);
		}
		return true;

	case BaseDeviceHandler::EfisVor2Down:
		if (m_vorAdf2 > -1.0)
		{
			m_vorAdf2--;
			XPLMSetDataf(m_refVorAdf2, m_vorAdf2);
		}
		return true;

	case BaseDeviceHandler::AptAtArm:
	case BaseDeviceHandler::AptAtDisarm:
		scheduleCommand(m_refAutoThrottleToggle);
		scheduleCommand(m_refAutoThrottleToggle);
		return true;
	case BaseDeviceHandler::AptFdArm:
	case BaseDeviceHandler::AptFdDisarm:
		scheduleCommand(m_refFlightDirectorTogglePt);
		scheduleCommand(m_refFlightDirectorToggleCp);
		return true;
	case BaseDeviceHandler::AptMasterConnect:
	case BaseDeviceHandler::AptMasterDisconnect:
		scheduleCommand(m_refApMaster);
		return true;

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

bool SsgB748::isLoaded()
{
	return (XPLMFindCommand("SSG/UFMC/AP_LNAV_Button") != nullptr);
}

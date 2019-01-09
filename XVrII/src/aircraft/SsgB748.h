#ifndef SSGB748H
#define SSGB748H

#include "BaseAircraft.h"
#include <mutex>

class SsgB748 : public BaseAircraft
{
public:
	SsgB748();
	virtual ~SsgB748();

	bool handleCommand(BaseDeviceHandler::VriCommand command, float value, bool boost) override;

	static bool isLoaded();

protected:
	//static const char *SSG_B747_ND;
	//static const char *S_B747_EFIS;
	//static const char *LAM_B747_TOGGLE;

	bool hardwareDisplayUpdateAllowed() override;
	void updateAltitude(const std::list<VRiCommPort*> &devices) override;
	void updateHeading(const std::list<VRiCommPort*> &devices) override;
	void relayToggle(XPLMDataRef ref, float &value);

	XPLMCommandRef m_refAltNNNup;
	XPLMCommandRef m_refAltNNNdn;

	XPLMCommandRef m_refHdgNNNup;
	XPLMCommandRef m_refHdgNNNdn;

	XPLMCommandRef m_refSpdNNNup;
	XPLMCommandRef m_refSpdNNNdn;

	//XPLMCommandRef m_refEfisVor1Up;
	//XPLMCommandRef m_refEfisVor1Down;
	//XPLMCommandRef m_refEfisVor2Up;
	//XPLMCommandRef m_refEfisVor2Down;

	//XPLMCommandRef m_refEfisMinsReset;
	//XPLMCommandRef m_refEfisMeters;
	//XPLMCommandRef m_refEfisFpv;

	//XPLMCommandRef m_refEfisWx;
	//XPLMCommandRef m_refEfisSta;
	//XPLMCommandRef m_refEfisWpt;
	//XPLMCommandRef m_refEfisArpt;
	//XPLMCommandRef m_refEfisData;
	//XPLMCommandRef m_refEfisPos;
	//XPLMCommandRef m_refEfisTerr;
	//XPLMCommandRef m_refEfisCtr;
	//XPLMCommandRef m_refEfisTfc;

	XPLMCommandRef m_refAptN1;
	XPLMCommandRef m_refAptSpd;
	XPLMCommandRef m_refAptFlch;
	XPLMCommandRef m_refAptSpdSel;

	XPLMCommandRef m_refAptHdgSel;
	XPLMCommandRef m_refAptHdgHold;
	XPLMCommandRef m_refAptAltHold;
	XPLMCommandRef m_refAptAltSel;
	XPLMCommandRef m_refAptVvsHold;

	XPLMCommandRef m_refAptVNav;
	XPLMCommandRef m_refAptLNav;

	XPLMCommandRef m_refAptCmdA;
	XPLMCommandRef m_refAptCmdB;
	XPLMCommandRef m_refAptCmdC;

	XPLMCommandRef m_refAptLoc;
	XPLMCommandRef m_refAptApp;

	//XPLMDataRef	m_refToggleSwitchPositions;
	//XPLMDataRef	m_refApButtonSwitchPositions;

	XPLMCommandRef m_refAutoThrottleToggle;
	XPLMCommandRef m_refFlightDirectorTogglePt;
	XPLMCommandRef m_refFlightDirectorToggleCp;

	XPLMCommandRef m_refApMaster;

	XPLMDataRef m_refNdRange;
	float m_ndRange;

	XPLMDataRef m_refNdMode;
	float m_ndMode;

	XPLMDataRef m_refShowWx;
	float m_showWx;
	XPLMDataRef m_refShowSta;
	float m_showSta;
	XPLMDataRef m_refShowWaypoint;
	float m_showWaypoint;
	XPLMDataRef m_refShowAirport;
	float m_showAirport;
	XPLMDataRef m_refShowData;
	float m_showData;
	XPLMDataRef m_refShowPos;
	float m_showPos;
	XPLMDataRef m_refShowTerr;
	float m_showTerr;

	XPLMDataRef m_refShowTCAS;
	float m_showTCAS;

	XPLMDataRef m_refVorAdf1;
	float m_vorAdf1;
	XPLMDataRef m_refVorAdf2;
	float m_vorAdf2;



	XPLMDataRef m_refTriggerCtr;

	XPLMDataRef m_refTemp;

private:
	static int ssgCommandCallback(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);
	std::mutex m_commandCounterMutex;
	int m_displayCommandsScheduled;
	float m_displayCommandCooldown;
};

#endif


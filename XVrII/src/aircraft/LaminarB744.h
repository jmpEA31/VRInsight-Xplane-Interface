#ifndef LAMINARB744H
#define LAMINARB744H

#include "BaseAircraft.h"

class LaminarB744 : public BaseAircraft
{
public:
	LaminarB744();

	bool handleCommand(BaseDeviceHandler::VriCommandParameters command) override;

	static bool isLoaded();

protected:
	static const char *LAM_B747_ND;
	static const char *LAM_B747_EFIS;
	static const char *LAM_B747_TOGGLE;

	void updateAltitude(const std::list<BaseDeviceHandler*> &devices) override;
	void updateHeading(const std::list<BaseDeviceHandler*> &devices) override;

	XPLMCommandRef m_refAltNNNup;
	XPLMCommandRef m_refAltNNNdn;

	XPLMCommandRef m_refHdgNNNup;
	XPLMCommandRef m_refHdgNNNdn;

	XPLMCommandRef m_refEfisVor1Up;
	XPLMCommandRef m_refEfisVor1Down;
	XPLMCommandRef m_refEfisVor2Up;
	XPLMCommandRef m_refEfisVor2Down;

	XPLMCommandRef m_refEfisMinsReset;
	XPLMCommandRef m_refEfisMeters;
	XPLMCommandRef m_refEfisFpv;

	XPLMCommandRef m_refEfisWx;
	XPLMCommandRef m_refEfisSta;
	XPLMCommandRef m_refEfisWpt;
	XPLMCommandRef m_refEfisArpt;
	XPLMCommandRef m_refEfisData;
	XPLMCommandRef m_refEfisPos;
	XPLMCommandRef m_refEfisTerr;
	XPLMCommandRef m_refEfisCtr;
	XPLMCommandRef m_refEfisTfc;

	XPLMCommandRef m_refAptN1;
	XPLMCommandRef m_refAptSpd;
	XPLMCommandRef m_refAptFlch;

	XPLMCommandRef m_refAptHdgSel;
	XPLMCommandRef m_refAptHdgHold;
	XPLMCommandRef m_refAptAltHold;
	XPLMCommandRef m_refAptVvsHold;

	XPLMCommandRef m_refAptVNav;
	XPLMCommandRef m_refAptLNav;

	XPLMCommandRef m_refAptCmdA;
	XPLMCommandRef m_refAptCmdB;
	XPLMCommandRef m_refAptCmdC;

	XPLMCommandRef m_refAptLoc;
	XPLMCommandRef m_refAptApp;

	XPLMDataRef	m_refToggleSwitchPositions;
	XPLMDataRef	m_refApButtonSwitchPositions;

	XPLMCommandRef m_refAutoThrottleToggle;
	XPLMCommandRef m_refFlightDirectorToggle;

	XPLMCommandRef m_refApMaster;
};

#endif


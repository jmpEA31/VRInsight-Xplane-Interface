#ifndef LAMINARB738H
#define LAMINARB738H

#include "BaseAircraft.h"

class LaminarB738 : public BaseAircraft
{
public:
	LaminarB738();

	bool handleCommand(BaseDeviceHandler::VriCommandParameters command) override;

	static bool isLoaded();

protected:
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
	XPLMCommandRef m_refAptAltHold;
	XPLMCommandRef m_refAptVvsHold;

	XPLMCommandRef m_refAptVNav;
	XPLMCommandRef m_refAptLNav;
	XPLMCommandRef m_refAptCmdA;
	XPLMCommandRef m_refAptCmdB;
	XPLMCommandRef m_refAptCwsA;
	XPLMCommandRef m_refAptCwsB;
	XPLMCommandRef m_refAptLoc;
	XPLMCommandRef m_refAptApp;

	XPLMCommandRef m_refAutoThrottleToggle;
	XPLMDataRef m_refAutoThrottleStatus;
	XPLMCommandRef m_refFDToggle;
	XPLMDataRef m_refFDStatus;

	XPLMCommandRef m_refApMaster;
	XPLMDataRef	m_refApMasterPos;
};

#endif


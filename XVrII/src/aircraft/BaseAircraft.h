#ifndef BASEAIRCRAFTH
#define BASEAIRCRAFTH

#include <queue>
#include <list>

#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMDataAccess.h>

#include "BaseDeviceHandler.h"

class VRiCommPort;

typedef std::queue<XPLMCommandRef> CommandQueue;

class BaseAircraft
{
public:
	BaseAircraft();
	virtual ~BaseAircraft();

	void updateDisplays(const std::list<BaseDeviceHandler*> &devices);

	virtual bool handleCommand(BaseDeviceHandler::VriCommandParameters command);
	CommandQueue *queuedCommands();

protected:
	static const char *SIM_COCKPIT2_AUTOPILOT;
	static const char *SIM_COCKPIT2_RADIOS;
	static const char *SIM_INSTRUMENTS;

	void scheduleCommand(XPLMCommandRef command, int repeat = 1);

	virtual bool hardwareDisplayUpdateAllowed();
	virtual void updateSpeed(const std::list<BaseDeviceHandler*> &devices);
	virtual void updateHeading(const std::list<BaseDeviceHandler*> &devices);
	virtual void updateAltitude(const std::list<BaseDeviceHandler*> &devices);
	virtual void updateRadios(const std::list<BaseDeviceHandler*> &devices);

	float m_altitude;
	float m_speed;
	float m_heading;

	float m_hardwareAltitude;
	float m_hardwareSpeed;
	float m_hardwareHeading;

	XPLMDataRef findDataRef(const char *prefix, char *reference = nullptr);
	XPLMCommandRef findCommandRef(const char *prefix, char *command = nullptr);

	XPLMDataRef m_refApDisplayedAltitude;
	XPLMDataRef m_refApDisplayedSpeed;
	XPLMDataRef m_refApDisplayedHeading;

	void efisZoomIn();
	void efisZoomOut();
	XPLMCommandRef m_refEfisZoomIn;
	XPLMCommandRef m_refEfisZoomOut;
	int m_efisZoomStepTracker;

	void efisModeUp();
	void efisModeDown();
	void handleEfisMode(bool up);
	XPLMCommandRef m_refEfisModeDown;
	XPLMCommandRef m_refEfisModeUp;
	int m_efisModeStepTracker;

	XPLMCommandRef m_refEfisBaroUp;
	XPLMCommandRef m_refEfisBaroDown;
	XPLMCommandRef m_refEfisBaroReset;

	XPLMCommandRef m_refEfisMinsUp;
	XPLMCommandRef m_refEfisMinsDown;

	XPLMCommandRef m_refAptToGa;

	XPLMCommandRef m_refObsUp;
	XPLMCommandRef m_refObsDown;
	XPLMCommandRef m_refAptVsUp;
	XPLMCommandRef m_refAptVsDown;

	XPLMDataRef m_refComm1;
	int m_hardwareComm1;
	int m_comm1;

	XPLMDataRef m_refComm1Stdby;
	int m_hardwareComm1Stdby;
	int m_comm1Stdby;

	XPLMDataRef m_refComm2;
	int m_hardwareComm2;
	int m_comm2;

	XPLMDataRef m_refComm2Stdby;
	int m_hardwareComm2Stdby;
	int m_comm2Stdby;

	XPLMDataRef m_refNav1;
	int m_hardwareNav1;
	int m_nav1;

	XPLMDataRef m_refNav1Stdby;
	int m_hardwareNav1Stdby;
	int m_nav1Stdby;

	XPLMDataRef m_refNav2;
	int m_hardwareNav2;
	int m_nav2;

	XPLMDataRef m_refNav2Stdby;
	int m_hardwareNav2Stdby;
	int m_nav2Stdby;

	XPLMDataRef m_refDme1Dist;
	XPLMDataRef m_refDme1Ident;
	XPLMDataRef m_refDme1Speed;
	XPLMDataRef m_refDme1Course;
	int m_dme1Dist;
	int m_dme1Speed;
	int m_dme1Course;
	char m_dme1Ident[8];

	XPLMDataRef m_refDme2Dist;
	XPLMDataRef m_refDme2Ident;
	XPLMDataRef m_refDme2Speed;
	XPLMDataRef m_refDme2Course;
	int m_dme2Dist;
	int m_dme2Speed;
	int m_dme2Course;
	char m_dme2Ident[8];

	XPLMDataRef m_refTransponderCode;
	XPLMDataRef m_refTransponderMode;
	XPLMCommandRef m_refTransponderModeUp;
	XPLMCommandRef m_refTransponderModeDown;
	int m_hardwareTransponderCode;
	int m_transponderCode;

private:
	void syncNavDisplay(BaseDeviceHandler::VriRadioDisplay radio, int &hardware, int &synced, XPLMDataRef ref, const std::list<BaseDeviceHandler*> &devices);
	void simulateKey(int keyNumber);

	CommandQueue *m_xpCommands;

	enum RadioMode
	{
		None, RM_Comm1, RM_Comm2, Nav1, Nav2, Dme1, Dme2, Trans
	};

	RadioMode m_radioMode;
	int m_typeResetCounter;
};

#endif

#ifndef BASEDEVICEHANDLERH
#define BASEDEVICEHANDLERH

class BaseAircraft;

class BaseDeviceHandler
{
public:
	BaseDeviceHandler();
	~BaseDeviceHandler();

	enum VriCommand 
	{ 
		None, 

		AptAltHold, AptAltSel, AltNNNup, AltNNNdn,
		AptHdgHold, AptHdgSel, HdgNNNup, HdgNNNdn,
		AptFlch, AptN1, AptSpd, AptSpdSel,  SpdNNNup, SpdNNNdn, SpdNNN, 
		AptVsUp, AptVsDown,

		AptMasterConnect, AptMasterDisconnect, AptAtArm, AptAtDisarm, AptFdArm, AptFdDisarm,
		AptCmdA, AptCmdB, AptCmdC, AptCwsA, AptCwsB, AptToGa,
		AptApp, AptLNav, AptVNav, AptLoc, AptVvsHold,
		

		EfisZoomIn, EfisZoomOut, EfisModeUp, EfisModeDown,
		EfisVor1Up, EfisVor1Down, EfisVor2Up, EfisVor2Down,
		EfisArpt, EfisFpv, EfisWx, EfisSta, EfisWpt, EfisData, EfisPos, EfisTerr,
		EfisBaroUp, EfisBaroDown, EfisBaroReset, EfisMeters,
		EfisMinsUp, EfisMinsDown, EfisMinsReset,
		EfisCtr, EfisTfc, 

		ObsUp, ObsDown,

		ComSel1, ComSel2, ComAux, Com1SNNN, Com1XNNN, Com2SNNN, Com2XNNN,
		NavSel1, NavSel2, NavAux, Nav1SNNN, Nav1XNNN, Nav2SNNN, Nav2XNNN,
		DmeSel1, DmeSel2,
		TrnSel, TrnAux, TrnSNNN, TrnXNNN,

		CtlBn0, CtlBn1, CtlBn2, CtlBn3, CtlBn4, CtlBn5, CtlBn6, CtlBn7
	};

	void parseCommand(char *cmd);
	float value() const;
	bool boost() const;

	void setPlane(BaseAircraft *plane);

	virtual char* identPrefix1() const;
	virtual char* identPrefix2() const;

protected:
	virtual VriCommand parse(char *cmd);
	virtual VriCommand a(char *cmd);
	virtual VriCommand b(char *cmd);
	virtual VriCommand c(char *cmd);
	virtual VriCommand d(char *cmd);
	virtual VriCommand e(char *cmd);
	virtual VriCommand f(char *cmd);
	virtual VriCommand g(char *cmd);
	virtual VriCommand h(char *cmd);
	virtual VriCommand i(char *cmd);
	virtual VriCommand j(char *cmd);
	virtual VriCommand k(char *cmd);
	virtual VriCommand l(char *cmd);
	virtual VriCommand m(char *cmd);
	virtual VriCommand n(char *cmd);
	virtual VriCommand o(char *cmd);
	virtual VriCommand p(char *cmd);
	virtual VriCommand q(char *cmd);
	virtual VriCommand r(char *cmd);
	virtual VriCommand s(char *cmd);
	virtual VriCommand t(char *cmd);
	virtual VriCommand u(char *cmd);
	virtual VriCommand v(char *cmd);
	virtual VriCommand w(char *cmd);
	virtual VriCommand x(char *cmd);
	virtual VriCommand y(char *cmd);
	virtual VriCommand z(char *cmd);

	float toFloat(char* cmd, int start, int end);

	float m_value;
	bool m_boost;

private:
	BaseAircraft *m_plane;
};

#endif

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

	struct VriCommandParameters
	{
		VriCommand m_command;
		bool m_boosted;
		float m_value;
	};

	void parseCommand(char *message);

	void setPlane(BaseAircraft *plane);

	virtual char* identPrefix1() const;
	virtual char* identPrefix2() const;

protected:
	virtual VriCommandParameters parse(char *message);
	virtual VriCommandParameters a(char *message, VriCommandParameters &command);
	virtual VriCommandParameters b(char *message, VriCommandParameters &command);
	virtual VriCommandParameters c(char *message, VriCommandParameters &command);
	virtual VriCommandParameters d(char *message, VriCommandParameters &command);
	virtual VriCommandParameters e(char *message, VriCommandParameters &command);
	virtual VriCommandParameters f(char *message, VriCommandParameters &command);
	virtual VriCommandParameters g(char *message, VriCommandParameters &command);
	virtual VriCommandParameters h(char *message, VriCommandParameters &command);
	virtual VriCommandParameters i(char *message, VriCommandParameters &command);
	virtual VriCommandParameters j(char *message, VriCommandParameters &command);
	virtual VriCommandParameters k(char *message, VriCommandParameters &command);
	virtual VriCommandParameters l(char *message, VriCommandParameters &command);
	virtual VriCommandParameters m(char *message, VriCommandParameters &command);
	virtual VriCommandParameters n(char *message, VriCommandParameters &command);
	virtual VriCommandParameters o(char *message, VriCommandParameters &command);
	virtual VriCommandParameters p(char *message, VriCommandParameters &command);
	virtual VriCommandParameters q(char *message, VriCommandParameters &command);
	virtual VriCommandParameters r(char *message, VriCommandParameters &command);
	virtual VriCommandParameters s(char *message, VriCommandParameters &command);
	virtual VriCommandParameters t(char *message, VriCommandParameters &command);
	virtual VriCommandParameters u(char *message, VriCommandParameters &command);
	virtual VriCommandParameters v(char *message, VriCommandParameters &command);
	virtual VriCommandParameters w(char *message, VriCommandParameters &command);
	virtual VriCommandParameters x(char *message, VriCommandParameters &command);
	virtual VriCommandParameters y(char *message, VriCommandParameters &command);
	virtual VriCommandParameters z(char *message, VriCommandParameters &command);

	float toFloat(char* cmd, int start, int end);

private:
	BaseAircraft *m_plane;
};

#endif

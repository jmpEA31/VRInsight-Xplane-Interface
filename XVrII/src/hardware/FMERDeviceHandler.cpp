#include "FMERDeviceHandler.h"

#include <XPLM/XPLMUtilities.h>

FMERDeviceHandler::FMERDeviceHandler()
	: BaseDeviceHandler()
{

}

FMERDeviceHandler::~FMERDeviceHandler()
{
}

char* FMERDeviceHandler::identPrefix1() const
{
	return "DSP0";
}

char* FMERDeviceHandler::identPrefix2() const
{
	return "DSP1";
}

//--------------------------------------------------
// Parse a message from the hardware starting with A
//--------------------------------------------------
BaseDeviceHandler::VriCommand FMERDeviceHandler::a(char *cmd)
{
	if (strncmp("ALT", cmd, 3) == 0)
	{
		// Handle ALT... messages
		char *subcmd = &cmd[3];

		if (strncmp("HLD", subcmd, 3) == 0) return AptAltHold;
		if (strncmp("SEL", subcmd, 3) == 0) return AptAltSel;
		else if (cmd[3] >= '0' && cmd[3] <= '9')
		{
			m_value = 100.0f * toFloat(cmd, 3, 5);
			return cmd[6] == '+' ? AltNNNup : AltNNNdn;
		}
	}
	else if (strncmp("APT", cmd, 3) == 0)
	{
		// Handle APT... messages
		char *subcmd = &cmd[3];

		if (strncmp("AT+", subcmd, 3) == 0) return AptAtArm;
		if (strncmp("AT-", subcmd, 3) == 0) return AptAtDisarm;
		if (strncmp("APP", subcmd, 3) == 0) return AptApp;					// combines APP+ and APP-
		if (strncmp("CMDA", subcmd, 4) == 0) return AptCmdA;
		if (strncmp("CMDB", subcmd, 4) == 0) return AptCmdB;
		if (strncmp("CMDC", subcmd, 4) == 0) return AptCmdC;
		if (strncmp("CWSA", subcmd, 4) == 0) return AptCwsA;
		if (strncmp("CWSB", subcmd, 4) == 0) return AptCwsB;
		if (strncmp("FD+", subcmd, 3) == 0) return AptFdArm;
		if (strncmp("FD-", subcmd, 3) == 0) return AptFdDisarm;
		if (strncmp("LOC", subcmd, 3) == 0) return AptLoc;					// combines APP+ and APP-
		if (strncmp("LNAV", subcmd, 4) == 0) return AptLNav;				// combines LNAV+ and LNAV-
		if (strncmp("MAST+", subcmd, 5) == 0) return AptMasterConnect;
		if (strncmp("MAST-", subcmd, 5) == 0) return AptMasterDisconnect;
		if (strncmp("TOGA", subcmd, 4) == 0) return AptToGa;				// combines TOGA+, TOGA-, TOGN+, TOGN-
		if (strncmp("VNAV", subcmd, 4) == 0) return AptVNav;				// combines VNAV+ and VNAV-
	}
	
	return BaseDeviceHandler::a(cmd);
}

//---------------------------------------------------
// Parse a message from the hardware starting with B
//---------------------------------------------------
BaseDeviceHandler::VriCommand FMERDeviceHandler::b(char *cmd)
{
	if (strncmp("BAR", cmd, 3) == 0)
	{
		// Handle BAR... messages
		char *subcmd = &cmd[3];
		// bool boost = (subcmd[1] == '+' || cmd[1] == '-'); -> BAR++/BAR-- treated as BAR+/BAR- for now

		switch (subcmd[0])
		{
		case '+':
			return EfisBaroUp;
		case '-':
			return EfisBaroDown;
		case 'S':
			return EfisBaroReset;
		default:
			break;
		}
	}
	return BaseDeviceHandler::b(cmd);
}

//---------------------------------------------------
// Parse a message from the hardware starting with C
//---------------------------------------------------
BaseDeviceHandler::VriCommand FMERDeviceHandler::c(char *cmd)
{
	if (strncmp("COM", cmd, 3) == 0)
	{
		// Handle COM... messages
		char *subcmd = &cmd[3];

		if (strncmp("AUX", subcmd, 3) == 0) return ComAux;
		if (strncmp("SEL1", subcmd, 4) == 0) return ComSel1;
		if (strncmp("SEL2", subcmd, 4) == 0) return ComSel2;

		switch (subcmd[0])
		{
		case 's':
			m_value = 10000 + toFloat(cmd, 4, 7);
			return Com1SNNN;
		case 'x':
			m_value = 10000 + toFloat(cmd, 4, 7);
			return Com1XNNN;
		case 'S':
			m_value = 10000 + toFloat(cmd, 4, 7);
			return Com2SNNN;
		case 'X':
			m_value = 10000 + toFloat(cmd, 4, 7);
			return Com2XNNN;
		default:
			break;
		}
	}
	return BaseDeviceHandler::c(cmd);
}

//---------------------------------------------------
// Parse a message from the hardware starting with D
//---------------------------------------------------
BaseDeviceHandler::VriCommand FMERDeviceHandler::d(char *cmd)
{
	if (strncmp("DME", cmd, 3) == 0)
	{
		// Handle DME... messages
		char *subcmd = &cmd[3];

		if (strncmp("SEL1", subcmd, 4) == 0) return DmeSel1;
		if (strncmp("SEL2", subcmd, 4) == 0) return DmeSel2;
	}

	return BaseDeviceHandler::d(cmd);
}


//---------------------------------------------------
// Parse a message from the hardware starting with E
//---------------------------------------------------
BaseDeviceHandler::VriCommand FMERDeviceHandler::e(char *cmd)
{
	if (strncmp("EFI", cmd, 3) == 0)
	{
		// Handle EFI... messages
		char *subcmd = &cmd[3];

		if (strncmp("ADF1", subcmd, 4) == 0) return EfisVor1Down;
		if (strncmp("ADF2", subcmd, 4) == 0) return EfisVor2Down;
		if (strncmp("ARPT", subcmd, 4) == 0) return EfisArpt;
		if (strncmp("DATA", subcmd, 4) == 0) return EfisData;
		if (strncmp("FPV", subcmd, 3) == 0) return EfisFpv;
		if (strncmp("MTRS", subcmd, 4) == 0) return EfisMeters;
		if (strncmp("POS", subcmd, 3) == 0) return EfisPos;
		if (strncmp("STA", subcmd, 3) == 0) return EfisSta;
		if (strncmp("TERR", subcmd, 4) == 0) return EfisTerr;
		if (strncmp("VOR1", subcmd, 4) == 0) return EfisVor1Up;
		if (strncmp("VOR2", subcmd, 4) == 0) return EfisVor2Up;
		if (strncmp("WPT", subcmd, 3) == 0) return EfisWpt;
		if (strncmp("WX", subcmd, 2) == 0) return EfisWx;
	}

	return BaseDeviceHandler::e(cmd);
}

BaseDeviceHandler::VriCommand FMERDeviceHandler::h(char *cmd)
{
	switch (cmd[4])
	{
	case 'D':				// HDGHDG
		return AptHdgSel;
	case 'L':				// HDGHLD
		return AptHdgHold;
	default:				// HDGnnn
		{
			if (cmd[3] >= '0' && cmd[3] <= '9')
			{
				m_value = (float)(((cmd[3] - '0') * 10) + (cmd[4] - '0')) * 10 + (cmd[5] - '0');
				return cmd[6] == '+' ? HdgNNNup : HdgNNNdn;

			}
			else
				return None;
		}
	}
}

BaseDeviceHandler::VriCommand FMERDeviceHandler::m(char *cmd)
{
	switch (cmd[3])
	{
	case '+':
		return EfisMinsUp;
	case '-':
		return EfisMinsDown;
	case 'S':
		return EfisMinsReset;
	default:
		return None;
	}
}

BaseDeviceHandler::VriCommand FMERDeviceHandler::n(char *cmd)
{
	if (cmd[1] == 'A') // NA....
	{
		if (cmd[4] == 'E') // NAVSEL1/NAVSEL2
		{
			return cmd[6] == '1' ? NavSel1 : NavSel2;
		}
		switch (cmd[3])
		{
		case 's':
			m_value = 10000 + toFloat(cmd,4,7);
			return Nav1SNNN;
		case 'x':
			m_value = 10000 + toFloat(cmd,4,7);
			return Nav1XNNN;
		case 'S':
			m_value = 10000 + toFloat(cmd,4,7);
			return Nav2SNNN;
		case 'X':
			m_value = 10000 + toFloat(cmd,4,7);
			return Nav2XNNN;
		case 'A':
			return NavAux;
		default:
			return None;
		}
	}

	switch (cmd[2])
	{
	case 'M':
		if (cmd[3] == '+')
			return EfisModeUp;
		else if (cmd[3] == '-')
			return EfisModeDown;
		else
			return EfisCtr;
	case 'R':
		if (cmd[3] == '+')
			return EfisZoomOut;
		else if (cmd[3] == '-')
			return EfisZoomIn;
		else
			return EfisTfc;
	default:
		return None;
	}
}

//---------------------------------------------------
// Parse a message from the hardware starting with O
//---------------------------------------------------
BaseDeviceHandler::VriCommand FMERDeviceHandler::o(char *cmd)
{
	if (strncmp("OBS", cmd, 3) == 0)
	{
		// Handle EFI... messages
		char *subcmd = &cmd[3];

		if (strncmp("+", subcmd, 1) == 0) return ObsUp;
		if (strncmp("-", subcmd, 1) == 0) return ObsDown;
	}

	return BaseDeviceHandler::o(cmd);
}


BaseDeviceHandler::VriCommand FMERDeviceHandler::s(char *cmd)
{
	switch (cmd[3])
	{
	case 'L':
		return AptFlch;
	case 'N':
		return AptN1;
	case 'S':
		if (cmd[4] == 'E')
			return AptSpdSel;
		else
			return AptSpd;
	default:
		{
			if (cmd[3] >= '0' && cmd[3] <= '9')
			{
				m_value = (float)(((cmd[3] - '0') * 10) + (cmd[4] - '0')) * 10 + (cmd[5] - '0');
				switch (cmd[6])
				{
				case '-':
					return SpdNNNdn;
				case '+':
					return SpdNNNup;
				default:
					return SpdNNN;
				}
			}
			else
				return None;
		}
	}
}

BaseDeviceHandler::VriCommand FMERDeviceHandler::t(char *cmd)
{
	switch (cmd[4])
	{
	case 'E':
		return TrnSel;
	case 'U':
		return TrnAux;
	}

	switch (cmd[3])
	{
	case 'S':
		m_value =  toFloat(cmd,4,7);
		return TrnSNNN;
	case 'X':
		m_value =  toFloat(cmd,4,7);
		return TrnXNNN;
	}

	return None;
}

BaseDeviceHandler::VriCommand FMERDeviceHandler::v(char *cmd)
{
	m_boost = (cmd[4] == '-' || cmd[4] == '+');
	switch (cmd[3])
	{
	case 'H':
		return AptVvsHold;
	case '-':
		return AptVsDown;
	case '+':
		return AptVsUp;
	default:
		return None;
	}
}
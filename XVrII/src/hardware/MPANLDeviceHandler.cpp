#include "MPANLDeviceHandler.h"

#include <XPLM/XPLMUtilities.h>

MPANLDeviceHandler::MPANLDeviceHandler(VRiCommPort *commPort)
	: BaseDeviceHandler(commPort)
{

}

MPANLDeviceHandler::~MPANLDeviceHandler()
{
}

char* MPANLDeviceHandler::identPrefix1() const
{
	return "DSP4";
}

char* MPANLDeviceHandler::identPrefix2() const
{
	return "DSP5";
}


/*
VRiCmdParser::VriCommandParameters FMERDeviceHandler::a(char *cmd)
{
	switch (cmd[1])
	{
		case 'L':
		{
			switch (cmd[3])
			{
				case 'H':	// ALTHLD+/-
					return AptAltHold;
				case 'S':	// ALTSEL+/-
					return AptAltSel;
				default:	// ALT###+/-
				{
					if (cmd[3] >= '0' && cmd[3] <= '9')
					{
						m_value = 100.0f * ((float)(((cmd[3] - '0') * 10) + (cmd[4] - '0')) * 10 + (cmd[5] - '0'));
						return cmd[6] == '+' ? AltNNNup : AltNNNdn;
					}
					else
						return None;
				}
			}
		}
		case 'P':
		{
			switch (cmd[3])
			{
				case 'A':
					if (cmd[4] == 'T')	// APTAT+/-
						if (cmd[5] == '+')
							return AptAtArm;
						else
							return AptAtDisarm;
					else				// APTAPP+/-
						return AptApp;
				case 'C':
					if (cmd[4] == 'M')
					{
						switch (cmd[6])	// APTCMDA/B/C
						{
						case 'A':
							return AptCmdA;
						case 'B':
							return AptCmdB;
						case 'C':
							return AptCmdC;
						}
					}
					else
					{
						switch (cmd[6])	// APTCWSA/B
						{
						case 'A':
							return AptCwsA;
						case 'B':
							return AptCwsB;
						}
					}
					break;
				case 'F':
					if (cmd[5] == '+')	// APTFD+/-
						return AptFdArm;
					else
						return AptFdDisarm;
				case 'L':
					if (cmd[4] == 'O')	// APTLOC+/-
						return AptLoc;
					else				// APTLNAV+/-
						return AptLNav;
				case 'M':
					if (cmd[7] == '+')	// APTMAST+/-
						return AptMasterConnect;
					else
						return AptMasterDisconnect;
				case 'T':				// APTTOGA/N+/- ??
					return AptToGa;
				case 'V':				// APTVNAV+/-
					return AptVNav;
			}		
		}
		default:
			return None;
	}
}

VRiCmdParser::VriCommandParameters FMERDeviceHandler::b(char *cmd)
{
	bool boost = (cmd[4] == '+' || cmd[4] =='-');
	switch (cmd[3])
	{
	case '+':
		return EfisBaroUp;
	case '-':
		return EfisBaroDown;
	case 'S':
		return EfisBaroReset;
	default:
		return None;
	}
}

VRiCmdParser::VriCommandParameters FMERDeviceHandler::d(char *cmd)
{
	switch (cmd[3])
	{
	case 'S':
		return (cmd[6] == '1' ? DmeSel1 : DmeSel2);
	default:
		return None;
	}
}

VRiCmdParser::VriCommandParameters FMERDeviceHandler::c(char *cmd)
{
	switch (cmd[1])
	{
	case 'O':
		if (cmd[4] == 'E')
		{
			return cmd[6] == '1' ? ComSel1 : ComSel2;
		}
		
		switch (cmd[3])
		{
		case 's':
			m_value = 10000 + toFloat(cmd,4,7);
			return Com1SNNN;
		case 'x':
			m_value = 10000 + toFloat(cmd,4,7);
			return Com1XNNN;
		case 'S':
			m_value = 10000 + toFloat(cmd,4,7);
			return Com2SNNN;
		case 'X':
			m_value = 10000 + toFloat(cmd,4,7);
			return Com2XNNN;
		case 'A':
			return ComAux;
		default:
			return None;
		}
		break;
	case 'T':
		switch (cmd[5])
		{
		case '0':
			return CtlBn0;
		case '1':
			return CtlBn1;
		case '2':
			return CtlBn2;
		case '3':
			return CtlBn3;
		case '4':
			return CtlBn4;
		case '5':
			return CtlBn5;
		case '6':
			return CtlBn6;
		case '7':
			return CtlBn7;
		default:
			return None;
		}
	default:
		return None;
	}
}

VRiCmdParser::VriCommandParameters FMERDeviceHandler::e(char *cmd)
{
	switch (cmd[3])
	{
	case 'A':
		if (cmd[6] == '1')				// EFIADF1
			return EfisVor1Down;
		else if (cmd[6] == '2')         // EFIADF2
			return EfisVor2Down;		
		else							// EFIARPT
			return EfisArpt;
	case 'D':
		return EfisData;
	case 'F':
		return EfisFpv;
	case 'M':
		return EfisMeters;
	case 'P':
		return EfisPos;
	case 'S':
		return EfisSta;
	case 'T':
		return EfisTerr;
	case 'V':
		if (cmd[6] == '1')				// EFIVOR1
			return EfisVor1Up;
		else							// EFIVOR2
			return EfisVor2Up;
	case 'W':
		if (cmd[4] == 'X')
			return EfisWx;
		else
			return EfisWpt;
	default:
		return None;
	}
	
}

VRiCmdParser::VriCommandParameters FMERDeviceHandler::h(char *cmd)
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

VRiCmdParser::VriCommandParameters FMERDeviceHandler::m(char *cmd)
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

VRiCmdParser::VriCommandParameters FMERDeviceHandler::n(char *cmd)
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

VRiCmdParser::VriCommandParameters FMERDeviceHandler::o(char *cmd)
{
	switch (cmd[3])
	{
	case '+':
		return ObsUp;
	case '-':
		return ObsDown;
	default:
		return None;
	}
}


VRiCmdParser::VriCommandParameters FMERDeviceHandler::s(char *cmd)
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

VRiCmdParser::VriCommandParameters FMERDeviceHandler::t(char *cmd)
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

VRiCmdParser::VriCommandParameters FMERDeviceHandler::v(char *cmd)
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
*/
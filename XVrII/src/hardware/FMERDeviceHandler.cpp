#include "FMERDeviceHandler.h"

#include <logger.h>
#include <XPLM/XPLMUtilities.h>

FMERDeviceHandler::FMERDeviceHandler(VRiCommPort *commPort)
	: BaseDeviceHandler(commPort)
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
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::a(char *message, VriCommandParameters &command)
{
	if (strncmp("ADF", message, 3) == 0)
	{
		// Handle ADF... messages
		char *subcmd = &message[3];

		if (strncmp("SEL1", subcmd, 3) == 0) command.m_command = AdfSel1;
		else if (strncmp("SEL2", subcmd, 3) == 0) command.m_command = AdfSel2;
	}
	else if (strncmp("ALT", message, 3) == 0)
	{
		// Handle ALT... messages
		char *subcmd = &message[3];

		if (strncmp("HLD", subcmd, 3) == 0) command.m_command = AptAltHold;
		else if (strncmp("SEL", subcmd, 3) == 0) command.m_command = AptAltSel;
		else if (message[3] >= '0' && message[3] <= '9')
		{
			command.m_value = 100.0f * toFloat(message, 3, 5);
			command.m_command = (message[6] == '+' ? AltNNNup : AltNNNdn);
		}
	}
	else if (strncmp("APL", message, 3) == 0)
	{
		// Handle APL... messages
		char *subcmd = &message[3];

		if (strncmp("AT+", subcmd, 3) == 0) command.m_command = AptAtArm;
		else if (strncmp("AT-", subcmd, 3) == 0) command.m_command = AptAtDisarm;
		else if (strncmp("APP", subcmd, 3) == 0) command.m_command = AptApp;				// combines APP+ and APP-
		else if (strncmp("CMDA", subcmd, 4) == 0) command.m_command = AptCmdA;
		else if (strncmp("CMDB", subcmd, 4) == 0) command.m_command = AptCmdB;
		else if (strncmp("CMDC", subcmd, 4) == 0) command.m_command = AptCmdC;
		else if (strncmp("CWSA", subcmd, 4) == 0) command.m_command = AptCwsA;
		else if (strncmp("CWSB", subcmd, 4) == 0) command.m_command = AptCwsB;
		else if (strncmp("FD+", subcmd, 3) == 0) command.m_command = AptFdArm;
		else if (strncmp("FD-", subcmd, 3) == 0) command.m_command = AptFdDisarm;
		else if (strncmp("LOC", subcmd, 3) == 0) command.m_command = AptLoc;				// combines APP+ and APP-
		else if (strncmp("LNAV", subcmd, 4) == 0) command.m_command = AptLNav;				// combines LNAV+ and LNAV-
		else if (strncmp("MAST+", subcmd, 5) == 0) command.m_command = AptMasterConnect;
		else if (strncmp("MAST-", subcmd, 5) == 0) command.m_command = AptMasterDisconnect;
		else if (strncmp("TOGA", subcmd, 4) == 0) command.m_command = AptToGa;				// combines TOGA+, TOGA-, TOGN+, TOGN-
		else if (strncmp("VNAV", subcmd, 4) == 0) command.m_command = AptVNav;				// combines VNAV+ and VNAV-
	}
	
	return (command.m_command == None ? BaseDeviceHandler::a(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with B
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::b(char *message, VriCommandParameters &command)
{
	if (strncmp("BAR", message, 3) == 0)
	{
		// Handle BAR... messages
		char *subcmd = &message[3];
		command.m_boosted = (subcmd[1] == '+' || subcmd[1] == '-');

		switch (subcmd[0])
		{
		case '+':
			command.m_command = EfisBaroUp;
			break;
		case '-':
			command.m_command = EfisBaroDown;
			break;
		case 'S':
			command.m_command = EfisBaroReset;
			break;
		default:
			break;
		}
	}

	return (command.m_command == None ? BaseDeviceHandler::b(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with C
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::c(char *message, VriCommandParameters &command)
{
	if (strncmp("COM", message, 3) == 0)
	{
		// Handle COM... messages
		char *subcmd = &message[3];

		if (strncmp("AUX", subcmd, 3) == 0) command.m_command = ComAux;
		else if (strncmp("SEL1", subcmd, 4) == 0) command.m_command = ComSel1;
		else if (strncmp("SEL2", subcmd, 4) == 0) command.m_command = ComSel2;
		else
		{
			switch (subcmd[0])
			{
			case 's':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Com1SNNN;
				break;
			case 'x':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Com1XNNN;
				break;
			case 'S':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Com2SNNN;
				break;
			case 'X':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Com2XNNN;
			default:
				break;
			}
		}
	}
	return (command.m_command == None ? BaseDeviceHandler::c(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with D
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::d(char *message, VriCommandParameters &command)
{
	if (strncmp("DME", message, 3) == 0)
	{
		// Handle DME... messages
		char *subcmd = &message[3];

		if (strncmp("SEL1", subcmd, 4) == 0) command.m_command = DmeSel1;
		else if (strncmp("SEL2", subcmd, 4) == 0) command.m_command = DmeSel2;
	}

	return (command.m_command == None ? BaseDeviceHandler::d(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with E
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::e(char *message, VriCommandParameters &command)
{
	if (strncmp("EFI", message, 3) == 0)
	{
		// Handle EFI... messages
		char *subcmd = &message[3];

		if (strncmp("ADF1", subcmd, 4) == 0) command.m_command = EfisVor1Down;
		else if (strncmp("ADF2", subcmd, 4) == 0) command.m_command = EfisVor2Down;
		else if (strncmp("ARPT", subcmd, 4) == 0) command.m_command = EfisArpt;
		else if (strncmp("DATA", subcmd, 4) == 0) command.m_command = EfisData;
		else if (strncmp("FPV", subcmd, 3) == 0) command.m_command = EfisFpv;
		else if (strncmp("MTR", subcmd, 3) == 0) command.m_command = EfisMeters;
		else if (strncmp("POS", subcmd, 3) == 0) command.m_command = EfisPos;
		else if (strncmp("STA", subcmd, 3) == 0) command.m_command = EfisSta;
		else if (strncmp("TERR", subcmd, 4) == 0) command.m_command = EfisTerr;
		else if (strncmp("VOR1", subcmd, 4) == 0) command.m_command = EfisVor1Up;
		else if (strncmp("VOR2", subcmd, 4) == 0) command.m_command = EfisVor2Up;
		else if (strncmp("WPT", subcmd, 3) == 0) command.m_command = EfisWpt;
		else if (strncmp("WX", subcmd, 2) == 0) command.m_command = EfisWx;
	}

	return (command.m_command == None ? BaseDeviceHandler::e(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with H
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::h(char *message, VriCommandParameters &command)
{
	if (strncmp("HDG", message, 3) == 0)
	{
		// Handle HDG... messages
		char *subcmd = &message[3];

		if (strncmp("HDG", subcmd, 3) == 0) command.m_command = AptHdgSel;
		else if (strncmp("HLD", subcmd, 3) == 0) command.m_command = AptHdgHold;
		else if (subcmd[0] >= '0' && subcmd[0] <= '9')
		{
			command.m_value = toFloat(subcmd, 0, 2);
			command.m_command = (subcmd[3] == '+' ? HdgNNNup : HdgNNNdn);
		}
	}

	return (command.m_command == None ? BaseDeviceHandler::h(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with M
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::m(char *message, VriCommandParameters &command)
{
	if (strncmp("MIN", message, 3) == 0)
	{
		// Handle MIN... messages
		char *subcmd = &message[3];
		command.m_boosted = (subcmd[1] == '+' || subcmd[1] == '-');

		switch (subcmd[0])
		{
		case '+':	// MIN+/++
			command.m_command = EfisMinsUp;
			break;
		case '-':	// MIN-/--
			command.m_command = EfisMinsDown;
			break;
		case 'S':	// MINSEL+/-
			command.m_command = EfisMinsReset;
			break;
		default:
			break;
		}
	}

	return (command.m_command == None ? BaseDeviceHandler::m(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with N
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::n(char *message, VriCommandParameters &command)
{
	if (strncmp("NAV", message, 3) == 0)
	{
		// Handle NAV... messages
		char *subcmd = &message[3];

		if (strncmp("AUX", subcmd, 3) == 0) command.m_command = NavAux;
		else if (strncmp("SEL1", subcmd, 4) == 0) command.m_command = NavSel1;
		else if (strncmp("SEL2", subcmd, 4) == 0) command.m_command = NavSel2;
		else
		{
			switch (subcmd[0])
			{
			case 's':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Nav1SNNN;
				break;
			case 'x':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Nav1XNNN;
				break;
			case 'S':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Nav2SNNN;
				break;
			case 'X':
				command.m_value = 10000 + toFloat(message, 4, 7);
				command.m_command = Nav2XNNN;
				break;
			default:
				break;
			}
		}
	}
	else if (strncmp("NDM", message, 3) == 0)
	{
		// Handle NDM... messages
		char *subcmd = &message[3];
		switch (subcmd[0])
		{
		case '+':
			command.m_command = EfisModeUp;
			break;
		case '-':
			command.m_command = EfisModeDown;
			break;
		case 'S':
			command.m_command = EfisCtr;
			break;
		default:
			break;
		}
	}
	else if (strncmp("NDR", message, 3) == 0)
	{
		// Handle NDR... messages
		char *subcmd = &message[3];
		switch (subcmd[0])
		{
		case '+':
			command.m_command = EfisZoomOut;
			break;
		case '-':
			command.m_command = EfisZoomIn;
			break;
		case 'S':
			command.m_command = EfisTfc;
			break;
		default:
			break;
		}
	}

	return (command.m_command == None ? BaseDeviceHandler::n(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with O
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::o(char *message, VriCommandParameters &command)
{
	if (strncmp("OBS", message, 3) == 0)
	{
		// Handle OBS... messages
		char *subcmd = &message[3];

		if (strncmp("+", subcmd, 1) == 0) command.m_command = ObsUp;
		else if (strncmp("-", subcmd, 1) == 0) command.m_command = ObsDown;
	}

	return (command.m_command == None ? BaseDeviceHandler::o(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with S
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::s(char *message, VriCommandParameters &command)
{
	switch (message[3])
	{
	case 'L':
		command.m_command = AptFlch;
		break;
	case 'N':
		command.m_command = AptN1;
		break;
	case 'S':
		if (message[4] == 'E')
			command.m_command = AptSpdSel;
		else
			command.m_command = AptSpd;
		break;
	default:
		{
			if (message[3] >= '0' && message[3] <= '9')
			{
				command.m_value = (float)(((message[3] - '0') * 10) + (message[4] - '0')) * 10 + (message[5] - '0');
				switch (message[6])
				{
				case '-':
					command.m_command = SpdNNNdn;
					break;
				case '+':
					command.m_command = SpdNNNup;
					break;
				default:
					command.m_command = SpdNNN;
					break;
				}
			}
			else
				break;
		}
	}

	return (command.m_command == None ? BaseDeviceHandler::s(message, command) : command);
}

//---------------------------------------------------
// Parse a message from the hardware starting with T
//---------------------------------------------------
BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::t(char *message, VriCommandParameters &command)
{
	if (strncmp("TRN", message, 3) == 0)
	{
		// Handle TRN... messages
		char *subcmd = &message[3];
		if (strncmp("SEL", subcmd, 3) == 0) command.m_command = TrnSel;
		else if (strncmp("AUX", subcmd, 3) == 0) command.m_command = TrnAux;
		else
		{
			switch (subcmd[0])
			{
			case 'S':
				command.m_value = toFloat(subcmd, 1, 4);
				command.m_command = TrnSNNN;
				break;
			case 'X':
				command.m_value = toFloat(subcmd, 1, 4);
				command.m_command = TrnXNNN;
				break;
			}
		}
	}

	return (command.m_command == None ? BaseDeviceHandler::t(message, command) : command);
}

BaseDeviceHandler::VriCommandParameters FMERDeviceHandler::v(char *message, VriCommandParameters &command)
{
	command.m_boosted = (message[4] == '-' || message[4] == '+');
	switch (message[3])
	{
	case 'H':
		command.m_command = AptVvsHold;
		break;
	case '-':
		command.m_command = AptVsDown;
		break;
	case '+':
		command.m_command = AptVsUp;
		break;
	default:
		break;
	}

	return (command.m_command == None ? BaseDeviceHandler::v(message, command) : command);
}
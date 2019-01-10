#include <mutex>
#include <sstream>

#include "BaseDeviceHandler.h"
#include "BaseAircraft.h"
#include "VRiCommPort.h"

#include "logger.h"

#include <XPLM/XPLMUtilities.h>

static std::mutex g_aircraftMutex;

BaseDeviceHandler::BaseDeviceHandler(VRiCommPort *commPort)
	: m_commPort(commPort)
	, m_aircraft(nullptr)
{
}

BaseDeviceHandler::~BaseDeviceHandler()
{
	if (m_aircraft != nullptr)
		m_aircraft = nullptr;

	if (m_commPort != nullptr)
	{
		delete m_commPort;
		m_commPort = nullptr;
	}
}

void BaseDeviceHandler::setAircraft(BaseAircraft *aircraft)
{
	std::lock_guard<std::mutex> guard(g_aircraftMutex);
	m_aircraft = aircraft;
}

void BaseDeviceHandler::parseCommand(char *message)
{
	VriCommandParameters command = parse(message);
	bool handled = false;
	{
		std::lock_guard<std::mutex> guard(g_aircraftMutex);
		if (command.m_command != None && m_aircraft != nullptr)
		{
			handled = m_aircraft->handleCommand(command);
		}	
	}

	if (!handled)
	{
		VLLog("%s translated to internal command #%d and was %s by the aircraft", message, command.m_command, (handled ? "handled" : "UNHANDLED"));
	}
}


void BaseDeviceHandler::displayIdent1(char *display)
{
	if (identPrefix1() == nullptr)
		return;
	
	char szCommand[9];
	strncpy(szCommand, identPrefix1(), 8);
	strncat(szCommand, display, 8);
	m_commPort->send(szCommand);
}

void BaseDeviceHandler::displayIdent2(char *display)
{
	if (identPrefix2() == nullptr)
		return;

	char szCommand[9];
	strncpy(szCommand, identPrefix2(), 8);
	strncat(szCommand, display, 8);
	m_commPort->send(szCommand);
}

void BaseDeviceHandler::displayMcpSpeed(float speed, bool isMach)
{
	char command[16];
	sprintf(command, "SPD%03.0f\0\0", speed);
	m_commPort->send(command);
}

void BaseDeviceHandler::displayMcpHeading(float heading)
{
	char command[16];
	sprintf(command, "HDG%03.0f\0\0", heading);
	m_commPort->send(command);
}

void BaseDeviceHandler::displayMcpAltitude(float altitude)
{
	char command[16];
	sprintf(command, "ALT%03.0f\0\0", (altitude / 100));
	m_commPort->send(command);
}

void BaseDeviceHandler::displayRadio(VriRadioDisplay radioDisplay, int frequency)
{
	char command[16];
	switch (radioDisplay)
	{
	case DSP_Com1:
		sprintf(command, "COMx%04d", frequency);
		break;
	case Com1Standby:
		sprintf(command, "COMs%04d", frequency);
		break;
	case DSP_Com2:
		sprintf(command, "COMX%04d", frequency);
		break;
	case Com2Standby:
		sprintf(command, "COMS%04d", frequency);
		break;
	case Nav1:
		sprintf(command, "NAVx%04d", frequency);
		break;
	case Nav1Standby:
		sprintf(command, "NAVs%04d", frequency);
		break;
	case Nav2:
		sprintf(command, "NAVX%04d", frequency);
		break;
	case Nav2Standby:
		sprintf(command, "NAVS%04d", frequency);
		break;
	case Transponder:
		sprintf(command, "TRN%04d", frequency);
		break;
	}
	m_commPort->send(command);
}

void BaseDeviceHandler::displayDme1(float distance, float speed, float course, char *ident)
{
	char command[9];

	sprintf(command, "DMEd%04u", (int)(distance * 10.0));
	sendIfChanged(command, m_dmeDistance);

	sprintf(command, "DMEs%03u", (int)speed);
	sendIfChanged(command, m_dmeSpeed);

	sprintf(command, "DMc%03u ", (int)(course + 360.5) % 360);
	sendIfChanged(command, m_dmeCourse);

	strcpy(command, "DMi");
	strncat(command, ident, 5);
	strncat(command, "        ", 8);
	sendIfChanged(command, m_dmeIdent);
}

void BaseDeviceHandler::displayDme2(float distance, float speed, float course, char *ident)
{
	char command[9];

	sprintf(command, "DMED%04u", (int)(distance * 10.0));
	sendIfChanged(command, m_dmeDistance);

	sprintf(command, "DMES%03u", (int)speed);
	sendIfChanged(command, m_dmeSpeed);

	sprintf(command, "DMC%03u ", (int)(course + 360.5) % 360);
	sendIfChanged(command, m_dmeCourse);

	strcpy(command, "DMI");
	strncat(command, ident, 5);
	strncat(command, "        ", 8);
	sendIfChanged(command, m_dmeIdent);
}


char* BaseDeviceHandler::identPrefix1() const
{
	return nullptr;
}

char* BaseDeviceHandler::identPrefix2() const
{
	return nullptr;
}

void BaseDeviceHandler::sendIfChanged(char *newCommand, char *lastCommand, int length)
{
	if (strncmp(lastCommand, newCommand, length))
	{
		m_commPort->send(newCommand);
		strncpy(lastCommand, newCommand, length);
	}
}

//---------------------------------------------------------------------------
// Parse a message from the hardware by relaying it to a specific sub class
// Return the identified command, optionally with additional parameters
//---------------------------------------------------------------------------
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::parse(char *message)
{
	VriCommandParameters command;
	command.m_boosted = false;
	command.m_value = 0.0;
	command.m_command = VriCommand::None;

	switch (message[0])
	{
	case 'A': return a(message, command);
	case 'B': return b(message, command);
	case 'C': return c(message, command);
	case 'D': return d(message, command);
	case 'E': return e(message, command);
	case 'F': return f(message, command);
	case 'G': return g(message, command);
	case 'H': return h(message, command);
	case 'I': return i(message, command);
	case 'J': return j(message, command);
	case 'K': return k(message, command);
	case 'L': return l(message, command);
	case 'M': return m(message, command);
	case 'N': return n(message, command);
	case 'O': return o(message, command);
	case 'P': return p(message, command);
	case 'Q': return q(message, command);
	case 'R': return r(message, command);
	case 'S': return s(message, command);
	case 'T': return t(message, command);
	case 'U': return u(message, command);
	case 'V': return v(message, command);
	case 'W': return w(message, command);
	case 'X': return x(message, command);
	case 'Y': return y(message, command);
	case 'Z': return z(message, command);
	default: 
		return command;
	}
}

BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::a(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::b(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::c(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::d(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::e(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::f(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::g(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::h(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::i(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::j(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::k(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::l(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::m(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::n(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::o(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::p(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::q(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::r(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::s(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::t(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::u(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::v(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::w(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::x(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::y(char *message, VriCommandParameters &command) { return command; }
BaseDeviceHandler::VriCommandParameters BaseDeviceHandler::z(char *message, VriCommandParameters &command) { return command; }

float BaseDeviceHandler::toFloat(char* cmd, int start, int end)
{
	float value = 0.0;
	for (int i=start; i<=end; i++)
	{
		value = (value*10.0f) + (cmd[i]-'0');
	}
	return value;
}
#include "BaseDeviceHandler.h"
#include "BaseAircraft.h"

#include "logger.h"

#include <mutex>
#include <sstream>

#include <XPLM/XPLMUtilities.h>

static std::mutex g_planeMutex;

BaseDeviceHandler::BaseDeviceHandler()
	: m_plane(nullptr)
{
}

BaseDeviceHandler::~BaseDeviceHandler()
{
	if (m_plane != nullptr)
		m_plane = nullptr;
}

void BaseDeviceHandler::setPlane(BaseAircraft *plane)
{
	std::lock_guard<std::mutex> guard(g_planeMutex);
	m_plane = plane;
}

void BaseDeviceHandler::parseCommand(char *message)
{
	VriCommandParameters command = parse(message);
	bool handled = false;
	{
		std::lock_guard<std::mutex> guard(g_planeMutex);
		if (command.m_command != None && m_plane != nullptr)
		{
			handled = m_plane->handleCommand(command);
		}	
	}

	if (!handled)
	{
		VLLog("%s translated to internal command #%d and was %s by the aircraft", message, command.m_command, (handled ? "handled" : "UNHANDLED"));
	}
}

char* BaseDeviceHandler::identPrefix1() const
{
	return nullptr;
}

char* BaseDeviceHandler::identPrefix2() const
{
	return nullptr;
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
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

float BaseDeviceHandler::value() const
{
	return m_value;
}

bool BaseDeviceHandler::boost() const
{
	return m_boost;
}

void BaseDeviceHandler::setPlane(BaseAircraft *plane)
{
	std::lock_guard<std::mutex> guard(g_planeMutex);
	m_plane = plane;
}

void BaseDeviceHandler::parseCommand(char *cmd)
{
	VriCommand command = parse(cmd);
	bool handled = false;
	{
		std::lock_guard<std::mutex> guard(g_planeMutex);
		if (command != None && m_plane != nullptr)
		{
			handled = m_plane->handleCommand(command, m_value, m_boost);
		}	
	}

	if (!handled)
	{
		VLLog("%s translated to internal command #%d and was %s by the aircraft", cmd, command, (handled ? "handled" : "UNHANDLED"));
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


BaseDeviceHandler::VriCommand BaseDeviceHandler::parse(char *cmd)
{
	m_boost = false;
	m_value = 0.0f;
	switch (cmd[0])
	{
	case 'A': return a(cmd);
	case 'B': return b(cmd);
	case 'C': return c(cmd);
	case 'D': return d(cmd);
	case 'E': return e(cmd);
	case 'F': return f(cmd);
	case 'G': return g(cmd);
	case 'H': return h(cmd);
	case 'I': return i(cmd);
	case 'J': return j(cmd);
	case 'K': return k(cmd);
	case 'L': return l(cmd);
	case 'M': return m(cmd);
	case 'N': return n(cmd);
	case 'O': return o(cmd);
	case 'P': return p(cmd);
	case 'Q': return q(cmd);
	case 'R': return r(cmd);
	case 'S': return s(cmd);
	case 'T': return t(cmd);
	case 'U': return u(cmd);
	case 'V': return v(cmd);
	case 'W': return w(cmd);
	case 'X': return x(cmd);
	case 'Y': return y(cmd);
	case 'Z': return z(cmd);
	default: return VriCommand::None;
	}
}

BaseDeviceHandler::VriCommand BaseDeviceHandler::a(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::b(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::c(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::d(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::e(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::f(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::g(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::h(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::i(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::j(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::k(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::l(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::m(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::n(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::o(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::p(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::q(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::r(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::s(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::t(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::u(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::v(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::w(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::x(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::y(char *cmd) { return VriCommand::None; }
BaseDeviceHandler::VriCommand BaseDeviceHandler::z(char *cmd) { return VriCommand::None; }

float BaseDeviceHandler::toFloat(char* cmd, int start, int end)
{
	float value = 0.0;
	for (int i=start; i<=end; i++)
	{
		value = (value*10.0f) + (cmd[i]-'0');
	}
	return value;
}
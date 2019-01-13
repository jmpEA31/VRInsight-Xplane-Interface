#include "ZiboB738.h"
#include "logger.h"


ZiboB738::ZiboB738()
	: LaminarB738()
{
	m_refAltNNNup = findCommandRef("laminar/B738/autopilot/altitude_up");
	m_refAltNNNdn = findCommandRef("laminar/B738/autopilot/altitude_dn");

	m_refEfisModeDown = findCommandRef("laminar/B738/EFIS_control/map_mode_dn");
	m_refEfisModeUp = findCommandRef("laminar/B738/EFIS_control/map_mode_up");

}

bool ZiboB738::isLoaded()
{
	return (XPLMFindDataRef("laminar/B738/navdata/navdata") != nullptr);
}

#define COMMAND(a) case BaseDeviceHandler::##a: scheduleCommand(m_ref##a); return true;

bool ZiboB738::handleCommand(BaseDeviceHandler::VriCommandParameters command)
{
	switch (command.m_command)
	{
		COMMAND(AltNNNup);
		COMMAND(AltNNNdn);

	default:
		return BaseAircraft::handleCommand(command);
	}
}
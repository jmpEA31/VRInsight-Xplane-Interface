#include "ZiboB738.h"
#include "logger.h"


ZiboB738::ZiboB738()
	: LaminarB738()
{
}

bool ZiboB738::isLoaded()
{
	return (XPLMFindDataRef("laminar/B738/navdata/navdata") != nullptr);
}


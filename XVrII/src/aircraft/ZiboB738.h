#ifndef ZIBOB738H
#define ZIBOB738H

#include "LaminarB738.h"

class ZiboB738 : public LaminarB738
{
public:
	ZiboB738();

	bool handleCommand(BaseDeviceHandler::VriCommandParameters command) override;

	static bool isLoaded();

protected:
	XPLMCommandRef m_refAltNNNup;
	XPLMCommandRef m_refAltNNNdn;
};

#endif


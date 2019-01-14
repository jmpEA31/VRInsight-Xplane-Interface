#pragma once

#include "XPLMDefs.h"

extern "C" {

	typedef float(*XPLMFlightLoop_f)(
		float                inElapsedSinceLastCall,
		float                inElapsedTimeSinceLastFlightLoop,
		int                  inCounter,
		void *               inRefcon);

	XPLM_API float XPLMGetElapsedTime(void);
	XPLM_API void XPLMRegisterFlightLoopCallback(XPLMFlightLoop_f inFlightLoop, float inInterval, void *inRefcon);
	XPLM_API void XPLMUnregisterFlightLoopCallback(XPLMFlightLoop_f inFlightLoop, void *inRefcon);

}

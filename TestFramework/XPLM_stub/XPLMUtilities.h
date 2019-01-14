#pragma once

#include "XPLMDefs.h"

extern "C" {

	typedef void * XPLMCommandRef;

	enum {
		xplm_CommandBegin = 0
		, xplm_CommandContinue = 1
		, xplm_CommandEnd = 2
	};

	typedef int XPLMCommandPhase;
	typedef int(*XPLMCommandCallback_f)(
		XPLMCommandRef       inCommand,
		XPLMCommandPhase     inPhase,
		void *               inRefcon);

	XPLM_API XPLMCommandRef XPLMFindCommand(const char *inName);
	XPLM_API void XPLMCommandOnce(XPLMCommandRef inCommand);
	XPLM_API void XPLMRegisterCommandHandler(XPLMCommandRef inComand, XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon);
	XPLM_API void XPLMUnregisterCommandHandler(XPLMCommandRef inComand, XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon);

	XPLM_API void XPLMDebugString(const char *inString);
}

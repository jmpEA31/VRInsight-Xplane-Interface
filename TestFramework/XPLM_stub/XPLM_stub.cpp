// XPLM_stub.cpp : Defines the exported functions for the DLL application.
//

#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include <iostream>


#include <chrono>

std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
bool chronoStarted = false;

XPLM_API float XPLMGetElapsedTime(void)
{
	if (!chronoStarted)
	{
		start = std::chrono::system_clock::now();
		chronoStarted = true;
	}
	return std::chrono::duration<float>((std::chrono::system_clock::now() - start)).count();
}

XPLM_API void XPLMRegisterFlightLoopCallback(XPLMFlightLoop_f inFlightLoop, float inInterval, void *inRefcon)
{

}

XPLM_API void XPLMUnregisterFlightLoopCallback(XPLMFlightLoop_f inFlightLoop, void *inRefcon)
{

}

XPLM_API XPLMCommandRef XPLMFindCommand(const char *inName)
{
	return nullptr;
}

XPLM_API void XPLMCommandOnce(XPLMCommandRef inCommand)
{

}

XPLM_API void XPLMRegisterCommandHandler(XPLMCommandRef inComand, XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon)
{

}

XPLM_API void XPLMUnregisterCommandHandler(XPLMCommandRef inComand, XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon)
{

}

XPLM_API XPLMDataRef XPLMFindDataRef(const char *inDataRefName)
{
	return nullptr;
}

XPLM_API int XPLMCanWriteDataRef(XPLMDataRef inDataRef)
{
	return 0;
}

XPLM_API XPLMDataTypeID XPLMGetDataRefTypes(XPLMDataRef inDataRef)
{
	return 2;
}

XPLM_API void XPLMSetDataf(XPLMDataRef inDataRef, float inValue)
{

}

XPLM_API float XPLMGetDataf(XPLMDataRef inDataRef)
{
	return 1.2;
}

XPLM_API int XPLMGetDatavi(XPLMDataRef inDataRef, int *outValues, int inOffset, int inMax)
{
	return 42;
}

XPLM_API int XPLMGetDatai(XPLMDataRef inDataRef)
{
	return 12;
}

XPLM_API int XPLMGetDatab(XPLMDataRef inDataRef, void *outValue, int inOffset, int inMaxBytes)
{
	return 1;
}

XPLM_API void XPLMSetDatai(XPLMDataRef inDataRef, int inValue)
{

}

XPLM_API void XPLMDebugString(const char *inString)
{
	if (inString != nullptr) std::cout << inString;
}
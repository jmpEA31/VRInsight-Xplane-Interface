#pragma once

#include "XPLMDefs.h"

extern "C" {
	typedef void * XPLMDataRef;

	enum {
		xplmType_Unknown = 0
		, xplmType_Int = 1
		, xplmType_Float = 2
		, xplmType_Double = 4
		, xplmType_FloatArray = 8
		, xplmType_IntArray = 16
		, xplmType_Data = 32
	};

	typedef int XPLMDataTypeID;

	XPLM_API XPLMDataRef XPLMFindDataRef(const char *inDataRefName);
	XPLM_API int XPLMCanWriteDataRef(XPLMDataRef inDataRef);

	XPLM_API XPLMDataTypeID XPLMGetDataRefTypes(XPLMDataRef inDataRef);

	XPLM_API int XPLMGetDatab(XPLMDataRef inDataRef, void *outValue, int inOffset, int inMaxBytes);

	XPLM_API float XPLMGetDataf(XPLMDataRef inDataRef);
	XPLM_API void XPLMSetDataf(XPLMDataRef inDataRef, float inValue);

	XPLM_API int XPLMGetDatai(XPLMDataRef inDataRef);    
	XPLM_API void XPLMSetDatai(XPLMDataRef inDataRef, int inValue);

	XPLM_API int XPLMGetDatavi(XPLMDataRef inDataRef, int *outValues, int inOffset, int inMax);

}
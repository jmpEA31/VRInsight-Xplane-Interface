#include "logger.h"
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMProcessing.h>
#include <stdio.h>
#include <cmath>

static char gPluginName[32];
static char buffer[512];
static char message[1024];

void vlogger_setPluginName(char *plugin)
{
	strncpy(gPluginName, plugin, 32);
}

void vlogger_log(char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vsprintf(buffer, fmt, va);
	va_end(va);

	float et = XPLMGetElapsedTime();

	sprintf(message, "%1u:%02u:%06.3f %s %s\n", (unsigned int)(et/3600), ((unsigned int)(et/60)%60), std::fmod(et, 60.0), gPluginName, buffer);
	XPLMDebugString(message);
}


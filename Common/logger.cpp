#include "logger.h"
#include <XPLM/XPLMUtilities.h>
#include <stdio.h>

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

	sprintf(message, "<%s> %s\n", gPluginName, buffer);
	XPLMDebugString(message);
}


#ifndef VORTEXLOGGERH
#define VORTEXLOGGERH

#include <stdarg.h>

#define VLLog(...) vlogger_log(__VA_ARGS__)
#define VLTrace(...) vlogger_log(__VA_ARGS__)

void vlogger_setPluginName(char *plugin);
void vlogger_log(char *fmt, ...);

#endif
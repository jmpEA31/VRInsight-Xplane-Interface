#ifndef FMERDEVICEHANDLERH
#define FMERDEVICEHANDLERH

#include "BaseDeviceHandler.h"

class FMERDeviceHandler : public BaseDeviceHandler
{
public:
	FMERDeviceHandler();
	virtual ~FMERDeviceHandler();

	char* identPrefix1() const override;
	char* identPrefix2() const override;

protected:
	VriCommand a(char *cmd) override;
	VriCommand b(char *cmd) override;
	VriCommand c(char *cmd) override;
	VriCommand d(char *cmd) override;
	VriCommand e(char *cmd) override;
	VriCommand h(char *cmd) override;
	VriCommand m(char *cmd) override;
	VriCommand n(char *cmd) override;
	VriCommand o(char *cmd) override;
	VriCommand s(char *cmd) override;
	VriCommand t(char *cmd) override;
	VriCommand v(char *cmd) override;
};

#endif

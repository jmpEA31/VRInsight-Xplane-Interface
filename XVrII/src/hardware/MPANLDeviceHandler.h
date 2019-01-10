#ifndef MPANLDEVICEHANDLERH
#define MPANLCOMMANDPARSERH

#include "BaseDeviceHandler.h"

class MPANLDeviceHandler : public BaseDeviceHandler
{
public:
	MPANLDeviceHandler();
	virtual ~MPANLDeviceHandler();

	char* identPrefix1() const override;
	char* identPrefix2() const override;

//protected:
//	VriCommandParameters a(char *cmd) override;
//	VriCommandParameters b(char *cmd) override;
//	VriCommandParameters c(char *cmd) override;
//	VriCommandParameters d(char *cmd) override;
//	VriCommandParameters e(char *cmd) override;
//	VriCommandParameters h(char *cmd) override;
//	VriCommandParameters m(char *cmd) override;
//	VriCommandParameters n(char *cmd) override;
//	VriCommandParameters o(char *cmd) override;
//	VriCommandParameters s(char *cmd) override;
//	VriCommandParameters t(char *cmd) override;
//	VriCommandParameters v(char *cmd) override;
};

#endif

#ifndef FMERDEVICEHANDLERH
#define FMERDEVICEHANDLERH

#include "BaseDeviceHandler.h"

class FMERDeviceHandler : public BaseDeviceHandler
{
public:
	FMERDeviceHandler(VRiCommPort *commPort);
	virtual ~FMERDeviceHandler();

	char* identPrefix1() const override;
	char* identPrefix2() const override;

protected:
	VriCommandParameters a(char *message, VriCommandParameters &command) override;
	VriCommandParameters b(char *message, VriCommandParameters &command) override;
	VriCommandParameters c(char *message, VriCommandParameters &command) override;
	VriCommandParameters d(char *message, VriCommandParameters &command) override;
	VriCommandParameters e(char *message, VriCommandParameters &command) override;
	VriCommandParameters h(char *message, VriCommandParameters &command) override;
	VriCommandParameters m(char *message, VriCommandParameters &command) override;
	VriCommandParameters n(char *message, VriCommandParameters &command) override;
	VriCommandParameters o(char *message, VriCommandParameters &command) override;
	VriCommandParameters s(char *message, VriCommandParameters &command) override;
	VriCommandParameters t(char *message, VriCommandParameters &command) override;
	VriCommandParameters v(char *message, VriCommandParameters &command) override;
};

#endif

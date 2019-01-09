#ifndef VRICOMMPORTH
#define VRICOMMPORTH

#include <Windows.h>
#include <thread>
#include <queue>
#include <mutex>

class BaseDeviceHandler;

typedef std::queue<char*> CommandOutQueue;

class VRiCommPort
{
public:
	VRiCommPort(char *szPortName);
	~VRiCommPort();

	enum Status { Init, Failed, Scanning, TimedOut, Found };

	DWORD errorCode() const;
	BaseDeviceHandler *parser() const;
	Status status() const;
	const char *portName() const;

	void sendIdent1(char *display);
	void sendIdent2(char *display);

	void send(char *command);

private:
	void poll();

	CommandOutQueue m_commands;
	std::mutex m_commandMutex;

	HANDLE m_hCommPort;
	char m_szPortName[64];
	std::thread *m_pollThread;

	Status m_status;

	bool m_poll;
	DWORD m_error;

	BaseDeviceHandler *m_parser;
};



#endif

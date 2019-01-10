#include "VRiCommPort.h"

#include <sstream>

#include "logger.h"
#include <XPLM/XPLMUtilities.h>

#include "BaseDeviceHandler.h"
#include "FMERDeviceHandler.h"
#include "MPANLDeviceHandler.h"

VRiCommPort::VRiCommPort(char *szPortName)
	: m_status(Init)
	, m_error(ERROR_SUCCESS)
	, m_pollThread(nullptr)
	, m_poll(true)
	, m_parser(nullptr)
{
	strncpy(m_szPortName, szPortName, 64);
	m_hCommPort = ::CreateFile(m_szPortName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, 0);
	if (m_hCommPort == INVALID_HANDLE_VALUE)
	{
		m_error = GetLastError();
		VLLog("%s scan failed to open with error code: %d", szPortName, m_error);
		m_status = Status::Failed;
		return;
	}

	DCB dcb = { 0 };
	dcb.DCBlength = sizeof(DCB);

	if (!::GetCommState(m_hCommPort, &dcb))
	{
		m_error = GetLastError();
		VLLog("%s failed to get state with error code: %d", szPortName, m_error);
		m_status = Status::Failed;
		::CloseHandle(m_hCommPort);
		return;
	}

	dcb.BaudRate = CBR_115200;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	if (!::SetCommState(m_hCommPort, &dcb))
	{
		m_error = GetLastError();
		VLLog("%s scan failed to set state with error code: %d", szPortName, m_error);
		m_status = Status::Failed;
		::CloseHandle(m_hCommPort);
		return;
	}

	COMMTIMEOUTS ctos;
	GetCommTimeouts(m_hCommPort, &ctos);
	ctos.ReadTotalTimeoutMultiplier = 5;
	ctos.ReadTotalTimeoutConstant = 10;
	SetCommTimeouts(m_hCommPort, &ctos);


	m_pollThread = new std::thread(&VRiCommPort::poll, this);
	if (m_pollThread)
	{
		DWORD written;
		if (::WriteFile(m_hCommPort, "CMDRST\0\0CMDCON\0\0", 16, &written, nullptr))
		{
			VLLog("%s scan ok, did send RST and CON commands [%d].", szPortName, written);
			m_status = Status::Scanning;
		}
		else
		{
			m_error = GetLastError();
			VLLog("%s scan failed to write init codes with error code: %d", szPortName, m_error);
			m_status = Status::Failed;
			::CloseHandle(m_hCommPort);
		}
	}
	else
	{
		m_error = GetLastError();
		VLLog("%s scan failed to start scan thread with error code: %d", szPortName, m_error);
		m_status = Status::Failed;
		::CloseHandle(m_hCommPort);
	}
}

VRiCommPort::~VRiCommPort()
{
	m_poll = false;

	if (m_pollThread != nullptr)
	{
		m_pollThread->join();
		delete m_pollThread;
	}

	if (m_parser != nullptr)
		m_parser = nullptr;

	if (m_status == Status::Scanning || m_status == Status::Found || m_status == Status::TimedOut)
	{
		::CloseHandle(m_hCommPort);
	}
}

BaseDeviceHandler *VRiCommPort::parser() const
{
	return m_parser;
}

const char *VRiCommPort::portName() const
{
	return m_szPortName;
}

VRiCommPort::Status VRiCommPort::status() const
{
	return m_status;
}

DWORD VRiCommPort::errorCode() const
{
	return m_error;
}

void VRiCommPort::send(char *command)
{
	if (status() == Status::Found)
	{
		char *c = (char*)malloc(16);
		strncpy(c, command, 16);
		m_commandMutex.lock();
		m_commands.push(c);
		m_commandMutex.unlock();
	}
}

void VRiCommPort::poll()
{
	char commandBuffer[64] = { 0 };
	int commandLength = 0;
	DWORD readCount = 0;
	char newcmd[8];

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

	while (m_poll)
	{
		bool didSend;

		// First send any pending commands from the plugin to the hardware
		do
		{
			didSend = false;
			m_commandMutex.lock();
			if (m_commands.size() > 0)
			{
				char *command = m_commands.front();
				m_commands.pop();

				// unlock asap and each iteration to not block the producing thread
				m_commandMutex.unlock();

				DWORD written;
				::WriteFile(m_hCommPort, command, 8, &written, nullptr);
				free(command);
				didSend = true;
			}
			else
			{
				m_commandMutex.unlock();
			}
		} 
		while (didSend);

		// Then see if there is any data coming in from the hardware, reading MAX of 32 bytes
		if (!::ReadFile(m_hCommPort, newcmd, 32, &readCount, nullptr))
		{
			m_status = Status::Failed;
			m_poll = false;
			VLLog("Read Error on Port");
			break;
		}

		if (readCount == 0)
		{
			// No data received, if we are in the scan phase check timeout
			if (m_status == Status::Scanning)
			{
				std::chrono::duration<double> scanTime = std::chrono::system_clock::now() - start;
				if (scanTime.count() > 3.0)
				{
					m_status = Status::TimedOut;
					m_poll = false;
					continue;
				}
			}

			// Not scanning or no time out yet, just wait and try again
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		// replace 0's with .'s for easier string handling
		for (DWORD i = 0; i < readCount; i++)											
			if (newcmd[i] == 0x00)
				newcmd[i] = 0x2e;

		// append the new data to any remaining data from the previous loop
		commandLength += readCount;
		strncat(commandBuffer, newcmd, readCount);

		// Reducing buffer to below 8 bytes, so commandBuffer max size = 7+32 = 39 bytes
		// Aka: Keep looping while any potential command remains in the buffer
		while (commandLength >= 8)													
		{
			// Get the head message (assumes all VrInsight hardware send 8 byte commands)
			char cmdNow[16];
			strncpy(cmdNow, commandBuffer, 8);
			cmdNow[8] = 0;
			VLTrace("Received <%s> on <%s>", cmdNow, m_szPortName);

			if (m_status == Status::Scanning)
			{
				// If we are scanning to see which hardware is present...
				if (!strncmp(cmdNow, "CMDCON", 6))
				{
					// It is VrInsight HW, now let's ask which...
					DWORD written;
					::WriteFile(m_hCommPort, "CMDFUN\0\0", 8, &written, nullptr);
				}
				else if (!strncmp(cmdNow, "CMDFMER", 7))
				{
					// MCP Combo I found
					std::chrono::duration<double> scanTime = std::chrono::system_clock::now() - start;
					VLLog("Indentified FMER (MCP Combo I) on <%s> after %.2f seconds", m_szPortName, scanTime.count());
					m_parser = new FMERDeviceHandler(this);
					m_status = Status::Found;
				}
				else if (!strncmp(cmdNow, "CMDMPanl", 8))
				{
					// M-Panel found
					std::chrono::duration<double> scanTime = std::chrono::system_clock::now() - start;
					VLLog("Indentified MPanl (M-Panel) on <%s> after %.2f seconds", m_szPortName, scanTime.count());
					m_parser = new MPANLDeviceHandler(this);
					m_status = Status::Found;
				}
			}
			else if (m_status == Status::Found)
			{
				// Not scanning, just parse the command
				m_parser->parseCommand(cmdNow);
			}

			// Shift the buffer
			strncpy(commandBuffer, &commandBuffer[8], commandLength - 8);
			commandBuffer[commandLength - 8] = 0;
			commandLength -= 8;
		}

		// At 115k2 8n1... 115200/9/8 = Max 1600 bytes/second, 
		// Sleeping for 5ms allows reading of 1000/5*32 = 6400 bytes (minus processing above)
		std::this_thread::sleep_for(std::chrono::milliseconds(5));					
	}
}
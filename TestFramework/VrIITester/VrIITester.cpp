// VrIITester.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>

typedef int XPLMPluginID;

typedef int (CALLBACK* LPPLUGINSTART)(char *, char*, char*);
typedef void (CALLBACK* LPPLUGINSTOP)(void);
typedef int (CALLBACK* LPPLUGINENABLE)(void);
typedef void (CALLBACK* LPPLUGINDISABLE)(void);
typedef void (CALLBACK* LPPLUGINRECEIVEMESSAGE)(XPLMPluginID inFrom, int inMsg, void * inParam);

LPPLUGINSTART XPluginStart = nullptr;
LPPLUGINSTOP XPluginStop = nullptr;
LPPLUGINENABLE XPluginEnable = nullptr;
LPPLUGINDISABLE XPluginDisable = nullptr;
LPPLUGINRECEIVEMESSAGE XPluginReceiveMessage = nullptr;

int main()
{
	bool failed = false;

	HMODULE hModule = LoadLibraryExA("win.xpl", nullptr, 0);
	if (hModule != nullptr)
	{
		std::cout << "DLL loaded." << std::endl;
	}
	else
	{		
		std::cout << "DLL load failed: " << GetLastError() << std::endl;
		std::cin.get();
		return -1;
	}

	XPluginStart = (LPPLUGINSTART)GetProcAddress(hModule, "XPluginStart");
	if (XPluginStart == nullptr)
	{
		std::cout << "Failed to find <XPluginStart> in actual plugin DLL\n" << std::endl;
		failed = true;
	}

	XPluginStop = (LPPLUGINSTOP)GetProcAddress(hModule, "XPluginStop");
	if (XPluginStop == nullptr)
	{
		std::cout << ("Failed to find <XPluginStop> in actual plugin DLL\n");
		failed = true;
	}

	XPluginEnable = (LPPLUGINENABLE)GetProcAddress(hModule, "XPluginEnable");
	if (XPluginEnable == nullptr)
	{
		std::cout << ("Failed to find <XPluginEnable> in actual plugin DLL\n");
		failed = true;
	}

	XPluginDisable = (LPPLUGINDISABLE)GetProcAddress(hModule, "XPluginDisable");
	if (XPluginDisable == nullptr)
	{
		std::cout << ("Failed to find <XPluginDisable> in actual plugin DLL\n");
		failed = true;
	}


	if (!failed)
	{
		char szName[256], szSignature[256], szDescription[356];
		int ok = XPluginStart(szName, szSignature, szDescription);
		if (ok)
		{
			std::cout << szName << " started ok, signature: " << szSignature << " (" << szDescription << ")" << std::endl;
		}

		XPluginEnable();
		XPluginDisable();
		XPluginStop();
	}

	FreeLibrary(hModule);
	std::cin.get();

    return 0;
}


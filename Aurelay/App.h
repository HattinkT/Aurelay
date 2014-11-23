#pragma once

#include "AudioInterfaces.h"


class App
{
public:
	App();
	~App();

	int Run(int argc, _TCHAR* argv[]);
	int RunWithFile(int argc, _TCHAR* argv[]);

private:
	const DWORD c_msPollingLength = 5; // ms
	const DWORD c_msTransmitBuffersize = 100; // ms

	int RunAsServer(USHORT port);
};


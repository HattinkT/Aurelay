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
	const DWORD c_msTransferLatency = 25; // ms

	int RunAsServer(_TCHAR* port);
	int RunAsClient(_TCHAR* port, _TCHAR* server);

	void PrintWaveInfo(WAVEFORMATEXTENSIBLE* pWaveformat);
};


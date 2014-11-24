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
	const DWORD c_msPollingLength = 4; // ms
	const DWORD c_msTransferMaxLatency = 9; // ms (best to make it dividable by 3)
	const DWORD c_msAudioMinLatency = 15; // ms
	const DWORD c_msAudioBuffersize = 100; // ms

	int RunAsServer(_TCHAR* port);
	int RunAsClient(_TCHAR* port, _TCHAR* server);

	void PrintWaveInfo(WAVEFORMATEXTENSIBLE* pWaveformat);
};

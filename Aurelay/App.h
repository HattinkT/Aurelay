#pragma once

#include "AudioInterfaces.h"


class App
{
public:
	App();
	~App();

	int Run(int argc, _TCHAR* argv[]);

private:
	const DWORD c_msPollingLength = 5; // 5 ms
};


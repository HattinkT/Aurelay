#pragma once

#include "AudioDevice.h"

class App
{
public:
	App();
	~App();

	int Run(int argc, _TCHAR* argv[]);

private:
	AudioDevice m_AudioDev;
};


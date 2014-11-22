#include "stdafx.h"
#include <Objbase.h>

#include "App.h"

App::App()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}


App::~App()
{
	CoUninitialize();
}

int App::Run(int argc, _TCHAR* argv[])
{
	HRESULT hr;

	printf("Starting Aurelay\n");

	hr = m_AudioDev.openAudioDevice();

	if (hr == S_OK)
	{
		hr = m_AudioDev.captureAudio();
	}

	return 0;
}
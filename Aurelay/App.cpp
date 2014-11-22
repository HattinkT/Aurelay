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

	hr = m_AudioDev.openForCapture();

	if (hr == S_OK)
	{
		hr = m_AudioDev.startCapture();
	}

	while (hr == S_OK)
	{
		hr = m_AudioDev.getAudio(NULL);

		Sleep(c_msPollingLength);
	}

	return 0;
}
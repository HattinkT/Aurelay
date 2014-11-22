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
	printf("Starting Aurelay\n");

	return m_AudioDev.openAudioCaptureDevice();
}
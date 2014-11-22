#include "stdafx.h"
#include <Objbase.h>

#include "App.h"

#include "AudioDevice.h"
#include "FileHandler.h"

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

	AudioDevice* pAudioDev;
	FileHandler* pFileDev;

	WAVEFORMATEXTENSIBLE audioFormat;

	printf("Starting Aurelay\n");

	pAudioDev = new AudioDevice();
	pFileDev = new FileHandler(L"D:\\Temp\\Audio.bin");

	hr = pFileDev->openForPlayback();

	if (hr == S_OK)
	{
		hr = pAudioDev->openForCapture();

		if (hr == S_OK)
		{
			hr = pAudioDev->getAudioFormat(&audioFormat);

			printf("Starting capture:\n");
			printf("  Number of channels:%d\n", audioFormat.Format.nChannels);
			printf("  Channel mask:0x%08x\n", audioFormat.dwChannelMask);
			printf("  Sample rate:%d\n", audioFormat.Format.nSamplesPerSec);
			printf("  Sample bitsize:%d (of %d)\n", audioFormat.Samples.wValidBitsPerSample, audioFormat.Format.wBitsPerSample);
			printf("  Average bytes per sec:%d\n", audioFormat.Format.nAvgBytesPerSec);
			printf("  Block size:%d\n", audioFormat.Format.nBlockAlign);
			printf("  Samples per block:%d\n", audioFormat.Samples.wSamplesPerBlock);

			hr = pFileDev->putAudioFormat(&audioFormat);

			if (hr == S_OK)
			{
				hr = pFileDev->startPlayback();
			}

			if (hr == S_OK)
			{
				hr = pAudioDev->startCapture();
			}
		}

		while (hr == S_OK)
		{
			hr = pAudioDev->getAudio(pFileDev);

			Sleep(c_msPollingLength);
		}

		hr = pAudioDev->stopCapture();
		hr = pFileDev->stopPlayback();
	}

	delete pAudioDev;
	delete pFileDev;

	return 0;
}
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
	pFileDev = new FileHandler(L"D:\\Temp\\Audio.bin", c_msPollingLength);

#if 0
	// Capture to file
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
			printf("  Subformat:{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n",
				audioFormat.SubFormat.Data1, audioFormat.SubFormat.Data2, audioFormat.SubFormat.Data3,
				audioFormat.SubFormat.Data4[0], audioFormat.SubFormat.Data4[1], audioFormat.SubFormat.Data4[2], audioFormat.SubFormat.Data4[3],
				audioFormat.SubFormat.Data4[4], audioFormat.SubFormat.Data4[5], audioFormat.SubFormat.Data4[6], audioFormat.SubFormat.Data4[7]);

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
#else
	// Play from file
	hr = pAudioDev->openForPlayback();

	if (hr == S_OK)
	{
		hr = pFileDev->openForCapture();

		if (hr == S_OK)
		{
			hr = pFileDev->getAudioFormat(&audioFormat);

			printf("Starting playback:\n");
			printf("  Number of channels:%d\n", audioFormat.Format.nChannels);
			printf("  Channel mask:0x%08x\n", audioFormat.dwChannelMask);
			printf("  Sample rate:%d\n", audioFormat.Format.nSamplesPerSec);
			printf("  Sample bitsize:%d (of %d)\n", audioFormat.Samples.wValidBitsPerSample, audioFormat.Format.wBitsPerSample);
			printf("  Average bytes per sec:%d\n", audioFormat.Format.nAvgBytesPerSec);
			printf("  Block size:%d\n", audioFormat.Format.nBlockAlign);
			printf("  Samples per block:%d\n", audioFormat.Samples.wSamplesPerBlock);
			printf("  Subformat:{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n",
				audioFormat.SubFormat.Data1, audioFormat.SubFormat.Data2, audioFormat.SubFormat.Data3,
				audioFormat.SubFormat.Data4[0], audioFormat.SubFormat.Data4[1], audioFormat.SubFormat.Data4[2], audioFormat.SubFormat.Data4[3],
				audioFormat.SubFormat.Data4[4], audioFormat.SubFormat.Data4[5], audioFormat.SubFormat.Data4[6], audioFormat.SubFormat.Data4[7]);

			hr = pAudioDev->putAudioFormat(&audioFormat);

			if (hr == S_OK)
			{
				hr = pAudioDev->startPlayback();
			}

			if (hr == S_OK)
			{
				hr = pFileDev->startCapture();
			}
		}

		while (hr == S_OK)
		{
			hr = pFileDev->getAudio(pAudioDev);

			Sleep(c_msPollingLength);
			printf(".");
		}

		hr = pFileDev->stopCapture();
		hr = pAudioDev->stopPlayback();
	}
#endif

	delete pAudioDev;
	delete pFileDev;

	return 0;
}
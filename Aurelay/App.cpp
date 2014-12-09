#include "stdafx.h"
#include <Objbase.h>

#include "App.h"

#include "AudioDevice.h"
#include "TcpTransport.h"
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
	if (argc < 2)
	{
		printf("Aurelay <port> : Start capture server.\n");
		printf("Aurelay <port> <host> : Connect to capture server on host.\n");
		return 0;
	}

	printf("Starting Aurelay\n");

	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	if (argc < 3)
	{
		// Server mode
		return RunAsServer(argv[1]);
	}
	else
	{
		// Client mode
		return RunAsClient(argv[1], argv[2]);
	}
}

int App::RunAsServer(_TCHAR* port)
{
	HRESULT hr;

	AudioDevice* pAudioDev;
	TcpTransport* pTcpTransport;

	WAVEFORMATEXTENSIBLE audioFormat;

	printf("Starting TCP server on port %S\n", port);

	while (true)
	{
		pAudioDev = new AudioDevice(c_msAudioBuffersize);
		pTcpTransport = new TcpTransport(NULL, port, c_msTransferMaxLatency, 0);

		hr = pTcpTransport->openForPlayback();

		if (hr == S_OK)
		{
			hr = pAudioDev->openForCapture();

			if (hr == S_OK)
			{
				hr = pAudioDev->getAudioFormat(&audioFormat);
			}
			if (hr == S_OK)
			{
				printf("Starting capture:\n");
				PrintWaveInfo(&audioFormat);

				hr = pTcpTransport->putAudioFormat(&audioFormat);
			}

			if (hr == S_OK)
			{
				hr = pTcpTransport->startPlayback();
			}

			if (hr == S_OK)
			{
				hr = pAudioDev->startCapture();
			}

			while (hr == S_OK)
			{
				hr = pAudioDev->getAudio(pTcpTransport);

				Sleep(c_msPollingLength);
			}

			hr = pAudioDev->stopCapture();
			hr = pTcpTransport->stopPlayback();
		}

		delete pAudioDev;
		delete pTcpTransport;

		Sleep(1000);
	}

	return 0;
}

int App::RunAsClient(_TCHAR* port, _TCHAR* server)
{
	HRESULT hr;

	AudioDevice* pAudioDev;
	TcpTransport* pTcpTransport;

	WAVEFORMATEXTENSIBLE audioFormat;

	while (true)
	{
		printf("Starting TCP client connecting to %S:%S\n", server, port);

		pAudioDev = new AudioDevice(c_msAudioBuffersize);
		pTcpTransport = new TcpTransport(server, port, c_msTransferMaxLatency, c_msAudioMinLatency);

		hr = pTcpTransport->openForCapture();

		if (hr == S_OK)
		{
			hr = pAudioDev->openForPlayback();

			if (hr == S_OK)
			{
				hr = pTcpTransport->getAudioFormat(&audioFormat);
			}

			if (hr == S_OK)
			{
				printf("Starting playback:\n");
				PrintWaveInfo(&audioFormat);

				hr = pAudioDev->putAudioFormat(&audioFormat);
			}

			if (hr == S_OK)
			{
				hr = pAudioDev->startPlayback();
			}

			if (hr == S_OK)
			{
				hr = pTcpTransport->startCapture();
			}

			while (hr == S_OK)
			{
				hr = pTcpTransport->getAudio(pAudioDev);

				Sleep(c_msPollingLength);
			}

			hr = pTcpTransport->stopCapture();
			hr = pAudioDev->stopPlayback();
		}

		delete pAudioDev;
		delete pTcpTransport;

		Sleep(1000);
	}

	return 0;
}

int App::RunWithFile(int argc, _TCHAR* argv[])
{
	HRESULT hr;

	AudioDevice* pAudioDev;
	FileHandler* pFileDev;

	WAVEFORMATEXTENSIBLE audioFormat;

	printf("Starting Aurelay\n");

	pAudioDev = new AudioDevice(c_msAudioBuffersize);
	pFileDev = new FileHandler(L"Y:\\Audio.bin", c_msTransferMaxLatency);

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
			PrintWaveInfo(&audioFormat);

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
			PrintWaveInfo(&audioFormat);

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
		}

		hr = pFileDev->stopCapture();
		hr = pAudioDev->stopPlayback();
	}
#endif

	delete pAudioDev;
	delete pFileDev;

	return 0;
}

void App::PrintWaveInfo(WAVEFORMATEXTENSIBLE* pWaveformat)
{
	printf("  Number of channels:%d\n", pWaveformat->Format.nChannels);
	printf("  Channel mask:0x%08x\n", pWaveformat->dwChannelMask);
	printf("  Sample rate:%d\n", pWaveformat->Format.nSamplesPerSec);
	printf("  Sample bitsize:%d (of %d)\n", pWaveformat->Samples.wValidBitsPerSample, pWaveformat->Format.wBitsPerSample);
	printf("  Average bytes per sec:%d\n", pWaveformat->Format.nAvgBytesPerSec);
	printf("  Block size:%d\n", pWaveformat->Format.nBlockAlign);
	printf("  Samples per block:%d\n", pWaveformat->Samples.wSamplesPerBlock);
	printf("  Subformat:{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n",
		pWaveformat->SubFormat.Data1, pWaveformat->SubFormat.Data2, pWaveformat->SubFormat.Data3,
		pWaveformat->SubFormat.Data4[0], pWaveformat->SubFormat.Data4[1], pWaveformat->SubFormat.Data4[2], pWaveformat->SubFormat.Data4[3],
		pWaveformat->SubFormat.Data4[4], pWaveformat->SubFormat.Data4[5], pWaveformat->SubFormat.Data4[6], pWaveformat->SubFormat.Data4[7]);
}
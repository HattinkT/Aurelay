#pragma once

#include <windows.h>

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

	HRESULT openAudioCaptureDevice();

private:
	const LONGLONG hnsBufferLength = 10000000;
};


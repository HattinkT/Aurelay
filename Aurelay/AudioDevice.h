#pragma once

#include <Mmdeviceapi.h>
#include <Audioclient.h>

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

	HRESULT openAudioDevice();
	HRESULT captureAudio();

private:
	const REFERENCE_TIME c_hnsBufferLength  = 1000 * 1000 * 10; // 1000 ms
	const DWORD c_msPollingLength = 5; // 5 ms

	IAudioClient* m_pAudioClient;
};


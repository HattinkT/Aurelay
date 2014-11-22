#pragma once

#include "AudioInterfaces.h"

#include <Mmdeviceapi.h>
#include <Audioclient.h>

class AudioDevice : public IAudioIn, IAudioOut
{
public:
	AudioDevice();
	~AudioDevice();

	virtual HRESULT openForCapture();
	virtual HRESULT startCapture();
	virtual HRESULT stopCapture();
	virtual HRESULT getAudio(IAudioOut* pOut);

	virtual HRESULT openForPlayback();
	virtual HRESULT startPlayback();
	virtual HRESULT stopPlayback();
	virtual HRESULT putAudio();

private:
	HRESULT openAudioDevice();

	const REFERENCE_TIME c_hnsBufferLength  = 1000 * 1000 * 10; // 1000 ms

	IAudioClient* m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;
};


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
	virtual HRESULT getAudioFormat(WAVEFORMATEXTENSIBLE* pFormat);
	virtual HRESULT startCapture();
	virtual HRESULT getAudio(IAudioOut* pOut);
	virtual HRESULT stopCapture();

	virtual HRESULT openForPlayback();
	virtual HRESULT putAudioFormat(const WAVEFORMATEXTENSIBLE* pFormat);
	virtual HRESULT startPlayback();
	virtual HRESULT putAudio(const BYTE* pData, UINT32 numBytes);
	virtual HRESULT stopPlayback();

private:
	HRESULT openAudioDevice();

	const REFERENCE_TIME c_hnsBufferLength  = 1000 * 1000 * 10; // 1000 ms

	IAudioClient* m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;

	WAVEFORMATEXTENSIBLE m_audioFormat;
};


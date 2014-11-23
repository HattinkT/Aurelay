#pragma once

#include "AudioInterfaces.h"

#include <Mmdeviceapi.h>
#include <Audioclient.h>


class AudioDevice : public IAudioIn, public IAudioOut
{
public:
	AudioDevice(UINT32 msBuffersize);
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
	void closeAudioDevice();

	IAudioClient* m_pAudioClient;
	IAudioCaptureClient *m_pCaptureClient;
	IAudioRenderClient *m_pRenderClient;
	BOOL m_isPlaying;

	UINT32 m_msBuffersize;
	WAVEFORMATEXTENSIBLE m_audioFormat;
};


#pragma once

#include <Windows.h>
#include <mmreg.h>


class IAudioOut
{
public:
	virtual HRESULT openForPlayback() = 0;
	virtual HRESULT putAudioFormat(const WAVEFORMATEXTENSIBLE* pFormat) = 0;
	virtual HRESULT startPlayback() = 0;
	virtual HRESULT putAudio(const BYTE* pData, UINT32 numBytes) = 0;
	virtual HRESULT stopPlayback() = 0;
};


class IAudioIn
{
public:
	virtual HRESULT openForCapture() = 0;
	virtual HRESULT getAudioFormat(WAVEFORMATEXTENSIBLE* pFormat) = 0;
	virtual HRESULT startCapture() = 0;
	virtual HRESULT getAudio(IAudioOut* pOut) = 0;
	virtual HRESULT stopCapture() = 0;
};

#pragma once

#include <Windows.h>

#include "AudioInterfaces.h"


class FileHandler : public IAudioIn, public IAudioOut
{
public:
	FileHandler(LPCTSTR sFilename);
	~FileHandler();

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
	WAVEFORMATEXTENSIBLE m_audioFormat;

	LPCTSTR m_sFilename;
	HANDLE m_hFile;
};


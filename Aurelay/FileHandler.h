#pragma once

#include <Windows.h>

#include "AudioInterfaces.h"


class FileHandler : public IAudioIn, public IAudioOut
{
public:
	FileHandler(LPCTSTR sFilename, UINT32 msLatency);
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
	UINT32 m_msLatency;
	LPCTSTR m_sFilename;
	HANDLE m_hFile;
	BYTE *m_bBuffer;
	DWORD m_dwBufferSize;
};


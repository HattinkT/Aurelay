#pragma once

#include <Windows.h>

#include "AudioInterfaces.h"

class TcpTransport : public IAudioIn, public IAudioOut
{
public:
	TcpTransport(_TCHAR* server, _TCHAR* port, UINT32 msLatency, UINT32 msSoundLatency);
	~TcpTransport();

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
	_TCHAR* m_server;
	_TCHAR* m_port;
	UINT32 m_msLatency;
	UINT32 m_msSoundLatency;
	SOCKET m_skClient;

	WAVEFORMATEXTENSIBLE m_audioFormat;
	BYTE *m_bBuffer;
	DWORD m_dwBufferSize;
	DWORD m_dwBufferFilled;

	void closeClient();
};

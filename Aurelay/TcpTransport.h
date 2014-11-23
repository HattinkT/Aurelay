#pragma once

#include <Windows.h>

#include "AudioInterfaces.h"

class TcpTransport : public IAudioIn, public IAudioOut
{
public:
	TcpTransport(USHORT port, UINT32 msLatency);
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
	USHORT m_port;
	UINT32 m_msLatency;
	SOCKET m_skClient;

	void closeClient();
};

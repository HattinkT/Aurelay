#include "stdafx.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "TcpTransport.h"

#pragma comment (lib, "Ws2_32.lib")

TcpTransport::TcpTransport(USHORT port, UINT32 msLatency)
{
	int iResult;

	WSADATA wsaData;

	m_port = port;
	m_msLatency = msLatency;

	m_skClient = INVALID_SOCKET;

	if (m_port > 0)
	{
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			m_port = 0;
		}
	}
}

TcpTransport::~TcpTransport()
{
	closeClient();

	if (m_port > 0)
	{
		WSACleanup();
		m_port = 0;
	}
}

HRESULT TcpTransport::openForCapture()
{
	return E_NOTIMPL;
}

HRESULT TcpTransport::getAudioFormat(WAVEFORMATEXTENSIBLE* pFormat)
{
	return E_NOTIMPL;
}

HRESULT TcpTransport::startCapture()
{
	return E_NOTIMPL;
}

HRESULT TcpTransport::getAudio(IAudioOut* pOut)
{
	return E_NOTIMPL;
}

HRESULT TcpTransport::stopCapture()
{
	return E_NOTIMPL;
}

HRESULT TcpTransport::openForPlayback()
{
	int iResult;

	struct sockaddr_in local;
	struct sockaddr_in remote;
	int adrSize;
	SOCKET skListen;

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(m_port);

	skListen = socket(AF_INET, SOCK_STREAM, 0);

	if (skListen == INVALID_SOCKET)
	{
		printf("Failed to open server socket: socket() failed, %d\n", WSAGetLastError());

		return E_FAIL;
	}

	iResult = bind(skListen, (struct sockaddr*)&local, sizeof(local));

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to open server socket: bind() failed, %d\n", WSAGetLastError());

		return E_FAIL;
	}

	iResult = listen(skListen, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to open server socket: listen() failed, %d\n", WSAGetLastError());
		closesocket(skListen);

		return E_FAIL;
	}

	adrSize = sizeof(remote);
	m_skClient = accept(skListen, (struct sockaddr*)&remote, &adrSize);
	if (m_skClient == INVALID_SOCKET)
	{
		printf("Failed to open server socket: accept() failed, %d\n", WSAGetLastError());
		closesocket(skListen);

		return E_FAIL;
	}

	closesocket(skListen);

	printf("Accepted client connection from %d.%d.%d.%d\n",
		remote.sin_addr.S_un.S_un_b.s_b1,
		remote.sin_addr.S_un.S_un_b.s_b2,
		remote.sin_addr.S_un.S_un_b.s_b3,
		remote.sin_addr.S_un.S_un_b.s_b4);

	return S_OK;
}

HRESULT TcpTransport::putAudioFormat(const WAVEFORMATEXTENSIBLE* pFormat)
{
	int iResult;

	u_long iMode;
	int iBufsize;

	if (m_skClient == INVALID_SOCKET)
	{
		return E_FAIL;
	}

	iResult = send(m_skClient, (char *)pFormat, sizeof(WAVEFORMATEXTENSIBLE), 0);

	if ((iResult == SOCKET_ERROR) || (iResult < sizeof(WAVEFORMATEXTENSIBLE)))
	{
		printf("Client connection closed\n");

		return E_FAIL;
	}

	iMode = 1;
	iResult = ioctlsocket(m_skClient, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to set non blocking mode, %d\n", WSAGetLastError());

		return E_FAIL;
	}

	iBufsize = ((pFormat->Format.nSamplesPerSec * m_msLatency) / 1000);

	if (pFormat->Samples.wSamplesPerBlock > 0)
	{
		iBufsize = (iBufsize + pFormat->Samples.wSamplesPerBlock - 1) / pFormat->Samples.wSamplesPerBlock;
		iBufsize *= pFormat->Samples.wSamplesPerBlock;
	}

	iBufsize *= pFormat->Format.nBlockAlign;

	if (iBufsize < 1)
	{
		printf("Failed to estimate buffersize\n");
		return E_FAIL;
	}

	printf("Transmit buffer size: %d\n", iBufsize);

	iResult = setsockopt(m_skClient, SOL_SOCKET, SO_SNDBUF, (char *)&iBufsize, sizeof(iBufsize));

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to accept client connection: cannot set buffer size, %d\n", WSAGetLastError());

		return E_FAIL;
	}

	return S_OK;
}

HRESULT TcpTransport::startPlayback()
{
	return S_OK;
}

HRESULT TcpTransport::putAudio(const BYTE* pData, UINT32 numBytes)
{
	int iResult;
	int iError;

	if (m_skClient == INVALID_SOCKET)
	{
		return E_FAIL;
	}

	iResult = send(m_skClient, (char *)pData, numBytes, 0);

	if (iResult == SOCKET_ERROR)
	{
		iError = WSAGetLastError();

		if (iError != WSAEWOULDBLOCK)
		{
			printf("Client connection closed\n");

			return E_FAIL;
		}
	}
	else if (iResult < (int)numBytes)
	{
		printf("Client connection closed\n");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT TcpTransport::stopPlayback()
{
	closeClient();

	return S_OK;
}

void TcpTransport::closeClient()
{
	if (m_skClient != INVALID_SOCKET)
	{
		closesocket(m_skClient);
		m_skClient = INVALID_SOCKET;
	}
}
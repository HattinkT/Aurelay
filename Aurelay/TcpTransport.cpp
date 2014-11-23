#include "stdafx.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "TcpTransport.h"

#pragma comment (lib, "Ws2_32.lib")

TcpTransport::TcpTransport(_TCHAR* server, _TCHAR* port, UINT32 msLatency)
{
	int iResult;

	WSADATA wsaData;

	m_server = server;
	m_port = port;
	m_msLatency = msLatency;
	m_bBuffer = NULL;
	m_dwBufferSize = 0;
	m_dwBufferFilled = 0;

	m_skClient = INVALID_SOCKET;

	if (m_port != NULL)
	{
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			m_port = NULL;
		}
	}
}

TcpTransport::~TcpTransport()
{
	closeClient();

	if (m_port != NULL)
	{
		WSACleanup();
		m_port = NULL;
	}
}

HRESULT TcpTransport::openForCapture()
{
	int iResult;

	struct addrinfoW* result;
	struct addrinfoW* ptr;
	struct addrinfoW hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = GetAddrInfoW(m_server, m_port, &hints, &result);

	if (iResult != 0)
	{
		printf("Failed to connect to server, cannot resolve host or port: %d\n", iResult);
		return E_FAIL;
	}

	iResult = SOCKET_ERROR;
	ptr = result;

	while ((iResult == SOCKET_ERROR) && (ptr != NULL))
	{
		m_skClient = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (m_skClient != INVALID_SOCKET)
		{
			iResult = connect(m_skClient, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (iResult == SOCKET_ERROR)
			{
				closesocket(m_skClient);
				m_skClient = INVALID_SOCKET;
			}
		}

		ptr = ptr->ai_next;
	}

	FreeAddrInfo(result);

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to connect to server, cannot connect: %d\n", iResult);
		return E_FAIL;
	}

	printf("Connected to server\n");

	return S_OK;
}

HRESULT TcpTransport::getAudioFormat(WAVEFORMATEXTENSIBLE* pFormat)
{
	int iResult;

	if (m_skClient == INVALID_SOCKET)
	{
		return E_FAIL;
	}

	iResult = recv(m_skClient, (char *)pFormat, sizeof(WAVEFORMATEXTENSIBLE), 0);

	if ((iResult == SOCKET_ERROR) || (iResult < sizeof(WAVEFORMATEXTENSIBLE)))
	{
		printf("Client connection closed\n");

		return E_FAIL;
	}

	m_audioFormat = *pFormat;

	return S_OK;
}

HRESULT TcpTransport::startCapture()
{
	HRESULT hr;
	hr = startPlayback();

	if (hr == S_OK)
	{
		m_bBuffer = new BYTE[m_dwBufferSize];
		m_dwBufferFilled = 0;
	}

	return hr;
}

HRESULT TcpTransport::getAudio(IAudioOut* pOut)
{
	int iResult;
	int iError;

	if (m_skClient == INVALID_SOCKET)
	{
		return E_FAIL;
	}

	iResult = recv(m_skClient, (char *)(m_bBuffer + m_dwBufferFilled), m_dwBufferSize - m_dwBufferFilled, 0);

	if (iResult == SOCKET_ERROR)
	{
		iError = WSAGetLastError();

		if (iError != WSAEWOULDBLOCK)
		{
			printf("Client connection closed\n");

			return E_FAIL;
		}
		else
		{
			iResult = 0;
		}
	}
	else if (iResult == 0)
	{
		printf("Client connection closed\n");

		return E_FAIL;
	}

	m_dwBufferFilled += iResult;

	if (m_dwBufferFilled >= m_dwBufferSize)
	{
		m_dwBufferFilled = 0;

		if (pOut != NULL)
		{
			return pOut->putAudio(m_bBuffer, m_dwBufferSize);
		}
	}
	else
	{
		return S_OK;
	}

	return S_OK;
}

HRESULT TcpTransport::stopCapture()
{
	closeClient();

	return S_OK;
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
	local.sin_port = htons(_ttoi(m_port));

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

	m_audioFormat = *pFormat;

	return S_OK;
}

HRESULT TcpTransport::startPlayback()
{
	int iResult;

	u_long iMode;

	iMode = 1;
	iResult = ioctlsocket(m_skClient, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to set non blocking mode, %d\n", WSAGetLastError());

		return E_FAIL;
	}

	m_dwBufferSize = ((m_audioFormat.Format.nSamplesPerSec * m_msLatency) / 1000);

	if (m_audioFormat.Samples.wSamplesPerBlock > 0)
	{
		m_dwBufferSize = (m_dwBufferSize + m_audioFormat.Samples.wSamplesPerBlock - 1) / m_audioFormat.Samples.wSamplesPerBlock;
		m_dwBufferSize *= m_audioFormat.Samples.wSamplesPerBlock;
	}

	m_dwBufferSize *= m_audioFormat.Format.nBlockAlign;

	if (m_dwBufferSize < 1)
	{
		printf("Failed to estimate buffersize\n");
		return E_FAIL;
	}

	printf("Socket buffer size: %d\n", m_dwBufferSize);

	iResult = setsockopt(m_skClient, SOL_SOCKET, SO_SNDBUF, (char *)&m_dwBufferSize, sizeof(m_dwBufferSize));

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to set socket send buffer size, %d\n", WSAGetLastError());

		return E_FAIL;
	}

	iResult = setsockopt(m_skClient, SOL_SOCKET, SO_RCVBUF, (char *)&m_dwBufferSize, sizeof(m_dwBufferSize));

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to set socket receive buffer size, %d\n", WSAGetLastError());

		return E_FAIL;
	}

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

	if (m_bBuffer != NULL)
	{
		delete[] m_bBuffer;
		m_bBuffer = NULL;
		m_dwBufferSize = 0;
		m_dwBufferFilled = 0;
	}
}
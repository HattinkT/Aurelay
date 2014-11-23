#include "stdafx.h"

#include <Audioclient.h>

#include "FileHandler.h"

FileHandler::FileHandler(LPCTSTR sFilename, UINT32 msLatency)
{
	m_msLatency = msLatency;
	m_sFilename = sFilename;
	m_hFile = INVALID_HANDLE_VALUE;
	m_bBuffer = NULL;
	m_dwBufferSize = 0;
}

FileHandler::~FileHandler()
{
	if (m_bBuffer != NULL)
	{
		delete[] m_bBuffer;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}
}

HRESULT FileHandler::openForCapture()
{
	printf("Opening file \"%S\" for reading\n", m_sFilename);

	m_hFile = CreateFile(m_sFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open file.\n");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT FileHandler::getAudioFormat(WAVEFORMATEXTENSIBLE* pFormat)
{
	BOOL br;

	DWORD dwBytesRead = 0;

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	br = ReadFile(m_hFile, pFormat, sizeof(WAVEFORMATEXTENSIBLE), &dwBytesRead, NULL);

	if ((br == FALSE) || (dwBytesRead < sizeof(WAVEFORMATEXTENSIBLE)))
	{
		printf("Failed to read audio format from file.\n");

		return E_FAIL;
	}

	m_dwBufferSize = ((pFormat->Format.nSamplesPerSec * m_msLatency) / 1000);

	if (pFormat->Samples.wSamplesPerBlock > 0)
	{
		m_dwBufferSize = (m_dwBufferSize + pFormat->Samples.wSamplesPerBlock - 1) / pFormat->Samples.wSamplesPerBlock;
		m_dwBufferSize *= pFormat->Samples.wSamplesPerBlock;
	}

	m_dwBufferSize *= pFormat->Format.nBlockAlign;

	if (m_dwBufferSize < 1)
	{
		printf("Failed to read audio: estimated buffersize is empty\n");
		return E_FAIL;
	}

	printf("Transport buffersize is %d\n", m_dwBufferSize);

	m_bBuffer = new BYTE[m_dwBufferSize];

	return S_OK;
}

HRESULT FileHandler::startCapture()
{
	return S_OK;
}

HRESULT FileHandler::getAudio(IAudioOut* pOut)
{
	HRESULT hr;
	BOOL br;

	DWORD dwBytesRead = 0;

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	hr = S_OK;

	while (hr == S_OK)
	{
		br = ReadFile(m_hFile, m_bBuffer, m_dwBufferSize, &dwBytesRead, NULL);

		if (dwBytesRead < m_dwBufferSize)
		{
			return E_FAIL;
		}

		if (br == FALSE)
		{
			printf("Failed to read audio from file.\n");

			return E_FAIL;
		}

		if (pOut != NULL)
		{
			hr = pOut->putAudio(m_bBuffer, dwBytesRead);

			if (hr == AUDCLNT_E_BUFFER_TOO_LARGE)
			{
				SetFilePointer(m_hFile, -(LONG)dwBytesRead, NULL, FILE_CURRENT);
			}
		}
	}

	if (hr == AUDCLNT_E_BUFFER_TOO_LARGE)
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT FileHandler::stopCapture()
{
	if (m_bBuffer != NULL)
	{
		delete[] m_bBuffer;
		m_bBuffer = NULL;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return S_OK;
}

HRESULT FileHandler::openForPlayback()
{
	printf("Opening file \"%S\" for writing\n", m_sFilename);

	m_hFile = CreateFile(m_sFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		printf("Failed to open file.\n");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT FileHandler::putAudioFormat(const WAVEFORMATEXTENSIBLE* pFormat)
{
	BOOL br;

	DWORD dwBytesWritten = 0;

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	br = WriteFile(m_hFile, pFormat, sizeof(WAVEFORMATEXTENSIBLE), &dwBytesWritten, NULL);

	if ((br == FALSE) || (dwBytesWritten < sizeof(WAVEFORMATEXTENSIBLE)))
	{
		printf("Failed to write audio format to file.\n");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT FileHandler::startPlayback()
{
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT FileHandler::putAudio(const BYTE* pData, UINT32 numBytes)
{
	BOOL br;

	DWORD dwBytesWritten = 0;

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	br = WriteFile(m_hFile, pData, numBytes, &dwBytesWritten, NULL);

	if ((br == FALSE) || (dwBytesWritten < numBytes))
	{
		printf("Failed to write audio to file.\n");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT FileHandler::stopPlayback()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return S_OK;
}

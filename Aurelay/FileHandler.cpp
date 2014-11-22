#include "stdafx.h"
#include "FileHandler.h"


FileHandler::FileHandler(LPCTSTR sFilename)
{
	m_sFilename = sFilename;
	m_hFile = INVALID_HANDLE_VALUE;
}

FileHandler::~FileHandler()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}
}

HRESULT FileHandler::openForCapture()
{
	return E_NOTIMPL;
}

HRESULT FileHandler::getAudioFormat(WAVEFORMATEXTENSIBLE* pFormat)
{
	return E_NOTIMPL;
}

HRESULT FileHandler::startCapture()
{
	return E_NOTIMPL;
}

HRESULT FileHandler::getAudio(IAudioOut* pOut)
{
	return E_NOTIMPL;
}

HRESULT FileHandler::stopCapture()
{
	return E_NOTIMPL;
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

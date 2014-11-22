#include "stdafx.h"

#include "AudioDevice.h"

#include <Functiondiscoverykeys_devpkey.h>

AudioDevice::AudioDevice()
{
	m_pAudioClient = NULL;
	m_pCaptureClient = NULL;
}


AudioDevice::~AudioDevice()
{
	if (m_pCaptureClient != NULL)
	{
		m_pCaptureClient->Release();
	}

	if (m_pAudioClient != NULL)
	{
		m_pAudioClient->Release();
	}
}

HRESULT AudioDevice::openForCapture()
{
	HRESULT hr;

	WAVEFORMATEX* pwfx;
	UINT32 bufferFrameCount;

	if (m_pCaptureClient != NULL)
	{
		printf("Failed to open for capture: already opened\n");
		return S_FALSE;
	}

	hr = openAudioDevice();

	if (hr == S_OK)
	{
		hr = m_pAudioClient->GetMixFormat(&pwfx);

		if (hr != S_OK)
		{
			printf("Failed to start capture: cannot get mix format, 0x%08x\n", hr);
		}
		else
		{
			if (pwfx->wFormatTag != WAVE_FORMAT_EXTENSIBLE)
			{
				printf("Failed to start capture: mix format not of extensible type\n");
			}
			else
			{
				m_audioFormat = *(WAVEFORMATEXTENSIBLE*)pwfx;

				hr = m_pAudioClient->Initialize(
					AUDCLNT_SHAREMODE_SHARED,
					AUDCLNT_STREAMFLAGS_LOOPBACK,
					c_hnsBufferLength,
					0,
					pwfx,
					NULL);

				if (hr != S_OK)
				{
					printf("Failed to start capture: cannot initialize, 0x%08x\n", hr);
				}
				else
				{
					hr = m_pAudioClient->GetBufferSize(&bufferFrameCount);

					if (hr != S_OK)
					{
						printf("Failed to start capture: cannot get buffer size, 0x%08x\n", hr);
					}
					else
					{
						printf("Allocated capture buffer size in samples:%d\n", bufferFrameCount);

						hr = m_pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pCaptureClient);

						if (hr != S_OK)
						{
							printf("Failed to start capture: cannot open capture client, 0x%08x\n", hr);
						}
					}
				}
			}
		}

		if (hr != S_OK)
		{
			m_pAudioClient->Release();
			m_pAudioClient = NULL;
		}
	}

	return hr;
}

HRESULT AudioDevice::getAudioFormat(WAVEFORMATEXTENSIBLE* pFormat)
{
	if (m_pCaptureClient == NULL)
	{
		return S_FALSE;
	}

	*pFormat = m_audioFormat;

	return S_OK;
}

HRESULT AudioDevice::startCapture()
{
	HRESULT hr;

	if (m_pCaptureClient == NULL)
	{
		return S_FALSE;
	}

	hr = m_pAudioClient->Start();

	if (hr != S_OK)
	{
		printf("Failed to start capture: 0x%08x\n", hr);
	}

	return hr;
}

HRESULT AudioDevice::getAudio(IAudioOut* pOut)
{
	HRESULT hr, hr2;

	UINT32 numFramesAvailable;
	BYTE *pData;
	DWORD flags;

	if (m_pCaptureClient == NULL)
	{
		return S_FALSE;
	}

	hr = m_pCaptureClient->GetNextPacketSize(&numFramesAvailable);

	while ((hr == S_OK) && (numFramesAvailable != 0))
	{
		hr = m_pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);

		if (hr == S_OK)
		{
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				printf(".");
			}
			else
			{
				printf("*");
				if (pOut != NULL)
				{
					hr = pOut->putAudio(pData, numFramesAvailable * m_audioFormat.Format.nBlockAlign);
				}
			}

			hr2 = m_pCaptureClient->ReleaseBuffer(numFramesAvailable);

			if (hr2 != S_OK)
			{
				hr = hr2;
			}
		}

		if (hr == S_OK)
		{
			hr = m_pCaptureClient->GetNextPacketSize(&numFramesAvailable);
		}
	}

	return hr;
}

HRESULT AudioDevice::stopCapture()
{
	HRESULT hr;

	if (m_pAudioClient != NULL)
	{
		hr = m_pAudioClient->Stop();

		if (hr != S_OK)
		{
			printf("Failed to stop capturing, 0x%08x\n", hr);
		}
	}

	if (m_pCaptureClient != NULL)
	{
		m_pCaptureClient->Release();
		m_pCaptureClient = NULL;
	}

	if (m_pAudioClient != NULL)
	{
		m_pAudioClient->Release();
		m_pAudioClient = NULL;
	}

	return hr;
}

HRESULT AudioDevice::openForPlayback()
{
	return E_NOTIMPL;
}

HRESULT AudioDevice::putAudioFormat(const WAVEFORMATEXTENSIBLE* pFormat)
{
	return E_NOTIMPL;
}

HRESULT AudioDevice::startPlayback()
{
	return E_NOTIMPL;
}

HRESULT AudioDevice::putAudio(const BYTE* pData, UINT32 numBytes)
{
	return E_NOTIMPL;
}

HRESULT AudioDevice::stopPlayback()
{
	return E_NOTIMPL;
}

HRESULT AudioDevice::openAudioDevice()
{
	HRESULT hr;

	IMMDeviceEnumerator* pEnumerator;
	IMMDevice* pDevice;
	IPropertyStore* pPropertyStore;

	PROPVARIANT varName;

	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator);

	if (hr != S_OK)
	{
		printf("Failed to open audio device: cannot instantiate enumerator, 0x%08x\n", hr);
	}
	else
	{
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

		pEnumerator->Release();

		if (hr != S_OK)
		{
			printf("Failed to open audio device: cannot get render device, 0x%08x\n", hr);
		}
		else
		{
			hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);

			if (hr != S_OK)
			{
				printf("Failed to open audio device: cannot get property store, 0x%08x\n", hr);
			}
			else
			{
				PropVariantInit(&varName);

				hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &varName);

				if (hr != S_OK)
				{
					printf("Failed to open audio device: cannot retrieve friendly name, 0x%08x\n", hr);
				}
				else
				{
					printf("Going to open audio device \"%S\"\n", varName.pwszVal);

					hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioClient);

					if (hr != S_OK)
					{
						printf("Failed to open audio device: cannot activate audio client, 0x%08x\n", hr);
					}
				}

				PropVariantClear(&varName);
				pPropertyStore->Release();
			}

			pDevice->Release();
		}
	}

	return hr;
}

#include "stdafx.h"

#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>

#include "AudioDevice.h"


AudioDevice::AudioDevice()
{
}


AudioDevice::~AudioDevice()
{
}

HRESULT AudioDevice::openAudioCaptureDevice()
{
	HRESULT hr;

	IMMDeviceEnumerator* pEnumerator;
	IMMDevice* pDevice;
	IPropertyStore* pPropertyStore;
	IAudioClient* pAudioClient;
	IAudioCaptureClient *pCaptureClient;

	PROPVARIANT varName;
	WAVEFORMATEX* pwfx;
	WAVEFORMATEXTENSIBLE* pwfxt;
	UINT32 bufferFrameCount;

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

					hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);

					if (hr != S_OK)
					{
						printf("Failed to open audio device: cannot activate audio client, 0x%08x\n", hr);
					}
					else
					{
						hr = pAudioClient->GetMixFormat(&pwfx);

						if (hr != S_OK)
						{
							printf("Failed to open audio device: cannot get mix format, 0x%08x\n", hr);
						}
						else
						{
							if (pwfx->wFormatTag != WAVE_FORMAT_EXTENSIBLE)
							{
								printf("Failed to open audio device: mix format not of extensible type\n");
							}
							else
							{
								pwfxt = (WAVEFORMATEXTENSIBLE*)pwfx;
								printf("Number of channels:%d\n", pwfxt->Format.nChannels);
								printf("Sample rate:%d\n", pwfxt->Format.nSamplesPerSec);
								printf("Sample bitsize:%d (of %d)\n", pwfxt->Samples.wValidBitsPerSample, pwfxt->Format.wBitsPerSample);
								printf("Average bytes per sec:%d\n", pwfxt->Format.nAvgBytesPerSec);
								printf("Block size:%d\n", pwfxt->Format.nBlockAlign);
								printf("Samples per block:%d\n", pwfxt->Samples.wSamplesPerBlock);
								printf("Channel mask:0x%08x\n", pwfxt->dwChannelMask);

								hr = pAudioClient->Initialize(
									AUDCLNT_SHAREMODE_SHARED,
									AUDCLNT_STREAMFLAGS_LOOPBACK,
									hnsBufferLength,
									0,
									pwfx,
									NULL);

								if (hr != S_OK)
								{
									printf("Failed to open audio device: cannot initialize, 0x%08x\n", hr);
								}
								else
								{
									hr = pAudioClient->GetBufferSize(&bufferFrameCount);

									if (hr != S_OK)
									{
										printf("Failed to open audio device: cannot get buffer size, 0x%08x\n", hr);
									}
									else
									{
										printf("Buffer size in samples:%d\n", bufferFrameCount);
										hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient);

										if (hr != S_OK)
										{
											printf("Failed to open audio device: cannot open capture client, 0x%08x\n", hr);
										}
										else
										{
											pCaptureClient->Release();
										}
									}
								}
							}
						}

						pAudioClient->Release();
					}
				}

				PropVariantClear(&varName);
				pPropertyStore->Release();
			}

			pDevice->Release();
		}

		pEnumerator->Release();
	}

	return hr;
}

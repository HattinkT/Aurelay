#include <Windows.h>

#ifndef __AUDIOINTERFACES__
#define __AUDIOINTERFACES__

class IAudioOut
{
public:
	virtual HRESULT openForPlayback() = 0;
	virtual HRESULT startPlayback() = 0;
	virtual HRESULT stopPlayback() = 0;
	virtual HRESULT putAudio() = 0;
};


class IAudioIn
{
public:
	virtual HRESULT openForCapture() = 0;
	virtual HRESULT startCapture() = 0;
	virtual HRESULT stopCapture() = 0;
	virtual HRESULT getAudio(IAudioOut* pOut) = 0;
};

#endif

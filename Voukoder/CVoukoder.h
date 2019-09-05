#pragma once

#include <stdint.h>
#include "ComBase.h"
#include "IVoukoder.h"
#include <EncoderEngine.h>

#define MAX_CHANNELS 6

class CVoukoder : public CComBase<>, public InterfaceImpl<IVoukoder>
{
public:
	CVoukoder();
	virtual ~CVoukoder();

	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv);
	STDMETHOD(Open)(const wchar_t* filename, const wchar_t* application, const int passes, const int width, const int height, const rational timebase, const rational aspectratio, const fieldorder fieldorder, const int samplerate, const char* channellayout);
	STDMETHOD(Close)(bool finalize = true);
	STDMETHOD(SetConfig)(VOUKODER_CONFIG config);
	STDMETHOD(GetConfig)(VOUKODER_CONFIG *config);
	STDMETHOD(IsAudioWaiting)();
	STDMETHOD(SendAudioSamples)(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, const char* layout, const char* format);
	STDMETHOD(SendVideoFrame)(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, const char* format);
	STDMETHOD(ShowVoukoderDialog)(HANDLE act_ctx = NULL, HINSTANCE instance = NULL);

private:
	ExportInfo exportInfo;
	EncoderEngine* encoder;
	int64_t aPts;
	int64_t vPts;
	int audioBufferPos = 0;
	uint8_t** audioBuffer;
	AVFrame* aFrame = NULL;
	AVFrame* vFrame = NULL;
};
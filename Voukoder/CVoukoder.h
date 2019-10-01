#pragma once

#include <stdint.h>
#include "ComBase.h"
#include "IVoukoder.h"
#include <EncoderEngine.h>

class CVoukoder : public CComBase<>, public InterfaceImpl<IVoukoder>
{
public:
	CVoukoder();
	virtual ~CVoukoder();

	STDMETHOD_(void, Close)(bool finalize = true);
	STDMETHOD_(void, GetConfig)(Voukoder::CONFIG *config);
	STDMETHOD_(void, GetAudioChunkSize)(int* chunkSize);
	STDMETHOD_(bool, GetFileExtension)(std::wstring& extension);
	STDMETHOD_(int, GetMaxPasses());
	STDMETHOD_(bool, IsAudioActive());
	STDMETHOD_(bool, IsAudioWaiting)();
	STDMETHOD_(bool, IsVideoActive());
	STDMETHOD_(void, Log)(std::string text);
	STDMETHOD_(bool, Open)(const Voukoder::INFO info);
	STDMETHOD_(bool, SendAudioSampleChunk)(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, const Voukoder::ChannelLayout layout, const char* format);
	STDMETHOD_(bool, SendVideoFrame)(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, int pass, const char* format);
	STDMETHOD_(bool, ShowVoukoderDialog)(bool video = true, bool audio = true, HANDLE act_ctx = NULL, HINSTANCE instance = NULL);
	STDMETHOD_(void, SetConfig)(Voukoder::CONFIG& config);
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv);

private:
	ExportInfo exportInfo;
	EncoderEngine* encoder = NULL;
	int64_t aPts;
	int64_t vPts;
};
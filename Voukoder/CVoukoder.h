#pragma once

#include <stdint.h>
#include "ComBase.h"
#include "VoukoderTypeLib_h.h"
#include <EncoderEngine.h>

class CVoukoder : public CComBase<>, public InterfaceImpl<IVoukoder>
{
public:
	CVoukoder();
	virtual ~CVoukoder();

	STDMETHOD(Close)(BOOL finalize);
	STDMETHOD(GetConfig)(VKENCODERCONFIG *config);
	STDMETHOD(GetAudioChunkSize)(UINT* chunkSize);
	STDMETHOD(GetFileExtension)(BSTR* extension);
	STDMETHOD(GetMaxPasses(UINT* passes));
	STDMETHOD(IsAudioActive(BOOL* isActive));
	STDMETHOD(IsAudioWaiting)(BOOL* isWaiting);
	STDMETHOD(IsVideoActive(BOOL* isActive));
	STDMETHOD(Log)(BSTR text);
	STDMETHOD(Open)(const VKENCODERINFO info);
	STDMETHOD(SendAudioSampleChunk)(VKAUDIOCHUNK chunk);
	STDMETHOD(SendVideoFrame)(VKVIDEOFRAME frame);
	STDMETHOD(ShowVoukoderDialog)(BOOL video, BOOL audio, BOOL* isOkay, HANDLE act_ctx, HINSTANCE instance);
	STDMETHOD(SetConfig)(VKENCODERCONFIG config);
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv);

private:
	ExportInfo exportInfo;
	EncoderEngine* encoder = NULL;
};
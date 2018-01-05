#pragma once

#include "premiere_cs6\PrSDKSequenceRenderSuite.h"
#include "premiere_cs6\PrSDKPPix2Suite.h"
#include "premiere_cs6\PrSDKImageProcessingSuite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#include "premiere_cs6\PrSDKExporterUtilitySuite.h"
#include "Encoder.h"

typedef char fourchars[4];

union M128 {
	struct plane { fourchars a, y, u, v; } plane;
	__m128i i128;
};

template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
	template <typename... Args>
	static Ret callback(Args... args) {
		return func(args...);
	}
	static function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
function<Ret(Params...)> Callback<Ret(Params...)>::func;

class VideoRenderer
{
public:
	explicit VideoRenderer(csSDK_uint32 pluginID, csSDK_uint32 width, csSDK_uint32 height, PrPixelFormat pixelFormat, PrSDKPPixSuite *ppixSuite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite, PrSDKImageProcessingSuite *imageProcessingSuite);
	~VideoRenderer();
	virtual prSuiteError render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback) = 0;

protected:
	PrPixelFormat pixelFormat;
	csSDK_uint32 width;
	csSDK_uint32 height;
	PrSDKPPixSuite *ppixSuite;
	PrSDKMemoryManagerSuite *memorySuite;
	PrSDKExporterUtilitySuite *exporterUtilitySuite;
	PrSDKImageProcessingSuite *imageProcessingSuite;
	char *conversionBuffer;
	EncodingData encodingData;
	function<bool(EncodingData)> callback;
	csSDK_uint32 videoRenderID;
	
	virtual prSuiteError frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData) = 0;
	void deinterleave(char* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV);
	void deinterleave(char* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV, char *bufferA);
	static bool isBt709(PrPixelFormat format);
};

class StandardVideoRenderer : public VideoRenderer
{
	using VideoRenderer::VideoRenderer;

public:
	prSuiteError render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback) override;

private:
	prSuiteError frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData);
};

class AccurateVideoRenderer : public VideoRenderer
{
	using VideoRenderer::VideoRenderer;

public:
	prSuiteError render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback) override;

private:
	prSuiteError frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData);
};
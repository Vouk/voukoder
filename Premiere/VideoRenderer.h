#pragma once

#include <tmmintrin.h>
#include <functional>
#include "premiere_cs6\PrSDKPPix2Suite.h"
#include "premiere_cs6\PrSDKImageProcessingSuite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#include "premiere_cs6\PrSDKExporterUtilitySuite.h"
#include "EncodingData.h"
#include "Structs.h"

using namespace std;

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
	VideoRenderer(csSDK_uint32 pluginID, csSDK_uint32 width, csSDK_uint32 height, PrSDKPPixSuite *ppixSuite, PrSDKPPix2Suite *ppix2Suite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite, PrSDKImageProcessingSuite *imageProcessingSuite);
	~VideoRenderer();
	prSuiteError render(ColorSpace colorSpace, PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData*)> callback);

private:
	EncodingData encodingData;
	char *conversionBuffer;
	ColorSpace colorSpace;
	csSDK_uint32 width;
	csSDK_uint32 height;
	PrSDKPPixSuite *ppixSuite;
	PrSDKPPix2Suite *ppix2Suite;
	PrSDKMemoryManagerSuite *memorySuite;
	PrSDKExporterUtilitySuite *exporterUtilitySuite;
	PrSDKImageProcessingSuite *imageProcessingSuite;
	function<bool(EncodingData*)> callback;
	csSDK_uint32 videoRenderID;

	prSuiteError frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData);
	prSuiteError frameFinished(EncodingData *encodingData, PrPixelFormat inFormat, const csSDK_uint32 inFrameRepeatCount);
	void deinterleave(char* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV);
	void deinterleave(float* pixels, csSDK_int32 rowBytes, char *bufferY, char *bufferU, char *bufferV, char *bufferA);
	void deinterleave_avx_fma(char* __restrict pixels, int rowBytes, char *__restrict bufferY, char *__restrict bufferU, char *__restrict bufferV, char *__restrict bufferA);
	static bool isBt709(PrPixelFormat format);
	static bool isPlanar(PrPixelFormat format);
	static bool isHighBitDepth(PrPixelFormat format);
};

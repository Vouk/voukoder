#pragma once

#include "premiere_cs6\PrSDKSequenceRenderSuite.h"
#include "premiere_cs6\PrSDKPPix2Suite.h"
#include "premiere_cs6\PrSDKImageProcessingSuite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#include "premiere_cs6\PrSDKExporterUtilitySuite.h"
#include "Encoder.h"
#include <functional>
typedef char fourchars[4];

union M128 {
	struct plane { fourchars a, y, u, v; } plane;
	__m128i i128;
};

class VideoRenderer
{
public:
	typedef std::function<int(int)> CallbackFunction;

	PrSDKPPixSuite *ppixSuite;
	PrSDKPPix2Suite *ppix2Suite;
	PrSDKImageProcessingSuite *imageProcessingSuite;
	PrSDKMemoryManagerSuite *memorySuite;
	PrSDKExporterUtilitySuite *exporterUtilitySuite;
	EncodingData encodingData;
	function<bool(EncodingData)> callback;
	VideoRenderer(csSDK_uint32 pluginID, csSDK_uint32 width, csSDK_uint32 height, PrPixelFormat pixelFormat, PrSDKPPixSuite *ppixSuite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite);
	~VideoRenderer();
	prSuiteError renderFrames(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback);
	prSuiteError deinterleave(PPixHand frame, char *bufferY, char *bufferU, char *bufferV);
	prSuiteError deinterleave(PPixHand renderedFrame, char *bufferY, char *bufferU, char *bufferV, char *bufferA);

private:
	csSDK_uint32 videoRenderID;
	PrPixelFormat pixelFormat;
	csSDK_uint32 width;
	csSDK_uint32 height;
	
};
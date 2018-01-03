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

class VideoRenderer
{
public:
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
	explicit VideoRenderer(csSDK_uint32 pluginID, csSDK_uint32 width, csSDK_uint32 height, PrPixelFormat pixelFormat, PrSDKPPixSuite *ppixSuite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite, PrSDKImageProcessingSuite *imageProcessingSuite);
	~VideoRenderer();
	virtual prSuiteError render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback) = 0;
	prSuiteError deinterleave(PPixHand frame, char *bufferY, char *bufferU, char *bufferV);
	prSuiteError deinterleave(PPixHand renderedFrame, char *bufferY, char *bufferU, char *bufferV, char *bufferA);

};

class StandardVideoRenderer : public VideoRenderer
{
	using VideoRenderer::VideoRenderer;

public:
	prSuiteError render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback) override;
};

class AccurateVideoRenderer : public VideoRenderer
{
	using VideoRenderer::VideoRenderer;

public:
	prSuiteError render(PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncodingData)> callback) override;
};
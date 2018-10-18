#pragma once

#include "premiere_cs6\PrSDKPPix2Suite.h"
#include "premiere_cs6\PrSDKImageProcessingSuite.h"
#include "premiere_cs6\PrSDKMemoryManagerSuite.h"
#include "premiere_cs6\PrSDKExporterUtilitySuite.h"
#include "..\LibVKDR\LibVKDR.h"

using namespace std;
using namespace LibVKDR;

class VideoRenderer
{
public:
	VideoRenderer(csSDK_uint32 pluginID, csSDK_uint32 width, csSDK_uint32 height, PrSDKPPixSuite *ppixSuite, PrSDKPPix2Suite *ppix2Suite, PrSDKMemoryManagerSuite *memorySuite, PrSDKExporterUtilitySuite *exporterUtilitySuite);
	~VideoRenderer();
	prSuiteError render(PrPixelFormat pixelFormat, PrTime startTime, PrTime endTime, csSDK_uint32 passes, function<bool(EncoderData*)> callback);
	static PrPixelFormat GetTargetRenderFormat(ExportSettings exportSettings);

private:
	csSDK_uint32 width;
	csSDK_uint32 height;
	EncoderData encodingData;
	PrSDKPPixSuite *ppixSuite;
	PrSDKPPix2Suite *ppix2Suite;
	PrSDKMemoryManagerSuite *memorySuite;
	PrSDKExporterUtilitySuite *exporterUtilitySuite;
	function<bool(EncoderData*)> callback;
	csSDK_uint32 videoRenderID;

	prSuiteError frameCompleteCallback(const csSDK_uint32 inWhichPass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData);
	prSuiteError frameFinished(EncoderData *encoderData, PrPixelFormat inFormat, const csSDK_uint32 inFrameRepeatCount);
	void unpackUint8(uint8_t *pixels, int rowBytes, uint8_t *bufferY, uint8_t *bufferU, uint8_t *bufferV);
	void unpackFloatToUint16(float* pixels, uint16_t *bufferY, uint16_t *bufferU, uint16_t *bufferV, uint16_t *bufferA);
	void flipImage(char* pixels, const csSDK_int32 rowBytes);
};

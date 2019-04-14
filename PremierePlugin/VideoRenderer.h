#pragma once

#include "Suites.h"
#include <ExportInfo.h>
#include <lavf.h>
#include <Callback.h>
#include <chrono>

#define vkdrReturnError(err) { \
	suites->exporterUtilitySuite->ReportEvent(pluginId, kEventTypeError, VKDR_APPNAME, err); \
	vkLogError(err); \
	return exportReturn_ErrLastErrorSet; }

class VideoRenderer
{
public:
	VideoRenderer(csSDK_uint32 pluginId, csSDK_uint32 width, csSDK_uint32 height, bool maxDepth, PrSuites *suites, std::function<bool(AVFrame *frame, int pass, int render, int process)> callback);
	prSuiteError render(PrPixelFormat pixelFormat, PrTime startTime, PrTime endTime, csSDK_uint32 passes);
	PrPixelFormat GetTargetRenderFormat(ExportInfo encoderInfo);
	PrTime GetPts();

private:
	PrSuites *suites;
	csSDK_uint32 pluginId;
	csSDK_uint32 width;
	csSDK_uint32 height;
	csSDK_int64 pts;
	csSDK_uint32 currentPass;
	bool maxDepth;
	std::function<bool(AVFrame *frame, int pass, int render, int process)> callback;
	void flipImage(char* pixels, const csSDK_int32 rowBytes);
	prSuiteError frameCompleteCallback(const csSDK_uint32 pass, const csSDK_uint32 inFrameNumber, const csSDK_uint32 inFrameRepeatCount, PPixHand inRenderedFrame, void* inCallbackData);
	prSuiteError frameFinished(AVFrame *frame, int pass, const csSDK_uint32 frameRepeatCount, int render);
	int createFrameFromBuffer(const uint8_t *pixels, const int rowBytes, AVFrame &frame);
	int createFrameFromBuffer(const float* pixels, AVFrame &frame);

	std::chrono::time_point<std::chrono::steady_clock> tp_frameStart;
	std::chrono::time_point<std::chrono::steady_clock> tp_frameProcess;
};
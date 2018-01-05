#pragma once

#include "Common.h"

struct FrameFilterOptions
{
	AVMediaType media_type;
	AVRational time_base;
	csSDK_int64 channel_layout;
	AVSampleFormat sample_fmt;
	csSDK_int32 width;
	csSDK_int32 height;
	AVPixelFormat pix_fmt;
	AVRational sar;
};

class FrameFilter
{
public:
	FrameFilter();
	~FrameFilter();
	int FrameFilter::configure(FrameFilterOptions options, const char *filters);
	int FrameFilter::sendFrame(AVFrame *frame);
	int FrameFilter::receiveFrame(AVFrame *frame);
private:
	AVFilterGraph *filterGraph;
	AVFilterContext *in_ctx = NULL;
	AVFilterContext *out_ctx = NULL;
};


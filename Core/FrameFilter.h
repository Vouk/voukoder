#pragma once

#include "lavf.h"

struct FrameFilterOptions
{
	int width = 0;
	int height = 0;
	AVMediaType media_type = AVMEDIA_TYPE_UNKNOWN;
	AVRational time_base = { 0, 0 };
	uint64_t channel_layout = 0;
	AVSampleFormat sample_fmt = AV_SAMPLE_FMT_NONE;
	AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
	AVRational sar = { 1, 1 };
};

class FrameFilter
{
public:
	FrameFilter();
	~FrameFilter();
	int configure(FrameFilterOptions options, const char *filters);
	int sendFrame(AVFrame *frame);
	int receiveFrame(AVFrame *frame);
private:
	AVFilterGraph *filterGraph = NULL;
	AVFilterContext *in_ctx = NULL;
	AVFilterContext *out_ctx = NULL;
};
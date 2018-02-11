#pragma once

#include "lavf.h"

namespace LibVKDR
{
	struct FrameFilterOptions
	{
		int width;
		int height;
		AVMediaType media_type;
		AVRational time_base;
		uint64_t channel_layout;
		AVSampleFormat sample_fmt;
		AVPixelFormat pix_fmt;
		AVRational sar;
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
		AVFilterGraph *filterGraph;
		AVFilterContext *in_ctx = NULL;
		AVFilterContext *out_ctx = NULL;
	};
}
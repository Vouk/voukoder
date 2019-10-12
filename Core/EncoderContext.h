#pragma once

#include "lavf.h"
#include "FrameFilter.h"

struct EncoderContext
{
	AVCodecContext* codecContext;
	AVStream* stream;
	FrameFilter* frameFilter;
	int64_t next_pts;
	char *stats_info;
	bool firstData;
};
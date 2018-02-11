#pragma once

#include <map>
#include "lavf.h"
#include "FrameFilter.h"

using namespace std;

namespace LibVKDR
{
	struct EncoderContext
	{
		AVCodecContext *codecContext;
		AVStream *stream;
		map<string, FrameFilter*> frameFilters;
		int64_t next_pts = 0;
	};
}
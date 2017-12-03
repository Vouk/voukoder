#pragma once

extern "C" {
#include "libavformat\avformat.h"
#include "libavcodec\avcodec.h"
#include "libavfilter\avfilter.h"
#include "libavutil\avutil.h"
#include "libavutil\channel_layout.h"
#include "libswscale\swscale.h"
#include "libavfilter\buffersrc.h"
#include "libavfilter\buffersink.h"
#include "libavutil\audio_fifo.h"
#include "libavutil\opt.h"
#include "libavutil\imgutils.h"
#include "libavutil\fifo.h"
#include "libavutil\log.h"
#include "libavutil\mathematics.h"
#include "x264.h"
#include "x265.h"
}

#pragma comment(lib, "libavformat.lib")
#pragma comment(lib, "libavcodec.lib")
#pragma comment(lib, "libavfilter.lib")
#pragma comment(lib, "libavutil.lib")
#pragma comment(lib, "libswscale.lib")
#pragma comment(lib, "libx264.lib")
#pragma comment(lib, "x265.lib")

#if defined(LIB_LIBAV) 
#pragma comment(lib, "libavresample.lib")
#endif  

#if defined(LIB_FFMPEG) 
#pragma comment(lib, "libpostproc.lib")
#pragma comment(lib, "libswresample.lib")
#endif  
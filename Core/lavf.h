#pragma once

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavutil/avutil.h"
#include "libavutil/channel_layout.h"
#include "libswscale/swscale.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavutil/fifo.h"
#include "libavutil/log.h"
#include "libavutil/mathematics.h"
#include "libavutil/spherical.h"
#include "libavutil/stereo3D.h"
#include "x264.h"
#include "x265.h"
}

#ifdef _WIN32
    #pragma comment(lib, "libavformat.lib")
    #pragma comment(lib, "libavcodec.lib")
    #pragma comment(lib, "libavfilter.lib")
    #pragma comment(lib, "libavutil.lib")
    #pragma comment(lib, "libswscale.lib")
    #pragma comment(lib, "libpostproc.lib")
    #pragma comment(lib, "libswresample.lib")
    #pragma comment(lib, "libx264.lib")
    #pragma comment(lib, "x265.lib")
    #pragma comment(lib, "zimg.lib")
    #pragma comment(lib, "mp3lame.lib")
    #pragma comment(lib, "ogg.lib")
    #pragma comment(lib, "vorbis.lib")
    #pragma comment(lib, "vorbisenc.lib")
    #pragma comment(lib, "vpx.lib")
    #pragma comment(lib, "opus.lib")
	#pragma comment(lib, "snappy.lib")    
	#pragma comment(lib, "mfx.lib")
	#pragma comment(lib, "Bcrypt.lib")
	#pragma comment(lib, "SvtAv1Enc.lib")
#else
    #pragma comment(lib, "libavformat.a")
    #pragma comment(lib, "libavcodec.a")
    #pragma comment(lib, "libavfilter.a")
    #pragma comment(lib, "libavutil.a")
    #pragma comment(lib, "libswscale.a")
    #pragma comment(lib, "libpostproc.a")
    #pragma comment(lib, "libswresample.a")
    #pragma comment(lib, "libx264.a")
    #pragma comment(lib, "x265.a")
    #pragma comment(lib, "zimg.a")
    #pragma comment(lib, "mp3lame.a")
    #pragma comment(lib, "ogg.a")
    #pragma comment(lib, "vorbis.a")
    #pragma comment(lib, "vorbisenc.a")
    #pragma comment(lib, "vpx.a")
    #pragma comment(lib, "opus.a")
    #pragma comment(lib, "snappy.a")
#endif

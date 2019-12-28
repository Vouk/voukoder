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
#include "libavutil/mastering_display_metadata.h"

#if __has_include("x264.h")
#include "x264.h"
#pragma comment(lib, "libx264.lib")
#endif

#if __has_include("x265.h")
#include "x265.h"
#pragma comment(lib, "x265.lib")
#endif
}

#pragma comment(lib, "libavformat.lib")
#pragma comment(lib, "libavcodec.lib")
#pragma comment(lib, "libavfilter.lib")
#pragma comment(lib, "libavutil.lib")
#pragma comment(lib, "libswscale.lib")
#pragma comment(lib, "libpostproc.lib")
#pragma comment(lib, "libswresample.lib")
#pragma comment(lib, "Bcrypt.lib")

#if __has_include("zimg.h")
#  pragma comment(lib, "zimg.lib")
#endif

#if __has_include("lame/lame.h")
#  pragma comment(lib, "mp3lame.lib")
#endif

#if __has_include("vorbis/codec.h")
#  pragma comment(lib, "ogg.lib")
#  pragma comment(lib, "vorbis.lib")
#  pragma comment(lib, "vorbisenc.lib")
#endif

#if __has_include("vpx/vpx_codec.h")
#  pragma comment(lib, "vpx.lib")
#endif

#if __has_include("opus/opus.h")
#  pragma comment(lib, "opus.lib")
#endif

#if __has_include("snappy.h")
#  pragma comment(lib, "snappy.lib")
#endif

#if __has_include("mfx/mfxenc.h")
#  pragma comment(lib, "mfx.lib")
#endif

#if __has_include("svt-av1/EbSvtAv1.h")
#  pragma comment(lib, "SvtAv1Enc.lib")
#endif
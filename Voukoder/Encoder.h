#pragma once

#include "ExporterX264Common.h"
#include "FrameFilter.h"

typedef struct AVContext
{
	AVCodec *codec;
	AVCodecContext *codecContext;
	AVStream *stream;
	AVDictionary *options = NULL;
	int64_t sampleCount = 0;
	int64_t next_pts = 0;
	FrameFilter *frameFilter;
} AVContext;

class Encoder
{
public:
	Encoder(const char *filename);
	~Encoder();
	void Encoder::setVideoCodec(const char *codec, csSDK_int32 width, csSDK_int32 height, AVPixelFormat pixelFormat, AVRational pixelAspectRation, AVRational timebase, AVFieldOrder fieldOrder);
	void Encoder::setAudioCodec(const char *codec, csSDK_int64 channellayout, csSDK_int64 bitrate, int sampleRate, csSDK_int32 frame_size);
	int Encoder::open(const char *videoOptions, const char *audioOptions);
	void Encoder::close();
	int Encoder::writeVideoFrame(char *data);
	int Encoder::writeAudioFrame(const uint8_t **data, int32_t sampleCount);
private:
	int Encoder::openStream(AVContext *context, AVDictionary *options);
	int Encoder::encodeAndWriteFrame(AVContext *context, AVFrame *frame);
	AVFormatContext *formatContext;
	AVContext *videoContext;
	AVContext *audioContext;
	const char *filename;
	AVAudioFifo *fifo = NULL;
};


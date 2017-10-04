#pragma once

#include "ExporterX264Common.h"
#include "FrameFilter.h"

enum FrameType {
	VideoFrame,
	AudioFrame
};

typedef struct AVContext
{
	AVCodec *codec;
	AVCodecContext *codecContext;
	AVStream *stream;
	AVDictionary *options = NULL;
	int64_t sampleCount = 0;
	int64_t next_pts = 0;
	FrameFilter *frameFilter;
	std::string configuration;
} AVContext;

class Encoder
{
public:
	Encoder(const char *short_name, const char *filename);
	~Encoder();
	void Encoder::setVideoCodec(const std::string codec, const std::string configuration, int width, int height, AVRational timebase);
	void Encoder::setAudioCodec(const std::string codec, const std::string configuration, csSDK_int64 channellayout, int sampleRate);
	int Encoder::open();
	void Encoder::close();
	int Encoder::writeVideoFrame(char *data);
	int Encoder::writeAudioFrame(const uint8_t **data, int32_t sampleCount);
	FrameType Encoder::getNextFrameType();
private:
	int Encoder::openStream(AVContext *context, std::string configuration);
	int Encoder::encodeAndWriteFrame(AVContext *context, AVFrame *frame);
	AVFormatContext *formatContext;
	AVContext *videoContext;
	AVContext *audioContext;
	const char *filename;
	AVAudioFifo *fifo = NULL;
};


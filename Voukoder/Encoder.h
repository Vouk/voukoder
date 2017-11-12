#pragma once

#include "Common.h"
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
} AVContext;

class Encoder
{
public:
	Encoder(const char *short_name, const char *filename);
	~Encoder();
	void Encoder::setVideoCodec(const std::string codec, AVDictionary *configuration, int width, int height, AVRational timebase, AVColorSpace colorSpace, AVColorRange colorRange, AVColorPrimaries colorPrimaries, AVColorTransferCharacteristic colorTransferCharateristic);
	void Encoder::setAudioCodec(const std::string codec, AVDictionary *configuration, csSDK_int64 channellayout, int sampleRate);
	int Encoder::open();
	void Encoder::close(bool writeTrailer);
	int Encoder::writeVideoFrame(EncodingData *encodingData);
	int Encoder::writeAudioFrame(const uint8_t **data, int32_t sampleCount);
	FrameType Encoder::getNextFrameType();
	const char* Encoder::getPixelFormat();
	const char* Encoder::dumpConfiguration();
private:
	int Encoder::openStream(AVContext *context);
	int Encoder::encodeAndWriteFrame(AVContext *context, AVFrame *frame);
	const char *filename;
	AVFormatContext *formatContext;
	AVContext *videoContext;
	AVContext *audioContext;
	AVAudioFifo *fifo = NULL;
};


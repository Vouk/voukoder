#pragma once

#include "Common.h"
#include "EncoderContext.h"
#include "FrameFilter.h"

typedef struct EncodingData
{
	int planes;
	const char *pix_fmt;
	char *plane[8] = {};
	unsigned int stride[8];
	int pass;
	struct filters
	{
		bool vflip = false;
		string scale = "";
	} filters;
} EncodingData;

enum FrameType {
	VideoFrame,
	AudioFrame
};

// reviewed 0.3.8
class Encoder
{
public:
	EncoderContext *videoContext;
	EncoderContext *audioContext;
	Encoder(const char *short_name, const char *filename);
	~Encoder();
	int open();
	void close(bool writeTrailer);
	int writeVideoFrame(EncodingData *encodingData);
	int writeAudioFrame(const uint8_t **data, int32_t sampleCount);
	FrameType getNextFrameType();

private:
	int encodeAndWriteFrame(EncoderContext *context, AVFrame *frame, FrameFilter *frameFilter);
	const char *filename;
	AVFormatContext *formatContext;
	AVAudioFifo *fifo = NULL;
};


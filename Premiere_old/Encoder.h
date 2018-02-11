#pragma once

#include "EncoderContext.h"
#include "FrameFilter.h"
#include "EncodingData.h"
#include "Structs.h"

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
	int writeAudioFrame(float **data, int32_t sampleCount);
	FrameType getNextFrameType();

private:
	int encodeAndWriteFrame(EncoderContext *context, AVFrame *frame, FrameFilter *frameFilter);
	const char *filename;
	AVFormatContext *formatContext;
};


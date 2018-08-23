#pragma once

#include <string>
#include "Encoder.h"
#include "EncoderData.h"
#include "EncoderContext.h"
#include "ExportSettings.h"
#include "Pipe.h"
#include "lavf.h"

using namespace std;

namespace LibVKDR
{
	enum FrameType {
		VideoFrame,
		AudioFrame
	};

	class Encoder
	{
	public:
		Encoder(ExportSettings exportSettings);
		int open();
		void close();
		void finalize();
		void flushContext(EncoderContext *encoderContext);
		int getAudioFrameSize();
		int testSettings();
		int writeVideoFrame(EncoderData *encoderData);
		int writeAudioFrame(float **data, int32_t sampleCount);
		FrameType getNextFrameType();
		int pass = 0;

	private:
		ExportSettings exportSettings;
		AVFormatContext *formatContext;
		EncoderContext videoContext;
		EncoderContext audioContext;
		Pipe pipe;
		int createCodecContext(string codecName, EncoderContext *encoderContext, int flags);
		int encodeAndWriteFrame(EncoderContext *context, AVFrame *frame);
		int getCodecFlags(const AVMediaType type);
		int openCodec(string codecName, const string options, EncoderContext *encoderContext, const int flags);
		int receivePackets(EncoderContext *context);
		int audioFrameSize;
	};
}
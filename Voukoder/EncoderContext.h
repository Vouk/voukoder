#pragma once

#include "lavf.h"
#include "EncoderConfig.h"
#include "FrameFilter.h"

struct EncoderContextInfo
{
	string name = "";
	int width = 0;
	int height = 0;
	AVRational timebase = { 0, 0 };
	uint64_t channelLayout = 0;
	AVColorSpace colorSpace = AVColorSpace::AVCOL_SPC_UNSPECIFIED;
	AVColorRange colorRange = AVColorRange::AVCOL_RANGE_UNSPECIFIED;
	AVColorPrimaries colorPrimaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
	AVColorTransferCharacteristic colorTRC = AVColorTransferCharacteristic::AVCOL_TRC_UNSPECIFIED;
};

// reviewed 0.3.8
class EncoderContext
{
public:
	AVCodec *codec;
	AVCodecContext *codecContext;
	AVStream *stream;
	map<string, FrameFilter*> frameFilters;
	EncoderConfig *encoderConfig;
	AVFrame *frame;
	int64_t next_pts = 0;
	EncoderContext(AVFormatContext *formatContext);
	~EncoderContext();
	int openCodec();
	void closeCodec();
	void setCodec(EncoderContextInfo encoderContestInfo, EncoderConfig *encoderConfig);
	void setCodecFlags(int flags);

private:
	AVFormatContext *formatContext;
	AVDictionary *options = NULL;
};
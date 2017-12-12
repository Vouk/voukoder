#pragma once

#include "lavf.h"

class Decoder
{
public:
	Decoder();
	int open(AVCodecID codecId, int width, int height);
	void close();
	AVFrame* decodeData(uint8_t *data, int size);

private:
	AVCodecContext *codecContext;
	AVPacket *packet;
	AVFrame *frame;
};


#include "Decoder.h"

Decoder::Decoder()
{
	packet = av_packet_alloc();
	frame = av_frame_alloc();
}

int Decoder::open(AVCodecID codecId, int width, int height)
{
	AVCodec *codec = avcodec_find_decoder(codecId);
	codecContext = avcodec_alloc_context3(codec);
	codecContext->width = width;
	codecContext->height = height;

	return avcodec_open2(codecContext, codec, NULL);
}

void Decoder::close()
{
	avcodec_free_context(&codecContext);
	av_frame_free(&frame);
	av_packet_free(&packet);
}

AVFrame* Decoder::decodeData(uint8_t *data, int size)
{
	// Fill packet
	packet->size = size;
	packet->data = data;

	char buf[1024];
	int ret;
	
	//
	ret = avcodec_send_packet(codecContext, packet);
	if (ret < 0)
	{
		fprintf(stderr, "Error sending a packet for decoding\n");
		exit(1);
	}

	//
	while (ret >= 0)
	{
		ret = avcodec_receive_frame(codecContext, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return NULL;
		else if (ret < 0) {
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}

		return frame;
	}


}
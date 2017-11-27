#include "Encoder.h"
#include "Common.h"

Encoder::Encoder(const char *short_name, const char *filename)
{
	this->filename = filename;

	formatContext = avformat_alloc_context();
	formatContext->oformat = av_guess_format(short_name, this->filename, NULL);

	videoContext = new AVContext();
	audioContext = new AVContext();
}

Encoder::~Encoder()
{	
	avformat_free_context(formatContext);
}

void Encoder::setVideoCodec(const std::string codec, EncoderConfig *configuration, int width, int height, AVRational timebase, AVColorSpace colorSpace, AVColorRange colorRange, AVColorPrimaries colorPrimaries, AVColorTransferCharacteristic colorTransferCharateristic, int codecFlags = 0)
{
	AVDictionary *options = NULL;
	configuration->getConfig(&videoContext->options, 1, 1); //TODO
	videoContext->config = configuration;

	// Find codec
	videoContext->codec = avcodec_find_encoder_by_name(codec.c_str());
	if (videoContext->codec == NULL)
	{
		return;
	}

	// Create the stream
	videoContext->stream = avformat_new_stream(formatContext, NULL);
	videoContext->stream->id = formatContext->nb_streams - 1;
	videoContext->stream->time_base = timebase;

	// Allocate the codec context
	videoContext->codecContext = avcodec_alloc_context3(videoContext->codec);
	videoContext->codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	videoContext->codecContext->codec_id = videoContext->codec->id;
	videoContext->codecContext->width = width;
	videoContext->codecContext->height = height;
	videoContext->codecContext->time_base = timebase;
	videoContext->codecContext->pix_fmt = videoContext->codec->pix_fmts ? videoContext->codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;
	videoContext->codecContext->flags |= codecFlags;

	if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
	{
		videoContext->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	int error = avcodec_parameters_from_context(videoContext->stream->codecpar, videoContext->codecContext);

	// Set color settings
	videoContext->codecContext->colorspace = colorSpace;
	videoContext->codecContext->color_range = colorRange;
	videoContext->codecContext->color_primaries = colorPrimaries;
	videoContext->codecContext->color_trc = colorTransferCharateristic;
}

void Encoder::setAudioCodec(const std::string codec, EncoderConfig *configuration, csSDK_int64 channelLayout, int sampleRate)
{
	AVDictionary *options = NULL;
	configuration->getConfig(&audioContext->options);
	audioContext->config = configuration;

	// Find codec
	audioContext->codec = avcodec_find_encoder_by_name(codec.c_str());
	if (audioContext->codec == NULL)
	{
		return;
	}

	/* Configure the audio encoder */
	audioContext->codecContext = avcodec_alloc_context3(audioContext->codec);
	audioContext->codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	audioContext->codecContext->channels = av_get_channel_layout_nb_channels(channelLayout);
	audioContext->codecContext->channel_layout = channelLayout;
	audioContext->codecContext->sample_rate = sampleRate;
	audioContext->codecContext->sample_fmt = audioContext->codec->sample_fmts ? audioContext->codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
	audioContext->codecContext->bit_rate = 0;

	/* Create the stream */
	audioContext->stream = avformat_new_stream(formatContext, NULL);
	audioContext->stream->id = formatContext->nb_streams - 1;
	audioContext->stream->time_base.den = audioContext->codecContext->sample_rate;
	audioContext->stream->time_base.num = 1;
	
	if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
	{
		audioContext->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	int error = avcodec_parameters_from_context(audioContext->stream->codecpar, audioContext->codecContext);
}

int Encoder::open()
{
	int ret;

	// Open video stream
	if ((ret = openStream(videoContext)) < 0)
	{
		close(false);

		return ret;
	}

	// Open audio stream
	if ((ret = openStream(audioContext)) > 0)
	{
		close(false);

		return ret;
	}

	// Create audio fifo buffer
	if (!(fifo = av_audio_fifo_alloc(audioContext->codecContext->sample_fmt, audioContext->codecContext->channels, 1))) 
	{
		close(false);

		return AVERROR_EXIT;
	}

	// Encoder to file or to memory?
	if (this->filename != NULL)
	{
		// Open the target file
		if ((ret = avio_open(&formatContext->pb, this->filename, AVIO_FLAG_WRITE)) != S_OK)
		{
			close(false);

			return ret;
		}
	}
	else
	{
		// Open a memory stream just to test muxer settings
		if ((ret = avio_open_dyn_buf(&formatContext->pb)) != S_OK)
		{
			close(false);

			return ret;
		}
	}

	// Check muxer/codec combination and write header
	if ((ret = avformat_write_header(formatContext, NULL)) != S_OK)
	{
		close(false);

		return ret;
	}

	return S_OK;
}

void Encoder::close(bool writeTrailer)
{
	// Write trailer
	if (writeTrailer)
	{
		av_write_trailer(formatContext);
	}

	if (videoContext->codecContext->internal != NULL)
	{
		avcodec_close(videoContext->codecContext);
	}
	if (audioContext->codecContext->internal != NULL)
	{
		avcodec_close(audioContext->codecContext);
	}

	// Close the file
	if (this->filename != NULL)
	{
		avio_close(formatContext->pb);
	}
	else
	{
		uint8_t *buffer = NULL;
		avio_close_dyn_buf(formatContext->pb, &buffer);
		av_free(buffer);
	}
}

int Encoder::openStream(AVContext *context)
{
	// Find the right pixel/sample format
	if (context->codec->type == AVMEDIA_TYPE_VIDEO)
	{
		const char* pix_fmt = context->config->getPixelFormat();
		context->codecContext->pix_fmt = av_get_pix_fmt(pix_fmt);

		/*
		const int len = sizeof(context->codec->pix_fmts) / sizeof(context->codec->pix_fmts[0]);
		for (int i = 0; i < len; i++)
		{
			AVPixelFormat format = context->codec->pix_fmts[i];
			const char *name = av_get_pix_fmt_name(format);
			OutputDebugStringA(name);
			OutputDebugStringA("\r\n");
		}
		*/
	}

	int ret;

	// Open the codec
	if ((ret = avcodec_open2(context->codecContext, context->codec, &context->options)) < 0)
	{
		return ret ;
	}

	// Copy the stream parameters to the context
	if ((ret = avcodec_parameters_from_context(context->stream->codecpar, context->codecContext)) < 0)
	{
		return ret;
	}

	return S_OK;
}

int Encoder::writeVideoFrame(EncodingData *encodingData)
{
	int ret;

	// Set up frame filter
	if (videoContext->frameFilter == NULL)
	{
		// Configure source format
		FrameFilterOptions options;
		options.media_type = videoContext->codec->type;
		options.width = videoContext->codecContext->width;
		options.height = videoContext->codecContext->height;
		options.pix_fmt = av_get_pix_fmt(encodingData->pix_fmt);
		options.time_base = videoContext->codecContext->time_base;
		options.sar.den = 1;
		options.sar.num = 1;

		// Set target format
		char filterConfig[256];
		sprintf_s(filterConfig, "format=pix_fmts=%s", videoContext->config->getPixelFormat());

		videoContext->frameFilter = new FrameFilter();
		videoContext->frameFilter->configure(options, filterConfig);
	}

	// Do we just want to flush the encoder?
	if (encodingData == NULL)
	{
		// Send the frame to the encoder
		if ((ret = encodeAndWriteFrame(videoContext, NULL)) < 0)
		{
			return ret;
		}

		// Destroy frame filter
		videoContext->frameFilter->~FrameFilter();
		videoContext->frameFilter = NULL;

		return S_OK;
	}

	// Create a new frame
	AVFrame *frame = av_frame_alloc();
	frame->width = videoContext->codecContext->width;
	frame->height = videoContext->codecContext->height;
	frame->format = av_get_pix_fmt(encodingData->pix_fmt);

	// Reserve buffer space
	if ((ret = av_frame_get_buffer(frame, 32)) < 0)
	{
		return ret;
	}

	// Fill the source frame
	frame->data[0] = (uint8_t*)encodingData->plane[0];
	frame->data[1] = (uint8_t*)encodingData->plane[1];
	frame->data[2] = (uint8_t*)encodingData->plane[2];
	frame->data[3] = (uint8_t*)encodingData->plane[3];
	frame->linesize[0] = encodingData->stride[0];
	frame->linesize[1] = encodingData->stride[1];
	frame->linesize[2] = encodingData->stride[2];
	frame->linesize[3] = encodingData->stride[3];

	// Presentation timestamp
	frame->pts = videoContext->next_pts++;
	
	// Send the frame to the encoder
	if ((ret = encodeAndWriteFrame(videoContext, frame)) < 0)
	{
		av_frame_free(&frame);
		return ret;
	}

	av_frame_free(&frame);

	return S_OK;
}

int Encoder::writeAudioFrame(const uint8_t **data, int32_t sampleCount)
{
	// Set up frame filter
	if (audioContext->frameFilter == NULL)
	{
		// Configure source format
		FrameFilterOptions options;
		options.media_type = audioContext->codec->type;
		options.channel_layout = audioContext->codecContext->channel_layout;
		options.sample_fmt = PLUGIN_AUDIO_SAMPLE_FORMAT;
		options.time_base = { 1, audioContext->codecContext->sample_rate };

		// Set target format
		char filterConfig[256];
		sprintf_s(filterConfig, "aformat=channel_layouts=stereo:sample_fmts=%s:sample_rates=%d", av_get_sample_fmt_name(audioContext->codecContext->sample_fmt), audioContext->codecContext->sample_rate);

		audioContext->frameFilter = new FrameFilter();
		audioContext->frameFilter->configure(options, filterConfig);
	}

	int ret = S_OK, err;
	bool finished = true;

	/* Do we have samples to add to the buffer? */
	if (data != NULL)
	{
		finished = false;

		/* Resize the buffer so it can store all samples */
		if ((err = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + sampleCount)) < 0)
		{
			return AVERROR_EXIT;
		}

		/* Add the new samples to the buffer */
		if (av_audio_fifo_write(fifo, (void **)data, sampleCount) < sampleCount)
		{
			return AVERROR_EXIT;
		}
	}

	/* Do we have enough samples for the encoder? */
	while (av_audio_fifo_size(fifo) >= audioContext->codecContext->frame_size || (finished && av_audio_fifo_size(fifo) > 0))
	{
		AVFrame *frame;

		int frame_size = FFMIN(av_audio_fifo_size(fifo), audioContext->codecContext->frame_size);
		/* ugly hack for PCM codecs (will be removed ASAP with new PCM
		   support to compute the input frame size in samples */
		if (frame_size <= 1) {
			frame_size = 10000 / 2;
frame_size >>= 1;

			}


		frame = av_frame_alloc();
		frame->nb_samples = frame_size;
		frame->channel_layout = audioContext->codecContext->channel_layout;
		frame->format = PLUGIN_AUDIO_SAMPLE_FORMAT;
		frame->sample_rate = audioContext->codecContext->sample_rate;

		/* Allocate the buffer for the frame */
		if ((err = av_frame_get_buffer(frame, 0)) < 0)
		{
			av_frame_free(&frame);
			return AVERROR_EXIT;
		}

		/* Fill buffers with data from the fifo */
		if (av_audio_fifo_read(fifo, (void **)frame->data, frame_size) < frame_size)
		{
			av_frame_free(&frame);
			return AVERROR_EXIT;
		}

		frame->pts = audioContext->next_pts;
		audioContext->next_pts += frame->nb_samples;

		/* Send the frame to the encoder */
		if ((ret = encodeAndWriteFrame(audioContext, frame)) < 0)
		{
			av_frame_free(&frame);
			return ret;
		}

		av_frame_free(&frame);
	}

	if (finished) 
	{
		// Flush the encoder
		if ((ret = encodeAndWriteFrame(audioContext, NULL)) < 0)
		{
			return ret;
		}

		// Destroy frame filter
		audioContext->frameFilter->~FrameFilter();
		audioContext->frameFilter = NULL;
	}

	return ret;
}

int Encoder::encodeAndWriteFrame(AVContext *context, AVFrame *frame)
{
	int ret;

	// In case we want to flush the encoder
	if (frame == NULL)
	{
		if ((ret = avcodec_send_frame(context->codecContext, frame)) != S_OK)
		{
			return AVERROR_UNKNOWN;
		}
	}
	else
	{
		// Send the uncompressed frame to frame filter
		if ((ret = context->frameFilter->sendFrame(frame)) != S_OK)
		{
			return AVERROR_UNKNOWN;
		}
	}

	AVFrame *tmp_frame;
	tmp_frame = av_frame_alloc();

	// Receive frames from the rame filter
	while ((ret = context->frameFilter->receiveFrame(tmp_frame)) >= 0)
	{
		// Send the frame to the encoder
		if ((ret = avcodec_send_frame(context->codecContext, tmp_frame)) != S_OK)
		{
			av_frame_free(&tmp_frame);
			return AVERROR_UNKNOWN;
		}

		av_frame_unref(tmp_frame);
	}

	av_frame_free(&tmp_frame);

	AVPacket *packet = av_packet_alloc();

	// If we receive a packet from the encoder write it to the stream
	while ((ret = avcodec_receive_packet(context->codecContext, packet)) >= 0)
	{
		av_packet_rescale_ts(packet, context->codecContext->time_base, context->stream->time_base);
		packet->stream_index = context->stream->index;
		av_interleaved_write_frame(formatContext, packet);
		av_packet_unref(packet);
	}

	return S_OK;
}

FrameType Encoder::getNextFrameType()
{
	if (av_compare_ts(videoContext->next_pts, videoContext->codecContext->time_base, audioContext->next_pts, audioContext->codecContext->time_base) <= 0)
	{
		return FrameType::VideoFrame;
	}

	return FrameType::AudioFrame;
}

const char* Encoder::getPixelFormat()
{
	return av_get_pix_fmt_name(videoContext->codecContext->pix_fmt);
}

const char* Encoder::dumpConfiguration()
{
	char buffer[1024];
	sprintf_s(
		buffer,
		"Muxer: %s\nVideo: %s(%s)\nAudio: %s(%s)",
		formatContext->oformat->name,
		videoContext->codec->name,
		"opt",
		audioContext->codec->name,
		"opt");

	return buffer;
}
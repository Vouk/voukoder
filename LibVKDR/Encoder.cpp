#include "Encoder.h"

using namespace LibVKDR;

Encoder::Encoder(ExportSettings exportSettings) :
	exportSettings(exportSettings)
{
	av_register_all();
	avfilter_register_all();

	formatContext = avformat_alloc_context();
	formatContext->oformat = av_guess_format(
		exportSettings.muxerName.c_str(),
		exportSettings.filename.c_str(),
		NULL);

	av_dict_set(&formatContext->metadata, "encoding_tool", LIB_VKDR_APPNAME, 0);

	pass = 0;
}

Encoder::~Encoder()
{
	avformat_free_context(formatContext);
}

int Encoder::createCodecContext(string codecName, EncoderContext *encoderContext)
{
	AVCodec *codec = avcodec_find_encoder_by_name(codecName.c_str());
	if (codec == NULL)
	{
		return AVERROR_ENCODER_NOT_FOUND;
	}

	encoderContext->codecContext = avcodec_alloc_context3(codec);
	encoderContext->codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	encoderContext->codecContext->thread_count = 0;

	if (codec->type == AVMEDIA_TYPE_VIDEO)
	{
		encoderContext->codecContext->width = exportSettings.width;
		encoderContext->codecContext->height = exportSettings.height;
		encoderContext->codecContext->time_base.num = exportSettings.videoTimebaseNum;
		encoderContext->codecContext->time_base.den = exportSettings.videoTimebaseDen;
		encoderContext->codecContext->pix_fmt = av_get_pix_fmt(exportSettings.pixelFormat.c_str());
		encoderContext->codecContext->colorspace = exportSettings.colorSpace;
		encoderContext->codecContext->color_range = exportSettings.colorRange;
		encoderContext->codecContext->color_primaries = exportSettings.colorPrimaries;
		encoderContext->codecContext->color_trc = exportSettings.colorTRC;
	}
	else if (codec->type == AVMEDIA_TYPE_AUDIO)
	{
		encoderContext->codecContext->channels = av_get_channel_layout_nb_channels(exportSettings.audioChannelLayout);
		encoderContext->codecContext->channel_layout = exportSettings.audioChannelLayout;
		encoderContext->codecContext->time_base.num = exportSettings.audioTimebaseNum;
		encoderContext->codecContext->time_base.den = exportSettings.audioTimebaseDen;
		encoderContext->codecContext->sample_rate = exportSettings.audioTimebaseDen;
		encoderContext->codecContext->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		encoderContext->codecContext->bit_rate = 0;
	}

	encoderContext->stream = avformat_new_stream(formatContext, codec);
	encoderContext->stream->id = formatContext->nb_streams - 1;
	encoderContext->stream->time_base = encoderContext->codecContext->time_base;

	if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
	{
		encoderContext->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	avcodec_parameters_from_context(encoderContext->stream->codecpar, encoderContext->codecContext);

	return 0;
}

int Encoder::openCodec(const string codecName, const string options, EncoderContext *encoderContext)
{
	int ret = 0;

	if ((ret = createCodecContext(codecName.c_str(), encoderContext)) == 0)
	{
		AVDictionary *dictionary = NULL;
		if ((ret = av_dict_parse_string(&dictionary, options.c_str(), "=", ",", 0)) == 0)
		{
			if ((ret = avcodec_open2(encoderContext->codecContext, encoderContext->codecContext->codec, &dictionary)) == 0)
			{
				return avcodec_parameters_from_context(encoderContext->stream->codecpar, encoderContext->codecContext);
			}
		}
	}

	return ret;
}

int Encoder::open()
{
	int ret;

	if ((ret = openCodec(
		exportSettings.videoCodecName.c_str(),
		exportSettings.videoOptions,
		&videoContext)) < 0)
	{
		return ret;
	}

	if ((ret = openCodec(
		exportSettings.audioCodecName.c_str(),
		exportSettings.audioOptions,
		&audioContext)) < 0)
	{
		return ret;
	}

	if ((ret = avio_open(
		&formatContext->pb, 
		exportSettings.filename.c_str(), 
		AVIO_FLAG_WRITE)) < 0)
	{
		close(false);

		return ret;
	}

	AVDictionary *options = NULL;
	//av_dict_set(&options, "movflags", "faststart", 0);

	// Check muxer/codec combination and write header
	ret = avformat_write_header(formatContext, &options);

	if (ret < 0)
		close(false);

	return ret;
}

void Encoder::close(bool writeTrailer)
{
	if (writeTrailer)
	{
		flushContext(&videoContext);
		flushContext(&audioContext);

		av_write_trailer(formatContext);
	}

	if (videoContext.codecContext != NULL && videoContext.codecContext->internal != NULL)
		avcodec_free_context(&videoContext.codecContext);

	if (audioContext.codecContext != NULL && audioContext.codecContext->internal != NULL)
		avcodec_free_context(&audioContext.codecContext);

	if (exportSettings.filename.length() > 0)
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

int Encoder::testSettings()
{
	int ret;

	if ((ret = openCodec(
		exportSettings.videoCodecName.c_str(),
		exportSettings.videoOptions,
		&videoContext)) < 0)
	{
		return ret;
	}

	if ((ret = openCodec(
		exportSettings.audioCodecName.c_str(),
		exportSettings.audioOptions,
		&audioContext)) < 0)
	{
		return ret;
	}

	if ((ret = avio_open_dyn_buf(&formatContext->pb)) < 0)
	{
		goto close;
	}
	
	AVDictionary *options = NULL;
	if ((ret = avformat_write_header(formatContext, &options)) < 0)
	{
		goto close;
	}

close:

	close(false);

	return ret;
}

int Encoder::prepare(EncoderData *encoderData)
{
	int ret;

	if (pass == 0 || (exportSettings.passes > 1 && encoderData->pass > pass))
	{
		if (pass > 0)
		{
			close(true);
		}

		pass = encoderData->pass;

		if ((ret = open()) < 0)
		{
			return ret;
		}

		if (exportSettings.fieldOrder != AVFieldOrder::AV_FIELD_PROGRESSIVE)
		{
			videoContext.codecContext->flags |= AV_CODEC_FLAG_INTERLACED_DCT | AV_CODEC_FLAG_INTERLACED_ME;
		}

		if (exportSettings.passes > 1)
		{
			if (pass == 1)
			{
				videoContext.codecContext->flags |= AV_CODEC_FLAG_PASS1;
			}
			else
			{
				videoContext.codecContext->flags |= AV_CODEC_FLAG_PASS2;
			}
		}
	}

	return 0;
}

FrameType Encoder::getNextFrameType()
{
	if (av_compare_ts(videoContext.next_pts, videoContext.codecContext->time_base, audioContext.next_pts, audioContext.codecContext->time_base) <= 0)
	{
		return FrameType::VideoFrame;
	}

	return FrameType::AudioFrame;
}

void Encoder::flushContext(EncoderContext *encoderContext)
{
	encodeAndWriteFrame(encoderContext, NULL, NULL);

	for (auto items : encoderContext->frameFilters)
	{
		delete(items.second);
	}

	encoderContext->frameFilters.clear();
}

int Encoder::writeVideoFrame(EncoderData *encoderData)
{
	FrameFilter *frameFilter = NULL;

	// Do we need a frame filter for pixel format conversion?
	if (av_get_pix_fmt(encoderData->pix_fmt) != videoContext.codecContext->pix_fmt)
	{
		// Set up frame filter
		if (videoContext.frameFilters.find(encoderData->pix_fmt) == videoContext.frameFilters.end())
		{
			// Configure source format
			FrameFilterOptions options;
			options.media_type = videoContext.codecContext->codec_type;
			options.width = videoContext.codecContext->width;
			options.height = videoContext.codecContext->height;
			options.pix_fmt = av_get_pix_fmt(encoderData->pix_fmt);
			options.time_base = videoContext.codecContext->time_base;
			options.sar.den = videoContext.codecContext->sample_aspect_ratio.den;
			options.sar.num = videoContext.codecContext->sample_aspect_ratio.num;

			// Set target format
			char filterConfig[256];
			sprintf_s(filterConfig, "format=pix_fmts=%s", exportSettings.pixelFormat.c_str());

			frameFilter = new FrameFilter();
			frameFilter->configure(options, filterConfig);

			videoContext.frameFilters.insert(pair<string, FrameFilter*>(encoderData->pix_fmt, frameFilter));
		}
		else
		{
			frameFilter = videoContext.frameFilters.at(encoderData->pix_fmt);
		}
	}

	// Create a new frame
	AVFrame *frame = av_frame_alloc();
	frame->width = videoContext.codecContext->width;
	frame->height = videoContext.codecContext->height;
	frame->format = av_get_pix_fmt(encoderData->pix_fmt);

	int ret;

	// Reserve buffer space
	if ((ret = av_frame_get_buffer(frame, 0)) < 0)
	{
		return ret;
	}

	// Fill the source frame
	for (int i = 0; i < encoderData->planes; i++)
	{
		frame->data[i] = (uint8_t*)encoderData->plane[i];
		frame->linesize[i] = encoderData->stride[i];
	}

	// Presentation timestamp
	frame->pts = videoContext.next_pts++;

	// Send the frame to the encoder
	if ((ret = encodeAndWriteFrame(&videoContext, frame, frameFilter)) < 0)
	{
		av_frame_free(&frame);
		return ret;
	}

	av_frame_free(&frame);

	return 0;
}

int Encoder::writeAudioFrame(float **data, int32_t sampleCount)
{
	FrameFilter *frameFilter;

	if (audioContext.frameFilters.size() == 0)
	{
		FrameFilterOptions options;
		options.media_type = audioContext.codecContext->codec->type;
		options.channel_layout = audioContext.codecContext->channel_layout;
		options.sample_fmt = AV_SAMPLE_FMT_FLTP;
		options.time_base = { 1, audioContext.codecContext->sample_rate };

		char *audioLayout;
		switch (audioContext.codecContext->channel_layout)
		{
		case AV_CH_LAYOUT_MONO:
			audioLayout = "mono";
			break;
		case AV_CH_LAYOUT_STEREO:
			audioLayout = "stereo";
			break;
		case AV_CH_LAYOUT_5POINT1_BACK:
			audioLayout = "5.1";
			break;
		default:
			audioLayout = "stereo";
		}

		// Find the right sample size (for PCM we take what we get from the host)
		int frame_size = audioContext.codecContext->frame_size;
		if (frame_size == 0)
		{
			frame_size = sampleCount;
		}

		// Set target format
		char filterConfig[256];
		sprintf_s(filterConfig,
			"aformat=channel_layouts=%s:sample_fmts=%s:sample_rates=%d,asetnsamples=n=%d:p=0",
			audioLayout,
			av_get_sample_fmt_name(audioContext.codecContext->sample_fmt),
			audioContext.codecContext->sample_rate,
			frame_size);

		frameFilter = new FrameFilter();
		frameFilter->configure(options, filterConfig);

		audioContext.frameFilters.insert(pair<string, FrameFilter*>("default", frameFilter));
	}
	else
	{
		// Take first frame filter found
		frameFilter = audioContext.frameFilters.begin()->second;
	}

	// Create the source frame
	AVFrame *frame;
	frame = av_frame_alloc();
	frame->nb_samples = sampleCount;
	frame->channel_layout = audioContext.codecContext->channel_layout;
	frame->format = AV_SAMPLE_FMT_FLTP;
	frame->sample_rate = audioContext.codecContext->sample_rate;
	frame->pts = audioContext.next_pts++;

	int ret;

	// Allocate the buffer for the frame
	if ((ret = av_frame_get_buffer(frame, 0)) == 0)
	{
		// Assign the audio plane pointer
		for (int c = 0; c < audioContext.codecContext->channels; c++)
		{
			frame->data[c] = (uint8_t*)data[c];
		}

		// Send the frame to the encoder
		ret = encodeAndWriteFrame(&audioContext, frame, frameFilter);
	}

	av_frame_free(&frame);

	return ret;
}

int Encoder::encodeAndWriteFrame(EncoderContext *context, AVFrame *frame, FrameFilter *frameFilter)
{
	int ret = 0;

	if (frame == NULL)
	{
		// Send NULL frame
		if ((ret = avcodec_send_frame(context->codecContext, frame)) < 0)
		{
			return ret;
		}

		goto receive_packets;
	}

	if (frameFilter != NULL)
	{
		// Send the uncompressed frame to frame filter
		if ((ret = frameFilter->sendFrame(frame)) < 0)
		{
			return ret;
		}

		AVFrame *tmp_frame;
		tmp_frame = av_frame_alloc();

		// Receive frames from the rame filter
		while (frameFilter->receiveFrame(tmp_frame) >= 0)
		{
			// Send the frame to the encoder
			if ((ret = avcodec_send_frame(context->codecContext, tmp_frame)) < 0)
			{
				av_frame_free(&tmp_frame);
				return ret;
			}

			av_frame_unref(tmp_frame);
		}

		av_frame_free(&tmp_frame);
	}
	else
	{
		// Send the frame directly to the encoder
		if ((ret = avcodec_send_frame(context->codecContext, frame)) < 0)
		{
			return ret;
		}
	}

receive_packets:

	AVPacket *packet = av_packet_alloc();

	// If we receive a packet from the encoder write it to the stream
	while (avcodec_receive_packet(context->codecContext, packet) >= 0)
	{
		av_packet_rescale_ts(packet, context->codecContext->time_base, context->stream->time_base);
		packet->stream_index = context->stream->index;

		if ((ret = av_interleaved_write_frame(formatContext, packet)) < 0)
			break;

		av_packet_unref(packet);
	}

	return ret;
}
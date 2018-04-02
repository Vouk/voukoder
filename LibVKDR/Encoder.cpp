#include "Encoder.h"

using namespace LibVKDR;

Encoder::Encoder(ExportSettings exportSettings) :
	exportSettings(exportSettings)
{
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
		encoderContext->codecContext->time_base = exportSettings.videoTimebase;
		encoderContext->codecContext->pix_fmt = av_get_pix_fmt(exportSettings.pixelFormat.c_str());
		encoderContext->codecContext->colorspace = exportSettings.colorSpace;
		encoderContext->codecContext->color_range = exportSettings.colorRange;
		encoderContext->codecContext->color_primaries = exportSettings.colorPrimaries;
		encoderContext->codecContext->color_trc = exportSettings.colorTRC;
		encoderContext->codecContext->sample_aspect_ratio = exportSettings.videoSar;
		encoderContext->codecContext->field_order = exportSettings.fieldOrder;
	}
	else if (codec->type == AVMEDIA_TYPE_AUDIO)
	{
		encoderContext->codecContext->channels = av_get_channel_layout_nb_channels(exportSettings.audioChannelLayout);
		encoderContext->codecContext->channel_layout = exportSettings.audioChannelLayout;
		encoderContext->codecContext->time_base = exportSettings.audioTimebase;
		encoderContext->codecContext->sample_rate = exportSettings.audioTimebase.den;
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

int Encoder::openCodec(const string codecName, const string options, EncoderContext *encoderContext, int flags)
{
	int ret;

	if ((ret = createCodecContext(codecName.c_str(), encoderContext)) == 0)
	{
		encoderContext->codecContext->flags |= flags;

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
	int vflags = 0;

	if (exportSettings.fieldOrder != AVFieldOrder::AV_FIELD_PROGRESSIVE)
	{
		vflags |= AV_CODEC_FLAG_INTERLACED_DCT | AV_CODEC_FLAG_INTERLACED_ME;
	}

	if (exportSettings.passes > 1)
	{
		if (pass == 1)
		{
			vflags |= AV_CODEC_FLAG_PASS1;
		}
		else
		{
			vflags |= AV_CODEC_FLAG_PASS2;
		}
	}

	int ret;

	if ((ret = openCodec(
		exportSettings.videoCodecName.c_str(), 
		exportSettings.videoOptions, 
		&videoContext,
		vflags)) == 0)
	{
		if (exportSettings.exportAudio)
		{
			if ((ret = openCodec(
				exportSettings.audioCodecName.c_str(),
				exportSettings.audioOptions,
				&audioContext,
				0)) < 0)
			{
				goto finish;
			}

			// Find the right sample size (for variable frame size codecs (PCM) we use a constant value)
			audioFrameSize = audioContext.codecContext->frame_size;
			if (audioFrameSize == 0)
				audioFrameSize = 5000 / audioContext.codecContext->channels;
		}

		formatContext->url = av_strdup(exportSettings.filename.c_str());

		string filename;
		if (vflags & AV_CODEC_FLAG_PASS1)
			filename = "NUL";
		else
			filename = exportSettings.filename;

		if ((ret = avio_open(
			&formatContext->pb, 
			filename.c_str(), 
			AVIO_FLAG_WRITE)) == 0)
		{
			AVDictionary *options = NULL;

			if (exportSettings.flagFaststart)
				av_dict_set(&options, "movflags", "faststart", 0);

			return avformat_write_header(formatContext, &options);
		}
	}

finish:

	close(false);

	return ret;
}

void Encoder::close(bool writeTrailer)
{
	if (writeTrailer)
	{
		flushContext(&videoContext);
		
		if (exportSettings.exportAudio)
			flushContext(&audioContext);

		av_write_trailer(formatContext);
	}

	if (videoContext.codecContext != NULL && videoContext.codecContext->internal != NULL)
		avcodec_free_context(&videoContext.codecContext);

	if (audioContext.codecContext != NULL && audioContext.codecContext->internal != NULL)
		avcodec_free_context(&audioContext.codecContext);

	if (audioContext.frame != NULL)
		av_frame_free(&audioContext.frame);

	if (videoContext.frame != NULL)
		av_frame_free(&videoContext.frame);

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
		&videoContext,
		0)) == 0)
	{
		if ((ret = openCodec(
			exportSettings.audioCodecName.c_str(),
			exportSettings.audioOptions,
			&audioContext,
			0)) == 0)
		{
			if ((ret = avio_open_dyn_buf(&formatContext->pb)) == 0)
			{
				AVDictionary *options = NULL;
				ret = avformat_write_header(formatContext, &options);
			}
		}
	}

	close(false);

	return ret;
}

int Encoder::prepare(EncoderData *encoderData)
{
	if (pass == 0 || (exportSettings.passes > 1 && encoderData->pass > pass))
	{
		if (pass > 0)
		{
			close(true);
		}

		pass = encoderData->pass;

		int ret;
		if ((ret = open()) < 0)
		{
			return ret;
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

int Encoder::getAudioFrameSize()
{
	return audioFrameSize;
}

int Encoder::writeVideoFrame(EncoderData *encoderData)
{
	int ret;

	if ((ret = prepare(encoderData)) < 0)
	{
		return ret;
	}

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

	// Initially create a frame (and reuse it)
	if (videoContext.frame == NULL)
	{
		videoContext.frame = av_frame_alloc();
		videoContext.frame->width = videoContext.codecContext->width;
		videoContext.frame->height = videoContext.codecContext->height;
		videoContext.frame->format = av_get_pix_fmt(encoderData->pix_fmt);
		videoContext.frame->top_field_first = videoContext.codecContext->field_order == AVFieldOrder::AV_FIELD_TT;

		if ((ret = av_frame_get_buffer(videoContext.frame, 0)) < 0)
		{
			return ret;
		}
	}

	// Fill the frame with data
	for (int i = 0; i < encoderData->planes; i++)
	{
		videoContext.frame->data[i] = (uint8_t*)encoderData->plane[i];
		videoContext.frame->linesize[i] = encoderData->stride[i];
	}

	videoContext.frame->pts = videoContext.next_pts++;

	return encodeAndWriteFrame(&videoContext, videoContext.frame, frameFilter);
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

		// Set target format
		char filterConfig[256];
		sprintf_s(filterConfig,
			"aformat=channel_layouts=%dc:sample_fmts=%s:sample_rates=%d,asetnsamples=n=%d:p=0",
			audioContext.codecContext->channels,
			av_get_sample_fmt_name(audioContext.codecContext->sample_fmt),
			audioContext.codecContext->sample_rate,
			audioFrameSize);

		frameFilter = new FrameFilter();
		frameFilter->configure(options, filterConfig);

		audioContext.frameFilters.insert(pair<string, FrameFilter*>("default", frameFilter));
	}
	else
	{
		// Take first frame filter found
		frameFilter = audioContext.frameFilters.begin()->second;
	}

	int ret = 0;

	// Initially create a frame (and reuse it)
	if (audioContext.frame == NULL)
	{
		audioContext.frame = av_frame_alloc();
		audioContext.frame->nb_samples = sampleCount;
		audioContext.frame->channel_layout = audioContext.codecContext->channel_layout;
		audioContext.frame->format = AV_SAMPLE_FMT_FLTP;
		audioContext.frame->sample_rate = audioContext.codecContext->sample_rate;

		if ((ret = av_frame_get_buffer(audioContext.frame, 0)) < 0)
		{
			return ret;
		}
	}

	audioContext.frame->pts = audioContext.next_pts;
	audioContext.next_pts += sampleCount;

	// Fill the frame with data
	for (int c = 0; c < audioContext.codecContext->channels; c++)
	{
		audioContext.frame->data[c] = (uint8_t*)data[c];
	}

	return encodeAndWriteFrame(&audioContext, audioContext.frame, frameFilter);
}

int Encoder::encodeAndWriteFrame(EncoderContext *context, AVFrame *frame, FrameFilter *frameFilter)
{
	int ret = 0;

	if (frameFilter == NULL || frame == NULL)
	{
		if ((ret = avcodec_send_frame(context->codecContext, frame)) < 0)
		{
			return ret;
		}
	}
	else
	{
		if ((ret = frameFilter->sendFrame(frame)) < 0)
		{
			return ret;
		}

		while (frameFilter->receiveFrame() >= 0)
		{
			ret = avcodec_send_frame(context->codecContext, frameFilter->frame);
			
			if (ret < 0)
				return ret;
		}
	}

	return receivePackets(context);
}

int Encoder::receivePackets(EncoderContext *context)
{
	int ret = 0;

	AVPacket *packet = av_packet_alloc();
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
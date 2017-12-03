#include "EncoderContext.h"

// reviewed 0.3.8
EncoderContext::EncoderContext(AVFormatContext *formatContext):
	formatContext(formatContext)
{
}

// reviewed 0.3.8
int EncoderContext::openCodec()
{
	int ret = S_OK;

	// Find the right pixel/sample format
	if (codec->type == AVMEDIA_TYPE_VIDEO)
	{
		const char* pix_fmt = encoderConfig->getPixelFormat();
		codecContext->pix_fmt = av_get_pix_fmt(pix_fmt);

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

	// Open the codec
	if ((ret = avcodec_open2(codecContext, codec, &options)) < 0)
	{
		return ret;
	}

	// Copy the stream parameters to the context
	if ((ret = avcodec_parameters_from_context(stream->codecpar, codecContext)) < 0)
	{
		return ret;
	}

	return ret;
}

// reviewed 0.3.8
void EncoderContext::closeCodec()
{
	if (codecContext->internal != NULL)
	{
		avcodec_close(codecContext);
	}
}

// reviewed 0.3.8
void EncoderContext::setCodec(EncoderContextInfo encoderContestInfo, EncoderConfig *encoderConfig)
{
	this->encoderConfig = encoderConfig;

	encoderConfig->getConfig(&options, 1, 1);

	// Find codec by name
	codec = avcodec_find_encoder_by_name(encoderContestInfo.name.c_str());
	if (codec == NULL)
	{
		return;
	}

	// Create codec context
	codecContext = avcodec_alloc_context3(codec);
	codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

	// Type specific codec context settings
	if (codec->type == AVMEDIA_TYPE_VIDEO)
	{
		codecContext->width = encoderContestInfo.width;
		codecContext->height = encoderContestInfo.height;
		codecContext->time_base = encoderContestInfo.timebase;
		codecContext->pix_fmt = codec->pix_fmts ? codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;

		// Set color settings
		codecContext->colorspace = encoderContestInfo.colorSpace;
		codecContext->color_range = encoderContestInfo.colorRange;
		codecContext->color_primaries = encoderContestInfo.colorPrimaries;
		codecContext->color_trc = encoderContestInfo.colorTRC;
	}
	else if (codec->type == AVMEDIA_TYPE_AUDIO)
	{
		codecContext->channels = av_get_channel_layout_nb_channels(encoderContestInfo.channelLayout);
		codecContext->channel_layout = encoderContestInfo.channelLayout;
		codecContext->sample_rate = encoderContestInfo.timebase.den;
		codecContext->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		codecContext->bit_rate = 0;
	}

	// Create the stream
	stream = avformat_new_stream(formatContext, NULL);
	stream->id = formatContext->nb_streams - 1;
	stream->time_base = codecContext->time_base; // den: samplerate

	// Format context options
	if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
	{
		codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	avcodec_parameters_from_context(stream->codecpar, codecContext);
}

// reviewed 0.3.8
void EncoderContext::setCodecFlags(int flags)
{
	codecContext->flags |= flags;
}
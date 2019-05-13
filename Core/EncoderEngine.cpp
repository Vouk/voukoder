#include "EncoderEngine.h"
#include "windows.h"
#include <sstream>
#include "Log.h"

EncoderEngine::EncoderEngine(ExportInfo encoderInfo) :
	exportInfo(encoderInfo)
{
	packet = av_packet_alloc();

	passLogFile = wxFileName::CreateTempFileName("voukoder", (wxFile*)NULL);
}

EncoderEngine::~EncoderEngine()
{
	av_packet_free(&packet);

	// Remove temporary files
	if (wxFileExists(passLogFile))
		wxRemoveFile(passLogFile);

	if (wxFileExists(passLogFile + ".cutree"))
		wxRemoveFile(passLogFile + ".cutree");
	
	if (wxFileExists(passLogFile + ".mbtree"))
		wxRemoveFile(passLogFile + ".mbtree");
}

int EncoderEngine::open()
{
	// Create format context
	formatContext = avformat_alloc_context();
	formatContext->oformat = av_guess_format(exportInfo.format.id.c_str(), exportInfo.filename.c_str(), NULL);
	//formatContext->debug = FF_FDEBUG_TS;

	// Mark myself as encoding tool in the mp4/mov container
	av_dict_set(&formatContext->metadata, "encoding_tool", exportInfo.application.c_str(), 0);
	// MOV
	// MKV

	// Logging multi pass information
	if (exportInfo.passes > 1)
	{
		vkLogInfo("Running pass #%d ...", pass);
		vkLogInfo("Using pass logfile: %s", passLogFile);
	}

	int ret;

	// Do we want video export?
	if (exportInfo.video.enabled)
	{
		if ((ret = openCodec(exportInfo.video.id.ToStdString(), exportInfo.video.options.Serialize().ToStdString(), &videoContext, getCodecFlags(AVMEDIA_TYPE_VIDEO))) < 0)
		{
			vkLogError("Unable to open video encoder: %s", exportInfo.video.id.c_str());
			close();
			return ret;
		}
	}

	// Try to open the audio encoder (if wanted)
	if (exportInfo.audio.enabled)
	{
		if ((ret = openCodec(exportInfo.audio.id.ToStdString(), exportInfo.audio.options.Serialize().ToStdString(), &audioContext, 0)) < 0)
		{
			vkLogError("Unable to open audio encoder: %s", exportInfo.audio.id.c_str());
			close();
			return ret;
		}

		// Find the right sample size (for variable frame size codecs (PCM) we use the number of samples that match for one video frame)
		audioFrameSize = audioContext.codecContext->frame_size;
		if (audioFrameSize == 0)
		{
			if (exportInfo.video.enabled)
			{
				audioFrameSize = (int)av_rescale_q(1, videoContext.codecContext->time_base, audioContext.codecContext->time_base);
			}
			else
			{
				audioFrameSize = 1024;
			}
		}
	}
	
	AVDictionary *options = NULL;

	string filename;
	if (pass < exportInfo.passes)
	{
		filename = "NUL";
	}
	else
	{
		filename = exportInfo.filename;
		formatContext->url = av_strdup(exportInfo.filename.c_str());

		// Set the faststart flag in the last pass
		if (exportInfo.format.faststart)
		{
			av_dict_set(&options, "movflags", "faststart", 0);
		}

		/*
			av_dict_set(&formatContext->metadata, "major_brand", type, 0);
			av_dict_set_int(&formatContext->metadata, "minor_version", minor_ver, 0);
		*/
	}

	// Open file writer
	ret = avio_open(&formatContext->pb, filename.c_str(), AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		vkLogError("Unable to open file buffer.");
		return ret;
	}

	// Dump format settings
	av_dump_format(formatContext, 0, filename.c_str(), 1);
	
	return avformat_write_header(formatContext, &options);
}

int EncoderEngine::openCodec(const string codecId, const string codecOptions, EncoderContext *encoderContext, const int flags)
{
	int ret;

	if ((ret = createCodecContext(codecId, encoderContext, flags)) == 0)
	{
		vkLogInfo("Opening codec: %s with options: %s", codecId.c_str(), codecOptions.c_str());

		AVDictionary *dictionary = NULL;
		if ((ret = av_dict_parse_string(&dictionary, codecOptions.c_str(), VALUE_SEPARATOR, PARAM_SEPARATOR, 0)) < 0)
		{
			vkLogError("Unable to parse encoder configuration: %s", codecOptions.c_str());
			return ret;
		}

		// Set the logfile for multi-pass encodes to a file in the temp directory
		if (encoderContext->codecContext->codec_type == AVMEDIA_TYPE_VIDEO &&
			exportInfo.passes > 1)
		{
			if (codecId == "libx265")
			{
				wxString params;
				AVDictionaryEntry *entry = av_dict_get(dictionary, "x265-params", NULL, 0);
				if (entry != NULL)
				{
					params = wxString::Format("%s:stats='%s':pass=%d", entry->value, passLogFile, pass);
				}
				else
				{
					params = wxString::Format("stats='%s':pass=%d", passLogFile, pass);
				}

				av_dict_set(&dictionary, "x265-params", params, 0);
			}
			else
			{
				av_dict_set(&dictionary, "passlogfile", passLogFile, 0);
			}
		}

		// Open the codec
		if ((ret = avcodec_open2(encoderContext->codecContext, encoderContext->codecContext->codec, &dictionary)) == 0)
		{
			ret = avcodec_parameters_from_context(encoderContext->stream->codecpar, encoderContext->codecContext);
		}
		else
		{
			vkLogError("Failed opening codec: %s", codecId.c_str());
		}
	}

	return ret;
}

int EncoderEngine::createCodecContext(const string codecId, EncoderContext *encoderContext, int flags)
{
	// Is this codec supported?
	const AVCodec *codec = avcodec_find_encoder_by_name(codecId.c_str());
	if (codec == NULL)
	{
		return AVERROR_ENCODER_NOT_FOUND;
	}

	// No frame server so far
	encoderContext->frameFilter = NULL;

	// Create codec context
	encoderContext->codecContext = avcodec_alloc_context3(codec);
	encoderContext->codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	encoderContext->codecContext->thread_count = 0;
	encoderContext->codecContext->flags |= flags;

	if (codec->type == AVMEDIA_TYPE_VIDEO)
	{
		encoderContext->codecContext->width = exportInfo.video.width;
		encoderContext->codecContext->height = exportInfo.video.height;
		encoderContext->codecContext->time_base = exportInfo.video.timebase;
		encoderContext->codecContext->framerate = av_inv_q(encoderContext->codecContext->time_base);
		encoderContext->codecContext->pix_fmt = exportInfo.video.pixelFormat;
		encoderContext->codecContext->colorspace = exportInfo.video.colorSpace;
		encoderContext->codecContext->color_range = exportInfo.video.colorRange;
		encoderContext->codecContext->color_primaries = exportInfo.video.colorPrimaries;
		encoderContext->codecContext->color_trc = exportInfo.video.colorTransferCharacteristics;
		encoderContext->codecContext->sample_aspect_ratio = exportInfo.video.sampleAspectRatio;
		encoderContext->codecContext->field_order = exportInfo.video.fieldOrder;

		// Check for a zscaler filter
		for (auto const& filter : exportInfo.video.filters.filters)
		{
			if (filter->at("_name") == "zscale")
			{
				encoderContext->codecContext->width = wxAtoi(filter->at("w"));
				encoderContext->codecContext->height = wxAtoi(filter->at("h"));
			}
		}

		// Add stats_info to second pass
		if (codecId != "libx264" && encoderContext->codecContext->flags & AV_CODEC_FLAG_PASS2)
		{
			encoderContext->codecContext->stats_in = encoderContext->stats_info;
		}
	}
	else if (codec->type == AVMEDIA_TYPE_AUDIO)
	{
		encoderContext->codecContext->channel_layout = exportInfo.audio.channelLayout;
		encoderContext->codecContext->channels = av_get_channel_layout_nb_channels(exportInfo.audio.channelLayout);
		encoderContext->codecContext->time_base = exportInfo.audio.timebase;
		encoderContext->codecContext->sample_fmt = exportInfo.audio.sampleFormat;
		encoderContext->codecContext->sample_rate = exportInfo.audio.timebase.den;
		encoderContext->codecContext->bit_rate = 0;
	}

	// Create new stream
	encoderContext->stream = avformat_new_stream(formatContext, codec);
	encoderContext->stream->id = formatContext->nb_streams - 1;
	encoderContext->stream->time_base = encoderContext->codecContext->time_base;
	encoderContext->stream->avg_frame_rate = av_inv_q(encoderContext->stream->time_base);

	if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
	{
		encoderContext->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	return 0;
}

int EncoderEngine::getCodecFlags(const AVMediaType type)
{
	int flags = 0;

	if (type == AVMEDIA_TYPE_VIDEO)
	{
		// Mark as interlaced
		if (exportInfo.video.fieldOrder != AVFieldOrder::AV_FIELD_PROGRESSIVE)
		{
			flags |= AV_CODEC_FLAG_INTERLACED_DCT | AV_CODEC_FLAG_INTERLACED_ME;
		}

		// Set encoding pass
		if (exportInfo.passes > 1)
		{
			if (pass == 1)
			{
				flags |= AV_CODEC_FLAG_PASS1;
			}
			else
			{
				flags |= AV_CODEC_FLAG_PASS2;
			}
		}
	}

	return flags;
}

void EncoderEngine::finalize()
{
	vkLogInfo("Flushing encoders and finalizing ...");

	if (exportInfo.video.enabled)
	{
		flushContext(&videoContext);
	}

	if (exportInfo.audio.enabled)
	{
		flushContext(&audioContext);
	}

	vkLogInfo("Video and audio buffers flushed.");

	int ret = 0;
	if ((ret = av_write_trailer(formatContext)) < 0)
	{
		vkLogError("Unable to write trailer. (Return code: %d)", ret);
		return;
	}

	vkLogInfo("Trailer has been written.");

	// Save stats data from first pass
	if (exportInfo.video.enabled && exportInfo.video.id != "libx264")
	{
		AVCodecContext* codec = videoContext.codecContext;
		if (codec->flags & AV_CODEC_FLAG_PASS1 && codec->stats_out)
		{
			const size_t size = strlen(codec->stats_out) + 1;
			videoContext.stats_info = (char*)malloc(size);
			strcpy_s(videoContext.stats_info, size, codec->stats_out);
		}
		else if (codec->flags & AV_CODEC_FLAG_PASS2 && videoContext.stats_info)
		{
			free(videoContext.stats_info);
		}
	}
}

void EncoderEngine::close()
{
	vkLogInfo("Closing encoders ...");

	// Free video context
	if (videoContext.codecContext != NULL)
	{
		avcodec_free_context(&videoContext.codecContext);
	}

	// Free audio context
	if (audioContext.codecContext != NULL)
	{
		avcodec_free_context(&audioContext.codecContext);
	}

	avio_close(formatContext->pb);

	avformat_free_context(formatContext);
}

AVMediaType EncoderEngine::getNextFrameType()
{
	return (av_compare_ts(videoContext.next_pts, videoContext.codecContext->time_base, audioContext.next_pts, audioContext.codecContext->time_base) <= 0) ? AVMEDIA_TYPE_VIDEO : AVMEDIA_TYPE_AUDIO;
}

void EncoderEngine::flushContext(EncoderContext *encoderContext)
{
	// Close frame filter
	if (encoderContext->frameFilter)
	{
		delete(encoderContext->frameFilter);
		encoderContext->frameFilter = NULL;
	}

	// Send a flush notification
	sendFrame(encoderContext->codecContext, encoderContext->stream, NULL);
	
	// Clear the output buffer
	receivePackets(encoderContext->codecContext, encoderContext->stream);
}

int EncoderEngine::getAudioFrameSize()
{
	return audioFrameSize;
}

int EncoderEngine::writeVideoFrame(AVFrame *frame)
{
	// Do we need a frame filter?
	if (frame->pts == 0)
	{
		stringstream filterconfig;
	
		// Convert color space or range?
		if (videoContext.codecContext->color_range != frame->color_range ||
			videoContext.codecContext->colorspace != frame->colorspace ||
			videoContext.codecContext->color_primaries != frame->color_primaries ||
			videoContext.codecContext->color_trc != frame->color_trc)
		{
			// Color range
			filterconfig << ",colorspace=range=" << (videoContext.codecContext->color_range == AVColorRange::AVCOL_RANGE_MPEG ? "mpeg" : "jpeg");

			// Color space
			switch (videoContext.codecContext->colorspace)
			{
			case AVColorSpace::AVCOL_SPC_BT470BG:
				filterconfig << ":space=bt470bg:trc=bt470bg:primaries=bt470bg";
				break;
			case AVColorSpace::AVCOL_SPC_SMPTE170M:
				filterconfig << ":space=smpte170m:trc=smpte170m:primaries=smpte170m";
				break;
			case AVColorSpace::AVCOL_SPC_BT709:
				filterconfig << ":space=bt709:trc=bt709:primaries=bt709";
				break;
			case AVColorSpace::AVCOL_SPC_BT2020_CL:
				filterconfig << ":space=bt2020ncl:trc=bt2020-10:primaries=bt2020"; // TODO
				break;
			case AVColorSpace::AVCOL_SPC_BT2020_NCL:
				filterconfig << ":space=bt2020ncl:trc=bt2020-10:primaries=bt2020"; // TODO
				break;
			}
		}
		
		// Convert pixel format
		if (frame->format != videoContext.codecContext->pix_fmt)
		{
			filterconfig << ",format=pix_fmts=" << av_get_pix_fmt_name(videoContext.codecContext->pix_fmt);
		}

		// Add users filter config
		filterconfig << exportInfo.video.filters.AsFilterString();

		// Create filter
		string flt = filterconfig.str();
		if (flt.size() > 0)
		{
			// Set frame filter options
			FrameFilterOptions options;
			options.media_type = videoContext.codecContext->codec_type;
			options.width = exportInfo.video.width;
			options.height = exportInfo.video.height;
			options.pix_fmt = (AVPixelFormat)frame->format;
			options.time_base = videoContext.codecContext->time_base;
			options.sar = videoContext.codecContext->sample_aspect_ratio;

			// Set up filter config
			videoContext.frameFilter = new FrameFilter();
			videoContext.frameFilter->configure(options, flt.substr(1).c_str());
		}
	}

	// Common frame settings
	frame->top_field_first = videoContext.codecContext->field_order == AVFieldOrder::AV_FIELD_TT;
	frame->sample_aspect_ratio = videoContext.codecContext->sample_aspect_ratio;

	// Send the frame to the encoder
	return encodeAndWriteFrame(&videoContext, frame);
}

int EncoderEngine::writeAudioFrame(AVFrame *frame)
{
	if (frame->pts == 0)
	{
		stringstream filterconfig;

		//Convert sample format
		filterconfig << "aformat=channel_layouts=" << audioContext.codecContext->channels << "c:";
		filterconfig << "sample_fmts=" << av_get_sample_fmt_name(audioContext.codecContext->sample_fmt) << ":";
		filterconfig << "sample_rates=" << audioContext.codecContext->sample_rate;

		// Buffer all samples
		//filterconfig << ",asetnsamples=n=" << audioFrameSize;

		// Set frame filter options
		FrameFilterOptions options;
		options.media_type = audioContext.codecContext->codec->type;
		options.channel_layout = audioContext.codecContext->channel_layout;
		options.sample_fmt = AV_SAMPLE_FMT_FLTP;
		options.time_base = { 1, audioContext.codecContext->sample_rate };

		// Set up filter config
		audioContext.frameFilter = new FrameFilter();
		audioContext.frameFilter->configure(options, filterconfig.str().c_str());
	}

	// Send the frame to the encoder
	return encodeAndWriteFrame(&audioContext, frame);
}

int EncoderEngine::encodeAndWriteFrame(EncoderContext *context, AVFrame *frame)
{
	int ret = 0;

	if (context->frameFilter != NULL)
	{
		// Send the uncompressed frame to frame filter
		if ((ret = context->frameFilter->sendFrame(frame)) < 0)
		{
			return ret;
		}

		AVFrame *tmp_frame;
		tmp_frame = av_frame_alloc();

		// Receive frames from the frame filter
		while (context->frameFilter->receiveFrame(tmp_frame) >= 0)
		{
			// Send an unfiltered frame 
			if ((ret = sendFrame(context->codecContext, context->stream, tmp_frame)) < 0)
			{
				return ret;
			}

			av_frame_unref(tmp_frame);
		}

		av_frame_free(&tmp_frame);
	}
	else
	{
		// Send an unfiltered frame 
		if ((ret = sendFrame(context->codecContext, context->stream, frame)) < 0)
		{
			return ret;
		}
	}

	return receivePackets(context->codecContext, context->stream);
}

int EncoderEngine::sendFrame(AVCodecContext *context, AVStream *stream, AVFrame *frame)
{
	int ret;

	// Send the frame to the encoder
	if ((ret = avcodec_send_frame(context, frame)) < 0)
	{
		// Is the send buffer full?
		if (ret == AVERROR(EAGAIN))
		{
			// Read output buffer first
			receivePackets(context, stream);

			// Retry sending the frame
			return sendFrame(context, stream, frame);
		}
	}

	return ret;
}

int EncoderEngine::receivePackets(AVCodecContext *codecContext, AVStream *stream)
{
	int ret = 0;

	while (ret >= 0)
	{
		// Receive next packet
		ret = avcodec_receive_packet(codecContext, packet);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			return 0;
		}
		else if (ret < 0)
		{
			vkLogError("Encoding packet failed (Code: %d)", ret);
			return ret;
		}

		// Force constant frame rate (necessary?)
		//if (codecContext->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO && packet->duration == 0)
		//	packet->duration = 1;

		av_packet_rescale_ts(packet, codecContext->time_base, stream->time_base);

		packet->stream_index = stream->index;

		// Write packet to disk
		if (av_interleaved_write_frame(formatContext, packet) < 0)
		{
			vkLogError("Unable to write packet to disk.");
			break;
		}

		av_packet_unref(packet);
	}

	return ret;
}

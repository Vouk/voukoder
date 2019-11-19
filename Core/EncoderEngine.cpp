﻿#include <sstream>
#include "EncoderEngine.h"
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
	formatContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	//formatContext->debug = FF_FDEBUG_TS;

	// Reset contexts
	audioContext.next_pts = 0;
	videoContext.next_pts = 0;
	audioContext.firstData = true;
	videoContext.firstData = true;

	// Mark myself as encoding tool in the mp4/mov container
	av_dict_set(&formatContext->metadata, "encoding_tool", exportInfo.application.c_str(), 0);
	// MOV
	// MKV

	// Logging multi pass information
	if (exportInfo.passes > 1)
	{
		vkLogInfoVA("Running pass #%d ...", pass);
		vkLogInfoVA("Using pass logfile: %s", passLogFile);
	}

	int ret;

	// Do we want video export?
	if (exportInfo.video.enabled)
	{
		if ((ret = openCodec(exportInfo.video.id.ToStdString(), exportInfo.video.options.Serialize().ToStdString(), &videoContext, getCodecFlags(AVMEDIA_TYPE_VIDEO))) < 0)
		{
			vkLogErrorVA("Unable to open video encoder: %s", exportInfo.video.id.c_str());
			close();
			return ret;
		}
	}

	// Try to open the audio encoder (if wanted)
	if (exportInfo.audio.enabled && (exportInfo.passes == 1 || exportInfo.passes == pass))
	{
		if ((ret = openCodec(exportInfo.audio.id.ToStdString(), exportInfo.audio.options.Serialize().ToStdString(), &audioContext, 0)) < 0)
		{
			vkLogErrorVA("Unable to open audio encoder: %s", exportInfo.audio.id.c_str());
			close();
			return ret;
		}

		// Find the right sample size (for variable frame size codecs (PCM) we use the number of samples that match for one video frame)
		audioFrameSize = audioContext.codecContext->frame_size;
		if (audioFrameSize == 0)
		{
			if (exportInfo.video.enabled)
				audioFrameSize = (int)av_rescale_q(1, videoContext.codecContext->time_base, audioContext.codecContext->time_base);
			else
				audioFrameSize = 1024;
		}
	}

	AVDictionary *options = NULL;

	wxString filename;
	if (pass < exportInfo.passes)
		filename = "NUL";
	else
	{
		filename = exportInfo.filename;
		formatContext->url = av_strdup(exportInfo.filename.c_str());

		// Set the faststart flag in the last pass
		if (exportInfo.format.faststart)
			av_dict_set(&options, "movflags", "faststart", 0);

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

int EncoderEngine::openCodec(const wxString codecId, const wxString codecOptions, EncoderContext *encoderContext, const int flags)
{
	int ret;

	if ((ret = createCodecContext(codecId, encoderContext, flags)) == 0)
	{
		vkLogInfoVA("Opening codec: %s with options: %s", codecId.c_str(), codecOptions.c_str());

		AVDictionary *dictionary = NULL;
		if ((ret = av_dict_parse_string(&dictionary, codecOptions.c_str(), VALUE_SEPARATOR, PARAM_SEPARATOR, 0)) < 0)
		{
			vkLogErrorVA("Unable to parse encoder configuration: %s", codecOptions.c_str());
			return ret;
		}

		// Handle special encoder options
		if (encoderContext->codecContext->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			// Handle interlaced modes
			if (exportInfo.video.id == "libx264" &&
				exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_BB ||
				exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_TT)
			{
				// Build interlaced string
				wxString params = "interlaced=1:";
				if (exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_TT)
					params += "tff=1";
				else
					params += "bff=1";

				// Get existing params
				AVDictionaryEntry* entry = av_dict_get(dictionary, "x264-params", NULL, 0);
				if (entry)
					params << ':' << entry->value;

				av_dict_set(&dictionary, "x264-params", params.c_str(), 0);
			}
			else if (exportInfo.video.id == "libx265" &&
				exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_BB ||
				exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_TT)
			{
				// Build interlaced string
				wxString params;
				if (exportInfo.video.fieldOrder == AVFieldOrder::AV_FIELD_TT)
					params = "interlace=tff:top=1";
				else
					params = "interlace=bff:top=0";

				// Get existing params
				AVDictionaryEntry* entry = av_dict_get(dictionary, "x265-params", NULL, 0);
				if (entry)
					params << ':' << entry->value;

				av_dict_set(&dictionary, "x265-params", params.c_str(), 0);
			}

			// Set the logfile for multi-pass encodes to a file in the temp directory
			if (exportInfo.passes > 1)
			{
				if (codecId == "libx265")
				{
					wxString params;
					AVDictionaryEntry *entry = av_dict_get(dictionary, "x265-params", NULL, 0);
					if (entry != NULL)
						params = wxString::Format("%s:stats='%s':pass=%d", entry->value, passLogFile, pass);
					else
						params = wxString::Format("stats='%s':pass=%d", passLogFile, pass);

					av_dict_set(&dictionary, "x265-params", params.c_str(), 0);
				}
				else
					av_dict_set(&dictionary, "passlogfile", passLogFile.c_str(), 0);
			}
		}

		// Open the codec
		if ((ret = avcodec_open2(encoderContext->codecContext, encoderContext->codecContext->codec, &dictionary)) == 0)
			ret = avcodec_parameters_from_context(encoderContext->stream->codecpar, encoderContext->codecContext);
		else
			vkLogErrorVA("Failed opening codec: %s", codecId.c_str());
	}

	return ret;
}

int EncoderEngine::injectStereoData(AVStream* stream)
{
	// Do we have stereo 3d data?
	wxString type = GetSideData(exportInfo.video.sideData, "s3d_type", "");
	if (type.IsEmpty())
		return 0;

	// Add basic data
	AVStereo3D* stereo_3d = av_stereo3d_alloc();
	stereo_3d->flags = 0;

	// Add type
	if (type == "2d")
		stereo_3d->type = AV_STEREO3D_2D;
	else if (type == "sidebyside")
		stereo_3d->type = AV_STEREO3D_SIDEBYSIDE;
	else if (type == "topbottom")
		stereo_3d->type = AV_STEREO3D_TOPBOTTOM;
	else if (type == "framesequence")
		stereo_3d->type = AV_STEREO3D_FRAMESEQUENCE;
	else if (type == "checkerboard")
		stereo_3d->type = AV_STEREO3D_CHECKERBOARD;
	else if (type == "sidebyside_quincunx")
		stereo_3d->type = AV_STEREO3D_SIDEBYSIDE_QUINCUNX;
	else if (type == "lines")
		stereo_3d->type = AV_STEREO3D_LINES;
	else if (type == "columns")
		stereo_3d->type = AV_STEREO3D_COLUMNS;

	// Add views
	wxString view = GetSideData(exportInfo.video.sideData, "s3d_view", "");
	if (view == "2d")
		stereo_3d->view = AV_STEREO3D_VIEW_PACKED;
	else if (view == "left")
		stereo_3d->view = AV_STEREO3D_VIEW_LEFT;
	else if (view == "right")
		stereo_3d->view = AV_STEREO3D_VIEW_RIGHT;

	return av_stream_add_side_data(stream, AV_PKT_DATA_STEREO3D, (uint8_t*)stereo_3d, sizeof(*stereo_3d));
}

int EncoderEngine::injectSphericalData(AVStream *stream)
{
	// Do we have spherical projection data?
	wxString projection = GetSideData(exportInfo.video.sideData, "sph_projection", "");
	if (projection.IsEmpty())
		return 0;

	size_t size;
	
	// Add basic data
	AVSphericalMapping* spherical = av_spherical_alloc(&size);
	spherical->yaw = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_yaw", "0"));
	spherical->pitch = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_pitch", "0"));
	spherical->roll = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_roll", "0"));

	// Add projection data
	if (projection == "equirectangular")
	{
		spherical->projection = AV_SPHERICAL_EQUIRECTANGULAR;
	}
	else if (projection == "cubemap")
	{
		spherical->projection = AV_SPHERICAL_CUBEMAP;
		spherical->padding = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_padding", "0"));
	}
	else if (projection == "equirectangular_tile")
	{
		spherical->projection = AV_SPHERICAL_EQUIRECTANGULAR_TILE;
		spherical->bound_left = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_bound-left", "0"));
		spherical->bound_top = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_bound-top", "0"));
		spherical->bound_right = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_bound-right", "0"));
		spherical->bound_bottom = wxAtoi(GetSideData(exportInfo.video.sideData, "sph_bound-bottom", "0"));
	}

	return av_stream_add_side_data(stream, AV_PKT_DATA_SPHERICAL, (uint8_t*)spherical, size);
}

int EncoderEngine::createCodecContext(const wxString codecId, EncoderContext *encoderContext, int flags)
{
	// Is this codec supported?
	const AVCodec *codec = avcodec_find_encoder_by_name(codecId.c_str());
	if (codec == NULL)
		return AVERROR_ENCODER_NOT_FOUND;

	// No frame server so far
	encoderContext->frameFilter = NULL;

	// Create new stream
	encoderContext->stream = avformat_new_stream(formatContext, codec);
	encoderContext->stream->id = formatContext->nb_streams - 1;
	encoderContext->stream->time_base = exportInfo.video.timebase;
	encoderContext->stream->avg_frame_rate = av_inv_q(exportInfo.video.timebase);

	// Create codec context
	encoderContext->codecContext = avcodec_alloc_context3(codec);
	encoderContext->codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	encoderContext->codecContext->thread_count = 0;
	encoderContext->codecContext->flags |= flags;

	if (codec->type == AVMEDIA_TYPE_VIDEO)
	{
		// Inject side data
		injectSphericalData(encoderContext->stream);
		injectStereoData(encoderContext->stream);

		// Configure context
		encoderContext->codecContext->width = exportInfo.video.width;
		encoderContext->codecContext->height = exportInfo.video.height;
		encoderContext->codecContext->time_base = exportInfo.video.timebase;
		encoderContext->codecContext->framerate = av_inv_q(encoderContext->codecContext->time_base);
		encoderContext->codecContext->colorspace = exportInfo.video.colorSpace;
		encoderContext->codecContext->color_range = exportInfo.video.colorRange;
		encoderContext->codecContext->color_primaries = exportInfo.video.colorPrimaries;
		encoderContext->codecContext->color_trc = exportInfo.video.colorTransferCharacteristics;
		encoderContext->codecContext->sample_aspect_ratio = exportInfo.video.sampleAspectRatio;
		encoderContext->codecContext->field_order = exportInfo.video.fieldOrder;

		// Find pixel format in options
		if (exportInfo.video.options.find("_pixelFormat") != exportInfo.video.options.end())
		{
			auto format = exportInfo.video.options.at("_pixelFormat");
			encoderContext->codecContext->pix_fmt = av_get_pix_fmt(format.c_str());
		}
		else
			return -1;

		// Check for a zscaler filter
		for (auto const& options : exportInfo.video.filters)
		{
			wxString id = options->id.After('.');
			if (id == "zscale" &&
				options->find("width") != options->end() &&
				options->find("height") != options->end())
			{
				encoderContext->codecContext->width = wxAtoi(options->at("width"));
				encoderContext->codecContext->height = wxAtoi(options->at("height"));
			}
			else if (id == "yadif" || id == "bwdif")
				encoderContext->codecContext->field_order = AV_FIELD_PROGRESSIVE;
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
		encoderContext->codecContext->sample_rate = exportInfo.audio.timebase.den;
		encoderContext->codecContext->bit_rate = 0;

		// Find sample format in options
		if (exportInfo.audio.options.find("_sampleFormat") != exportInfo.audio.options.end())
		{
			auto format = exportInfo.audio.options.at("_sampleFormat");
			encoderContext->codecContext->sample_fmt = av_get_sample_fmt(format.c_str());
		}
		else
			return -1;
	}

	if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
		encoderContext->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

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

	if (videoContext.codecContext)
		flushContext(&videoContext);

	if (audioContext.codecContext)
		flushContext(&audioContext);

	vkLogInfo("Video and audio buffers flushed.");

	int ret = 0;
	if ((ret = av_write_trailer(formatContext)) < 0)
	{
		vkLogErrorVA("Unable to write trailer. (Return code: %d)", ret);
		return;
	}

	vkLogInfo("Trailer has been written.");

	// Save stats data from first pass
	if (videoContext.codecContext && exportInfo.video.id != "libx264")
	{
		AVCodecContext* codec = videoContext.codecContext;
		if (codec->flags & AV_CODEC_FLAG_PASS1 && codec->stats_out)
		{
			const size_t size = strlen(codec->stats_out) + 1;
			videoContext.stats_info = (char*)malloc(size);
			wxStrncpy(videoContext.stats_info, codec->stats_out, size);
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
	if (videoContext.codecContext)
		avcodec_free_context(&videoContext.codecContext);

	// Free audio context
	if (audioContext.codecContext)
		avcodec_free_context(&audioContext.codecContext);

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

bool EncoderEngine::hasVideo()
{
	return exportInfo.video.enabled && videoContext.codecContext;
}

bool EncoderEngine::hasAudio()
{
	return exportInfo.audio.enabled && audioContext.codecContext && (exportInfo.passes == 1 || pass == exportInfo.passes);
}

int EncoderEngine::writeVideoFrame(AVFrame *frame)
{
	// Do we need a frame filter?
	if (videoContext.firstData)
	{
		videoContext.firstData = false;

		// Add users filter config
		wxString filterconfig = exportInfo.video.filters.AsFilterString();

		// Convert pixel format
		if (frame->format != (int)videoContext.codecContext->pix_fmt)
			filterconfig << ",format=pix_fmts=" << av_get_pix_fmt_name(videoContext.codecContext->pix_fmt);

		// Create filter
		if (filterconfig.size() > 0)
		{
			// Set frame filter options
			FrameFilterOptions options;
			options.media_type = AVMEDIA_TYPE_VIDEO;
			options.width = frame->width;
			options.height = frame->height;
			options.pix_fmt = (AVPixelFormat)frame->format;
			options.time_base = videoContext.codecContext->time_base;
			options.sar = videoContext.codecContext->sample_aspect_ratio;

			// Set up filter config
			videoContext.frameFilter = new FrameFilter();
			videoContext.frameFilter->configure(options, filterconfig.substr(1).c_str());

			// Log filters
			vkLogInfo("Applying video filters: " + filterconfig.substr(1));
		}
	}

	// Common frame settings
	frame->top_field_first = videoContext.codecContext->field_order == AVFieldOrder::AV_FIELD_TT;
	frame->sample_aspect_ratio = videoContext.codecContext->sample_aspect_ratio;

	frame->pts = videoContext.next_pts;
	videoContext.next_pts++;

	// Send the frame to the encoder
	return encodeAndWriteFrame(&videoContext, frame);
}

int EncoderEngine::writeAudioFrame(AVFrame *frame)
{
	if (audioContext.firstData)
	{
		audioContext.firstData = false;

		wxString filterconfig;

		// Keep the number of samples constant
		filterconfig << "asetnsamples=n=" << getAudioFrameSize() << ":p=0";
		
		// Convert sample format
		if (audioContext.codecContext->channels != frame->channels ||
			(int)audioContext.codecContext->sample_fmt != frame->format ||
			audioContext.codecContext->sample_rate != frame->sample_rate)
		{
			filterconfig << ",aformat=channel_layouts=" << audioContext.codecContext->channels << "c:";
			filterconfig << "sample_fmts=" << av_get_sample_fmt_name(audioContext.codecContext->sample_fmt) << ":";
			filterconfig << "sample_rates=" << audioContext.codecContext->sample_rate;
		}

		// Add users filter config
		filterconfig << exportInfo.audio.filters.AsFilterString();

		// Set up filter config
		if (filterconfig.size() > 0)
		{	
			// Set frame filter options
			FrameFilterOptions options;
			options.media_type = AVMEDIA_TYPE_AUDIO;
			options.channel_layout = frame->channel_layout;
			options.sample_fmt = (AVSampleFormat)frame->format;
			options.time_base = { 1, frame->sample_rate };

			// Set up filter config
			audioContext.frameFilter = new FrameFilter();
			audioContext.frameFilter->configure(options, filterconfig.c_str());

			// Log filters
			vkLogInfo("Applying audio filters: " + filterconfig.substr(1));
		}
	}

	frame->pts = audioContext.next_pts;

	audioContext.next_pts += frame->nb_samples;

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
			vkLogErrorVA("Encoding packet failed (Code: %d)", ret);
			return ret;
		}

		// Force constant frame rate (necessary, but why???)
		if (codecContext->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO && packet->duration == 0)
			packet->duration = 1;

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

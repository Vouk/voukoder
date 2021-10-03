/**
 * Voukoder
 * Copyright (C) 2017-2020 Daniel Stankewitz, All Rights Reserved
 * https://www.voukoder.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
 */
#include <sstream>
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
	int ret = 0;

	if (exportInfo.video.id.EndsWith("_qsv"))
		ret = av_hwdevice_ctx_create(&hwDev, AV_HWDEVICE_TYPE_QSV, "auto", NULL, 0);

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
		if (audioContext.codecContext->frame_size == 0)
		{
			if (exportInfo.video.enabled)
				audioContext.codecContext->frame_size = (int)av_rescale_q(1, videoContext.codecContext->time_base, audioContext.codecContext->time_base);
			else
				audioContext.codecContext->frame_size = 1024;
		}
	}

	AVDictionary* options = NULL;

	char filename[MAX_PATH];
	if (pass < exportInfo.passes)
		strcpy_s(filename, "NUL");
	else
	{
		// Convert filename to utf8 char*
		strcpy(filename, (const char*)exportInfo.filename.mb_str(wxConvUTF8));
		formatContext->url = av_strdup(filename);

		// Set the faststart flag in the last pass
		if (exportInfo.format.faststart)
			av_dict_set(&options, "movflags", "faststart", 0);

		// Adding timecode information for mp4/mov
		av_dict_set(&options, "write_tmcd", "1", 0);
	}

	// Open file writer
	ret = avio_open(&formatContext->pb, filename, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		vkLogError("Unable to open file buffer.");
		return ret;
	}

	// Still images should be numbered
	if (exportInfo.format.id == "image2")
		av_dict_set(&options, "frame_pts", "1", 0);

	// Produce a more compatible file
	//if (exportInfo.format.id == "mp4" ||
	//	exportInfo.format.id == "mov")
	//{
	//	av_dict_set(&options, "brand", "mp42", 0);
	//	av_dict_set(&options, "minor_version", "0", 0);
	//	av_dict_set(&options, "compatible_brands", "M4V isommp42mp41", 0);
	//}

	// Dump format settings
	av_dump_format(formatContext, 0, filename, 1);

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

int EncoderEngine::injectMasteringDisplayData(AVStream* stream)
{
	// Only proceed if we have at least one of the fields set
	if (exportInfo.video.sideData.find("mdcv_primaries") == exportInfo.video.sideData.end() &&
		exportInfo.video.sideData.find("min_luminance") == exportInfo.video.sideData.end() &&
		exportInfo.video.sideData.find("max_luminance") == exportInfo.video.sideData.end())
		return 0;

	// Mastering Display Data
	AVMasteringDisplayMetadata* mdcv = av_mastering_display_metadata_alloc();

	// Do we have mdcv data?
	wxString primaries = GetSideData(exportInfo.video.sideData, "mdcv_primaries", "");
	if (!primaries.IsEmpty())
	{
		mdcv->has_primaries = 1;
		if (primaries == "bt709")
		{
			// Rec.709 : --master - display G(15000, 30000)B(7500, 3000)R(32000, 16500)WP(15635, 16450)L(10000000, 1) --max - cll 1000, 1
			// RGB : G(x = 0.30, y = 0.60), B(x = 0.150, y = 0.060), R(x = 0.640, y = 0.330), WP(x = 0.3127, y = 0.329), L(max = 1000, min = 0.0000)
			mdcv->display_primaries[0][0] = av_d2q(0.64, INT_MAX); // Rx
			mdcv->display_primaries[0][1] = av_d2q(0.33, INT_MAX); // Ry
			mdcv->display_primaries[1][0] = av_d2q(0.3, INT_MAX);  // Gx
			mdcv->display_primaries[1][1] = av_d2q(0.6, INT_MAX);  // Gy
			mdcv->display_primaries[2][0] = av_d2q(0.15, INT_MAX); // Bx
			mdcv->display_primaries[2][1] = av_d2q(0.06, INT_MAX); // By
		}
		else if (primaries == "dcip3")
		{
			// DCI - P3: --master - display G(13250, 34500)B(7500, 3000)R(34000, 16000)WP(15635, 16450)L(10000000, 1) --max - cll 1000, 1
			// RGB : G(x = 0.265, y = 0.690), B(x = 0.150, y = 0.060), R(x = 0.680, y = 0.320), WP(x = 0.3127, y = 0.329), L(max = 1000, min = 0.0000)
			mdcv->display_primaries[0][0] = av_d2q(0.68, INT_MAX);  // Rx
			mdcv->display_primaries[0][1] = av_d2q(0.32, INT_MAX);  // Ry
			mdcv->display_primaries[1][0] = av_d2q(0.265, INT_MAX); // Gx
			mdcv->display_primaries[1][1] = av_d2q(0.69, INT_MAX);  // Gy
			mdcv->display_primaries[2][0] = av_d2q(0.15, INT_MAX);  // Bx
			mdcv->display_primaries[2][1] = av_d2q(0.06, INT_MAX);  // By
		}
		else if (primaries == "rec2020")
		{
			// Rec.2020 : --master - display G(8500, 39850)B(6550, 2300)R(35400, 14600)WP(15635, 16450)L(10000000, 1) --max - cll 1000, 1
			// RGB : G(x = 0.170, y = 0.797), B(x = 0.131, y = 0.046), R(x = 0.708, y = 0.292), WP(x = 0.3127, y = 0.329), L(max = 1000, min = 0.0000)
			mdcv->display_primaries[0][0] = av_d2q(0.708, INT_MAX); // Rx
			mdcv->display_primaries[0][1] = av_d2q(0.292, INT_MAX); // Ry
			mdcv->display_primaries[1][0] = av_d2q(0.17, INT_MAX);  // Gx
			mdcv->display_primaries[1][1] = av_d2q(0.797, INT_MAX); // Gy
			mdcv->display_primaries[2][0] = av_d2q(0.131, INT_MAX); // Bx
			mdcv->display_primaries[2][1] = av_d2q(0.046, INT_MAX); // By
		}

		mdcv->white_point[0] = av_d2q(0.3127, INT_MAX); // WPx
		mdcv->white_point[1] = av_d2q(0.329, INT_MAX);  // WPy
	}

	// Luminance
	if (exportInfo.video.sideData.find("mdcv_min_luminance") != exportInfo.video.sideData.end() ||
		exportInfo.video.sideData.find("mdcv_max_luminance") != exportInfo.video.sideData.end())
	{
		mdcv->has_luminance = 1;
		mdcv->min_luminance = { static_cast<int>(wxAtof(GetSideData(exportInfo.video.sideData, "mdcv_min_luminance", "0")) * 10000), 10000 };
		mdcv->max_luminance = { wxAtoi(GetSideData(exportInfo.video.sideData, "mdcv_max_luminance", "0")), 1 };
	}

	return av_stream_add_side_data(stream, AV_PKT_DATA_MASTERING_DISPLAY_METADATA, (uint8_t*)mdcv, sizeof(*mdcv));

}

int EncoderEngine::injectContentLightLevels(AVStream* stream)
{
	// Only proceed if we have at least one of both fields set
	if (exportInfo.video.sideData.find("cll_maxCLL") == exportInfo.video.sideData.end() &&
		exportInfo.video.sideData.find("cll_maxFALL") == exportInfo.video.sideData.end())
		return 0;

	size_t size;

	// Add basic data
	AVContentLightMetadata* cll = av_content_light_metadata_alloc(&size);
	cll->MaxCLL = wxAtoi(GetSideData(exportInfo.video.sideData, "cll_maxCLL", "0"));
	cll->MaxFALL = wxAtoi(GetSideData(exportInfo.video.sideData, "cll_maxFALL", "0"));

	return av_stream_add_side_data(stream, AV_PKT_DATA_CONTENT_LIGHT_LEVEL, (uint8_t*)cll, size);
}

int EncoderEngine::createCodecContext(const wxString codecId, EncoderContext *encoderContext, int flags)
{
	// Is this codec supported?
	auto codec = avcodec_find_encoder_by_name(codecId.c_str());
	if (codec == NULL)
		return AVERROR_ENCODER_NOT_FOUND;

	// No frame server so far
	encoderContext->frameFilter = NULL;

	// Create codec context
	encoderContext->codecContext = avcodec_alloc_context3(codec);
	encoderContext->codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	encoderContext->codecContext->thread_count = 0;
	encoderContext->codecContext->flags |= flags;

	if (codec->type == AVMEDIA_TYPE_VIDEO)
	{
		// Configure context
		encoderContext->codecContext->width = exportInfo.video.width;
		encoderContext->codecContext->height = exportInfo.video.height;
		encoderContext->codecContext->time_base = exportInfo.video.timebase;
		encoderContext->codecContext->framerate = av_inv_q(encoderContext->codecContext->time_base);
		encoderContext->codecContext->color_range = exportInfo.video.colorRange;
		encoderContext->codecContext->colorspace = exportInfo.video.colorSpace;
		encoderContext->codecContext->color_primaries = exportInfo.video.colorPrimaries;
		encoderContext->codecContext->color_trc = exportInfo.video.colorTransferCharacteristics;
		encoderContext->codecContext->sample_aspect_ratio = exportInfo.video.sampleAspectRatio;
		encoderContext->codecContext->field_order = exportInfo.video.fieldOrder;

		// Bobbing doubles the frame rate
		if (exportInfo.video.flags & VKEncVideoFlags::VK_FLAG_DEINTERLACE_BOBBING)
			encoderContext->codecContext->time_base.den *= 2;

		// Find pixel format in options
		if (exportInfo.video.options.find("_pixelFormat") != exportInfo.video.options.end())
		{
			auto format = exportInfo.video.options.at("_pixelFormat");
			encoderContext->codecContext->pix_fmt = av_get_pix_fmt(format.c_str());
		}
		else
			return -1;

		// Add stats_info to second pass
		if (codecId != "libx264" && encoderContext->codecContext->flags & AV_CODEC_FLAG_PASS2)
			encoderContext->codecContext->stats_in = encoderContext->stats_info;
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

	// Create new stream
	encoderContext->stream = avformat_new_stream(formatContext, codec);
	encoderContext->stream->id = formatContext->nb_streams - 1;
	encoderContext->stream->time_base = encoderContext->codecContext->time_base;
	encoderContext->stream->avg_frame_rate = av_inv_q(encoderContext->stream->time_base);

	// Inject side data
	if (codec->type == AVMEDIA_TYPE_VIDEO)
	{
		injectSphericalData(encoderContext->stream);
		injectStereoData(encoderContext->stream);
		injectMasteringDisplayData(encoderContext->stream);
		injectContentLightLevels(encoderContext->stream);

		// Custom timecode
		if (exportInfo.video.options.find("_timecode") != exportInfo.video.options.end())
		{
			auto timecode = exportInfo.video.options.at("_timecode");
			av_dict_set(&encoderContext->stream->metadata, "timecode", timecode.c_str(), 0);
		}

		// Language
		if (exportInfo.video.options.find("_language") != exportInfo.video.options.end())
		{
			auto language = exportInfo.video.options.at("_language");
			av_dict_set(&encoderContext->stream->metadata, "language", language.c_str(), 0);
		}
	}
	else if (codec->type == AVMEDIA_TYPE_AUDIO)
	{
		// Language
		if (exportInfo.audio.options.find("_language") != exportInfo.audio.options.end())
		{
			auto language = exportInfo.audio.options.at("_language");
			av_dict_set(&encoderContext->stream->metadata, "language", language.c_str(), 0);
		}
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

	if (hwDev)
		av_buffer_unref(&hwDev);
}

AVMediaType EncoderEngine::getNextFrameType()
{
	// Bobbing doubles the frame rate and thus the video pts
	int videoPts = videoContext.next_pts;
	if (exportInfo.video.flags & VKEncVideoFlags::VK_FLAG_DEINTERLACE_BOBBING)
		videoPts *= 2;

	return (av_compare_ts(videoPts, videoContext.codecContext->time_base, audioContext.next_pts, audioContext.codecContext->time_base) <= 0) ? AVMEDIA_TYPE_VIDEO : AVMEDIA_TYPE_AUDIO;
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
	return audioContext.codecContext->frame_size;
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
	// Return false
	if (!videoContext.codecContext)
		return -1;

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
			videoContext.frameFilter->configure(videoContext.codecContext, options, filterconfig.substr(1).c_str());

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
	// Return false
	if (!audioContext.codecContext)
		return -1;

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
			audioContext.frameFilter->configure(audioContext.codecContext, options, filterconfig.c_str());

			// Log filters
			vkLogInfo("Applying audio filters: " + filterconfig);
		}
	}

	// Adding 1024 somehow fixes the AAC offset
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
			return ret;

		AVFrame *tmp_frame;
		tmp_frame = av_frame_alloc();

		// Receive frames from the frame filter
		while (context->frameFilter->receiveFrame(tmp_frame) >= 0)
		{
			// Not sure why nb_samples != frame_size the first 2 times
			if (tmp_frame->nb_samples == context->codecContext->frame_size && 
				(ret = sendFrame(context->codecContext, context->stream, tmp_frame)) < 0)
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
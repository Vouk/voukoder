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
#include <map>
#include "EncoderUtils.h"
#include "LanguageUtils.h"
#include "NvidiaCustomOptions.h"
#include "OptionResourceUtils.h"
#include "Log.h"
#include "RegistryUtils.h"

bool EncoderUtils::Create(EncoderInfo &encoderInfo, json resource, bool validateEncoder)
{
	encoderInfo.id = resource["id"].get<std::string>();
	encoderInfo.name = resource["name"].get<std::string>();
	encoderInfo.type = GetMediaType(encoderInfo.id);

	// Default parameters
	for (auto &item : resource["defaults"].items())
	{
		std::string value = item.value().get<std::string>();
		encoderInfo.defaults.insert(make_pair(item.key(), value));
	}

	// Additional NVENC parameters
	if (encoderInfo.id == "h264_nvenc" || encoderInfo.id == "hevc_nvenc")
	{
		NvidiaCustomOptions::GetOptions(encoderInfo);
	}

	// Parse groups
	for (json group : resource["groups"])
	{
		if (group["id"].get<std::string>().find('.') == std::string::npos)
			group["id"] = encoderInfo.id + '.' + group["id"].get<std::string>();

		EncoderGroupInfo encoderGroupInfo;
		encoderGroupInfo.id = group["id"].get<std::string>();
		encoderGroupInfo.name = Trans(encoderGroupInfo.id);
		encoderGroupInfo.groupClass = group["class"].get<std::string>();

		for (json obj2 : group["properties"])
		{
			if (obj2["id"].get<std::string>().find('.') == std::string::npos)
				obj2["id"] = encoderGroupInfo.id + '.' + obj2["id"].get<std::string>();

			EncoderOptionInfo encoderOptionInfo;
			if (OptionResourceUtils::CreateOptionInfo(encoderOptionInfo, obj2))
			{
				encoderGroupInfo.options.push_back(encoderOptionInfo);
			}
		}

		encoderInfo.groups.push_back(encoderGroupInfo);
	}

	// Optional: Is a format set?
	if (resource.find("parameterGroups") != resource.end())
	{
		for (auto &item : resource["parameterGroups"].items())
		{
			std::vector<std::string> paramGroup = item.value().get<std::vector<std::string>>();
			encoderInfo.paramGroups.insert(make_pair(item.key(), paramGroup));
		}
	}

	// Optional: Presets
	if (resource.find("presets") != resource.end())
		encoderInfo.presets = resource["presets"].get<std::vector<PresetInfo>>();

	return true;
}

AVMediaType EncoderUtils::GetMediaType(const wxString codecId)
{
	// Is this a codec?
	AVCodec *codec = avcodec_find_encoder_by_name(codecId);
	if (codec != NULL)
		return codec->type;

	// Is it a filter?
	const AVFilter *filter = avfilter_get_by_name(codecId.After('.'));
	if (filter != NULL)
	{
		if (avfilter_pad_count(filter->outputs) > 0)
			return avfilter_pad_get_type(filter->outputs, 0);
	}

	// Workaround
	if (codecId == "filter.zscale")
		return AVMEDIA_TYPE_VIDEO;

	return AVMEDIA_TYPE_UNKNOWN;
}

bool EncoderUtils::IsEncoderAvailable(const wxString name)
{
	// Enable logging
	if (RegistryUtils::GetValue(VKDR_REG_LOW_LEVEL_LOGGING, false))
	{
		av_log_set_level(AV_LOG_DEBUG);
		av_log_set_callback([](void*, int level, const char* szFmt, va_list varg) 
			{
				char logbuf[2000];
				vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
				logbuf[sizeof(logbuf) - 1] = '\0';

				Log::instance()->AddLine(wxT("    FF: ") + wxString(logbuf).Trim());
			});
	}

	bool ret = false;

	AVCodec *codec = avcodec_find_encoder_by_name(name);
	if (codec != NULL)
	{
		AVCodecContext* codecContext = avcodec_alloc_context3(codec);
		if (codecContext != NULL)
		{
			codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

			if (codec->type == AVMEDIA_TYPE_VIDEO)
			{
				codecContext->width = 1280;
				codecContext->height = 720;
				codecContext->time_base = { 1, 25 };
				codecContext->framerate = av_inv_q(codecContext->time_base);
				codecContext->sample_aspect_ratio = { 1, 1 };
				codecContext->field_order = AV_FIELD_PROGRESSIVE;
				codecContext->pix_fmt = codec->pix_fmts ? codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;
				const char* name = av_get_pix_fmt_name(codecContext->pix_fmt);
				vkLogInfoVA("Testing pix fmt: %s", name);
			}
			else if (codec->type == AVMEDIA_TYPE_AUDIO)
			{
				codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
				codecContext->channels = 2;
				codecContext->sample_rate = 48000;
				codecContext->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
			}
			
			// Open the codec
			int res = avcodec_open2(codecContext, codec, NULL);
			if (res != 0)
				vkLogInfoVA("Encoder returned: %d", res);

			// Only 0 is successful
			ret = res == 0;

			// Close the codec
			avcodec_free_context(&codecContext);
		}
	}

	// Disable log again
	av_log_set_level(AV_LOG_QUIET);
	av_log_set_callback(NULL);

	return ret;
}

void EncoderUtils::InitOptionsWithDefaults(EncoderInfo encoderInfo, OptionContainer &options)
{
	options.insert(encoderInfo.defaults.begin(), encoderInfo.defaults.end());

	for (auto group : encoderInfo.groups)
	{
		for (auto option : group.options)
		{
			if (option.isForced)
			{
				wxString value;

				switch (option.control.type)
				{
				case EncoderOptionType::ComboBox:
					value = option.control.items[option.control.selectedIndex].value;
					break;

				case EncoderOptionType::Integer:
					value = std::to_string(option.control.value.intValue);
					break;

				case EncoderOptionType::Float:
					value = std::to_string(option.control.value.floatValue);
					break;

				case EncoderOptionType::Boolean:
					value = option.control.value.boolValue ? "1" : "0";
					break;

				case EncoderOptionType::String:
					value = option.control.value.stringValue;
					break;
				default:
					continue;
				}

				if (!value.empty())
				{
					options[option.parameter] = value;
				}
			}
		}
	}
}

std::string EncoderUtils::GetParameterGroup(EncoderInfo encoderInfo, std::string parameter)
{
	for (auto& parameterGroup : encoderInfo.paramGroups)
	{
		if (find(parameterGroup.second.begin(), parameterGroup.second.end(), parameter) != parameterGroup.second.end())
		{
			return parameterGroup.first;
		}
	}

	return "";
}

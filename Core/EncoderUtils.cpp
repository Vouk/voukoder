#include <map>
#include "EncoderUtils.h"
#include "LanguageUtils.h"
#include "NvidiaCustomOptions.h"
#include "OptionResourceUtils.h"
#include "Log.h"

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
	{
		for (auto& item : resource["presets"])
		{
			EncoderOptionPresetGroup group;
			CreateEncoderOptionPresetGroup(group, item);

			encoderInfo.presets.push_back(group);
		}
	}

	return true;
}

void EncoderUtils::CreateEncoderOptionPresetGroup(EncoderOptionPresetGroup& group, const json json)
{
	group.id = json["id"].get<std::string>();

	//
	if (json.find("presets") != json.end())
	{
		for (auto& preset : json["presets"])
		{
			EncoderOptionPreset p;
			p.id = preset["id"].get<std::string>();
			p.options = preset["options"].get<std::string>();
		}
	}

	//
	if (json.find("group") != json.end())
	{
		for (auto& item : json["group"])
		{
			EncoderOptionPresetGroup subgroup;
			CreateEncoderOptionPresetGroup(subgroup, item);

			group.group.push_back(subgroup);
		}
	}
}

AVMediaType EncoderUtils::GetMediaType(const wxString codecId)
{
	// Is this a codec?
	AVCodec *codec = avcodec_find_encoder_by_name(codecId);
	if (codec != NULL)
	{
		return codec->type;
	}

	// Is it a filter?
	const AVFilter *filter = avfilter_get_by_name(codecId.After('.'));
	if (filter != NULL)
	{
		if (avfilter_pad_count(filter->outputs) > 0)
			return avfilter_pad_get_type(filter->outputs, 0);
	}

	return AVMEDIA_TYPE_UNKNOWN;
}

bool EncoderUtils::IsEncoderAvailable(const std::string name)
{
	bool ret = false;

	AVCodec *codec = avcodec_find_encoder_by_name(name.c_str());
	if (codec != NULL)
	{
		AVCodecContext *codecContext = avcodec_alloc_context3(codec);
		if (codecContext != NULL)
		{
			codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

			if (codec->type == AVMEDIA_TYPE_VIDEO)
			{
				codecContext->width = 1920;
				codecContext->height = 1080;
				codecContext->time_base = { 1, 25 };
				codecContext->pix_fmt = codec->pix_fmts ? codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;
			}
			else if (codec->type == AVMEDIA_TYPE_AUDIO)
			{
				codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
				codecContext->channels = 2;
				codecContext->sample_rate = 48000;
				codecContext->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
				//codecContext->bit_rate = 0;
			}

			// Open the codec
			ret = (avcodec_open2(codecContext, codec, NULL) == 0);

			// Close the codec
			avcodec_free_context(&codecContext);
		}
	}

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

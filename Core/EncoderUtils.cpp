#include <map>
#include "EncoderUtils.h"
#include "LanguageUtils.h"
#include "NvidiaCustomOptions.h"
#include "OptionResourceUtils.h"
#include "Log.h"

bool EncoderUtils::Create(EncoderInfo &encoderInfo, const json resource)
{
	string codecId = resource["id"].get<string>();
	
	vkLogInfo("Loading: encoders/%s.json", codecId.c_str());

	// Is this encoder supported?
	if (!IsAvailable(codecId))
	{
		vkLogInfo("Unloading: encoders/%s.json", codecId.c_str());
		return false;
	}

	// Parse encoder info
	encoderInfo.id = codecId;
	string name = resource["name"].get<string>();
	encoderInfo.name = wstring(name.begin(), name.end());
	encoderInfo.type = GetMediaType(codecId);

	// Don't support unkown media types
	if (encoderInfo.type == AVMediaType::AVMEDIA_TYPE_UNKNOWN)
	{
		return false;
	}

	// Default parameters
	for (auto &item : resource["defaults"].items())
	{
		string value = item.value().get<string>();
		encoderInfo.defaults.insert(make_pair(item.key(), value));
	}

	// Additional NVENC parameters
	if (codecId == "h264_nvenc" || codecId == "hevc_nvenc")
	{
		NvidiaCustomOptions::GetOptions(encoderInfo);
	}

	// Parse groups
	for (json group : resource["groups"])
	{
		EncoderGroupInfo encoderGroupInfo;
		encoderGroupInfo.id = group["id"].get<string>();
		encoderGroupInfo.name = Trans(encoderGroupInfo.id);
		encoderGroupInfo.groupClass = group["class"].get<string>();

		for (json obj2 : group["properties"])
		{
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
			vector<string> paramGroup = item.value().get<vector<string>>();
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
	group.id = json["id"].get<string>();

	//
	if (json.find("presets") != json.end())
	{
		for (auto& preset : json["presets"])
		{
			EncoderOptionPreset p;
			p.id = preset["id"].get<string>();
			p.options = preset["options"].get<string>();
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

AVMediaType EncoderUtils::GetMediaType(const string codecId)
{
	AVCodec *codec = avcodec_find_encoder_by_name(codecId.c_str());
	if (codec != NULL)
	{
		return codec->type;
	}

	return AVMEDIA_TYPE_UNKNOWN;
}

bool EncoderUtils::IsAvailable(const string name)
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
				codecContext->time_base = { 25 , 1 };
				codecContext->pix_fmt = codec->pix_fmts ? codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;
			}
			else if (codec->type == AVMEDIA_TYPE_AUDIO)
			{
				codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
				codecContext->channels = 2;
				codecContext->sample_rate = 48000;
				codecContext->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
				codecContext->bit_rate = 0;
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
				string value;

				switch (option.control.type)
				{
				case EncoderOptionType::ComboBox:
					value = option.control.items[option.control.selectedIndex].value;
					break;

				case EncoderOptionType::Integer:
					value = to_string(option.control.value.intValue);
					break;
				
				case EncoderOptionType::Float:
					value = to_string(option.control.value.floatValue);
					break;

				case EncoderOptionType::Boolean:
					value = option.control.value.boolValue ? "1" : "0";
					break;

				case EncoderOptionType::String:
					value = option.control.value.stringValue;
					break;
				}

				if (!value.empty())
				{
					options[option.parameter] = value;
				}
			}
		}
	}
}

string EncoderUtils::GetParameterGroup(EncoderInfo encoderInfo, string parameter)
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
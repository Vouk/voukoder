#include <map>
#include "EncoderUtils.h"
#include "Translation.h"
#include "NvidiaCustomOptions.h"

bool EncoderUtils::Create(EncoderInfo &encoderInfo, const json resource)
{
	string codecId = resource["id"].get<string>();
	
	// Is this encoder supported?
	if (!IsAvailable(codecId))
	{
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
			if (CreateOptionInfo(encoderOptionInfo, obj2))
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

	return true;
}

bool EncoderUtils::CreateOptionInfo(EncoderOptionInfo &optionInfo, const json resource)
{
	optionInfo.id = resource["id"].get<string>();
	optionInfo.name = Trans(optionInfo.id, "label");
	optionInfo.description = Trans(optionInfo.id, "description");

	// Optional: Is a format set?
	if (resource.find("format") != resource.end())
	{
		optionInfo.format = resource["format"].get<string>();
	}

	// Optional: Is it a forced parameter?
	if (resource.find("forced") != resource.end())
	{
		optionInfo.isForced = resource["forced"].get<bool>();
	}

	// Optional: Is this property assigned to a parameter name?
	if (resource.find("parameter") != resource.end())
	{
		optionInfo.parameter = resource["parameter"].get<string>();
	}

	// Optional: 
	if (resource.find("preprendNoIfFalse") != resource.end())
	{
		optionInfo.preprendNoIfFalse = resource["preprendNoIfFalse"].get<bool>();
	}

	// Get the control type
	string type = resource["control"]["type"].get<string>();

	// Parse data types
	if (type == "integer")
	{
		optionInfo.control.type = EncoderOptionType::Integer;
		optionInfo.control.minimum.intValue = resource["control"]["minimum"].get<int>();
		optionInfo.control.maximum.intValue = resource["control"]["maximum"].get<int>();
		optionInfo.control.singleStep.intValue = resource["control"]["singleStep"].get<int>();
		optionInfo.control.value.intValue = resource["control"]["value"].get<int>();
	}
	else if (type == "float")
	{
		optionInfo.control.type = EncoderOptionType::Float;
		optionInfo.control.minimum.floatValue = resource["control"]["minimum"].get<float>();
		optionInfo.control.maximum.floatValue = resource["control"]["maximum"].get<float>();
		optionInfo.control.singleStep.floatValue = resource["control"]["singleStep"].get<float>();
		optionInfo.control.value.floatValue = resource["control"]["value"].get<float>();
	}
	else if (type == "boolean")
	{
		optionInfo.control.type = EncoderOptionType::Boolean;
		optionInfo.control.value.boolValue = resource["control"]["value"].get<bool>();
	}
	else if (type == "string")
	{
		optionInfo.control.type = EncoderOptionType::String;
		optionInfo.control.value.stringValue = resource["control"]["value"].get<string>();

		if (resource["control"].find("regex") != resource["control"].end())
		{
			optionInfo.control.regex = resource["control"]["regex"].get<string>();
		}
	}
	else if (type == "combobox")
	{
		optionInfo.control.type = EncoderOptionType::ComboBox;

		// Default selected item
		optionInfo.control.selectedIndex = resource["control"]["selectedIndex"].get<int>();

		// All items
		int idx = 0;
		for (json item : resource["control"]["items"])
		{
			// Create combo item
			EncoderOptionInfo::ComboItem comboItem;
			comboItem.id = optionInfo.id + "._item_" + to_string(idx++);
			comboItem.name = Trans(comboItem.id);

			if (item.find("value") != item.end())
			{
				comboItem.value = item["value"].get<string>();
			}

			// Parse filters
			CreateOptionFilterInfos(comboItem.filters, item, optionInfo.id);

			optionInfo.control.items.push_back(comboItem);
		}
	}

	// Parse filters
	CreateOptionFilterInfos(optionInfo.filters, resource, optionInfo.id);

	return true;
}

bool EncoderUtils::CreateOptionFilterInfos(vector<OptionFilterInfo> &filters, json resource, string id)
{
	if (resource.find("filters") != resource.end())
	{
		for (json filterDefinition : resource["filters"])
		{
			OptionFilterInfo optionFilterInfo;
			if (CreateOptionFilterInfo(optionFilterInfo, filterDefinition, id))
			{
				filters.push_back(optionFilterInfo);
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool EncoderUtils::CreateOptionFilterInfo(OptionFilterInfo &optionFilterInfo, const json resource, const string ownerId)
{
	optionFilterInfo.name = resource["filter"].get<string>();
	optionFilterInfo.ownerId = ownerId;

	OptionFilterInfo::Params params;

	for (auto param : resource["params"].items())
	{
		vector<OptionFilterInfo::Arguments> options;

		for (auto& item : param.value())
		{
			OptionFilterInfo::Arguments option;

			for (auto b : item.items())
			{
				json data = b.value();

				OptionValue value;
				if (data.is_string())
				{
					value.stringValue = data.get<string>();
				}
				else if (data.is_number_integer())
				{
					value.intValue = data.get<int>();
				}
				else if (data.is_number_float())
				{
					value.floatValue = data.get<float>();
				}
				else if (data.is_boolean())
				{
					value.boolValue = data.get<bool>();
				}

				option.insert(make_pair(b.key(), value));
			}

			options.push_back(option);
		}

		optionFilterInfo.params.insert(make_pair(param.key(), options));
	}

	return true;
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

	int level = av_log_get_level();
	av_log_set_level(AV_LOG_QUIET);

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

	av_log_set_level(level);

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
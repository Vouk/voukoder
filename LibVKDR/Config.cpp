#include "Config.h"
#include "lavf.h"

using namespace LibVKDR;

Config::Config()
{
	av_register_all();
	avfilter_register_all();

	// Get the DLL module handle
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery(&dummy, &mbi, sizeof(mbi));
	HMODULE hModule = reinterpret_cast<HMODULE>(mbi.AllocationBase);

	map<string, json> resources;
	if (loadResources(hModule, MAKEINTRESOURCE(ID_JSON), &resources))
	{
		initEncoders(resources);
		initFixedParams(resources);
		initMultiplexers(resources);
	}
}

#pragma region Resource loader

static inline BOOL EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG lParam)
{
	map<string, json>& resources = *(map<string, json>*)lParam;

	const HRSRC hRes = FindResourceEx(hModule, lpType, lpName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	if (NULL != hRes)
	{
		HGLOBAL hData = LoadResource(hModule, hRes);
		if (NULL != hData)
		{
			const DWORD dataSize = SizeofResource(hModule, hRes);
			char *resource = new char[dataSize + 1];
			memcpy(resource, LockResource(hData), dataSize);
			resource[dataSize] = 0;
			FreeResource(hData);

			const json jsonRes = json::parse(resource);
			const string name = jsonRes["name"].get<string>();

			resources.insert(make_pair(name, jsonRes));
		}
	}

	return TRUE;
}

bool Config::loadResources(HMODULE hModule, LPTSTR lpType, map<string, json> *resources)
{
	resources->clear();

	return EnumResourceNames(hModule, lpType, (ENUMRESNAMEPROC)EnumNamesFunc, (LONG_PTR)resources);
}

#pragma endregion

bool Config::initFixedParams(const json resources)
{
	// Framerates
	for (const json framerate : resources["config"]["videoFramerates"])
	{
		FramerateInfo framerateInfo;
		framerateInfo.num = framerate["num"].get<int>();
		framerateInfo.den = framerate["den"].get<int>();
		framerateInfo.text = framerate["text"].get<string>();

		Framerates.push_back(framerateInfo);
	}

	// Samplerates
	for (const json samplerate : resources["config"]["audioSampleRates"])
	{
		SamplerateInfo samplerateInfo;
		samplerateInfo.id = samplerate["id"].get<int>();
		samplerateInfo.text = samplerate["text"].get<string>();

		Samplerates.push_back(samplerateInfo);
	}

	// Channels
	for (const json channels : resources["config"]["audioChannels"])
	{
		ChannelInfo channelInfo;
		channelInfo.id = channels["id"].get<int>();
		channelInfo.text = channels["text"].get<string>();

		Channels.push_back(channelInfo);
	}

	return true;
}

bool Config::initMultiplexers(const json resources)
{
	for (const json multiplexer : resources["config"]["multiplexers"])
	{
		const string multiplexerName = multiplexer["name"].get<string>();

		AVOutputFormat *format = av_guess_format(multiplexerName.c_str(), NULL, NULL);
		if (format != NULL)
		{
			MultiplexerInfo multiplexerInfo;
			multiplexerInfo.id = multiplexer["id"].get<int>();
			multiplexerInfo.name = multiplexerName;
			multiplexerInfo.extension = multiplexer["extension"].get<string>();
			multiplexerInfo.text = format->long_name;

			Multiplexers.push_back(multiplexerInfo);

			if (DefaultMultiplexer == -1)
				DefaultMultiplexer = multiplexerInfo.id;
		}
	}

	return true;
}

bool Config::initEncoders(const json resources)
{
	for (const json encoderDescriptor : resources["config"]["encoders"])
	{
		const string encoderName = encoderDescriptor["name"].get<string>();

		if (resources.find(encoderName) != resources.end() && checkEncoderAvailability(encoderName))
		{
			const json encoder = resources[encoderName];

			const AVCodec *codec = avcodec_find_encoder_by_name(encoderName.c_str());
			if (codec != NULL)
			{
				EncoderInfo encoderInfo;
				encoderInfo.id = encoderDescriptor["id"].get<int>();
				encoderInfo.name = encoder["name"].get<string>();
				encoderInfo.text = codec->long_name;

				if (codec->type == AVMEDIA_TYPE_VIDEO)
				{
					encoderInfo.encoderType = EncoderType::Video;
				}
				else if (codec->type == AVMEDIA_TYPE_AUDIO)
				{
					encoderInfo.encoderType = EncoderType::Audio;
				}
				
				if (encoder.find("defaultPixelFormat") != encoder.end())
				{
					encoderInfo.defaultPixelFormat = encoder["defaultPixelFormat"].get<string>();
				}

				if (encoder.find("multipass") != encoder.end())
				{
					encoderInfo.multipassParameter = encoder["multipass"]["name"].get<string>();
				}

				if (encoder.find("groups") != encoder.end())
				{
					for (const json group : encoder["groups"])
					{
						ParamGroupInfo elementGroup;
						elementGroup.name = group["name"].get<string>();
						elementGroup.label = group["label"].get<string>();
						elementGroup.parent = group["parent"].get<string>();

						encoderInfo.groups.push_back(elementGroup);
					}
				}

				if (encoder.find("parameterGroups") != encoder.end())
				{
					for (auto iterator = encoder["parameterGroups"].begin(); iterator != encoder["parameterGroups"].end(); ++iterator)
					{
						json group = iterator.value();
						EncoderParameterGroup paramGroup;
						paramGroup.name = iterator.key();
						paramGroup.delimiter = group["delimiter"].get<string>();
						paramGroup.separator = group["separator"].get<string>();
						paramGroup.noValueReplacement = group["noValueReplacement"].get<string>();
						paramGroup.parameters = group["parameters"].get<vector<string>>();

						encoderInfo.paramGroups.push_back(paramGroup);
					}
				}

				if (encoder.find("params") != encoder.end())
				{
					for (const json param : encoder["params"])
					{
						const ParamInfo paramInfo = createParamInfo(param);
						encoderInfo.params.push_back(paramInfo);
					}
				}

				Encoders.push_back(encoderInfo);
				
				if (encoderInfo.encoderType == EncoderType::Video && DefaultVideoEncoder == -1)
					DefaultVideoEncoder = encoderInfo.id;

				if (encoderInfo.encoderType == EncoderType::Audio && DefaultAudioEncoder == -1)
					DefaultAudioEncoder = encoderInfo.id;
			}
		}
	}

	return true;
}

ParamInfo Config::createParamInfo(json param)
{
	ParamInfo paramInfo;
	paramInfo.isSubValue = false;
	paramInfo.name = param["name"].get<string>();
	paramInfo.label = param["label"].get<string>();
	paramInfo.type = param["type"].get<string>();
	paramInfo.group = param["group"].get<string>();
	paramInfo.flags = param["flags"].get<vector<string>>();

	if (param.find("useDefaultValue") != param.end())
	{
		paramInfo.useDefaultValue = param["useDefaultValue"].get<bool>();
	}
	else
	{
		paramInfo.useDefaultValue = false;
	}

	// Type specific fields
	if (paramInfo.type == "int")
	{
		paramInfo.default.intValue = param["defaultValue"].get<int>();
		if (param.find("minValue") != param.end())
		{
			paramInfo.min.intValue = param["minValue"].get<int>();
		}
		if (param.find("maxValue") != param.end())
		{
			paramInfo.max.intValue = param["maxValue"].get<int>();
		}
	}
	else if (paramInfo.type == "float")
	{
		paramInfo.default.floatValue = param["defaultValue"].get<float>();
		if (param.find("minValue") != param.end())
		{
			paramInfo.min.floatValue = param["minValue"].get<float>();
		}
		if (param.find("maxValue") != param.end())
		{
			paramInfo.max.floatValue = param["maxValue"].get<float>();
		}
	}
	else if (paramInfo.type == "bool")
	{
		paramInfo.default.intValue = param["defaultValue"].get<int>();
	}

	// Are the values?
	if (param.find("values") != param.end())
	{
		for (auto itValue = param["values"].begin(); itValue != param["values"].end(); ++itValue)
		{
			ParamValueInfo paramValueInfo = createValueInfo(*itValue);
			paramInfo.values.push_back(paramValueInfo);
		}
	}

	// Parameters
	if (param.find("parameters") != param.end())
	{
		for (auto iterator = param["parameters"].begin(); iterator != param["parameters"].end(); ++iterator)
		{
			string name = iterator.key();
			string value = iterator.value();
			paramInfo.parameters.insert({ name, value });
		}
	}

	return paramInfo;
}

ParamValueInfo Config::createValueInfo(json json)
{
	ParamValueInfo paramValueInfo;
	paramValueInfo.id = json["id"].get<int>();
	paramValueInfo.name = json["name"].get<string>();

	if (json.find("parameters") != json.end())
	{
		paramValueInfo.parameters = json["parameters"].get<map<string, string>>();
	}

	if (json.find("pixelFormat") != json.end())
	{
		paramValueInfo.pixelFormat = json["pixelFormat"].get<string>();
	}

	// Parameters
	if (json.find("parameters") != json.end())
	{
		for (auto iterator = json["parameters"].begin(); iterator != json["parameters"].end(); ++iterator)
		{
			string name = iterator.key();
			string value = iterator.value();
			paramValueInfo.parameters.insert({ name, value });
		}
	}

	// Do we have suboptions at all?
	if (json.find("subvalues") != json.end())
	{
		for (auto itSubvalues = json["subvalues"].begin(); itSubvalues != json["subvalues"].end(); ++itSubvalues)
		{
			ParamSubValueInfo paramSubValueInfo = createSubValue(*itSubvalues);
			paramValueInfo.subValues.push_back(paramSubValueInfo);
		}
	}

	return paramValueInfo;
}

ParamSubValueInfo Config::createSubValue(json json)
{
	ParamSubValueInfo paramSubValueInfo;
	paramSubValueInfo.isSubValue = true;
	paramSubValueInfo.name = json["name"].get<string>();
	paramSubValueInfo.label = json["label"].get<string>();
	paramSubValueInfo.type = json["type"].get<string>();
	paramSubValueInfo.flags = json["flags"].get<vector<string>>();

	if (json.find("pixelFormat") != json.end())
	{
		paramSubValueInfo.pixelFormat = json["pixelFormat"].get<string>();
	}

	if (json.find("useDefaultValue") != json.end())
	{
		paramSubValueInfo.useDefaultValue = json["useDefaultValue"].get<bool>();
	}

	if (json.find("parameters") != json.end())
	{
		paramSubValueInfo.parameters = json["parameters"].get<map<string, string>>();
	}

	if (paramSubValueInfo.type == "int")
	{
		paramSubValueInfo.default.intValue = json["defaultValue"].get<int>();

		if (json.find("minValue") != json.end())
		{
			paramSubValueInfo.min.intValue = json["minValue"].get<int>();
		}

		if (json.find("maxValue") != json.end())
		{
			paramSubValueInfo.max.intValue = json["maxValue"].get<int>();
		}
	}
	else if (paramSubValueInfo.type == "float")
	{
		paramSubValueInfo.default.floatValue = json["defaultValue"].get<float>();

		if (json.find("minValue") != json.end())
		{
			paramSubValueInfo.min.floatValue = json["minValue"].get<float>();
		}

		if (json.find("maxValue") != json.end())
		{
			paramSubValueInfo.max.floatValue = json["maxValue"].get<float>();
		}
	}
	else if (paramSubValueInfo.type == "bool")
	{
		paramSubValueInfo.default.intValue = json["defaultValue"].get<int>();
	}

	if (json.find("parameters") != json.end())
	{
		for (auto iterator = json["parameters"].begin(); iterator != json["parameters"].end(); ++iterator)
		{
			string name = iterator.key();
			string value = iterator.value();
			paramSubValueInfo.parameters.insert({ name, value });
		}
	}

	return paramSubValueInfo;
}

bool Config::checkEncoderAvailability(const string encoderName)
{
	bool ret = false;

	AVCodec *codec = avcodec_find_encoder_by_name(encoderName.c_str());
	if (codec != NULL)
	{
		AVCodecContext *codecContext = avcodec_alloc_context3(codec);
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

	return ret;
}

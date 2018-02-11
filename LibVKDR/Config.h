#pragma once

#include <windows.h>
#include "json.hpp"
#include "resource.h"
#include "EncoderInfo.h"
#include "FramerateInfo.h"
#include "SamplerateInfo.h"
#include "ChannelInfo.h"
#include "MultiplexerInfo.h"

using json = nlohmann::json;

using namespace std;

namespace LibVKDR
{
	class Config
	{
	public:
		Config();
		int DefaultVideoEncoder = -1;
		int DefaultAudioEncoder = -1;
		int DefaultMultiplexer = -1;
		vector<EncoderInfo> Encoders;
		vector<FramerateInfo> Framerates;
		vector<SamplerateInfo> Samplerates;
		vector<ChannelInfo> Channels;
		vector<MultiplexerInfo> Multiplexers;

	private:
		ParamInfo createParamInfo(json json);
		ParamValueInfo createValueInfo(json json);
		ParamSubValueInfo createSubValue(json json);
		bool checkEncoderAvailability(const string encoderName);
		bool initEncoders(const json resources);
		bool initFixedParams(const json resources);
		bool initMultiplexers(const json resources);
		bool loadResources(HMODULE hModule, LPTSTR lpType, map<string, json> *resources);
	};
}
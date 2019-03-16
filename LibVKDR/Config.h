#pragma once

#include <windows.h>
#include "Callback.h"
#include "json.hpp"
#include "resource.h"
#include "EncoderInfo.h"
#include "FramerateInfo.h"
#include "SamplerateInfo.h"
#include "ChannelInfo.h"
#include "MultiplexerInfo.h"
#include "FilterInfo.h"
#include "FrameSizeInfo.h"

#include <curl/curl.h>
#pragma comment(lib, "libcurld.lib")
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wldap32.lib")

#define VKDR_UPDATE_CHECK_URL "https://www.voukoder.org/version.php?ver=%d.%d.%d"

using json = nlohmann::json;

using namespace std;

namespace LibVKDR
{
	union Version
	{
		struct number
		{
			uint8_t build;
			uint8_t patch;
			uint8_t minor;
			uint8_t major;
		} number;
		uint32_t code = 0;
	};

	struct PluginUpdate
	{
		Version version;
		string url;
		bool isUpdateAvailable;
	};

	class Config
	{
	public:
		int DefaultVideoEncoder = 0; // X264
		int DefaultAudioEncoder = 9; // PCM16
		int DefaultMultiplexer = 1;  // MKV
		vector<EncoderInfo> Encoders;
		vector<FramerateInfo> Framerates;
		vector<SamplerateInfo> Samplerates;
		vector<ChannelInfo> Channels;
		vector<MultiplexerInfo> Multiplexers;
		vector<FilterInfo> Filters;
		vector<FrameSizeInfo> FrameSizes;
		void Init();
		static int CheckForUpdate(Version version, PluginUpdate *pluginUpdate);

	private:
		map<string, json> resources;
		ParamInfo createParamInfo(json json);
		ParamValueInfo createValueInfo(json json);
		ParamSubValueInfo createSubValue(json json);
		bool checkEncoderAvailability(const string encoderName);
		bool initEncoders(const json resources);
		bool initFixedParams(const json resources);
		bool initMultiplexers(const json resources);
		bool initFilters(const json resources);
		bool loadResources(HMODULE hModule, LPTSTR lpType);
		BOOL EnumNamesFunc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG lParam);
	};
}
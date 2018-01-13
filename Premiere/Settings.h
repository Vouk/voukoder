#pragma once

#include <Windows.h>
#include "resource.h"
#include "json.hpp"
#include "EncoderInfo.h"
#include "MuxerInfo.h"
#include "lavf.h"

using namespace std;
using json = nlohmann::json;

enum class EncoderType {
	VIDEO,
	AUDIO
};

// reviewed 0.3.8
class Settings
{
public:
	const static int defaultMuxer = 0;
	const static int defaultAudioEncoder = 0;
	const static int defaultVideoEncoder = 0;
	json mainConfig;
	vector<MuxerInfo> muxerInfos;
	vector<EncoderInfo> videoEncoderInfos;
	vector<EncoderInfo> audioEncoderInfos;
	void initFromResources(HMODULE hModule);

private:
	void loadEncoderInfos(json encoders, vector<EncoderInfo> *encoderInfos);
	static json LoadSingleResource(HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName);
	static bool IsEncoderAvailable(EncoderInfo encoderinfo);
};
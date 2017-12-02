#pragma once

#include <Windows.h>
#include "resource.h"
#include "json.hpp"
#include "EncoderInfo.h"
#include "MuxerInfo.h"

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
	int defaultMuxer = 0;
	int defaultAudioEncoder = 0;
	int defaultVideoEncoder = 0;
	Settings(HMODULE hModule);
	vector<EncoderInfo> getEncoders(EncoderType encoderType);
	vector<MuxerInfo> getMuxers();
	json getConfiguration(); //TODO: Return struct

private:
	json mainConfig;
	json encoderConfig;
	json muxerConfig;
	json Settings::loadResource(HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName);
};
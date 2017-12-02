#pragma once

#include <vector>
#include <string>
#include <map>
#include "json.hpp"
#include "Settings.h"
#include "premiere_cs6\PrSDKTypes.h"
#include "premiere_cs6\PrSDKExportParamSuite.h"
extern "C" {
#include "libavutil\avutil.h"
#include "libavutil\opt.h"
}

using namespace std;
using json = nlohmann::json;

// reviewed 0.3.8
class EncoderConfig
{
public:
	EncoderConfig(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 exporterPluginID);
	void EncoderConfig::initFromSettings(EncoderInfo *encoderInfo);
	void parseCommandLine(string input);
	void EncoderConfig::getConfig(AVDictionary **options);
	void EncoderConfig::getConfig(AVDictionary **options, int maxPasses, int pass);
	string EncoderConfig::getConfigAsParamString(string dashes);
	const char* EncoderConfig::getPixelFormat();

private:
	PrSDKExportParamSuite *exportParamSuite;
	csSDK_uint32 exporterPluginID;
	csSDK_int32 multiGroupIndex;
	EncoderInfo *encoderInfo;
	map<string, string> config;
	map<string, ParamGroup> groups;
	string pixelFormat;
	void addParameters(map<string, string> parameters);
	template<class T> void addParameters(map<string, string> parameters, T value);
	bool EncoderConfig::getGroupFromParameter(string name, ParamGroup &retGroup);
	static void AddParametersToDictionary(AVDictionary **options, map<string, string> parameters);
};
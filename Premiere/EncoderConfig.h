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
	EncoderInfo *encoderInfo;
	EncoderConfig(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 exporterPluginID);
	void initFromSettings(EncoderInfo *encoderInfo);
	int getMaxPasses();
	void parseCommandLine(string input);
	void getConfig(AVDictionary **options);
	void getConfig(AVDictionary **options, int maxPasses, int pass);
	string getConfigAsParamString(string dashes);
	string getPixelFormat();

private:
	PrSDKExportParamSuite *exportParamSuite;
	csSDK_uint32 exporterPluginID;
	csSDK_int32 multiGroupIndex;
	map<string, string> config;
	map<string, ParamGroup> groups;
	string pixelFormat;
	string multipassParameter;
	void addParameters(map<string, string> parameters);
	template<class T> void addParameters(map<string, string> parameters, T value);
	bool getGroupFromParameter(string name, ParamGroup &retGroup);
	static void AddParametersToDictionary(AVDictionary **options, map<string, string> parameters);
};
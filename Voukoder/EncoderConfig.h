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

using json = nlohmann::json;

typedef struct ParamGroup
{
	std::string name;
	std::string delimiter;
	std::string separator;
	std::string noValueReplacement;
	std::vector<std::string> parameters;
} ParamGroup;

class EncoderConfig
{
public:
	EncoderConfig(PrSDKExportParamSuite *exportParamSuite, csSDK_uint32 exporterPluginID);
	~EncoderConfig();
	void initFromSettings(json settings);
	void setParam(std::string key, std::string value);
	void parseCommandLine(std::string input);
	void EncoderConfig::getConfig(AVDictionary **options);
	void EncoderConfig::getConfig(AVDictionary **options, int maxPasses, int pass);
	std::string EncoderConfig::getConfigAsParamString(std::string dashes);
	int EncoderConfig::getMaxPasses();

private:
	PrSDKExportParamSuite *exportParamSuite;
	csSDK_uint32 exporterPluginID;
	csSDK_int32 multiGroupIndex;
	json rawConfig;
	std::map<std::string, std::string> config;
	std::map<std::string, ParamGroup> groups;
	void EncoderConfig::addParameters(json parameters);
	template<class T> void addParameters(json parameters, T value);
	ParamGroup* EncoderConfig::getGroupFromParameter(std::string name);
	static void AddParametersToDictionary(AVDictionary **options, std::map<std::string, std::string> parameters);
};
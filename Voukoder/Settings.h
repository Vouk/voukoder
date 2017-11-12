#pragma once

#include <Windows.h>
#include "resource.h"
#include "json.hpp"

using json = nlohmann::json;

class Settings
{
public:
	Settings(HMODULE hModule);
	~Settings();
	json getConfiguration();
	json getDefaultAudioCodecId();
	json getDefaultMuxerId();
	json getDefaultVideoCodecId();
	json getAudioEncoders();
	json getMuxers();
	json getVideoEncoders();
	static json filterArrayById(json array, int id);
	static json filterArrayByName(json array, std::string name);
	static json filterArrayInArrayByName(json array, std::string name, std::string subname);
	static int getArrayIndex(json array, std::string value);
	static json Settings::find(json array, std::string field, std::string value);

private:
	json settings;
	json Settings::loadResource(HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName);
};
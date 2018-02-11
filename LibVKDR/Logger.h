#pragma once

#include <vector>
#include <string>
#include "lavf.h"

using namespace std;

namespace LibVKDR
{
	class Logger
	{
	public:
		Logger();
		vector<string> getLastEntries(int lines);
	};

	static inline std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	static inline std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	static inline  std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		return ltrim(rtrim(str, chars), chars);
	}
}
#pragma once

#include <functional>
#include <vector>
#include <string>
#include <fstream> 
#include "lavf.h"

using namespace std;

template <typename T>
struct AvCallback;

template <typename Ret, typename... Params>
struct AvCallback<Ret(Params...)> {
	template <typename... Args>
	static Ret callback(Args... args) {
		return func(args...);
	}
	static function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
function<Ret(Params...)> AvCallback<Ret(Params...)>::func;

namespace LibVKDR
{
	class Logger
	{
	public:
		Logger();
		~Logger();
		vector<string> getLastEntries(int lines);
		void message(const string message);

	private:
		vector<string> messages;
		ofstream ofs;
		void avCallback(void *, int level, const char * szFmt, va_list varg);
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

	static inline std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		return ltrim(rtrim(str, chars), chars);
	}
}
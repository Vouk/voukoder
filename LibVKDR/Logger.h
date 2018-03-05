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
		Logger(int pluginId);
		~Logger();
		static void vkdrInit();
		vector<string> getLastEntries(int lines);

	private:
		int pluginId;
	};
}
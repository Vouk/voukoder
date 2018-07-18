#include <cstdio>
#include <vector>
#include <sstream>
#include <Windows.h>
#include "Logger.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

using namespace LibVKDR;

Logger::Logger(int pluginId):
	pluginId(pluginId)
{
	LOG(INFO) << "(" << pluginId << ") Created plug-in instance";
}

Logger::~Logger()
{
	LOG(INFO) << "(" << pluginId << ") END OF LINE.";
}

static inline void AvCallback(void *, int level, const char * szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	string msg(logbuf);
	msg.erase(0, msg.find_first_not_of("\t\n\v\f\r "));
	msg.erase(msg.find_last_not_of("\t\n\v\f\r ") + 1);

	LOG(INFO) << msg;
}

void Logger::vkdrInit()
{
	av_log_set_level(AV_LOG_TRACE);
	av_log_set_callback(AvCallback);

	char charPath[MAX_PATH];
	if (GetTempPathA(MAX_PATH, charPath))
	{
		strcat_s(charPath, "voukoder.log");

		stringstream cfg;
		cfg << "* GLOBAL:\n FORMAT = \"[%datetime][%level] %msg\"\n FILENAME = \"";
		cfg << charPath;
		cfg << "\"\n ENABLED = true\n TO_FILE = true\n TO_STANDARD_OUTPUT = true\n SUBSECOND_PRECISION = 6\n PERFORMANCE_TRACKING = true\n";

		remove(charPath);

		el::Configurations conf;
		conf.setToDefault();
		conf.parseFromText(cfg.str());
		el::Loggers::reconfigureAllLoggers(conf);

		LOG(INFO) <<  "LibVKDR (www.voukoder.org)";
		LOG(INFO) << "by Daniel Stankewitz";
	}
}
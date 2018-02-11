#include <cstdio>
#include <vector>
#include <Windows.h>
#include "Logger.h"

using namespace LibVKDR;

static vector<string> messages;

static inline void avlog_cb(void *, int level, const char * szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	messages.push_back(trim(string(logbuf)));

	OutputDebugStringA(logbuf);
}

Logger::Logger()
{
	av_log_set_level(AV_LOG_DEBUG);
	av_log_set_callback(avlog_cb);
}

vector<string> Logger::getLastEntries(int lines)
{
	if (messages.size() < lines)
		lines = (int)messages.size();

	return vector<string>(messages.end() - lines, messages.end());
}
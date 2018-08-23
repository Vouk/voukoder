#pragma once

#include <string>
#include <Windows.h>
#include "lavf.h"

using namespace std;

class Pipe
{
public:
	Pipe();
	~Pipe();
	bool open(const wstring commandLine);
	void close();
	bool write(const uint8_t *buffer, const int size);

private:
	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
};


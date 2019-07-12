#pragma once

#include <Windows.h>

struct VoukoderSettings
{

}; 

#ifdef VOUKODER_EXPORTS
#define VOUKODER_API __declspec(dllexport)
#else
#define VOUKODER_API __declspec(dllimport)
#endif

extern "C" VOUKODER_API int open_config_dialog(VoukoderSettings settings, HWND hwnd = NULL);

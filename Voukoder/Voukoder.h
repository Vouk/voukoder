#pragma once

#include <Windows.h>
#include <string>

#define VOUKODER_API __declspec(dllexport)

extern "C" VOUKODER_API void init();
extern "C" VOUKODER_API bool open_config_dialog(std::string* settings, HWND hwnd = NULL);

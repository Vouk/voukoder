#pragma once

#include <Windows.h>
#include <string>

#define VOUKODER_API __declspec(dllexport)

extern "C" VOUKODER_API void init();
extern "C" VOUKODER_API bool open_config_dialog(std::string* settings, HWND hwnd = NULL);
extern "C" VOUKODER_API bool encoder_open(const char* settings);
extern "C" VOUKODER_API void encoder_close();
extern "C" VOUKODER_API bool encoder_write_video_frame(int width, int height, const char* pixfmt, int idx, uint8_t** data, int* linesize);
extern "C" VOUKODER_API bool encoder_write_audio_frame(const char* smpfmt, uint8_t** data, int size);


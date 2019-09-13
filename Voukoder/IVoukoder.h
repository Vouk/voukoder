/**
 * MIT License
 *
 * Copyright (c) 2019 Daniel Stankewitz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <ComDef.h>

#define VOUKODER_CONFIG_VERSION 1

 // {E9661BFA-4B8E-4217-BCD8-24074D75000B}
_declspec(selectany) GUID CLSID_Voukoder = {
	0xe9661bfa, 0x4b8e, 0x4217, { 0xbc, 0xd8, 0x24, 0x7, 0x4d, 0x75, 0x0, 0xb }
};

struct rational
{
	int64_t num;
	int64_t den;
};

enum fieldorder
{
	progressive = 0,
	top,
	bottom
};

enum LogLevel
{
	Info = 0,
	Warn,
	Error
};

struct VOUKODER_TRACK_CONFIG
{
	char encoder[16];
	char options[16384];
	char filters[16384];
	char sidedata[16384];
	char format[16];
};

struct VOUKODER_FORMAT_CONFIG
{
	char container[16];
	bool faststart;
};

struct VOUKODER_CONFIG
{
	int version;
	VOUKODER_TRACK_CONFIG video;
	VOUKODER_TRACK_CONFIG audio;
	VOUKODER_FORMAT_CONFIG format;
};

interface __declspec(uuid("E26427F6-CBCA-4859-BCC3-162AF1E06CEE")) IVoukoder : public IUnknown
{
	STDMETHOD(Open)(const wchar_t* filename, const wchar_t* application, const int passes, const int width, const int height, const rational timebase, const rational aspectratio, const fieldorder fieldorder, const int samplerate, const char* channellayout)PURE;
	STDMETHOD(Close)(bool finalize = true)PURE;
	STDMETHOD(Log)(const wchar_t* text, LogLevel level = LogLevel::Info)PURE;
	STDMETHOD(SetConfig)(VOUKODER_CONFIG config)PURE;
	STDMETHOD(GetConfig)(VOUKODER_CONFIG* config)PURE;
	STDMETHOD(GetAudioChunkSize)(int* chunkSize)PURE;
	STDMETHOD(IsAudioWaiting)()PURE;
	STDMETHOD(SendAudioSamples)(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, const char* layout, const char* format)PURE;
	STDMETHOD(SendVideoFrame)(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, const char* format)PURE;
	STDMETHOD(ShowVoukoderDialog)(HANDLE act_ctx = NULL, HINSTANCE instance = NULL)PURE;
};
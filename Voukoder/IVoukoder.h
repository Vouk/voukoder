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

// {E9661BFA-4B8E-4217-BCD8-24074D75000B}
_declspec(selectany) GUID CLSID_Voukoder = {
	0xe9661bfa, 0x4b8e, 0x4217, { 0xbc, 0xd8, 0x24, 0x7, 0x4d, 0x75, 0x0, 0xb } 
};

struct rational
{
	int num;
	int den;
};

enum fieldorder
{
	progressive = 0,
	top,
	bottom
};

struct VOUKODERINFO
{
	wchar_t filename[MAX_PATH];
	wchar_t application[128];
	int passes;

	struct
	{
		char container[16];
		bool faststart;
	} format;

	struct
	{
		int width;
		int height;
		rational timebase;
		rational aspectratio;
		fieldorder fieldorder;
		char pixelformat[16];
		char encoder[16];
		//exportInfo.video.options.Deserialize("");
		//exportInfo.video.filters.Deserialize("");
		//exportInfo.video.sideData.Deserialize("");
	} video;

	struct
	{
		int samplerate;
		char sampleformat[16];
		char channellayout[32];
		char encoder[16];
		//exportInfo.audio.options.Deserialize("");
		//exportInfo.audio.filters.Deserialize("");
		//exportInfo.audio.sideData.Deserialize("");
	} audio;
};

interface __declspec(uuid("E26427F6-CBCA-4859-BCC3-162AF1E06CEE")) IVoukoder : public IUnknown
{
	STDMETHOD(Open)(VOUKODERINFO info)PURE;
	STDMETHOD(Close)(bool finalize = true)PURE;
	STDMETHOD(IsAudioWaiting)()PURE;
	STDMETHOD(SendAudioSamples)(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, const char* layout, const char* format)PURE;
	STDMETHOD(SendVideoFrame)(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, const char* format)PURE;
};
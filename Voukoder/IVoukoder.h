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
#include <string>

#define VOUKODER_CONFIG_VERSION 1

 // {E9661BFA-4B8E-4217-BCD8-24074D75000B}
_declspec(selectany) GUID CLSID_Voukoder = {
	0xe9661bfa, 0x4b8e, 0x4217, { 0xbc, 0xd8, 0x24, 0x7, 0x4d, 0x75, 0x0, 0xb }
};

namespace Voukoder
{
	struct Rational
	{
		int num;
		int den;
	};

	enum FieldOrder
	{
		Progressive = 0,
		Top,
		Bottom
	};

	enum LogLevel
	{
		Info = 0,
		Warn,
		Error
	};

	enum ChannelLayout
	{
		Mono = 0,
		Stereo,
		FivePointOne
	};

	enum ColorRange {
		Limited = 0,
		Full
	};

	enum ColorSpace {
		bt601_PAL = 0,
		bt601_NTSC,
		bt709,
		bt2020_CL,
		bt2020_NCL
	};

	struct TRACK_CONFIG
	{
		wchar_t encoder[16] = {};
		wchar_t options[16384] = {};
		wchar_t filters[16384] = {};
		wchar_t sidedata[16384] = {};
		wchar_t format[16] = {};
	};

	struct FORMAT_CONFIG
	{
		wchar_t container[16] = {};
		bool faststart = false;
	};

	struct CONFIG
	{
		int version;
		TRACK_CONFIG video;
		TRACK_CONFIG audio;
		FORMAT_CONFIG format;
	};

	struct INFO
	{
		std::wstring filename;
		std::wstring application;

		struct {
			bool enabled;
			int width;
			int height;
			Rational timebase;
			Rational aspectratio;
			FieldOrder fieldorder;
			ColorRange colorRange;
			ColorSpace colorSpace;
		} video;

		struct {
			bool enabled;
			int samplerate;
			ChannelLayout channellayout;
			int numberChannels;
		} audio;
	};
}

interface __declspec(uuid("E26427F6-CBCA-4859-BCC3-162AF1E06CEE")) IVoukoder : public IUnknown
{
	STDMETHOD_(void, Close)(bool finalize = true)PURE;
	STDMETHOD_(void, GetConfig)(Voukoder::CONFIG* config)PURE;
	STDMETHOD_(void, GetAudioChunkSize)(int* chunkSize)PURE;
	STDMETHOD_(bool, GetFileExtension)(std::wstring& extension)PURE;
	STDMETHOD_(int, GetMaxPasses)()PURE;
	STDMETHOD_(bool, IsAudioActive)()PURE;
	STDMETHOD_(bool, IsAudioWaiting)()PURE;
	STDMETHOD_(bool, IsVideoActive)()PURE;
	STDMETHOD_(void, Log)(std::string text)PURE;
	STDMETHOD_(bool, Open)(const Voukoder::INFO info)PURE;
	STDMETHOD_(bool, SendAudioSampleChunk)(uint8_t** buffer, int samples, int blockSize, int planes, int sampleRate, const Voukoder::ChannelLayout layout, const char* format)PURE;
	STDMETHOD_(bool, SendVideoFrame)(int64_t idx, uint8_t** buffer, int* rowsize, int planes, int width, int height, int pass, const char* format)PURE;
	STDMETHOD_(bool, ShowVoukoderDialog)(bool video = true, bool audio = true, HANDLE act_ctx = NULL, HINSTANCE instance = NULL)PURE;
	STDMETHOD_(void, SetConfig)(Voukoder::CONFIG& config)PURE;
};
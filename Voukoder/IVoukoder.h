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

struct Rational
{
	INT num;
	INT den;
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

struct VKTRACKCONFIG
{
	WCHAR encoder[16];
	WCHAR options[16384];
	WCHAR filters[16384];
	WCHAR sidedata[16384];
	WCHAR format[16];
};

struct VKENCODERCONFIG
{
	INT version;
	VKTRACKCONFIG video;
	VKTRACKCONFIG audio;

	struct
	{
		WCHAR container[16];
		BOOL faststart;
	} format;
};

struct VKENCODERINFO
{
	BSTR filename;
	BSTR application;

	struct {
		BOOL enabled;
		INT width;
		INT height;
		Rational timebase;
		Rational aspectratio;
		FieldOrder fieldorder;
		ColorRange colorRange;
		ColorSpace colorSpace;
	} video;

	struct {
		BOOL enabled;
		INT samplerate;
		ChannelLayout channellayout;
		INT numberChannels;
	} audio;
};


struct VKAUDIOCHUNK
{
	BYTE** buffer;
	INT samples;
	INT blockSize;
	INT planes;
	INT sampleRate;
	ChannelLayout layout;
	CHAR format[16];
};

struct VKVIDEOFRAME
{
	LONGLONG idx; // Not needed anymore
	BYTE** buffer;
	INT* rowsize;
	INT planes;
	INT width;
	INT height;
	INT pass;
	CHAR format[16];
};

interface __declspec(uuid("E26427F6-CBCA-4859-BCC3-162AF1E06CEE")) IVoukoder : public IUnknown
{
	STDMETHOD(Close)(BOOL finalize)PURE;
	STDMETHOD(GetConfig)(VKENCODERCONFIG* config)PURE;
	STDMETHOD(GetAudioChunkSize)(UINT* chunkSize)PURE;
	STDMETHOD(GetFileExtension)(BSTR* extension)PURE;
	STDMETHOD(GetMaxPasses)(UINT* passes)PURE;
	STDMETHOD(IsAudioActive)(BOOL* isActive)PURE;
	STDMETHOD(IsAudioWaiting)(BOOL* isWaiting)PURE;
	STDMETHOD(IsVideoActive)(BOOL* isActive)PURE;
	STDMETHOD(Log)(BSTR text)PURE;
	STDMETHOD(Open)(const VKENCODERINFO info)PURE;
	STDMETHOD(SendAudioSampleChunk)(VKAUDIOCHUNK chunk)PURE;
	STDMETHOD(SendVideoFrame)(VKVIDEOFRAME frame)PURE;
	STDMETHOD(ShowVoukoderDialog)(BOOL video, BOOL audio, BOOL* isOkay, HANDLE act_ctx, HINSTANCE instance)PURE;
	STDMETHOD(SetConfig)(VKENCODERCONFIG& config)PURE;
};
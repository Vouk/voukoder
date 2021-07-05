/**
 * Voukoder
 * Copyright (C) 2017-2020 Daniel Stankewitz, All Rights Reserved
 * https://www.voukoder.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
 */
#pragma once

#include "EncoderContext.h"
#include "ExportInfo.h"
#include "lavf.h"
#include <wx/wx.h>
#include <wx/filename.h>

#define GetSideData(data, id, def) data.find(id) != data.end() ? data[id] : ""

class EncoderEngine
{
public:
	EncoderEngine(ExportInfo encoderInfo);
	~EncoderEngine();
	int open();
	void close();
	void finalize();
	void flushContext(EncoderContext *encoderContext);
	bool hasVideo();
	bool hasAudio();
	int writeVideoFrame(AVFrame *frame);
	int writeAudioFrame(AVFrame *frame);
	int getAudioFrameSize();
	AVMediaType getNextFrameType();
	int pass = 1;

private:
	AVPacket *packet = NULL;
	ExportInfo exportInfo;
	AVFormatContext *formatContext;
	EncoderContext videoContext = { 0 };
	EncoderContext audioContext = { 0 };
	int createCodecContext(const wxString codecId, EncoderContext *encoderContext, int flags);
	int encodeAndWriteFrame(EncoderContext *context, AVFrame *frame);
	int getCodecFlags(const AVMediaType type);
	int openCodec(const wxString codecId, const wxString codecOptions, EncoderContext *encoderContext, const int flags);
	int sendFrame(AVCodecContext *context, AVStream *stream, AVFrame *frame);
	int receivePackets(AVCodecContext *context, AVStream *stream);
	int injectStereoData(AVStream* stream);
	int injectSphericalData(AVStream* stream);
	wxString passLogFile;
};

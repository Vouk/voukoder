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

#include <stdint.h>
#include "ComBase.h"
#include "VoukoderTypeLib_h.h"
#include <EncoderEngine.h>

class CVoukoder : public CComBase<>, public InterfaceImpl<IVoukoder>
{
public:
	CVoukoder();
	virtual ~CVoukoder();

	STDMETHOD(Close)(BOOL finalize);
	STDMETHOD(GetConfig)(VKENCODERCONFIG *config);
	STDMETHOD(GetFileExtension)(BSTR* extension);
	STDMETHOD(GetMaxPasses(UINT* passes));
	STDMETHOD(IsAudioActive(BOOL* isActive));
	STDMETHOD(IsAudioWaiting)(BOOL* isWaiting);
	STDMETHOD(IsVideoActive(BOOL* isActive));
	STDMETHOD(Log)(BSTR text);
	STDMETHOD(Open)(const VKENCODERINFO info);
	STDMETHOD(SendAudioSampleChunk)(VKAUDIOCHUNK chunk);
	STDMETHOD(SendVideoFrame)(VKVIDEOFRAME frame);
	STDMETHOD(ShowVoukoderDialog)(BOOL video, BOOL audio, BOOL* isOkay, HANDLE act_ctx, HINSTANCE instance);
	STDMETHOD(SetConfig)(VKENCODERCONFIG config);
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv);

private:
	ExportInfo exportInfo;
	EncoderEngine* encoder = NULL;
};
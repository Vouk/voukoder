/**
 * Voukoder
 * Copyright (C) 2017-2022 Daniel Stankewitz, All Rights Reserved
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

#include <map>
#include <wx/wx.h>
#include <wx/filename.h>

#define vkLogInfo(msg) Log::instance()->AddLine(msg);
#define vkLogWarn(msg) Log::instance()->AddLine(msg);
#define vkLogError(msg) Log::instance()->AddLine(msg);
#define vkLogInfoVA(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogWarnVA(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogErrorVA(msg, ...) Log::instance()->AddLine(wxString::Format(msg, __VA_ARGS__));
#define vkLogSep() Log::instance()->AddSep();

class Log
{
private:
	std::map<wxString, wxFile*> files;
	wxString filename;
	Log();
	~Log();
	void Init(wxFile* file);
	wxString GetBaseDir();
	wxString CreateFileName();
	void Clean(int days = 7);

public:
	static Log* instance();
	void AddFile(wxString filename);
	void AddSep();
	void AddLine(wxString line);
	void AddLineToFile(wxFile* file, wxString line);
	wxString GetFilename();
	void RemoveFile(wxString filename);
};
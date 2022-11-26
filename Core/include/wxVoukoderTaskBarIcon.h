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

#include "wx/taskbar.h"
#include "wx/process.h"
#include "PluginUpdate.h"

enum
{
	PU_OPEN_HOMEPAGE = 10001,
	PU_LOG_VIEW,
	PU_LOG_SAVE,
	PU_LOG_COPY,
	PU_LOG_CLEAR,
	PU_OPEN_UPDATE_PAGE
};

class wxTempFileProcess : public wxProcess
{
public:
	wxTempFileProcess(wxString filename):
		filename(filename)
	{}

	void OnTerminate(int pid, int status)
	{
		if (wxFileExists(filename))
			wxRemoveFile(filename);
	}

private:
	wxString filename;
};

class wxVoukoderTaskBarIcon: public wxTaskBarIcon
{
public:
	wxVoukoderTaskBarIcon();
	void CheckForUpdate();
	virtual wxMenu *CreatePopupMenu() wxOVERRIDE;

private:
	PluginUpdate pluginUpdate;
	void OnOpenHomepage(wxCommandEvent&);
	void OnLogView(wxCommandEvent&);
	void OnOpenUpdatePage(wxCommandEvent&);

	wxDECLARE_EVENT_TABLE();
};
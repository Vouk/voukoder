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
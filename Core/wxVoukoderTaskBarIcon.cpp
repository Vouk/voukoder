#include "wxVoukoderTaskBarIcon.h"
#include "Voukoder.h"
#include "LanguageUtils.h"
#include <wx/clipbrd.h>
#include <wx/filename.h>

wxBEGIN_EVENT_TABLE(wxVoukoderTaskBarIcon, wxTaskBarIcon)
EVT_MENU(PU_OPEN_HOMEPAGE, wxVoukoderTaskBarIcon::OnOpenHomepage)
EVT_MENU(PU_LOG_VIEW, wxVoukoderTaskBarIcon::OnLogView)
EVT_MENU(PU_LOG_SAVE, wxVoukoderTaskBarIcon::OnLogSave)
EVT_MENU(PU_LOG_COPY, wxVoukoderTaskBarIcon::OnLogCopy)
EVT_MENU(PU_LOG_CLEAR, wxVoukoderTaskBarIcon::OnLogClear)
EVT_MENU(PU_OPEN_UPDATE_PAGE, wxVoukoderTaskBarIcon::OnOpenUpdatePage)
wxEND_EVENT_TABLE()

static bool NotificationShown = false;

wxVoukoderTaskBarIcon::wxVoukoderTaskBarIcon()
{
	pluginUpdate = Voukoder::Config::Get().update;
}

wxMenu* wxVoukoderTaskBarIcon::CreatePopupMenu()
{
	// Log menu
	wxMenu* logMenu = new wxMenu;
	logMenu->Append(PU_LOG_VIEW, Trans("ui.systray.menu.log.view"));
	logMenu->AppendSeparator();
	logMenu->Append(PU_LOG_SAVE, Trans("ui.systray.menu.log.save"));
	logMenu->Append(PU_LOG_COPY, Trans("ui.systray.menu.log.copy"));
	logMenu->AppendSeparator();
	logMenu->Append(PU_LOG_CLEAR, Trans("ui.systray.menu.log.clear"));

	// Main menu
	wxMenu *menu = new wxMenu;
	menu->Append(PU_OPEN_HOMEPAGE, Trans("ui.systray.menu.open_voukoder"));
	menu->AppendSeparator();
	menu->AppendSubMenu(logMenu, Trans("ui.systray.menu.log"));

	// Show update button
	if (pluginUpdate.isUpdateAvailable)
	{
		wxString appname = wxString::Format("%s %d.%d%s",
			VKDR_APPNAME,
			pluginUpdate.version.number.major,
			pluginUpdate.version.number.minor,
			(pluginUpdate.version.number.patch > 0) ? " beta" + pluginUpdate.version.number.patch : "");

		menu->AppendSeparator();
		menu->Append(PU_OPEN_UPDATE_PAGE, wxString::Format(Trans("ui.systray.menu.download"), appname));
	}

	return menu;
}

void wxVoukoderTaskBarIcon::CheckForUpdate()
{
	// Check for updates
	if (!NotificationShown && pluginUpdate.isUpdateAvailable)
	{
		NotificationShown = true;
		ShowBalloon(m_strTooltip, pluginUpdate.headline, 5000, 0, m_icon);
	}
}

void wxVoukoderTaskBarIcon::OnOpenHomepage(wxCommandEvent&)
{
	wxLaunchDefaultBrowser("https://www.voukoder.org/forum", wxBROWSER_NEW_WINDOW);
}

void wxVoukoderTaskBarIcon::OnLogView(wxCommandEvent&)
{
	// Create a temp log file
	wxFile file;
	wxString filename = wxFileName::CreateTempFileName("log", &file);

	// Prepare log
	wxString log = Log::instance()->GetAsString();
	log.Replace("\n", "\r\n");

	// Write a temp log file
	if (file.IsOpened())
	{
		file.Write(log);
		file.Close();
	}

	// Open temp logfile in notepad
	wxTempFileProcess* process = new wxTempFileProcess(filename);
	wxExecute("notepad.exe " + filename, 0, process);
}

void wxVoukoderTaskBarIcon::OnLogSave(wxCommandEvent&)
{
	wxFileDialog saveFileDialog(NULL, Trans("ui.systray.menu.log.save"), "", "", Trans("ui.systray.menu.log.save.filter"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() != (int)wxID_CANCEL)
	{
		wxFile file(saveFileDialog.GetPath(), wxFile::write);
		if (file.IsOpened())
		{
			wxString log = Log::instance()->GetAsString();
			log.Replace("\n", "\r\n");
			file.Write(log);
			file.Close();
		}
	}
}

void wxVoukoderTaskBarIcon::OnLogCopy(wxCommandEvent&)
{
	wxClipboard wxClip;
	if (wxClip.Open())
	{
		wxClip.Clear();
		wxClip.SetData(new wxTextDataObject(Log::instance()->GetAsString()));
		wxClip.Flush();
		wxClip.Close();
	}
}

void wxVoukoderTaskBarIcon::OnLogClear(wxCommandEvent&)
{
	Log::instance()->Clear();
}

void wxVoukoderTaskBarIcon::OnOpenUpdatePage(wxCommandEvent&)
{
	wxLaunchDefaultBrowser(pluginUpdate.url, wxBROWSER_NEW_WINDOW);
}
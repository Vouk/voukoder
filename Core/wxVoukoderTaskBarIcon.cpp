#include "wxVoukoderTaskBarIcon.h"
#include "Voukoder.h"
#include "LanguageUtils.h"

wxBEGIN_EVENT_TABLE(wxVoukoderTaskBarIcon, wxTaskBarIcon)
EVT_MENU(PU_OPEN_HOMEPAGE, wxVoukoderTaskBarIcon::OnOpenHomepage)
EVT_MENU(PU_LOG_VIEW, wxVoukoderTaskBarIcon::OnLogView)
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
	wxExecute("notepad.exe " + Log::instance()->GetFilename());
}

void wxVoukoderTaskBarIcon::OnOpenUpdatePage(wxCommandEvent&)
{
	wxLaunchDefaultBrowser(pluginUpdate.url, wxBROWSER_NEW_WINDOW);
}
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

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/gbsizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/imaglist.h>
#include <wx/base64.h>
#include <wx/event.h>
#include <wx/hyperlink.h>
#include <wx/richtext/richtextctrl.h>
#include "Voukoder.h"
#include "EncoderInfo.h"
#include "OptionContainer.h"
#include "wxPGOptionProperty.h"
#include "ExportInfo.h"
#include "wxFilterPanel.h"
#include "wxVoukoderTaskBarIcon.h"
#include "wxEncoderPage.h"
#include "BannerService.h"

WX_PG_DECLARE_EDITOR_WITH_DECL(SpinCtrl, WXDLLIMPEXP_PROPGRID)

class wxVoukoderDialog: public wxDialog
{
protected:
	wxBanner* m_topBanner = NULL;
	wxBanner* m_bottomBanner = NULL;
	wxChoice* m_genMuxFormatChoice = NULL;
	wxCheckBox* m_genMuxFaststartCheck = NULL;
	wxChoice* m_genLocLanguageChoice = NULL;
	wxCheckBox* m_checkBox1;
	wxCheckBox* m_checkBox2;
	wxVoukoderTaskBarIcon* m_voukoderTaskBarIcon = NULL;
	wxEncoderPage* m_videoPanel = NULL;
	wxEncoderPage* m_audioPanel = NULL;

public:
	wxVoukoderDialog(wxWindow *parent, ExportInfo &exportInfo);
	~wxVoukoderDialog();

private:
	std::vector<MuxerInfo> muxers;
	std::vector<EncoderInfo> videoEncoders;
	std::vector<EncoderInfo> audioEncoders;
	BannerService* bannerService;
	ExportInfo& exportInfo;
	TrackSettings videoSettings;
	TrackSettings audioSettings;
	wxControl* CreateGeneralPanel(wxWindow* parent);
	wxControl* CreateSettingsPanel(wxWindow* parent);
	wxControl* CreateAboutPanel(wxWindow* parent);
	wxPanel* CreateCenteredText(wxPanel* parent, wxString label, wxString value, wxString link = "");
	wxSize minLabelWidth;
	wxRichTextCtrl* CreateTopPatrons(wxPanel* parent);
	void InitGUI();
	template <class T> T FindInfoById(wxChoice *choice, wxString id);
	template <class T> T GetDataFromSelectedChoice(wxChoice *choice);
	template <class T> T SelectChoiceById(wxChoice *choice, wxString id, wxString def);
	void SetConfiguration();
	void OnEncoderChanged(wxEncoderChangedEvent& event);
	void OnMuxerChanged(wxCommandEvent& event);
	void OnOkayClick(wxCommandEvent& event);
};

#pragma once

#include <set>
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

WX_PG_DECLARE_EDITOR_WITH_DECL(SpinCtrl, WXDLLIMPEXP_PROPGRID)

class wxVoukoderDialog: public wxDialog
{
protected:
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
	std::set<MuxerInfo> muxers;
	std::set<EncoderInfo> videoEncoders;
	std::set<EncoderInfo> audioEncoders;
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

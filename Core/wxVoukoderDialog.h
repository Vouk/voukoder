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
#include "wxConfigurationDialog.h"
#include "wxVoukoderTaskBarIcon.h"

WX_PG_DECLARE_EDITOR_WITH_DECL(SpinCtrl, WXDLLIMPEXP_PROPGRID)

class wxVoukoderDialog: public wxDialog
{
protected:
	wxChoice* m_genEncVideoChoice = NULL;
	wxButton* m_genEncVideoConfig = NULL;
	wxChoice* m_genEncAudioChoice = NULL;
	wxButton* m_genEncAudioConfig = NULL;
	wxChoice* m_genMuxFormatChoice = NULL;
	wxCheckBox* m_genMuxFaststartCheck = NULL;
	wxChoice* m_genLocLanguageChoice = NULL;
	wxVoukoderTaskBarIcon* m_voukoderTaskBarIcon = NULL;

public:
	wxVoukoderDialog(wxWindow *parent, ExportInfo &exportInfo);
	~wxVoukoderDialog();

private:
	ExportInfo& exportInfo;
	TrackSettings videoSettings;
	TrackSettings audioSettings;
	wxPanel* CreateGeneralPanel(wxWindow* parent);
	wxPanel* CreateSettingsPanel(wxWindow* parent);
	wxPanel* CreateAboutPanel(wxWindow* parent);
	wxPanel* CreateCenteredText(wxPanel* parent, wxString label, wxString value);
	wxSize minLabelWidth;
	wxRichTextCtrl* CreateTopPatrons(wxPanel* parent);
	void InitGUI();
	template <class T> T FindInfoById(wxChoice *choice, wxString id);
	template <class T> T GetDataFromSelectedChoice(wxChoice *choice);
	template <class T> T SelectChoiceById(wxChoice *choice, wxString id, wxString def);
	void SetConfiguration();
	void UpdateFormats();
	void OnVideoEncoderChanged(wxCommandEvent& event);
	void OnAudioEncoderChanged(wxCommandEvent& event);
	void OnMuxerChanged(wxCommandEvent& event);
	void OnOkayClick(wxCommandEvent& event);
};

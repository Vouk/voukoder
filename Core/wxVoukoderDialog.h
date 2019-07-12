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

WX_PG_DECLARE_EDITOR_WITH_DECL(SpinCtrl, WXDLLIMPEXP_PROPGRID)

class wxVoukoderDialog: public wxDialog
{
protected:
	wxNotebook* m_generalTab = NULL;
	wxPanel* m_generalPanel = NULL;
	wxNotebook* m_notebook2 = NULL;
	wxPanel* m_panel2 = NULL;
	wxStdDialogButtonSizer* m_sdbSizer1 = NULL;
	wxButton* m_btnOK = NULL;
	wxButton* m_btnCancel = NULL;
	wxPanel* m_btnPanel = NULL;
	wxListbook* m_Categories = NULL;
	wxPanel* m_MainCategory = NULL;

	wxPanel* m_genPanel = NULL;
	wxNotebook* m_genNotebook = NULL;
	wxPanel* m_genTab1 = NULL;
	wxPanel* m_genEncPanel = NULL;
	wxPanel* m_genEncFormPanel = NULL;
	wxStaticText* m_genEncVideoLabel = NULL;
	wxChoice* m_genEncVideoChoice = NULL;
	wxButton* m_genEncVideoConfig = NULL;
	wxStaticText* m_genEncAudioLabel = NULL;
	wxChoice* m_genEncAudioChoice = NULL;
	wxButton* m_genEncAudioConfig = NULL;
	wxPanel* m_genMuxPanel = NULL;
	wxPanel* m_genMuxFormPanel = NULL;
	wxStaticText* m_genMuxFormatLabel = NULL;
	wxChoice* m_genMuxFormatChoice = NULL;
	wxCheckBox* m_genMuxFaststartCheck = NULL;

	wxPanel* m_genLocPanel = NULL;
	wxPanel* m_genLocFormPanel = NULL;
	wxStaticText* m_genLocFormatLabel = NULL;
	wxChoice* m_genLocLanguageChoice = NULL;
	wxCheckBox* m_genLocAutoLanguageCheck = NULL;

	wxPanel* m_generalOtherPanel = NULL;
	wxPanel* m_generalAboutPanel = NULL;
	wxPanel* m_headerPanel = NULL;
	wxPanel* m_infoPanel = NULL;

	wxPanel* m_settingsCategory = NULL;
	wxNotebook* m_settingsNotebook = NULL;
	wxPanel* m_settingsBasicPanel = NULL;
	wxPanel* m_settingsPanel = NULL;
	wxPanel* m_settingsLogPanel = NULL;
	wxTextCtrl* m_generalLogText = NULL;
	wxPanel* m_genLogButtonsPanel = NULL;
	wxButton* m_genLogClear = NULL;
	wxButton* m_genLogCopyToClipboard = NULL;

	wxPanel* m_updateCategory = NULL;
	wxNotebook* m_updateNotebook = NULL;
	wxPanel* m_updatePanel = NULL;
	wxTextCtrl* m_updateText = NULL;
	wxButton* m_updateButton = NULL;

public:
	wxVoukoderDialog(wxWindow *parent, ExportInfo &exportInfo);

private:
	ExportInfo& exportInfo;
	TrackSettings videoSettings;
	TrackSettings audioSettings;
	wxPanel* CreateCenteredText(wxPanel* parent, wxString label, wxString value);
	wxRichTextCtrl* CreateTopPatrons(wxPanel* parent);
	template <class T> T FindInfoById(wxChoice *choice, wxString id);
	template <class T> T GetDataFromSelectedChoice(wxChoice *choice);
	void InitGUI();
	template <class T> T SelectChoiceById(wxChoice *choice, wxString id, wxString def);
	void SetConfiguration();
	void UpdateFormats();
	void OnVideoEncoderChanged(wxCommandEvent& event);
	void OnAudioEncoderChanged(wxCommandEvent& event);
	void OnMuxerChanged(wxCommandEvent& event);
	void OnOkayClick(wxCommandEvent& event);
};

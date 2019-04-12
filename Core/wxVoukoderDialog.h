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
#include "Voukoder.h"
#include "EncoderInfo.h"
#include "OptionContainer.h"
#include "wxPGOptionProperty.h"
#include "ExportInfo.h"
#include "wxEncoderConfigEditor.h"
#include "wxFilterPanel.h"

WX_PG_DECLARE_EDITOR_WITH_DECL(SpinCtrl, WXDLLIMPEXP_PROPGRID)

class wxVoukoderDialog: public wxDialog
{
protected:
	wxNotebook* m_generalTab;
	wxPanel* m_generalPanel;
	wxNotebook* m_notebook2;
	wxPanel* m_panel2;
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_btnOK;
	wxButton* m_btnCancel;
	wxPanel* m_btnPanel;
	wxListbook* m_Categories;
	wxPanel* m_MainCategory;

	wxPanel* m_genPanel;
	wxNotebook* m_genNotebook;
	wxPanel* m_genTab1;
	wxPanel* m_genEncPanel;
	wxPanel* m_genEncFormPanel;
	wxStaticText* m_genEncVideoLabel;
	wxChoice* m_genEncVideoChoice;
	wxStaticText* m_genEncAudioLabel;
	wxChoice* m_genEncAudioChoice;
	wxPanel* m_genMuxPanel;
	wxPanel* m_genMuxFormPanel;
	wxStaticText* m_genMuxFormatLabel;
	wxChoice* m_genMuxFormatChoice;
	wxCheckBox* m_genMuxFaststartCheck;

	wxPanel* m_genLocPanel;
	wxPanel* m_genLocFormPanel;
	wxStaticText* m_genLocFormatLabel;
	wxChoice* m_genLocLanguageChoice;
	wxCheckBox* m_genLocAutoLanguageCheck;

	wxPanel* m_generalOtherPanel;
	wxPanel* m_generalLogPanel;
	wxTextCtrl* m_generalLogText;
	wxPanel* m_genLogButtonsPanel;
	wxButton* m_genLogClear;
	wxButton* m_genLogCopyToClipboard;
	wxPanel* m_generalAboutPanel;

	wxEncoderConfigEditor* m_videoEditor;
	wxPanel* m_videoCategory;
	wxNotebook* m_videoNotebook;
	wxPanel* m_videoEditorPanel;

	wxEncoderConfigEditor* m_audioEditor;
	wxPanel* m_audioCategory;
	wxNotebook* m_audioNotebook;
	wxPanel* m_audioEditorPanel;

	wxPanel* m_filterCategory;
	wxNotebook* m_filterNotebook;
	wxFilterPanel* m_filterPanel;

	wxPanel* m_updateCategory;
	wxNotebook* m_updateNotebook;
	wxPanel* m_updatePanel;
	wxTextCtrl* m_updateText;
	wxButton* m_updateButton;

public:
	wxVoukoderDialog(wxWindow *parent, ExportInfo &exportInfo);
	void SetConfiguration();

private:
	ExportInfo& exportInfo;
	EncoderInfo* GetSelectedEncoder(wxChoice *choice);
	void ApplyStoredFilters();
	void UpdateFormats();
	void OnVideoEncoderChanged(wxCommandEvent& event);
	void OnAudioEncoderChanged(wxCommandEvent& event);
	void OnMuxerChanged(wxCommandEvent& event);
	void OnFaststartChanged(wxCommandEvent& event);
	void OnOkayClick(wxCommandEvent& event);

};

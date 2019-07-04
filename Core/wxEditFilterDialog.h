#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include "Voukoder.h"
#include "OptionContainer.h"
#include "wxOptionEditor.h"

class wxEditFilterDialog : public wxDialog
{
public:
	wxEditFilterDialog(wxWindow* parent, EncoderInfo filterInfo, OptionContainer** options);

protected:
	wxNotebook* m_notebook = NULL;
	wxOptionEditor* m_editor = NULL;
	wxButton* m_sdbSizer1OK = NULL;
	wxButton* m_sdbSizer1Cancel = NULL;

private:
	EncoderInfo filterInfo;
	OptionContainer** options = NULL;
	void OnOkayClick(wxCommandEvent& event);
};

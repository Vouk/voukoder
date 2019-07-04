#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include "OptionContainer.h"
#include "EncoderInfo.h"
#include "ExportInfo.h"
#include "wxOptionEditor.h"
#include "wxFilterPanel.h"
#include "Voukoder.h"

class wxConfigurationDialog : public wxDialog
{
public:
	wxConfigurationDialog(wxWindow* parent, EncoderInfo encoderInfo, EncoderInfo sideData, std::vector<EncoderInfo> filterInfos, OptionContainer** encoderOptions, OptionContainer** sideDataOptions, FilterConfig** filterOptions);
	~wxConfigurationDialog();

protected:
	wxNotebook* m_notebook = NULL;
	wxOptionEditor* m_encoderOptions = NULL;
	wxOptionEditor* m_sideDataOptions = NULL;
	wxFilterPanel* m_filterPanel = NULL;
	wxStdDialogButtonSizer* m_sdbSizer1 = NULL;
	wxButton* m_sdbSizer1OK = NULL;
	wxButton* m_sdbSizer1Cancel = NULL;

private:
	OptionContainer** encoderOptions = NULL;
	OptionContainer** sideDataOptions = NULL;
	FilterConfig** filterOptions = NULL;
	void OnOkayClick(wxCommandEvent& event);
};
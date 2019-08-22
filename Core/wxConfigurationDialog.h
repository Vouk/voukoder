#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include "OptionContainer.h"
#include "EncoderInfo.h"
#include "ExportInfo.h"
#include "wxOptionEditor.h"
#include "wxFilterPanel.h"
#include "Voukoder.h"

struct TrackSettings {
	OptionContainer options;
	OptionContainer sideData;
	FilterConfig filters;
};

class wxConfigurationDialog : public wxDialog
{
public:
	wxConfigurationDialog(wxWindow* parent, EncoderInfo encoderInfo, EncoderInfo sideData, std::vector<EncoderInfo> filterInfos, TrackSettings& settings);
	void ApplyChanges();

protected:
	wxOptionEditor* m_encoderOptions = NULL;
	wxOptionEditor* m_sideDataOptions = NULL;
	wxFilterPanel* m_filterPanel = NULL;

private:
	EncoderInfo info;
	TrackSettings& settings;
	void GenerateEncoderTabs(wxNotebook* notebook);
	void OnOkayClick(wxCommandEvent& event);
};
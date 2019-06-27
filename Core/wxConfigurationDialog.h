#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include "OptionContainer.h"
#include "EncoderInfo.h"
#include "ExportInfo.h"
#include "wxPGOptionProperty.h"
#include "wxEncoderConfigEditor.h"
#include "wxFilterPanel.h"
#include "Voukoder.h"

#define VKDR_GRID_COLOR_BASIC 0xffffff
#define VKDR_GRID_COLOR_STANDARD 0xcdffff
#define VKDR_GRID_COLOR_OTHER 0xfff0f0

class wxConfigurationDialog : public wxDialog
{
public:
	wxConfigurationDialog(wxWindow* parent, EncoderInfo encoderInfo, EncoderInfo sideData, std::vector<FilterInfo> filterInfos, OptionContainer** encoderOptions, OptionContainer** sideDataOptions, FilterConfig** filterOptions);
	~wxConfigurationDialog();

protected:
	wxNotebook* m_notebook = NULL;
	wxEncoderConfigEditor* m_encoderOptions = NULL;
	wxPanel* m_propPanel = NULL;
	wxPropertyGrid* m_propertyGrid = NULL;
	wxFilterPanel* m_filterPanel = NULL;
	wxStdDialogButtonSizer* m_sdbSizer1 = NULL;
	wxButton* m_sdbSizer1OK = NULL;
	wxButton* m_sdbSizer1Cancel = NULL;

private:
	OptionContainer** encoderOptions = NULL;
	OptionContainer** sideDataOptions = NULL;
	FilterConfig** filterOptions = NULL;
	void Configure(EncoderInfo sideData);
	void ExecuteFilters(wxOptionProperty* optionProperty);
	void OnPropertyGridChanged(wxPropertyGridEvent& event);
	void OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnOkayClick(wxCommandEvent& event);
	bool SendEvent(wxEventType eventType, wxPGProperty* p);
};
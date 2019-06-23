#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include "OptionContainer.h"
#include "EncoderInfo.h"
#include "wxPGOptionProperty.h"

#define VKDR_GRID_COLOR_BASIC 0xffffff
#define VKDR_GRID_COLOR_STANDARD 0xcdffff
#define VKDR_GRID_COLOR_OTHER 0xfff0f0

class wxCustomOptionsDialog : public wxDialog
{
public:
	OptionContainer options;
	wxCustomOptionsDialog(wxWindow* parent);
	~wxCustomOptionsDialog();
	void Configure(EncoderInfo encoderInfo, OptionContainer options);

protected:
	wxNotebook* m_notebook;
	wxPanel* m_tabPanel;
	wxPropertyGrid* m_propertyGrid;
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1OK;
	wxButton* m_sdbSizer1Cancel;

private:
	EncoderInfo encoderInfo;
	OptionContainer origOptions;
	void ExecuteFilters(wxOptionProperty* optionProperty);
	void OnPropertyGridChanged(wxPropertyGridEvent& event);
	void OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnOkayClick(wxCommandEvent& event);
	bool SendEvent(wxEventType eventType, wxPGProperty* p);
};


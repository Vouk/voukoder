#pragma once

#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/richtext/richtextctrl.h>
#include "wxPGOptionProperty.h"
#include "EncoderInfo.h"

#define VKDR_GRID_COLOR_BASIC 0xffffff
#define VKDR_GRID_COLOR_STANDARD 0xcdffff
#define VKDR_GRID_COLOR_OTHER 0xfff0f0

class wxEncoderConfigEditor : public wxPanel
{
public:
	wxEncoderConfigEditor() : results(*(new OptionContainer)) // UGLY!
	{
		OptionContainer v;
		wxEncoderConfigEditor(NULL, v);
	};

	wxEncoderConfigEditor(wxWindow *parent, OptionContainer &results);
	void Configure(EncoderInfo encoderInfo, OptionContainer options);

protected:
	wxPropertyGrid* m_propertyGrid;
	wxRichTextCtrl* m_preview;
	wxPanel* m_btnPanel;
	wxButton* m_btnReset;
	wxButton* m_btnClear;

private:
	EncoderInfo encoderInfo;
	OptionContainer options;
	OptionContainer& results;
	void ExecuteFilters(wxOptionProperty *optionProperty);
	void OnPropertyGridChanged(wxPropertyGridEvent& event);
	void OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnResetClick(wxCommandEvent& event);
	void OnClearClick(wxCommandEvent& event);
	void RefreshResults();
	bool SendEvent(wxEventType eventType, wxPGProperty* p);

	wxDECLARE_DYNAMIC_CLASS(wxEncoderConfigEditor);
	wxDECLARE_EVENT_TABLE();
};
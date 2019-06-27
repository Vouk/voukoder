#pragma once

#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/richtext/richtextctrl.h>
#include "wxPGOptionProperty.h"
#include "EncoderInfo.h"

#define VKDR_GRID_COLOR_BASIC 0xffffff
#define VKDR_GRID_COLOR_STANDARD 0xcdffff
#define VKDR_GRID_COLOR_OTHER 0xfff0f0

class wxOptionEditor : public wxPanel
{
public:
	OptionContainer results;
	wxOptionEditor() : wxOptionEditor(NULL) {};
	wxOptionEditor(wxWindow *parent, bool hasPreview = true);
	void Configure(EncoderInfo encoderInfo, OptionContainer options);

protected:
	wxPropertyGrid* m_propertyGrid = NULL;
	wxRichTextCtrl* m_preview = NULL;
	wxButton* m_btnReset = NULL;
	wxButton* m_btnClear = NULL;

private:
	EncoderInfo encoderInfo;
	OptionContainer options;
	bool hasPreview = true;
	void ExecuteFilters(wxOptionProperty *optionProperty);
	void OnPropertyGridChanged(wxPropertyGridEvent& event);
	void OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnResetClick(wxCommandEvent& event);
	void OnClearClick(wxCommandEvent& event);
	void RefreshResults();
	bool SendEvent(wxEventType eventType, wxPGProperty* p);

	wxDECLARE_DYNAMIC_CLASS(wxOptionEditor);
	wxDECLARE_EVENT_TABLE();
};
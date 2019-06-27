#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/propgrid/propgrid.h>
#include "Voukoder.h"
#include "OptionContainer.h"
#include "wxPGOptionProperty.h"

#define VKDR_GRID_COLOR_BASIC 0xffffff
#define VKDR_GRID_COLOR_STANDARD 0xcdffff
#define VKDR_GRID_COLOR_OTHER 0xfff0f0

class wxAddFilterDialog : public wxDialog
{
public:
	wxAddFilterDialog(wxWindow* parent, OptionContainer** options, AVMediaType type = AVMEDIA_TYPE_UNKNOWN);
    void GetFilterOptions(std::vector<OptionContainer>** options);

protected:
	wxNotebook* m_notebook = NULL;
	wxPanel* m_tabPanel = NULL;
	wxChoice* m_FilterChoice = NULL;
	wxPropertyGrid* m_propertyGrid = NULL;
	wxStdDialogButtonSizer* m_sdbSizer1 = NULL;
	wxButton* m_sdbSizer1OK = NULL;
	wxButton* m_sdbSizer1Cancel = NULL;

private:
	FilterInfo* filterInfo = NULL;
	void OnFilterChanged(wxCommandEvent& event);
	OptionContainer** options = NULL;
	void OnOkayClick(wxCommandEvent& event);
	void OnPropertyGridChanged(wxPropertyGridEvent& event);
	void OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	bool SendEvent(wxEventType eventType, wxPGProperty* p);
};

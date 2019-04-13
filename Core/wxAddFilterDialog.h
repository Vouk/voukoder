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

#define VKDR_GRID_COLOR_BASIC 0xffffff
#define VKDR_GRID_COLOR_STANDARD 0xcdffff
#define VKDR_GRID_COLOR_OTHER 0xfff0f0

class wxAddFilterDialog : public wxDialog
{
private:
	FilterInfo *filterInfo;
	void OnFilterChanged(wxCommandEvent& event);

protected:
	wxNotebook* m_notebook;
	wxPanel* m_tabPanel;
	wxChoice* m_FilterChoice;
	wxPropertyGrid* m_propertyGrid;
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1OK;
	wxButton* m_sdbSizer1Cancel;

public:
	wxAddFilterDialog(wxWindow* parent);
	void GetFilterOptions(vector<OptionContainer> &options);
	const wxString GetLabel();
};
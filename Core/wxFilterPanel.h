#pragma once

#include <wx/wx.h>
#include "OptionContainer.h"

class wxFilterPanel : public wxPanel
{
public:
	wxFilterPanel(wxWindow *parent = NULL);
	wxString CreateLabelFromData(wxString data);
	//void GetFilterConfig(OptionContainer &options);

protected:
	wxListBox* m_filterList;
	wxPanel* m_filterButtons;
	wxButton* m_filterAdd;
	wxButton* m_filterRemove;

private:
	void OnAddFilterClick(wxCommandEvent& event);
	void OnRemoveFilterClick(wxCommandEvent& event);
};


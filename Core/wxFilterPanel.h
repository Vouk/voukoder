#pragma once

#include <wx/wx.h>
#include "OptionContainer.h"
#include "FilterConfig.h"
#include "lavf.h"

class wxFilterPanel : public wxPanel
{
public:
	wxFilterPanel(wxWindow *parent = NULL, AVMediaType type = AVMEDIA_TYPE_UNKNOWN);
	void GetFilterConfig(FilterConfig &filterConfig);
	void SetFilterConfig(FilterConfig filterConfig);

protected:
	wxListBox* m_filterList;
	wxPanel* m_filterButtons;
	wxButton* m_filterAdd;
	wxButton* m_filterEdit;
	wxButton* m_filterRemove;

private:
	AVMediaType type;
	void OnAddFilterClick(wxCommandEvent& event);
	void OnEditFilterClick(wxCommandEvent& event);
	void OnRemoveFilterClick(wxCommandEvent& event);
};
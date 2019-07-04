#pragma once

#include <wx/wx.h>
#include "FilterConfig.h"
#include "lavf.h"
#include "LanguageUtils.h"

class wxFilterMenuItem : public wxMenuItem
{
public:
	wxFilterMenuItem(wxMenu *parentMenu = NULL) :
		wxMenuItem(parentMenu, wxID_ANY) {};

	wxFilterMenuItem(wxMenu *parentMenu, EncoderInfo filterInfo) :
		wxMenuItem(parentMenu, wxID_ANY, Trans(filterInfo.id)),
		filterInfo(filterInfo) {};

	EncoderInfo GetFilterInfo()
	{
		return filterInfo;
	}

private:
	EncoderInfo filterInfo;
};

class wxFilterPanel : public wxPanel
{
public:
	wxFilterPanel(wxWindow *parent, std::vector<EncoderInfo> filters);
	void Configure(FilterConfig filterConfig);
	void GetFilterConfig(FilterConfig &filterConfig);

protected:
	wxListBox* m_filterList = NULL;
	wxPanel* m_filterButtons = NULL;
	wxButton* m_filterAdd = NULL;
	wxButton* m_filterEdit = NULL;
	wxButton* m_filterRemove = NULL;
	wxMenu* m_filterMenu = NULL;

private:
	std::vector<EncoderInfo> filters;
	void OnAddFilterClick(wxCommandEvent& event);
	void OnEditFilterClick(wxCommandEvent& event);
	void OnRemoveFilterClick(wxCommandEvent& event);
	void OnPopupClick(wxCommandEvent &evt);
};
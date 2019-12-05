#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/dnd.h>
#include "FilterConfig.h"
#include "lavf.h"
#include "LanguageUtils.h"

class wxFilterMenuItem : public wxMenuItem
{
public:
	wxFilterMenuItem(wxMenu *parentMenu = NULL) :
		wxMenuItem(parentMenu, wxID_ANY) {};

	wxFilterMenuItem(wxMenu *parentMenu, EncoderInfo filterInfo) :
		wxMenuItem(parentMenu, wxID_ANY, filterInfo.name + " - " + Trans(filterInfo.id)),
		filterInfo(filterInfo) {};

	EncoderInfo GetFilterInfo()
	{
		return filterInfo;
	}

private:
	EncoderInfo filterInfo;
};

class wxFilterDropTarget : public wxTextDropTarget
{
public:
	wxFilterDropTarget(wxListCtrl *listctrl) { m_listctrl = listctrl; }

	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text)
	{
		long oldPos;
		if (text.ToLong(&oldPos)) // TODO
		{
			// Get new position
			int flags;
			long pos = m_listctrl->HitTest(wxPoint(x, y), flags);
			if (pos == wxNOT_FOUND)
				pos = m_listctrl->GetItemCount() - 1;

			// Move item
			wxListItem item;
			item.SetId(oldPos);
			m_listctrl->GetItem(item);
			m_listctrl->DeleteItem(oldPos);
			item.SetId(pos);
			m_listctrl->InsertItem(item);

			// Update parameters
			OptionContainer* options = reinterpret_cast<OptionContainer*>(item.GetData());
			if (options != NULL)
			{
				wxListItem item2;
				item2.SetId(pos);
				item2.SetColumn(1);
				item2.SetText(options->Serialize(false, "", ','));
				m_listctrl->SetItem(item2);
			}

			return true;
		}

		return false;
	};

private:
	wxListCtrl* m_listctrl;
};

class FilterDropSource : public wxDropSource
{
public:
	FilterDropSource(wxListCtrl *listctrl) : wxDropSource(listctrl, wxDROP_ICON(dnd_copy), wxDROP_ICON(dnd_move), wxDROP_ICON(dnd_none))
	{
		m_listctrl = listctrl;
	};

private:
	wxListCtrl *m_listctrl;
};

class wxFilterPanel : public wxPanel
{
public:
	wxFilterPanel(wxWindow *parent, std::vector<EncoderInfo> filters);
	void Configure(FilterConfig filterConfig);
	void GetFilterConfig(FilterConfig &filterConfig);

protected:
	wxListCtrl* m_filterList = NULL;
	wxButton* m_filterAdd = NULL;
	wxButton* m_filterEdit = NULL;
	wxButton* m_filterRemove = NULL;
	wxMenu* m_filterMenu = NULL;

private:
	std::vector<EncoderInfo> filters;
	void PopulateFilterMenu();
	void ValidateSelection(bool isSelected);
	void OnAddFilterClick(wxCommandEvent& event);
	void OnEditFilterClick(wxCommandEvent& event);
	void OnItemActivate(wxListEvent& event);
	void OnRemoveFilterClick(wxCommandEvent& event);
	void OnPopupClick(wxCommandEvent &evt);
	void OnSelected(wxListEvent& event);
	void OnStartDrag(wxListEvent& event);
	void OnDeselected(wxListEvent& event);
};
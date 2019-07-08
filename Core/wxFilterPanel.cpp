#include "wxFilterPanel.h"
#include "wxEditFilterDialog.h"
#include "Voukoder.h"

wxFilterPanel::wxFilterPanel(wxWindow *parent, std::vector<EncoderInfo> filters) :
	wxPanel(parent),
	filters(filters)
{
	wxBoxSizer* bFilterLayout = new wxBoxSizer(wxHORIZONTAL);

	m_filterList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	m_filterList->InsertColumn(0, Trans("ui.voukoder.configuration.filters.column_0"), wxLIST_FORMAT_LEFT, 125);
	m_filterList->InsertColumn(1, Trans("ui.voukoder.configuration.filters.column_1"), wxLIST_FORMAT_LEFT, 400);
	m_filterList->SetDropTarget(new wxFilterDropTarget(m_filterList));
	m_filterList->Bind(wxEVT_LIST_ITEM_SELECTED, &wxFilterPanel::OnSelected, this);
	m_filterList->Bind(wxEVT_LIST_ITEM_DESELECTED, &wxFilterPanel::OnDeselected, this);
	m_filterList->Bind(wxEVT_LIST_BEGIN_DRAG, &wxFilterPanel::OnStartDrag, this);
	bFilterLayout->Add(m_filterList, 1, wxALL | wxEXPAND, 5);

	m_filterButtons = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bFilterButtonsLayout = new wxBoxSizer(wxVERTICAL);

	m_filterAdd = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.voukoder.configuration.filters.add"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterAdd->Bind(wxEVT_BUTTON, &wxFilterPanel::OnAddFilterClick, this);
	bFilterButtonsLayout->Add(m_filterAdd, 0, wxALL, 5);

	m_filterEdit = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.voukoder.configuration.filters.edit"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterEdit->Bind(wxEVT_BUTTON, &wxFilterPanel::OnEditFilterClick, this);
	bFilterButtonsLayout->Add(m_filterEdit, 0, wxALL, 5);

	m_filterRemove = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.voukoder.configuration.filters.remove"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterRemove->Bind(wxEVT_BUTTON, &wxFilterPanel::OnRemoveFilterClick, this);
	bFilterButtonsLayout->Add(m_filterRemove, 0, wxALL, 5);

	m_filterButtons->SetSizer(bFilterButtonsLayout);
	m_filterButtons->Layout();
	bFilterButtonsLayout->Fit(m_filterButtons);
	bFilterLayout->Add(m_filterButtons, 0, wxEXPAND | wxALL, 0);

	this->SetSizer(bFilterLayout);
	this->Layout();
	bFilterLayout->Fit(this);

	// Populate a sorted filter menu
	PopulateFilterMenu();
}

void wxFilterPanel::OnStartDrag(wxListEvent& event)
{
	FilterDropSource dragSource(m_filterList);
	wxTextDataObject textData(wxString::Format("%d", event.GetIndex())); // TODO!
	dragSource.SetData(textData);
	dragSource.DoDragDrop(0);
}

void wxFilterPanel::PopulateFilterMenu()
{
	m_filterMenu = new wxMenu();
	for (auto& filterInfo : filters)
	{
		size_t pos = 0;

		// Find insert position
		for (auto item : m_filterMenu->GetMenuItems())
		{
			wxString name = Trans(filterInfo.id);
			if (name.CmpNoCase(item->GetItemLabelText()) <= 0)
				break;

			pos++;
		}

		// Insert the item
		wxFilterMenuItem* item = new wxFilterMenuItem(m_filterMenu, filterInfo);
		m_filterMenu->Insert(pos, item);
	}
	m_filterMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxFilterPanel::OnPopupClick), NULL, this);
}

void wxFilterPanel::Configure(FilterConfig filterConfig)
{
	// Load the stored filters
	for (auto& options : filterConfig)
	{
		for (auto info : filters)
		{
			if (info.id == options->id)
			{
				wxListItem item;
				item.m_itemId = 0;
				item.SetText(Trans(info.id));
				item.SetData(options);
				m_filterList->InsertItem(item);
				item.SetColumn(1);
				item.SetText(options->Serialize(false, "", ','));
				m_filterList->SetItem(item);

				break;
			}
		}
	}

	// Preselect first filter
	if (m_filterList->GetItemCount() > 0)
	{
		//m_filterList->  ->SetSelection(0);
		m_filterEdit->Enable();
		m_filterRemove->Enable();
	}
}

void wxFilterPanel::GetFilterConfig(FilterConfig &filterConfig)
{
	filterConfig.clear();

	// Loop through all selected items
	long itemId = wxNOT_FOUND;
	while ((itemId = m_filterList->GetNextItem(itemId, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE)) > wxNOT_FOUND)
	{
		wxListItem item;
		item.m_itemId = itemId;

		// Get the item
		if (m_filterList->GetItem(item))
		{
			OptionContainer* options = (OptionContainer*)item.GetData();
			if (options)
				filterConfig.push_back(options);
		}
	}
}

void wxFilterPanel::OnAddFilterClick(wxCommandEvent& event)
{
	m_filterAdd->PopupMenu(m_filterMenu, m_filterAdd->GetPosition());
}

void wxFilterPanel::OnEditFilterClick(wxCommandEvent& event)
{
	// Get the item index to delete
	long itemId = m_filterList->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (itemId != wxNOT_FOUND)
	{
		wxListItem item;
		item.m_itemId = itemId;

		// Get the item
		if (m_filterList->GetItem(item))
		{
			OptionContainer* options = (OptionContainer*)item.GetData();

			// Find Filter
			for (auto info : filters)
			{
				if (info.id == options->id)
				{
					// Show the filter editor dialog
					wxEditFilterDialog dialog(this, info, &options);
					if (dialog.ShowModal() == (int)wxID_OK)
					{
						// Update parameters
						item.SetColumn(1);
						item.SetText(options->Serialize(false, "", ','));
						m_filterList->SetItem(item);
					}

					break;
				}
			}
		}
	}
}

void wxFilterPanel::OnRemoveFilterClick(wxCommandEvent& event)
{
	// Find the selected item
	long itemId = m_filterList->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (itemId != wxNOT_FOUND)
	{
		wxListItem item;
		item.m_itemId = itemId;

		// Get the item
		if (m_filterList->GetItem(item))
		{
			// Delete it and the payload
			delete (OptionContainer*)item.GetData();
			m_filterList->DeleteItem(itemId);
		}
	}
}

void wxFilterPanel::OnPopupClick(wxCommandEvent &evt)
{
	for (auto &item : m_filterMenu->GetMenuItems())
	{
		if (item->GetId() == evt.GetId())
		{
			wxFilterMenuItem* filterItem = reinterpret_cast<wxFilterMenuItem*>(item);
			EncoderInfo info = filterItem->GetFilterInfo();

			// Create the options config
			OptionContainer* options = new OptionContainer;
			options->id = info.id;

			// Insert after selection or at the end
			long itemId = m_filterList->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if (itemId == wxNOT_FOUND)
				itemId = m_filterList->GetItemCount();
			else
				itemId += 1;

			// Create a new filter item
			wxListItem item;
			item.SetId(itemId);
			item.SetText(Trans(info.id));
			item.SetData(options);
			item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_DATA);
			item.SetState(wxLIST_STATE_SELECTED);
			m_filterList->InsertItem(item);

			wxListItem item2;
			item2.SetId(itemId);
			item2.SetColumn(1);
			item2.SetText(options->Serialize(false, "", ','));
			m_filterList->SetItem(item2);

			// Set focus
			m_filterList->SetFocus();
		}
	}
}

void wxFilterPanel::OnSelected(wxListEvent& event)
{
	ValidateSelection(true);
}

void wxFilterPanel::OnDeselected(wxListEvent& event)
{
	ValidateSelection(false);
}

void wxFilterPanel::ValidateSelection(bool isSelected)
{
	m_filterEdit->Enable(isSelected);
	m_filterRemove->Enable(isSelected);
}
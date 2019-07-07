#include "wxFilterPanel.h"
#include "wxEditFilterDialog.h"
#include "Voukoder.h"

wxFilterPanel::wxFilterPanel(wxWindow *parent, std::vector<EncoderInfo> filters) :
	wxPanel(parent),
	filters(filters)
{
	wxBoxSizer* bFilterLayout = new wxBoxSizer(wxHORIZONTAL);

	m_filterList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	bFilterLayout->Add(m_filterList, 1, wxALL | wxEXPAND, 5);

	m_filterButtons = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bFilterButtonsLayout = new wxBoxSizer(wxVERTICAL);

	m_filterAdd = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.voukoder.configuration.filters.add"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterAdd->Bind(wxEVT_BUTTON, &wxFilterPanel::OnAddFilterClick, this);
	bFilterButtonsLayout->Add(m_filterAdd, 0, wxALL, 5);

	m_filterEdit = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.voukoder.configuration.filters.edit"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterEdit->Bind(wxEVT_BUTTON, &wxFilterPanel::OnEditFilterClick, this);
	m_filterEdit->Enable(false);
	bFilterButtonsLayout->Add(m_filterEdit, 0, wxALL, 5);

	m_filterRemove = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.voukoder.configuration.filters.remove"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterRemove->Bind(wxEVT_BUTTON, &wxFilterPanel::OnRemoveFilterClick, this);
	m_filterRemove->Enable(false);
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
				int pos = m_filterList->Append(Trans(info.id), (void*)options);
				m_filterList->SetSelection(pos);

				break;
			}
		}
	}

	// Preselect first filter
	if (m_filterList->GetCount() > 0)
	{
		m_filterList->SetSelection(0);
		m_filterEdit->Enable();
		m_filterRemove->Enable();
	}
}

void wxFilterPanel::GetFilterConfig(FilterConfig &filterConfig)
{
	filterConfig.clear();

	for (unsigned int i = 0; i < m_filterList->GetCount(); i++)
	{
		OptionContainer* options = reinterpret_cast<OptionContainer*>(m_filterList->GetClientData(i));
		filterConfig.push_back(options);
	}
}

void wxFilterPanel::OnAddFilterClick(wxCommandEvent& event)
{
	wxPoint pos = m_filterAdd->GetPosition();
	m_filterAdd->PopupMenu(m_filterMenu, pos);
}

void wxFilterPanel::OnEditFilterClick(wxCommandEvent& event)
{
	// Get the item index to delete
	int pos = m_filterList->GetSelection();
	if (pos != wxNOT_FOUND)
	{
		OptionContainer* options = reinterpret_cast<OptionContainer*>(m_filterList->GetClientData(pos));

		for (auto info : filters)
		{
			if (info.id == options->id)
			{
				// Show the filter editor dialog
				wxEditFilterDialog dialog(this, info, &options);
				dialog.ShowModal();

				break;
			}
		}
	}
}

void wxFilterPanel::OnRemoveFilterClick(wxCommandEvent& event)
{
	// Get the item index to delete
	int pos = m_filterList->GetSelection();
	if (pos != wxNOT_FOUND)
	{
		// Delete data from heap
		OptionContainer* options = reinterpret_cast<OptionContainer*>(m_filterList->GetClientData(pos));
		if (options != NULL)
			delete options;

		// Remove entry
		m_filterList->Delete(pos);

		// There is no item left
		if (m_filterList->GetCount() == 0)
		{
			m_filterEdit->Enable(false);
			m_filterRemove->Enable(false);
		}
		else
		{
			// If the last item has been removed
			if (pos == m_filterList->GetCount())
				pos = m_filterList->GetCount() - 1;

			m_filterList->SetSelection(pos);
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

			// Append the filter and select it
			int pos = m_filterList->Append(Trans(info.id), (void*)options);
			m_filterList->SetSelection(pos);

			// There is always a filter selected so enable the remove button
			m_filterEdit->Enable();
			m_filterRemove->Enable();
		}
	}
}
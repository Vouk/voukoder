#include "wxFilterPanel.h"
#include "wxAddFilterDialog.h"
#include "LanguageUtils.h"
#include "Voukoder.h"

wxFilterPanel::wxFilterPanel(wxWindow *parent, AVMediaType type) :
	wxPanel(parent),
	type(type)
{
	wxBoxSizer* bFilterLayout = new wxBoxSizer(wxHORIZONTAL);

	m_filterList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	bFilterLayout->Add(m_filterList, 1, wxALL | wxEXPAND, 5);

	m_filterButtons = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bFilterButtonsLayout = new wxBoxSizer(wxVERTICAL);

	m_filterAdd = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.encoderconfig.filters.add"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterAdd->Bind(wxEVT_BUTTON, &wxFilterPanel::OnAddFilterClick, this);
	bFilterButtonsLayout->Add(m_filterAdd, 0, wxALL, 5);

	m_filterEdit = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.encoderconfig.filters.edit"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterEdit->Bind(wxEVT_BUTTON, &wxFilterPanel::OnEditFilterClick, this);
	m_filterEdit->Enable(false);
	bFilterButtonsLayout->Add(m_filterEdit, 0, wxALL, 5);

	m_filterRemove = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.encoderconfig.filters.remove"), wxDefaultPosition, wxDefaultSize, 0);
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
}

void wxFilterPanel::GetFilterConfig(FilterConfig &filterConfig)
{
	filterConfig.filters.clear();

	for (unsigned int i = 0; i < m_filterList->GetCount(); i++)
	{
		OptionContainer *options = reinterpret_cast<OptionContainer*>(m_filterList->GetClientData(i));
		filterConfig.filters.push_back(options);
	}
}

void wxFilterPanel::SetFilterConfig(FilterConfig filterConfig)
{
	// Append the filter
	for (auto options : filterConfig.filters)
		m_filterList->Append(Trans("filter." + options->at("_name")), (void*)options);

	// There is always a filter selected so enable the remove button
	if (m_filterList->GetCount() > 0)
	{
		m_filterList->SetSelection(0);
		m_filterEdit->Enable();
		m_filterRemove->Enable();
	}
}

void wxFilterPanel::OnAddFilterClick(wxCommandEvent& event)
{
	OptionContainer *options = new OptionContainer;

	// Show the filter creation dialog
	wxAddFilterDialog dialog(this, &options, type);
	if (dialog.ShowModal() == (int)wxID_OK)
	{
		// Append the filter and select it
		int pos = m_filterList->Append(Trans("filter." + options->at("_name")), (void*)options);
		m_filterList->SetSelection(pos);

		// There is always a filter selected so enable the remove button
		m_filterEdit->Enable();
		m_filterRemove->Enable();
	}
}

void wxFilterPanel::OnEditFilterClick(wxCommandEvent& event)
{
	// Get the item index to delete
	int pos = m_filterList->GetSelection();
	if (pos != wxNOT_FOUND)
	{
		OptionContainer* options = (OptionContainer*)m_filterList->GetClientData(pos);
		OptionContainer* newOptions = new OptionContainer(options);

		// Show the filter creation dialog
		wxAddFilterDialog dialog(this, &newOptions, type);
		if (dialog.ShowModal() == (int)wxID_OK)
		{
			delete options;
			m_filterList->SetClientData(pos, (void*)newOptions);
		}
		else
		{
			delete newOptions;
		}
	}
}

void wxFilterPanel::OnRemoveFilterClick(wxCommandEvent& event)
{
	// Get the item index to delete
	int pos = m_filterList->GetSelection();
	if (pos != wxNOT_FOUND)
	{
		delete m_filterList->GetClientData(pos);
		m_filterList->Delete(pos);

		// There is no item left
		if (m_filterList->GetCount() == 0)
		{
			m_filterRemove->Enable(false);

			return;
		}

		// If the last item has been removed
		if (pos == m_filterList->GetCount())
		{
			pos = m_filterList->GetCount() - 1;
		}

		m_filterList->SetSelection(pos);
	}
}

#include "wxFilterPanel.h"
#include "wxAddFilterDialog.h"
#include "LanguageUtils.h"
#include "Voukoder.h"

wxFilterPanel::wxFilterPanel(wxWindow *parent):
	wxPanel(parent)
{
	wxBoxSizer* bFilterLayout = new wxBoxSizer(wxHORIZONTAL);

	m_filterList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	bFilterLayout->Add(m_filterList, 1, wxALL | wxEXPAND, 5);

	m_filterButtons = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bFilterButtonsLayout = new wxBoxSizer(wxVERTICAL);

	m_filterAdd = new wxButton(m_filterButtons, wxID_ANY, Trans("ui.encoderconfig.filters.add"), wxDefaultPosition, wxDefaultSize, 0);
	m_filterAdd->Bind(wxEVT_BUTTON, &wxFilterPanel::OnAddFilterClick, this);
	bFilterButtonsLayout->Add(m_filterAdd, 0, wxALL, 5);

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

wxString wxFilterPanel::CreateLabelFromData(wxString data)
{
	return "";
}

void wxFilterPanel::OnAddFilterClick(wxCommandEvent& event)
{
	/*
	// Show the filter creation dialog
	wxAddFilterDialog dialog(this);
	if (dialog.ShowModal() == (int)wxID_OK)
	{
		// Append the filter and select it
		OptionContainer *options = new OptionContainer;
		dialog.GetFilterOptions(options);

		wxString *data = new wxString(dialog.GetFilterOptionsAsString());
		int pos = m_filterList->Append(dialog.GetLabel(), (void*)data);
		m_filterList->SetSelection(pos);

		// There is always a filter selected so enable the remove button
		m_filterRemove->Enable();
	}

	//UpdateFilterConfig();
	*/
}

void wxFilterPanel::OnRemoveFilterClick(wxCommandEvent& event)
{
	// Get the item index to delete
	int pos = m_filterList->GetSelection();
	if (pos != wxNOT_FOUND)
	{
		m_filterList->Delete(pos);

		// There is no item left
		if (m_filterList->GetCount() == 0)
		{
			m_filterRemove->Enable(false);

			//UpdateFilterConfig();

			return;
		}

		// If the last item has been removed
		if (pos == m_filterList->GetCount())
		{
			pos = m_filterList->GetCount() - 1;
		}

		m_filterList->SetSelection(pos);
	}

	//UpdateFilterConfig();
}

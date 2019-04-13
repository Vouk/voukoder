#include "wxAddFilterDialog.h"
#include "wxPGOptionProperty.h"
#include "LanguageUtils.h"

wxAddFilterDialog::wxAddFilterDialog(wxWindow* parent) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(480, 320), wxDEFAULT_DIALOG_STYLE)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	SetTitle(Trans("ui.addfilter.title"));

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_tabPanel = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bTabLayout = new wxBoxSizer(wxVERTICAL);

	wxArrayString m_FilterChoiceChoices;
	m_FilterChoice = new wxChoice(m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_FilterChoiceChoices, 0);
	m_FilterChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &wxAddFilterDialog::OnFilterChanged, this);
	bTabLayout->Add(m_FilterChoice, 0, wxALL | wxEXPAND, 5);

	m_propertyGrid = new wxPropertyGrid(m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	bTabLayout->Add(m_propertyGrid, 1, wxEXPAND | wxALL, 5);

	m_tabPanel->SetSizer(bTabLayout);
	m_tabPanel->Layout();
	bTabLayout->Fit(m_tabPanel);
	m_notebook->AddPage(m_tabPanel, Trans("ui.addfilter.options"), true);

	bDialogLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bDialogLayout->Add(m_sdbSizer1, 0, wxEXPAND, 5);
	
	this->SetSizer(bDialogLayout);
	this->Layout();

	this->Centre(wxBOTH);

	// Populate filters
	for (auto& filterInfo : Voukoder::Config::Get().filterInfos)
	{
		m_FilterChoice->Append(Trans(filterInfo.id), (void*)&filterInfo);
	}
	m_FilterChoice->SetSelection(0);

	wxCommandEvent event;
	OnFilterChanged(event);
}

/*
void wxAddFilterDialog::GetFilterOptions(vector<OptionContainer> &options)
{
	wxString filter = filterInfo->name + "=";

	// Iterate over all options
	wxPropertyGridConstIterator it;
	for (it = m_propertyGrid->GetIterator(wxPG_ITERATE_VISIBLE); !it.AtEnd(); it++)
	{
		wxOptionProperty *prop = wxDynamicCast(*it, wxOptionProperty);
		if (prop)
		{
			EncoderOptionInfo optionInfo = prop->GetOptionInfo();
			wxString parameter = optionInfo.parameter;

			if (!parameter.IsEmpty() && prop->IsChecked())
			{
				wxString value;

				// Format the value if required (Make sure parsing unformats the value!!)
				if (optionInfo.control.type == EncoderOptionType::Integer)
				{
					int val = prop->GetValue().GetInteger() * optionInfo.multiplicationFactor;
					value = wxString::Format(wxT("%d"), val);
				}
				else if (optionInfo.control.type == EncoderOptionType::Float)
				{
					double val = prop->GetValue().GetDouble() * optionInfo.multiplicationFactor;
					value = wxString::Format(wxT("%.1f"), val);
				}
				else
				{
					value = prop->GetValueAsString(wxPG_FULL_VALUE);
				}

				// Assign the value
				if (!value.IsEmpty())
				{
					string param = parameter.ToStdString();

					filter += param + "=" + value + ":";
				}
			}
		}
	}

	return filter.BeforeLast(':');
}
*/

const wxString wxAddFilterDialog::GetLabel()
{
	wxString label = Trans(filterInfo->id) + " (";

	// Iterate over all options
	wxPropertyGridConstIterator it;
	for (it = m_propertyGrid->GetIterator(wxPG_ITERATE_VISIBLE); !it.AtEnd(); it++)
	{
		wxOptionProperty *prop = wxDynamicCast(*it, wxOptionProperty);
		if (prop)
		{
			EncoderOptionInfo optionInfo = prop->GetOptionInfo();
			wxString parameter = optionInfo.parameter;

			if (!parameter.IsEmpty() && prop->IsChecked())
			{
				wxString value;

				// Format the value if required (Make sure parsing unformats the value!!)
				if (optionInfo.control.type == EncoderOptionType::Integer)
				{
					int val = prop->GetValue().GetInteger() * optionInfo.multiplicationFactor;
					value = wxString::Format(wxT("%d"), val);
				}
				else if (optionInfo.control.type == EncoderOptionType::Float)
				{
					double val = prop->GetValue().GetDouble() * optionInfo.multiplicationFactor;
					value = wxString::Format(wxT("%.1f"), val);
				}
				else
				{
					value = prop->GetValueAsString(wxPG_FULL_VALUE);
				}

				// Assign the value
				if (!value.IsEmpty())
				{
					string param = parameter.ToStdString();

					label += Trans(optionInfo.id, "label") + ": " + value + ", ";
				}
			}
		}
	}

	return label.BeforeLast(',') + ")";
}

void wxAddFilterDialog::OnFilterChanged(wxCommandEvent& event)
{
	int selectedIndex = m_FilterChoice->GetSelection();
	filterInfo = reinterpret_cast<FilterInfo*>(m_FilterChoice->GetClientData(selectedIndex));

	// Reset the options map
	m_propertyGrid->Clear();

	wxColour bgColor;

	// Build all groups and properties
	for (const EncoderGroupInfo &group : filterInfo->groups)
	{
		wxPGProperty *category = m_propertyGrid->Append(new wxPropertyCategory(group.name));

		// Set background color
		if (group.groupClass == "basic")
		{
			bgColor = wxColour(VKDR_GRID_COLOR_BASIC);
		}
		else if (group.groupClass == "standard")
		{
			bgColor = wxColour(VKDR_GRID_COLOR_STANDARD);
		}
		else
		{
			bgColor = wxColour(VKDR_GRID_COLOR_OTHER);
		}

		// Add all options to the group
		for (const EncoderOptionInfo &optionInfo : group.options)
		{
			wxOptionProperty *optionProperty = new wxOptionProperty(optionInfo);
			m_propertyGrid->AppendIn(category, optionProperty);

			// Make inactive options gray
			wxColour colour = wxSystemSettings::GetColour(optionProperty->IsChecked() ? wxSYS_COLOUR_LISTBOXTEXT : wxSYS_COLOUR_GRAYTEXT);
			optionProperty->SetTextColour(colour);
			optionProperty->SetBackgroundColour(bgColor);
		}
	}
}
#include "wxAddFilterDialog.h"
#include "LanguageUtils.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxAddFilterDialog::wxAddFilterDialog(wxWindow* parent, OptionContainer **options, AVMediaType type) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(480, 520), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	options(options)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	// Set the right window title
	SetTitle(Trans((*options)->size() > 0 ? "ui.editfilter.title" : "ui.addfilter.title"));

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_tabPanel = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bTabLayout = new wxBoxSizer(wxVERTICAL);

	wxArrayString m_FilterChoiceChoices;
	m_FilterChoice = new wxChoice(m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_FilterChoiceChoices, 0);
	m_FilterChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &wxAddFilterDialog::OnFilterChanged, this);
	bTabLayout->Add(m_FilterChoice, 0, wxALL | wxEXPAND, 5);

	m_propertyGrid = new wxPropertyGrid(m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	m_propertyGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
	m_propertyGrid->SetValidationFailureBehavior(0);
	m_propertyGrid->Bind(wxEVT_LEFT_DOWN, &wxAddFilterDialog::OnLeftDown, this);
	m_propertyGrid->Bind(wxEVT_PG_CHANGED, &wxAddFilterDialog::OnPropertyGridChanged, this, m_propertyGrid->GetId());
	m_propertyGrid->Bind(wxEVT_CHECKBOX_CHANGE, &wxAddFilterDialog::OnPropertyGridCheckboxChanged, this, m_propertyGrid->GetId());
	bTabLayout->Add(m_propertyGrid, 1, wxEXPAND | wxALL, 5);

	m_tabPanel->SetSizer(bTabLayout);
	m_tabPanel->Layout();
	bTabLayout->Fit(m_tabPanel);
	m_notebook->AddPage(m_tabPanel, Trans("ui.addfilter.options"), true);

	bDialogLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1OK->Bind(wxEVT_BUTTON, &wxAddFilterDialog::OnOkayClick, this);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bDialogLayout->Add(m_sdbSizer1, 0, wxEXPAND | wxALL, 5);

	this->SetSizer(bDialogLayout);
	this->Layout();

	this->Centre(wxBOTH);

	// Populate filters
	for (auto& filterInfo : Voukoder::Config::Get().filterInfos)
	{
		if (type == AVMEDIA_TYPE_UNKNOWN || filterInfo.type == type)
			m_FilterChoice->Append(Trans(filterInfo.id), (void*)& filterInfo);
	}

	int selection = 0;

	// Edit mode
	if ((*options)->size() > 0)
	{
		m_FilterChoice->Enable(false);

		wxString filterName = (*options)->at("_name");

		// Find the pre-selected filter
		for (auto& filterInfo : Voukoder::Config::Get().filterInfos)
			if (filterInfo.name == filterName)
			{
				selection = m_FilterChoice->FindString(Trans(filterInfo.id));
				break;
			}
	}

	// Select the right filter
	m_FilterChoice->SetSelection(selection);

	wxCommandEvent event;
	OnFilterChanged(event);
}

void wxAddFilterDialog::GetFilterOptions(std::vector<OptionContainer> **options)
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
					filter += parameter + "=" + value + ":";
				}
			}
		}
	}

	//return filter.BeforeLast(':');
	return;
}

bool wxAddFilterDialog::SendEvent(wxEventType eventType, wxPGProperty* p)
{
	wxPropertyGridEvent event(wxEVT_CHECKBOX_CHANGE, m_propertyGrid->GetId());
	event.SetPropertyGrid(m_propertyGrid);
	event.SetProperty(p);
	m_propertyGrid->HandleWindowEvent(event);

	return event.WasVetoed();
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

			// Import stored settings
			OptionContainer opts = **options;
			if (opts.find(optionInfo.parameter) != opts.end())
			{
				optionProperty->SetChecked();

				if (optionInfo.control.type == EncoderOptionType::ComboBox)
				{
					// Select combo items by name not by value
					for (auto& item : optionInfo.control.items)
					{
						if (item.value == opts[optionInfo.parameter])
						{
							optionProperty->SetValueFromString(Trans(item.id));
							break;
						}
					}
				}
				else if (optionInfo.control.type == EncoderOptionType::Integer)
				{
					long longVal;
					wxString val = opts[optionInfo.parameter];
					val.ToLong(&longVal);

					optionProperty->SetValueFromInt(longVal / optionInfo.multiplicationFactor);
				}
				else if (optionInfo.control.type == EncoderOptionType::Float)
				{
					double doubleVal;
					wxString val = opts[optionInfo.parameter];
					val.ToDouble(&doubleVal);

					optionProperty->SetValueFromString(wxString::Format(wxT("%.1f"), doubleVal));
				}
				else
				{
					optionProperty->SetValueFromString(opts[optionInfo.parameter]);
				}
			}

			// Make inactive options gray
			wxColour colour = wxSystemSettings::GetColour(optionProperty->IsChecked() ? wxSYS_COLOUR_LISTBOXTEXT : wxSYS_COLOUR_GRAYTEXT);
			optionProperty->SetTextColour(colour);
			optionProperty->SetBackgroundColour(bgColor);
		}
	}
}

void wxAddFilterDialog::OnLeftDown(wxMouseEvent& event)
{
	wxPropertyGridHitTestResult htr = m_propertyGrid->HitTest(event.GetPosition());
	wxPGProperty* prop = htr.GetProperty();
	wxOptionProperty* msp = wxDynamicCast(prop, wxOptionProperty);

	if (msp && htr.GetColumn() == 0)
	{
		int minx = msp->GetCheckMinX();
		int maxx = msp->GetCheckMaxX();
		int evx = event.GetPosition().x;

		if (minx <= evx && evx <= maxx)
		{
			msp->SetChecked(!msp->IsChecked());

			SendEvent(wxEVT_CHECKBOX_CHANGE, msp);
		}
	}

	event.Skip();
}

void wxAddFilterDialog::OnPropertyGridChanged(wxPropertyGridEvent& event)
{
	wxOptionProperty* prop = wxDynamicCast(event.GetProperty(), wxOptionProperty);
	if (prop)
	{
		if (!prop->IsChecked())
		{
			prop->SetChecked();
			Refresh();

			SendEvent(wxEVT_CHECKBOX_CHANGE, prop);
		}
	}
}

void wxAddFilterDialog::OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event)
{
	wxOptionProperty* property = wxDynamicCast(event.GetProperty(), wxOptionProperty);
	if (property)
	{
		// Make inactive options gray
		wxColour col = wxSystemSettings::GetColour(property->IsChecked() ? wxSYS_COLOUR_LISTBOXTEXT : wxSYS_COLOUR_GRAYTEXT);
		property->SetTextColour(col);
	}
}

void wxAddFilterDialog::OnOkayClick(wxCommandEvent& event)
{
	FilterInfo* filterInfo = reinterpret_cast<FilterInfo*> (m_FilterChoice->GetClientData(m_FilterChoice->GetSelection()));

	(*options)->clear();
	(*options)->insert(filterInfo->defaults.begin(), filterInfo->defaults.end());

	// Iterate over all options
	wxPropertyGridConstIterator it;
	for (it = m_propertyGrid->GetIterator(wxPG_ITERATE_VISIBLE); !it.AtEnd(); it++)
	{
		wxOptionProperty* prop = wxDynamicCast(*it, wxOptionProperty);
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
					(*options)->insert_or_assign(parameter.ToStdString(), value);
				}
			}
		}
	}

	EndDialog(wxID_OK);
}

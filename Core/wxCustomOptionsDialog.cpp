#include "wxCustomOptionsDialog.h"
#include "LanguageUtils.h"
#include "OnChangeValueOptionFilter.h"
#include "OnSelectionOptionFilter.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxCustomOptionsDialog::wxCustomOptionsDialog(wxWindow* parent) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(480, 520), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer * bDialogLayout = new wxBoxSizer(wxVERTICAL);

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_tabPanel = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer * bTabLayout = new wxBoxSizer(wxVERTICAL);

	m_propertyGrid = new wxPropertyGrid(m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	m_propertyGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
	m_propertyGrid->SetValidationFailureBehavior(0);
	m_propertyGrid->Bind(wxEVT_LEFT_DOWN, &wxCustomOptionsDialog::OnLeftDown, this);
	m_propertyGrid->Bind(wxEVT_PG_CHANGED, &wxCustomOptionsDialog::OnPropertyGridChanged, this, m_propertyGrid->GetId());
	m_propertyGrid->Bind(wxEVT_CHECKBOX_CHANGE, &wxCustomOptionsDialog::OnPropertyGridCheckboxChanged, this, m_propertyGrid->GetId());
	bTabLayout->Add(m_propertyGrid, 1, wxEXPAND | wxALL, 5);

	m_tabPanel->SetSizer(bTabLayout);
	m_tabPanel->Layout();
	bTabLayout->Fit(m_tabPanel);
	m_notebook->AddPage(m_tabPanel, Trans("ui.addfilter.options"), true);

	bDialogLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1OK->Bind(wxEVT_BUTTON, &wxCustomOptionsDialog::OnOkayClick, this);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bDialogLayout->Add(m_sdbSizer1, 0, wxEXPAND | wxALL, 5);

	this->SetSizer(bDialogLayout);
	this->Layout();
	this->Centre(wxBOTH);
}

wxCustomOptionsDialog::~wxCustomOptionsDialog()
{
}

void wxCustomOptionsDialog::Configure(EncoderInfo encoderInfo, OptionContainer options)
{
	this->encoderInfo = encoderInfo;
	this->origOptions = options;

	// Reset the options map
	m_propertyGrid->Clear();

	// Set fixed default parameters
	//results.insert(encoderInfo.defaults.begin(), encoderInfo.defaults.end());

	wxColour bgColor;

	// Build all groups and properties
	for (const EncoderGroupInfo& group : encoderInfo.groups)
	{
		wxPGProperty* category = m_propertyGrid->Append(new wxPropertyCategory(group.name));

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
		for (const EncoderOptionInfo& optionInfo : group.options)
		{
			wxOptionProperty* optionProperty = new wxOptionProperty(optionInfo);
			m_propertyGrid->AppendIn(category, optionProperty);

			// Import stored settings
			//EncoderOptionInfo optionInfo = optionProperty->GetOptionInfo();
			if (options.find(optionInfo.parameter) != options.end())
			{
				optionProperty->SetChecked();

				if (optionInfo.control.type == EncoderOptionType::ComboBox)
				{
					// Select combo items by name not by value
					for (auto& item : optionInfo.control.items)
					{
						if (item.value == options[optionInfo.parameter])
						{
							optionProperty->SetValueFromString(Trans(item.id));
							break;
						}
					}
				}
				else if (optionInfo.control.type == EncoderOptionType::Integer)
				{
					long longVal;
					wxString val = options[optionInfo.parameter];
					val.ToLong(&longVal);

					optionProperty->SetValueFromInt(longVal / optionInfo.multiplicationFactor);
				}
				else if (optionInfo.control.type == EncoderOptionType::Float)
				{
					double doubleVal;
					wxString val = options[optionInfo.parameter];
					val.ToDouble(&doubleVal);

					optionProperty->SetValueFromString(wxString::Format(wxT("%.1f"), doubleVal));
				}
				else
				{
					optionProperty->SetValueFromString(options[optionInfo.parameter]);
				}
			}

			// Make inactive options gray
			wxColour colour = wxSystemSettings::GetColour(optionProperty->IsChecked() ? wxSYS_COLOUR_LISTBOXTEXT : wxSYS_COLOUR_GRAYTEXT);
			optionProperty->SetTextColour(colour);
			optionProperty->SetBackgroundColour(bgColor);
		}
	}

	// Components enabled
	Enable(encoderInfo.groups.size() > 0);

	// Execute filters
	wxPropertyGridConstIterator it;
	for (it = m_propertyGrid->GetIterator(); !it.AtEnd(); it++)
	{
		ExecuteFilters(wxDynamicCast(*it, wxOptionProperty));
	}
}

void wxCustomOptionsDialog::ExecuteFilters(wxOptionProperty* optionProperty)
{
	EncoderOptionInfo optionInfo = optionProperty->GetOptionInfo();

	// Basic filters
	std::vector<OptionFilterInfo> filters;
	filters.assign(optionInfo.filters.begin(), optionInfo.filters.end());

	// Add ComboBox item filters
	if (optionInfo.control.type == EncoderOptionType::ComboBox)
	{
		int idx = optionProperty->GetIndexForValue(optionProperty->GetValue().GetInteger());
		EncoderOptionInfo::ComboItem item = optionInfo.control.items[idx];
		filters.insert(filters.end(), item.filters.begin(), item.filters.end());
	}

	// Process all filters
	for (auto filterInfo : filters)
	{
		if (filterInfo.name == "OnChangeValue")
		{
			OnChangeValueOptionFilter filter(filterInfo, m_propertyGrid);
			filter.Execute();
		}
		else if (filterInfo.name == "OnSelection")
		{
			OnSelectionOptionFilter filter(filterInfo, m_propertyGrid);
			filter.Execute();
		}
	}
}

bool wxCustomOptionsDialog::SendEvent(wxEventType eventType, wxPGProperty* p)
{
	wxPropertyGridEvent event(wxEVT_CHECKBOX_CHANGE, m_propertyGrid->GetId());
	event.SetPropertyGrid(m_propertyGrid);
	event.SetProperty(p);
	m_propertyGrid->HandleWindowEvent(event);

	return event.WasVetoed();
}

void wxCustomOptionsDialog::OnLeftDown(wxMouseEvent& event)
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

void wxCustomOptionsDialog::OnPropertyGridChanged(wxPropertyGridEvent& event)
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

		ExecuteFilters(prop);
	}
}

void wxCustomOptionsDialog::OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event)
{
	wxOptionProperty* property = wxDynamicCast(event.GetProperty(), wxOptionProperty);
	if (property)
	{
		// Make inactive options gray
		wxColour col = wxSystemSettings::GetColour(property->IsChecked() ? wxSYS_COLOUR_LISTBOXTEXT : wxSYS_COLOUR_GRAYTEXT);
		property->SetTextColour(col);
	}
}

void wxCustomOptionsDialog::OnOkayClick(wxCommandEvent& event)
{
	options.clear();

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
					options.insert_or_assign(parameter.ToStdString(), value);
				}
			}
		}
	}

	EndDialog(wxID_OK);
}

#include "wxOptionEditor.h"
#include "LanguageUtils.h"
#include "OnChangeValueOptionFilter.h"
#include "OnSelectionOptionFilter.h"
#include "EncoderUtils.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxIMPLEMENT_DYNAMIC_CLASS(wxOptionEditor, wxPanel);

wxBEGIN_EVENT_TABLE(wxOptionEditor, wxControl)
wxEND_EVENT_TABLE()

wxOptionEditor::wxOptionEditor(wxWindow *parent, bool hasPreview):
	wxPanel(parent),
	hasPreview(hasPreview)
{
	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	// Property grid
	m_propertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	m_propertyGrid->SetMinSize(wxSize(100, 100));
	m_propertyGrid->SetValidationFailureBehavior(0);
	m_propertyGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
	m_propertyGrid->Bind(wxEVT_LEFT_DOWN, &wxOptionEditor::OnLeftDown, this);
	m_propertyGrid->Bind(wxEVT_PG_CHANGED, &wxOptionEditor::OnPropertyGridChanged, this, m_propertyGrid->GetId());
	m_propertyGrid->Bind(wxEVT_CHECKBOX_CHANGE, &wxOptionEditor::OnPropertyGridCheckboxChanged, this, m_propertyGrid->GetId());
	bSizer->Add(m_propertyGrid, 1, wxALL | wxEXPAND, 5);

	// Preview
	if (hasPreview)
	{
		m_preview = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 | wxVSCROLL | wxHSCROLL | wxWANTS_CHARS | wxTE_READONLY);
		m_preview->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Fixedsys")));
		m_preview->SetMinSize(wxSize(100, 100));
		bSizer->Add(m_preview, 0, wxEXPAND | wxALL, 5);
	}

	// Buttons
	wxPanel* m_btnPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* btnSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_btnReset = new wxButton(m_btnPanel, wxID_ANY, Trans("ui.encoderconfig.buttonReset"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnReset->Bind(wxEVT_BUTTON, &wxOptionEditor::OnResetClick, this);
	btnSizer2->Add(m_btnReset, 0, wxALL, 5);
	m_btnClear = new wxButton(m_btnPanel, wxID_ANY, Trans("ui.encoderconfig.buttonClear"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnClear->Bind(wxEVT_BUTTON, &wxOptionEditor::OnClearClick, this);
	btnSizer2->Add(m_btnClear, 0, wxALL, 5);
	m_btnPanel->SetSizer(btnSizer2);
	m_btnPanel->Layout();
	btnSizer2->Fit(m_btnPanel);
	bSizer->Add(m_btnPanel, 0, wxEXPAND | wxALL);

	this->SetSizer(bSizer);
	this->Layout();
	bSizer->Fit(this);
}

void wxOptionEditor::Configure(EncoderInfo encoderInfo, OptionContainer options)
{
	this->encoderInfo = encoderInfo;
	this->options = options;

	// Reset the options map
	m_propertyGrid->Clear();

	// Set fixed default parameters
	results.insert(encoderInfo.defaults.begin(), encoderInfo.defaults.end());

	wxColour bgColor;

	// Build all groups and properties
	for (const EncoderGroupInfo &group : encoderInfo.groups)
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

	RefreshResults();
}

void wxOptionEditor::ExecuteFilters(wxOptionProperty *optionProperty)
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

bool wxOptionEditor::SendEvent(wxEventType eventType, wxPGProperty* p)
{
	wxPropertyGridEvent event(wxEVT_CHECKBOX_CHANGE, m_propertyGrid->GetId());
	event.SetPropertyGrid(m_propertyGrid);
	event.SetProperty(p);
	m_propertyGrid->HandleWindowEvent(event);

	return event.WasVetoed();
}

void wxOptionEditor::OnLeftDown(wxMouseEvent& event)
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

void wxOptionEditor::OnPropertyGridChanged(wxPropertyGridEvent& event)
{
	wxOptionProperty *prop = wxDynamicCast(event.GetProperty(), wxOptionProperty);
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

	RefreshResults();
}

void wxOptionEditor::OnResetClick(wxCommandEvent& event)
{
	Configure(encoderInfo, options);
}

void wxOptionEditor::OnClearClick(wxCommandEvent& event)
{
	options.clear();
	options.insert(encoderInfo.defaults.begin(), encoderInfo.defaults.end());

	Configure(encoderInfo, options);
}

void wxOptionEditor::OnPropertyGridCheckboxChanged(wxPropertyGridEvent& event)
{
	wxOptionProperty *property = wxDynamicCast(event.GetProperty(), wxOptionProperty);
	if (property)
	{
		// Make inactive options gray
		wxColour col = wxSystemSettings::GetColour(property->IsChecked() ? wxSYS_COLOUR_LISTBOXTEXT : wxSYS_COLOUR_GRAYTEXT);
		property->SetTextColour(col);
	}

	RefreshResults();
}

void wxOptionEditor::RefreshResults()
{
	results.clear();

	// Set fixed default parameters
	this->results.insert(encoderInfo.defaults.begin(), encoderInfo.defaults.end());

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
                    std::string param = parameter.ToStdString();
					if (optionInfo.preprendNoIfFalse && value == "0")
					{
						param = "no-" + param;
						value = "1";
					}

                    std::string paramGroup = EncoderUtils::GetParameterGroup(encoderInfo, param);

					// Is it a grouped parameter?
					if (paramGroup.empty())
					{
						// Add the parameter and its value
						results[param] = value;
					}
					else
					{
						// Add a separator between multiple parameters
						if (results.find(paramGroup) != results.end())
						{
							results[paramGroup] += ":";
						}

						// Add the parameter and its value
						results[paramGroup] += param + "=" + value;
					}
				}
			}
		}
	}

	// Update the preview
	if (hasPreview)
		m_preview->ChangeValue(results.Serialize(false, "", ' '));
}

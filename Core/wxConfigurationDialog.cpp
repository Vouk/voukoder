#include "wxConfigurationDialog.h"
#include "LanguageUtils.h"
#include "OnChangeValueOptionFilter.h"
#include "OnSelectionOptionFilter.h"
#include <wx/gbsizer.h>

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxConfigurationDialog::wxConfigurationDialog(wxWindow* parent, EncoderInfo encoderInfo, EncoderInfo sideData, std::vector<EncoderInfo> filterInfos, TrackSettings& settings) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	settings(settings),
	info(encoderInfo)
{
	SetSize(wxDLG_UNIT(this, wxSize(300, 320)));
	SetMinSize(wxDLG_UNIT(this, wxSize(300, 250)));

	SetTitle(Trans("ui.voukoder.configuration"));

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);
	wxNotebook* m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	//GenerateEncoderTabs(m_notebook);

	// Encoder config
	if (encoderInfo.groups.size() > 0)
	{
		m_encoderOptions = new wxOptionEditor(m_notebook);
		wxBoxSizer* bVideoLayout = new wxBoxSizer(wxVERTICAL);
		m_notebook->AddPage(m_encoderOptions, Trans("ui.voukoder.configuration.options"), true);

		m_encoderOptions->Configure(encoderInfo, settings.options);
	}

	// Side data
	if (sideData.groups.size() > 0)
	{
		m_sideDataOptions = new wxOptionEditor(m_notebook, false, false);
		m_notebook->AddPage(m_sideDataOptions, Trans("ui.voukoder.configuration.sidedata"), false);

		m_sideDataOptions->Configure(sideData, settings.sideData);
	}

	// Filters
	if (filterInfos.size() > 0)
	{
		m_filterPanel = new wxFilterPanel(m_notebook, filterInfos);
		m_notebook->AddPage(m_filterPanel, Trans("ui.voukoder.configuration.filters"), false);

		m_filterPanel->Configure(settings.filters);
	}

	bDialogLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

	wxStdDialogButtonSizer* m_sdbSizer1 = new wxStdDialogButtonSizer();
	wxButton* m_sdbSizer1OK = new wxButton(this, wxID_OK, Trans("ui.voukoder.buttonOkay"), wxDefaultPosition, wxDefaultSize, 0);
	m_sdbSizer1OK->Bind(wxEVT_BUTTON, &wxConfigurationDialog::OnOkayClick, this);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	wxButton* m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL, Trans("ui.voukoder.buttonCancel"), wxDefaultPosition, wxDefaultSize, 0);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bDialogLayout->Add(m_sdbSizer1, 0, wxEXPAND | wxALL, 10);

	this->SetSizer(bDialogLayout);
	this->Layout();
	this->Centre(wxBOTH);
}

void wxConfigurationDialog::GenerateEncoderTabs(wxNotebook* notebook)
{
	for (const EncoderGroupInfo &group : info.groups)
	{
		wxPanel* page = new wxPanel(notebook);
		wxGridBagSizer* sizer = new wxGridBagSizer(0, 0);
		page->SetSizer(sizer);

		int row = 0;
		for (const EncoderOptionInfo &optionInfo : group.options)
		{
			wxCheckBox* checkBox = new wxCheckBox(page, wxID_ANY, optionInfo.name);
			sizer->Add(checkBox, wxGBPosition(row, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 0);

			switch (optionInfo.control.type)
			{
			case EncoderOptionType::ComboBox:
			{
				wxArrayString items;
				for (int i = 0; i < optionInfo.control.items.size(); i++)
					items.Add(optionInfo.control.items[i].name);

				wxChoice* choice = new wxChoice(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, items);
				choice->Select((long)optionInfo.control.selectedIndex);
				sizer->Add(choice, wxGBPosition(row, 1), wxGBSpan(1, 3), wxALL, 0);
				break;
			}
			case EncoderOptionType::Integer:
			{
				wxTextCtrl* m_textCtrl1 = new wxTextCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
				sizer->Add(m_textCtrl1, wxGBPosition(row, 1), wxGBSpan(1, 1), wxALL, 5);

				wxSlider* slider1 = new wxSlider(page, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
				sizer->Add(slider1, wxGBPosition(row, 2), wxGBSpan(1, 1), wxALL, 5);
				break;
			}
			case EncoderOptionType::Float:
				break;
			case EncoderOptionType::Boolean:
				break;
			case EncoderOptionType::String:
				break;


			}

			row++;
		}

		page->Layout();
		sizer->Fit(page);

		notebook->AddPage(page, group.name);
	}
}

void wxConfigurationDialog::ApplyChanges()
{
	// Clear all current settings
	settings.options.clear();
	settings.sideData.clear();
	settings.filters.clear();

	// Set encoder config
	if (m_encoderOptions)
		settings.options.insert(m_encoderOptions->results.begin(), m_encoderOptions->results.end());
	else
		settings.options.insert(info.defaults.begin(), info.defaults.end());

	// Set side data config
	if (m_sideDataOptions)
		settings.sideData.insert(m_sideDataOptions->results.begin(), m_sideDataOptions->results.end());

	// Filters
	if (m_filterPanel)
		m_filterPanel->GetFilterConfig(settings.filters);
}

void wxConfigurationDialog::OnOkayClick(wxCommandEvent& event)
{
	ApplyChanges();

	EndDialog(wxID_OK);
}

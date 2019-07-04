#include "wxConfigurationDialog.h"
#include "LanguageUtils.h"
#include "OnChangeValueOptionFilter.h"
#include "OnSelectionOptionFilter.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxConfigurationDialog::wxConfigurationDialog(wxWindow* parent, EncoderInfo encoderInfo, EncoderInfo sideData, std::vector<EncoderInfo> filterInfos, OptionContainer** encoderOptions, OptionContainer** sideDataOptions, FilterConfig** filterConfig) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(480, 520), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	encoderOptions(encoderOptions),
	sideDataOptions(sideDataOptions),
	filterOptions(filterConfig)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	SetTitle(Trans("ui.voukoder.configuration"));

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);
	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	// Encoder config
	if (encoderInfo.groups.size() > 0)
	{
		m_encoderOptions = new wxOptionEditor(m_notebook);
		wxBoxSizer* bVideoLayout = new wxBoxSizer(wxVERTICAL);
		m_notebook->AddPage(m_encoderOptions, Trans("ui.voukoder.configuration.options"), true);

		m_encoderOptions->Configure(encoderInfo, **encoderOptions);
	}

	// Side data
	if (sideData.groups.size() > 0)
	{
		m_sideDataOptions = new wxOptionEditor(m_notebook, false);
		m_notebook->AddPage(m_sideDataOptions, Trans("ui.voukoder.configuration.sidedata"), false);

		m_sideDataOptions->Configure(sideData, **sideDataOptions);
	}

	// Filters
	if (filterInfos.size() > 0)
	{
		m_filterPanel = new wxFilterPanel(m_notebook, filterInfos);
		m_notebook->AddPage(m_filterPanel, Trans("ui.voukoder.configuration.filters"), false);

		m_filterPanel->Configure(**filterConfig);
	}

	bDialogLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK, Trans("ui.voukoder.buttonOkay"), wxDefaultPosition, wxDefaultSize, 0);
	m_sdbSizer1OK->Bind(wxEVT_BUTTON, &wxConfigurationDialog::OnOkayClick, this);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL, Trans("ui.voukoder.buttonCancel"), wxDefaultPosition, wxDefaultSize, 0);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bDialogLayout->Add(m_sdbSizer1, 0, wxEXPAND | wxALL, 10);

	this->SetSizer(bDialogLayout);
	this->Layout();
	this->Centre(wxBOTH);
}

wxConfigurationDialog::~wxConfigurationDialog()
{}

void wxConfigurationDialog::OnOkayClick(wxCommandEvent& event)
{
	// Set encoder config
	if (m_encoderOptions)
	{
		(*encoderOptions)->clear();
		(*encoderOptions)->insert(m_encoderOptions->results.begin(), m_encoderOptions->results.end());
	}

	// Set side data config
	if (m_sideDataOptions)
	{
		(*sideDataOptions)->clear();
		(*sideDataOptions)->insert(m_sideDataOptions->results.begin(), m_sideDataOptions->results.end());
	}

	// Filters
	if (m_filterPanel)
	{
		m_filterPanel->GetFilterConfig(**filterOptions);
	}

	EndDialog(wxID_OK);
}

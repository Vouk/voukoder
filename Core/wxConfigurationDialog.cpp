#include "wxConfigurationDialog.h"
#include "LanguageUtils.h"
#include "OnChangeValueOptionFilter.h"
#include "OnSelectionOptionFilter.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxConfigurationDialog::wxConfigurationDialog(wxWindow* parent, EncoderInfo encoderInfo, EncoderInfo sideData, std::vector<FilterInfo> filterInfos, OptionContainer** encoderOptions, OptionContainer** sideDataOptions, FilterConfig** filterOptions) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(480, 520), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	encoderOptions(encoderOptions),
	sideDataOptions(sideDataOptions),
	filterOptions(filterOptions)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);
	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	// Encoder config
	if (encoderInfo.groups.size() > 0)
	{
		m_encoderOptions = new wxOptionEditor(m_notebook);
		wxBoxSizer* bVideoLayout = new wxBoxSizer(wxVERTICAL);
		m_notebook->AddPage(m_encoderOptions, Trans("ui.voukoder.confguration.options"), true);

		m_encoderOptions->Configure(encoderInfo, **encoderOptions);
	}

	// Side data
	if (sideData.groups.size() > 0)
	{
		m_sideDataOptions = new wxOptionEditor(m_notebook, false);
		wxBoxSizer* bSideDataLayout = new wxBoxSizer(wxVERTICAL);
		m_notebook->AddPage(m_sideDataOptions, Trans("ui.voukoder.confguration.sidedata"), false);

		m_sideDataOptions->Configure(sideData, **sideDataOptions);
	}

	// Filters
	if (filterInfos.size() > 0)
	{
		m_filterPanel = new wxFilterPanel(m_notebook, AVMEDIA_TYPE_VIDEO);
		m_notebook->AddPage(m_filterPanel, Trans("ui.voukoder.confguration.filters"), false);

		// Configure Filters
		m_filterPanel->SetFilterConfig(**filterOptions);
	}

	bDialogLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1OK->Bind(wxEVT_BUTTON, &wxConfigurationDialog::OnOkayClick, this);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bDialogLayout->Add(m_sdbSizer1, 0, wxEXPAND | wxALL, 5);

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
		m_filterPanel->GetFilterConfig(**filterOptions);

	EndDialog(wxID_OK);
}

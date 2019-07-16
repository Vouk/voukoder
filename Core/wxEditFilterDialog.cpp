#include "wxEditFilterDialog.h"
#include "LanguageUtils.h"

wxDEFINE_EVENT(wxEVT_CHECKBOX_CHANGE, wxPropertyGridEvent);

wxEditFilterDialog::wxEditFilterDialog(wxWindow* parent, EncoderInfo filterInfo, OptionContainer **options) :
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	filterInfo(filterInfo),
	options(options)
{
	SetSize(wxDLG_UNIT(this, wxSize(220, 260)));
	SetMinSize(wxDLG_UNIT(this, wxSize(150, 160)));

	// Set the right window title
	SetTitle(Trans("ui.voukoder.editfilter.title"));

	wxBoxSizer* bDialogLayout = new wxBoxSizer(wxVERTICAL);

	wxNotebook* m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	wxBoxSizer* bTabLayout = new wxBoxSizer(wxVERTICAL);

	// Filter options editor
	m_editor = new wxOptionEditor(m_notebook, false);
	m_notebook->AddPage(m_editor, Trans("ui.voukoder.editfilter.options"), false);
	m_editor->Configure(filterInfo, **options);
	   
	bDialogLayout->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

	// Button bar
	wxStdDialogButtonSizer* m_sdbSizer1 = new wxStdDialogButtonSizer();
	wxButton* m_sdbSizer1OK = new wxButton(this, wxID_OK, Trans("ui.voukoder.buttonOkay"), wxDefaultPosition, wxDefaultSize, 0);
	m_sdbSizer1OK->Bind(wxEVT_BUTTON, &wxEditFilterDialog::OnOkayClick, this);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	wxButton* m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL, Trans("ui.voukoder.buttonCancel"), wxDefaultPosition, wxDefaultSize, 0);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bDialogLayout->Add(m_sdbSizer1, 0, wxEXPAND | wxALL, 10);

	this->SetSizer(bDialogLayout);
	this->Layout();

	this->Centre(wxBOTH);
}

void wxEditFilterDialog::OnOkayClick(wxCommandEvent& event)
{
	// Write new options
	(*options)->clear();
	(*options)->insert(m_editor->results.begin(), m_editor->results.end());

	EndDialog(wxID_OK);
}

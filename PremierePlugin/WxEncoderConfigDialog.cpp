#include "stdafx.h"
#include "WxEncoderConfigDialog.h"


WxEncoderConfigDialog::WxEncoderConfigDialog()
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_panel = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_notebook->AddPage(m_panel, wxT("a page"), true);

	bSizer3->Add(m_notebook);
	
	this->SetSizer(bSizer3);
	this->Layout();

	this->Centre(wxBOTH);
}


WxEncoderConfigDialog::~WxEncoderConfigDialog()
{
}

#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>

class WxEncoderConfigDialog: public wxDialog
{
public:
	WxEncoderConfigDialog();
	~WxEncoderConfigDialog();

protected:
	wxNotebook *m_notebook;
	wxPanel *m_panel;
};


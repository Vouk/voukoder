#include "ConfigImportDialog.h"
#include "resource.h"

ConfigImportDialog::ConfigImportDialog()
{
	setup.dialogId = IDD_IMPORTDIALOG;
	
	on_message(WM_INITDIALOG, [&](wl::params p)->INT_PTR
	{
		edit1.assign(this, IDC_ARGUMENTS);
		return TRUE;
	});

	on_message(WM_COMMAND, [&](wl::params p)->INT_PTR
	{
		if (LOWORD(p.wParam) == IDC_IMPORT)
		{
/*
const int length = SendMessage(edit1.hwnd(), WM_GETTEXTLENGTH, 0, 0);
			//char* buffer = new char[length];
			TCHAR text[length];
			SendMessage(edit1.hwnd(), WM_GETTEXT, sizeof(text) / sizeof(text[0]), LPARAM(text));

			SendMessage(edit1.hwnd(), WM_GETTEXT, (WPARAM)length + 1, (LPARAM)buffer);
			result.assign(buffer, length + 1);
			*/
			EndDialog(hwnd(), TRUE);
			return TRUE;
		}

		if (LOWORD(p.wParam) == IDC_CANCEL)
		{
			EndDialog(hwnd(), FALSE);
			return TRUE;
		}
		return FALSE;
	});

	on_message(WM_CLOSE, [&](wl::params p)->LRESULT
	{
		EndDialog(hwnd(), FALSE);
		return TRUE;
	});
}

std::string ConfigImportDialog::getValue()
{	
	return this->result;
}
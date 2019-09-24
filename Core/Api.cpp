#include "Api.h"
#include <wx/wx.h>
#include "wxVoukoderDialog.h"

int open_config_dialog(VoukoderSettings settings, HWND hwnd)
{
	// Default window is the active window
	if (hwnd == NULL)
		hwnd = GetActiveWindow();

	// Create wxWidget-approved parent window.
	wxWindow parent;
	parent.SetHWND((WXHWND)hwnd);
	parent.AdoptAttributesFromHWND();
	wxTopLevelWindows.Append(&parent);

	// Create and launch configuration dialog.
	ExportInfo e;
	wxVoukoderDialog dialog(&parent, e);
	dialog.SetConfiguration();
	int result = dialog.ShowModal();

	wxTopLevelWindows.DeleteObject(&parent);
	parent.SetHWND((WXHWND)NULL);

	return result;
}

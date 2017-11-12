#pragma once

#include <string.h>
#include "winlamb/dialog_modal.h"
#include "winlamb/native_control.h"

class ConfigImportDialog : public wl::dialog_modal
{
public:
	ConfigImportDialog();
	std::string ConfigImportDialog::getValue();

private:
	std::string result;
	wl::native_control edit1;
};


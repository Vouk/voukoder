#include "AEGP_SuiteHandler.h"

void AEGP_SuiteHandler::MissingSuiteError() const
{
	PF_Err poop = PF_Err_BAD_CALLBACK_PARAM;

	throw poop;
}
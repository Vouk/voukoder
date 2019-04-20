#include <AEGP_SuiteHandler.h>
#include <AE_Macros.h>

AEGP_SuiteHandler::AEGP_SuiteHandler(const SPBasicSuite *pica_basicP) :
	i_pica_basicP(pica_basicP)
{
	AEFX_CLR_STRUCT(i_suites);

	if (!i_pica_basicP) {						//can't construct w/out basic suite, everything else is demand loaded
		MissingSuiteError();
	}
}

AEGP_SuiteHandler::~AEGP_SuiteHandler()
{
	ReleaseAllSuites();
}

// Had to go to the header file to be inlined to please CW mach-o target
/*void *AEGP_SuiteHandler::pLoadSuite(A_char *nameZ, A_long versionL) const
{
	const void *suiteP;
	A_long err = i_pica_basicP->AcquireSuite(nameZ, versionL, &suiteP);

	if (err || !suiteP) {
		MissingSuiteError();
	}

	return (void*)suiteP;
}*/

// Free a particular suite. Ignore errors, because, what is there to be done if release fails?
void AEGP_SuiteHandler::ReleaseSuite(const A_char *nameZ, A_long versionL)
{
	i_pica_basicP->ReleaseSuite(nameZ, versionL);
}


#include "AEConfig.h"

#ifndef AE_OS_WIN
	#include "AE_General.r"
#endif

resource 'PiPL' (16000) {
	{	/* array properties: 7 elements */
		/* [1] */
		Kind {
			AEGP
		},
		/* [2] */
		Name {
			"Voukoder R2"
		},
		/* [3] */
		Category {
			"General Plugin"
		},
		/* [4] */
		Version {
			65536
		},
		/* [5] */
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"GPMain_IO"},
	#else
		CodeWin32X86 { "GPMain_IO" },
	#endif
#else	
	#ifdef AE_PROC_INTELx64
		CodeMacIntel64 { "GPMain_IO" },
	#else
		CodeMachOPowerPC { "GPMain_IO" },
		CodeMacIntel32 { "GPMain_IO" },
	#endif
#endif
	}
};



#ifdef AE_OS_MAC_OBSOLETE

#include "MacTypes.r"

#define NAME				"Voukoder"
#define VERSION_STRING		"2.0"
resource 'vers' (1, NAME " Version", purgeable)
{
	5, 0x50, final, 0, verUs,
	VERSION_STRING,
	VERSION_STRING
	"\nbla"
};

resource 'vers' (2, NAME " Version", purgeable)
{
	5, 0x50, final, 0, verUs,
	VERSION_STRING,
	"by Daniel Stankewitz"
};

#endif


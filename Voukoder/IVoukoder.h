/**
* License header. Completely free license. Commercial and open source
*/
#pragma once

#include <ComDef.h>

// {E9661BFA-4B8E-4217-BCD8-24074D75000B}
_declspec(selectany) GUID CLSID_Voukoder = {
	0xe9661bfa, 0x4b8e, 0x4217, { 0xbc, 0xd8, 0x24, 0x7, 0x4d, 0x75, 0x0, 0xb } 
};

interface __declspec(uuid("E26427F6-CBCA-4859-BCC3-162AF1E06CEE")) IVoukoder : public IUnknown
{
	STDMETHOD(Init)(const char* appname)PURE;
};
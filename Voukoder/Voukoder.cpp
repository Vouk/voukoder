#include "Voukoder.h"

long g_cRefThisDll;
HANDLE g_module;

#include "ClassFactory.h"
#include "CVoukoder.h"

long * CObjRoot::p_ObjCount = NULL; 

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		g_module = hModule;
		CObjRoot::p_ObjCount = &g_cRefThisDll;
	}
	return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
	*ppvOut = NULL;
	if (IsEqualIID(rclsid, CLSID_Voukoder))
	{
		CClassFactory<CVoukoder> *pcf = new CClassFactory<CVoukoder>;
		return pcf->QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void)
{
	return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllRegisterServer(void)
{
	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	return S_OK;
}
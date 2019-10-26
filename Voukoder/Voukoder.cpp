#include "Voukoder.h"

long g_cRefThisDll;
HANDLE g_module;

#include "ClassFactory.h"
#include "CVoukoder.h"
#include "Registrar.h"
#include "VoukoderTypeLib_i.c"

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
	if (IsEqualIID(rclsid, CLSID_CoVoukoder))
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
	CDllRegistrar registrar;
	
	wchar_t path[MAX_PATH];
	GetModuleFileName((HMODULE)g_module, path, MAX_PATH);

	return registrar.RegisterObject(CLSID_CoVoukoder, L"Voukoder", L"COMServer", path) ? S_OK : S_FALSE;
}

STDAPI DllUnregisterServer(void)
{
	CDllRegistrar registrar;

	return registrar.UnRegisterObject(CLSID_CoVoukoder, L"Voukoder", L"COMServer") ? S_OK : S_FALSE;
}
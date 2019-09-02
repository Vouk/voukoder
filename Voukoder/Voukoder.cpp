#include "Voukoder.h"

long g_cRefThisDll;
HANDLE g_module;

#include "ClassFactory.h"
#include "CVoukoder.h"
#include "Registrar.h"

long * CObjRoot::p_ObjCount = NULL; // this is just because i didnt want to use any globals inside the
									// class framework.

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_module = hModule;
		CObjRoot::p_ObjCount = &g_cRefThisDll;
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
	*ppvOut = NULL;
	if (IsEqualIID(rclsid, CLSID_Voukoder))
	{
		// declare a classfactory for CmyInterface class 
		CClassFactory<CVoukoder> *pcf = new  CClassFactory<CVoukoder>;
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

	return registrar.RegisterObject(CLSID_Voukoder, L"VoukoderLib", L"VoukoderObj", path) ? S_OK : S_FALSE;
}

STDAPI DllUnregisterServer(void)
{
	CDllRegistrar registrar;

	return registrar.UnRegisterObject(CLSID_Voukoder, L"VoukoderLib", L"VoukoderObj") ? S_OK : S_FALSE;
}
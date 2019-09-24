#pragma once

#ifdef VOUKODER_EXPORTS
#define VOUKODER_API __declspec(dllexport)
#else
#define VOUKODER_API __declspec(dllimport)
#endif

#include <objbase.h>

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut);
STDAPI DllCanUnloadNow(void);
STDAPI DllRegisterServer(void);
STDAPI DllUnregisterServer(void);
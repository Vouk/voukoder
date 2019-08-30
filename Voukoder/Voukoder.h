#pragma once

#ifdef MYSERV_EXPORTS
#define MYSERV_API __declspec(dllexport)
#else
#define MYSERV_API __declspec(dllimport)
#endif

#include <objbase.h>

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut);
STDAPI DllCanUnloadNow(void);
STDAPI DllRegisterServer(void);
STDAPI DllUnregisterServer(void);
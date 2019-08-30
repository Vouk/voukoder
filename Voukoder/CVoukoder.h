#pragma once

#include "ComBase.h"
#include "IVoukoder.h"

class CVoukoder : public CComBase<>, public InterfaceImpl<IVoukoder>
{
public:
	CVoukoder();
	virtual ~CVoukoder();

	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv);
	STDMETHOD(Init)(const char* appname);
};
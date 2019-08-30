#include "CVoukoder.h"

CVoukoder::CVoukoder()
{
}

CVoukoder::~CVoukoder()
{
}

STDMETHODIMP CVoukoder::QueryInterface(REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, __uuidof(IVoukoder)))
	{
		*ppv = (IVoukoder *)this;
		_AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP CVoukoder::Init(const char* appname)
{
	OutputDebugStringA(appname);

	return S_OK;
}
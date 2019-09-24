#pragma once

#include "stdio.h"
#include "ComBase.h"

template <class comObj >
class CClassFactory : public CComBase<>, public InterfaceImpl<IClassFactory>
{
public:
	CClassFactory() {};
	virtual ~CClassFactory() {};

	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv)
	{
		*ppv = NULL;
		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
		{
			*ppv = (IClassFactory *)this;
			_AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
	{
		*ppvObj = NULL;
		if (pUnkOuter)
			return CLASS_E_NOAGGREGATION;
		m_pObj = new comObj;
		if (!m_pObj)
			return E_OUTOFMEMORY;
		HRESULT hr = m_pObj->QueryInterface(riid, ppvObj);

		if (hr != S_OK)
			delete m_pObj;

		return hr;
	}

	STDMETHODIMP LockServer(BOOL) { return S_OK; }

private:
	comObj * m_pObj = NULL;
};
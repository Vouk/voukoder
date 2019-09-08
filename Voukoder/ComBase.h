#pragma once

#include <objbase.h>

#pragma warning(disable : 4250)

class CSingleThreaded
{
protected:
	STDMETHODIMP_(ULONG) Increment(long &reflong)
	{
		reflong++;
		return reflong;
	}

	STDMETHODIMP_(ULONG) Decrement(long &reflong)
	{
		reflong--;
		return reflong;
	}
};

class CMultiThreaded
{
protected:
	STDMETHODIMP_(ULONG) Increment(long &reflong)
	{
		::InterlockedIncrement(&reflong);
		return reflong;
	}

	STDMETHODIMP_(ULONG) Decrement(long &reflong)
	{
		::InterlockedDecrement(&reflong);
		return reflong;
	}
};

class CObjRoot
{
protected:
	long	m_cRef;
protected:
	STDMETHOD_(ULONG, _AddRef)() = 0;
	STDMETHOD_(ULONG, _Release)() = 0;
public:
	static long *p_ObjCount;
};

template <class Interface>
class InterfaceImpl : public virtual CObjRoot, public Interface
{
public:
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv) = 0;
	STDMETHODIMP_(ULONG) AddRef()
	{
		return _AddRef();
	}
	STDMETHODIMP_(ULONG) Release()
	{
		return _Release();
	}
};

template <class ThreadModel = CSingleThreaded >
class CComBase : public virtual CObjRoot, public ThreadModel
{
public:
	CComBase() {};
	virtual ~CComBase() {};

protected:

	STDMETHODIMP_(ULONG) _AddRef()
	{
		if (p_ObjCount)
			ThreadModel::Increment(*p_ObjCount);
		return ThreadModel::Increment(m_cRef);
	}
	STDMETHODIMP_(ULONG) _Release()
	{
		long Value = ThreadModel::Decrement(m_cRef);
		if (!m_cRef)
			delete this;
		if (p_ObjCount)
			ThreadModel::Decrement(*p_ObjCount);
		return Value;
	}
};
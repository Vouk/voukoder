/**
 * Voukoder
 * Copyright (C) 2017-2022 Daniel Stankewitz, All Rights Reserved
 * https://www.voukoder.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
 */
#pragma once

#include "stdio.h"
#include "ComBase.h"

template <class comObj >
class CClassFactory : public CComBase<>, public InterfaceImpl<IClassFactory>
{
public:
	CClassFactory() {
	};
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
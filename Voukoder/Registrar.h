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

class CRegistrar
{
protected:
	CRegistrar() {};

	BOOL SetInRegistry(HKEY hRootKey, LPCWSTR subKey, LPCWSTR keyName, LPCWSTR keyValue)
	{
		HKEY hKeyResult;
		DWORD dataLength;
		DWORD dwDisposition;

		if (RegCreateKeyEx(hRootKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyResult, &dwDisposition) != ERROR_SUCCESS)
			return FALSE;

		dataLength = (DWORD)wcslen(keyValue) * sizeof(wchar_t);

		DWORD retVal = RegSetValueEx(hKeyResult, keyName, 0, REG_SZ, (const BYTE *)keyValue, dataLength);

		RegCloseKey(hKeyResult);

		return (retVal == ERROR_SUCCESS) ? TRUE : FALSE;
	}

	BOOL DelFromRegistry(HKEY hRootKey, LPCTSTR subKey)
	{
		long retCode;
		retCode = RegDeleteKey(hRootKey, subKey);
		if (retCode != ERROR_SUCCESS)
			return false;
		return true;
	}

	bool StrFromCLSID(REFIID riid, LPWSTR strCLSID)
	{
		LPOLESTR pOleStr = NULL;
		HRESULT hr = ::StringFromCLSID(riid, &pOleStr);
		if (FAILED(hr))
			return false;

		wcscpy(strCLSID, pOleStr);

		return true;
	}
public:
	bool RegisterObject(REFIID riid, LPCWSTR LibId, LPCWSTR ClassId)
	{
		wchar_t strCLSID[MAX_PATH];
		wchar_t Buffer[MAX_PATH];

		if (!wcslen(ClassId))
			return false;

		if (!StrFromCLSID(riid, strCLSID))
			return false;

		if (!wcslen(LibId) && wcslen(ClassId))
			swprintf_s(Buffer, L"%s.%s\\CLSID", ClassId, ClassId);
		else
			swprintf_s(Buffer, L"%s.%s\\CLSID", LibId, ClassId);

		BOOL result;
		result = SetInRegistry(HKEY_CLASSES_ROOT, Buffer, L"", strCLSID);
		if (!result)
			return false;

		swprintf_s(Buffer, L"CLSID\\%s", strCLSID);

		wchar_t Class[MAX_PATH];
		swprintf_s(Class, L"%s Class", ClassId);

		if (!SetInRegistry(HKEY_CLASSES_ROOT, Buffer, L"", Class))
			return false;

		swprintf_s(Class, L"%s.%s", LibId, ClassId);
		wcscat(Buffer, L"\\ProgId");

		return SetInRegistry(HKEY_CLASSES_ROOT, Buffer, L"", Class) ? true : false;
	}

	bool UnRegisterObject(REFIID riid, LPCWSTR LibId, LPCWSTR ClassId)
	{
		wchar_t strCLSID[MAX_PATH];
		wchar_t Buffer[MAX_PATH];

		if (!StrFromCLSID(riid, strCLSID))
			return false;

		swprintf_s(Buffer, L"%s.%s\\CLSID", LibId, ClassId);

		if (!DelFromRegistry(HKEY_CLASSES_ROOT, Buffer))
			return false;

		swprintf_s(Buffer, L"%s.%s", LibId, ClassId);

		if (!DelFromRegistry(HKEY_CLASSES_ROOT, Buffer))
			return false;

		swprintf_s(Buffer, L"CLSID\\%s\\ProgId", strCLSID);

		if (!DelFromRegistry(HKEY_CLASSES_ROOT, Buffer))
			return false;

		swprintf_s(Buffer, L"CLSID\\%s", strCLSID);

		return DelFromRegistry(HKEY_CLASSES_ROOT, Buffer) ? true : false;
	}
};

class CDllRegistrar : public CRegistrar
{
public:
	bool RegisterObject(REFIID riid, LPCWSTR LibId, LPCWSTR ClassId, LPCWSTR Path)
	{
		if (!CRegistrar::RegisterObject(riid, LibId, ClassId))
			return false;

		wchar_t strCLSID[MAX_PATH];
		wchar_t Buffer[MAX_PATH];

		if (!StrFromCLSID(riid, strCLSID))
			return false;

		swprintf_s(Buffer, L"CLSID\\%s\\InProcServer32", strCLSID);

		if (!SetInRegistry(HKEY_CLASSES_ROOT, Buffer, L"ThreadingModel", L"Both"))
			return false;

		return SetInRegistry(HKEY_CLASSES_ROOT, Buffer, L"", Path) ? true : false;
	}

	bool UnRegisterObject(REFIID riid, LPCWSTR LibId, LPCWSTR ClassId)
	{
		wchar_t strCLSID[MAX_PATH];
		wchar_t Buffer[MAX_PATH];

		if (!StrFromCLSID(riid, strCLSID))
			return false;

		swprintf_s(Buffer, L"CLSID\\%s\\InProcServer32", strCLSID);

		if (!DelFromRegistry(HKEY_CLASSES_ROOT, Buffer))
			return false;

		return CRegistrar::UnRegisterObject(riid, LibId, ClassId);
	}
};
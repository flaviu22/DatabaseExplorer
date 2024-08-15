// SettingsStoreEx.cpp : implementation of the CSettingsStoreEx class
//

#include "pch.h"
#include "SettingsStoreEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsStoreEx

IMPLEMENT_DYNCREATE(CSettingsStoreEx, CSettingsStore)

/////////////////////////////////////////////////////////////////////////////
// CTestDatabaseDoc construction/destruction

CSettingsStoreEx::CSettingsStoreEx(BOOL bAdmin, BOOL bReadOnly)
	:CSettingsStore(bAdmin, bReadOnly)
{
	// TODO: add one-time construction code here
}
#ifdef _UNICODE
void CSettingsStoreEx::EnumKeys(std::vector<CString>& keys)
{
	keys.clear();
	DWORD dwCount = 0;
	if (ERROR_SUCCESS != RegQueryInfoKey(m_reg.m_hKey, NULL, NULL, NULL,
		&dwCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
		return;

	for (DWORD i = 0; i < dwCount; i++)
	{
		std::vector<wchar_t> buff(_MAX_PATH);
		DWORD dwLength = static_cast<DWORD>(buff.size());
		if (ERROR_SUCCESS == m_reg.EnumKey(i, buff.data(), &dwLength))
			keys.push_back(buff.data());
	}
}

void CSettingsStoreEx::EnumValues(std::vector<CString>& values)
{
	values.clear();
	DWORD dwCount = 0;
	if (ERROR_SUCCESS != RegQueryInfoKey(m_reg.m_hKey, NULL, NULL, NULL,
		NULL, NULL, NULL, &dwCount, NULL, NULL, NULL, NULL))
		return;

	for (DWORD i = 0; i < dwCount; ++i)
	{
		std::vector<wchar_t> buff(_MAX_PATH);
		DWORD dwLength = static_cast<DWORD>(buff.size());
		if (ERROR_SUCCESS == RegEnumValue(m_reg.m_hKey, i, buff.data(), &dwLength, 
			NULL, NULL, NULL, NULL))
			values.push_back(buff.data());
	}
}
// return a pair value -> data
void CSettingsStoreEx::EnumValues(std::unordered_map<std::wstring, std::wstring>& values)
{
	values.clear();
	DWORD dwCount = 0;
	if (ERROR_SUCCESS != RegQueryInfoKey(m_reg.m_hKey, NULL, NULL, NULL,
		NULL, NULL, NULL, &dwCount, NULL, NULL, NULL, NULL))
		return;

	for (DWORD i = 0; i < dwCount; ++i)
	{
		std::vector<wchar_t> value(_MAX_PATH);
		std::vector<wchar_t> data(_MAX_PATH);
		DWORD dwLengthValue = static_cast<DWORD>(value.size());
		DWORD dwLengthData = static_cast<DWORD>(data.size());
		if (ERROR_SUCCESS == RegEnumValue(m_reg.m_hKey, i,
			value.data(), &dwLengthValue, NULL, NULL, 
			reinterpret_cast<LPBYTE>(data.data()), &dwLengthData))
			values[value.data()] = data.data();
	}
}
#else
void CSettingsStoreEx::EnumKeys(std::vector<CString>& keys)
{
	keys.clear();
	DWORD dwCount = 0;
	if (ERROR_SUCCESS != RegQueryInfoKey(m_reg.m_hKey, NULL, NULL, NULL,
		&dwCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
		return;

	for (DWORD i = 0; i < dwCount; i++)
	{
		std::vector<char> buff(_MAX_PATH);
		DWORD dwLength = buff.size();
		if (ERROR_SUCCESS == m_reg.EnumKey(i, buff.data(), &dwLength))
			keys.push_back(buff.data());
	}
}

void CSettingsStoreEx::EnumValues(std::vector<CString>& values)
{
	values.clear();
	DWORD dwCount = 0;
	if (ERROR_SUCCESS != RegQueryInfoKey(m_reg.m_hKey, NULL, NULL, NULL,
		NULL, NULL, NULL, &dwCount, NULL, NULL, NULL, NULL))
		return;

	for (DWORD i = 0; i < dwCount; ++i)
	{
		std::vector<char> buff(_MAX_PATH);
		DWORD dwLength = buff.size();
		if (ERROR_SUCCESS == RegEnumValue(m_reg.m_hKey, i, buff.data(), &dwLength,
			NULL, NULL, NULL, NULL))
			values.push_back(buff.data());
	}
}
// return a pair value -> data
void CSettingsStoreEx::EnumValues(std::unordered_map<std::string, std::string>& values)
{
	values.clear();
	DWORD dwCount = 0;
	if (ERROR_SUCCESS != RegQueryInfoKey(m_reg.m_hKey, NULL, NULL, NULL,
		NULL, NULL, NULL, &dwCount, NULL, NULL, NULL, NULL))
		return;

	for (DWORD i = 0; i < dwCount; ++i)
	{
		std::vector<char> value(_MAX_PATH);
		std::vector<char> data(_MAX_PATH);
		DWORD dwLengthValue = value.size();
		DWORD dwLengthData = data.size();
		if (ERROR_SUCCESS == RegEnumValue(m_reg.m_hKey, i,
			value.data(), &dwLengthValue, NULL, NULL,
			reinterpret_cast<LPBYTE>(data.data()), &dwLengthData))
			values[value.data()] = data.data();
	}
}
#endif // _UNICODE

#pragma once

#include <afxsettingsstore.h>
#include <string>
#include <vector>
#include <unordered_map>

class CSettingsStoreEx : public CSettingsStore
{
	DECLARE_DYNCREATE(CSettingsStoreEx)

public:
	CSettingsStoreEx(BOOL bAdmin, BOOL bReadOnly);

public:
#ifdef _UNICODE
	void EnumKeys(std::vector<CString>& keys);
	void EnumValues(std::vector<CString>& values);
	void EnumValues(std::unordered_map<std::wstring, std::wstring>& values);
#else
	void EnumKeys(std::vector<CString>& keys);
	void EnumValues(std::vector<CString>& values);
	void EnumValues(std::unordered_map<std::string, std::string>& values);
#endif
protected:
	CSettingsStoreEx() = default;
};

// DatabaseExplorer.h : main header file for the DatabaseExplorer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include "DatabaseExplorerDoc.h"

#include <vector>
#include <string>
#include <unordered_map>

#define WMU_SETMESSAGETEXT					(WM_APP + 39)
#define WMU_CHILDFRAMEADDED					(WM_APP + 40)
#define WMU_CHILDFRAMEREMOVED				(WM_APP + 41)
#define WMU_CHILDFRAMEACTIVATE				(WM_APP + 42)
#define WMU_CHILDFRAMEACTIVATED				(WM_APP + 43)
#define WMU_ISPOPULATEMODE					(WM_APP + 44)
#define WMU_RESTOREQUERIES					(WM_APP + 45)
#define WMU_SETWORDWRAP						(WM_APP + 46)
#define WMU_DARKMODE						(WM_APP + 47)
#define WMU_POSTINIT						(WM_APP + 80)

#define TIME_ONEMINUTE						(60 * 1000)

#define STR_DSNSOURCE						_T("DSNSource")
#define STR_DATABASETYPE					_T("DatabaseType")
#define STR_RSTYPE							_T("RsType")
#define STR_MSSQLAUTHENTICATIONREQUIRED		_T("MsSqlAuthenticationRequired")

constexpr COLORREF g_crDummy = RGB(77, 77, 77);
constexpr COLORREF g_crColorDark = RGB(44, 44, 44);
constexpr COLORREF g_crColorWhite = RGB(254, 255, 255);
constexpr COLORREF g_crHeaderColorDark = RGB(44, 44, 44);
constexpr COLORREF g_crHeaderColorDarkHot = RGB(64, 64, 64);
constexpr COLORREF g_crHeaderColorDarkPressed = RGB(144, 144, 144);

// CDatabaseExplorerApp:
// See DatabaseExplorer.cpp for the implementation of this class
//

class CDatabaseExplorerApp : public CWinAppEx
{
public:
	CDatabaseExplorerApp() noexcept;

	enum UH_POPULATELIST
	{
		UH_INITDATABASE		= 1,
		UH_SELECTDATABASE,
		UH_POPULATEDATABASEPANEL,
		UH_RUNSQLSATEMENTS,
		UH_REFRESH,
		UH_LISTTABLE
	};

public:
	BOOL m_bDark{ FALSE };
	BOOL m_bWordWrap{ FALSE };
	BOOL m_bHiColorIcons{ TRUE };
	BOOL m_bVirtualMode{ FALSE };
	UINT m_nAppLook{ ID_VIEW_APPLOOK_OFF_2007_BLACK };

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
public:
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	CString GetAppPath() const;
	CString GetAppPathTemp() const;
	CString GetFileNameFrom(const CString& sPath) const;
	void UpdateBackupFiles();
	CString GetTitleNormalized(const CString& sTitle) const;
	BOOL HasValidDocumentTitle(CString sTitle) const;
	void DisconnectAllDatabases() const;

#ifdef _UNICODE
	BOOL FileExist(const CString sFilePath) const { return (-1 != _waccess(sFilePath, 0)); }
#else
	BOOL FileExist(const CString sFilePath) const { return (-1 != _access(sFilePath, 0)); }
#endif

private:
	std::vector<std::wstring> GetBackupFiles() const;
	void SaveQueries(const std::wstring& filename, std::vector<CString>&& queries) const;
	std::unordered_map<std::wstring, SDocData> GetDocumentsData() const;
	void RemoveOldBackup(const std::unordered_map<std::wstring, SDocData>& docdata);
	void SaveNewBackup(std::unordered_map<std::wstring, SDocData>&& docdata);
	void SaveDocsOrder(std::vector<CString>&& names) const;
	std::vector<std::wstring> GetDocsOrder() const;
	CString GetBackupPath() const { return GetAppPath() + _T("Backup\\"); }

// Generated message map functions
protected:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDatabaseExplorerApp theApp;

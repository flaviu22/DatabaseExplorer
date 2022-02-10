// DatabaseExplorer.h : main header file for the DatabaseExplorer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#define WMU_CHILDFRAMEADDED					(WM_APP + 40)
#define WMU_CHILDFRAMEREMOVED				(WM_APP + 41)
#define WMU_CHILDFRAMEACTIVATE				(WM_APP + 42)
#define WMU_CHILDFRAMEACTIVATED				(WM_APP + 43)
#define WMU_ISPOPULATEMODE					(WM_APP + 44)
#define WMU_POSTINIT						(WM_APP + 80)

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
		UH_LISTTABLE
	};

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT m_nAppLook;
	BOOL m_bHiColorIcons;
	BOOL m_bVirtualMode{ FALSE };

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	int GetOpenDocumentCount() const;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDatabaseExplorerApp theApp;

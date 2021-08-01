#if !defined(AFX_DATASOURCEDLG_H__40646A61_2741_4177_A31A_A594D1F08709__INCLUDED_)
#define AFX_DATASOURCEDLG_H__40646A61_2741_4177_A31A_A594D1F08709__INCLUDED_

#include "EditExt.h"
#include "DatabaseExplorerDoc.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataSourceDlg.h : header file
//

#define g_sAppDSNPasswordKey					_T("AlaBalaPortocala")
#define RegistryEntryDSNFormatDate				_T("DSNFormatDate")
#define RegistryEntryDSNSource					_T("DSNSource")

/////////////////////////////////////////////////////////////////////////////
// CDataSourceDlg dialog

class CDataSourceDlg : public CDialog
{
// Construction
public:
	CDataSourceDlg(CDatabaseExplorerDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

public:
	UINT m_nRSTypeInit;
	int m_nDSNSource, m_nDSModel, m_nFormatDateInit;
	CString m_sDSNInit, 
		m_sDriverInit, 
		m_sProtocolInit, 
		m_sHostInit, 
		m_sServerInit, 
		m_sServiceInit, 
		m_sUserInit, 
		m_sDatabaseInit, 
		m_sPasswordInit;

// Dialog Data
	//{{AFX_DATA(CDataSourceDlg)
	enum { IDD = IDD_DIALOG_DATASOURCE };
	CEditExt m_editDSN;
	CString	m_sDSN;
	CString	m_sDatabase;
	CString	m_sDriver;
	CString	m_sHost;
	CString	m_sPassword;
	CString	m_sProtocol;
	CString	m_sServer;
	CString	m_sService;
	CString	m_sUser;
	//}}AFX_DATA

private:
	void RestoreInitValues();
	void GetKey(HKEY hKey, LPCTSTR lpSubKey, CStringArray& saResult);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataSourceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CDatabaseExplorerDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CDataSourceDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioUserDsn();
	afx_msg void OnRadioSystemDsn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATASOURCEDLG_H__40646A61_2741_4177_A31A_A594D1F08709__INCLUDED_)

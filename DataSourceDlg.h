#if !defined(AFX_DATASOURCEDLG_H__40646A61_2741_4177_A31A_A594D1F08709__INCLUDED_)
#define AFX_DATASOURCEDLG_H__40646A61_2741_4177_A31A_A594D1F08709__INCLUDED_

#include "ComboBoxExt.h"
#include "DatabaseExplorerDoc.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataSourceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COraclePasswordHandler class

class COraclePasswordHandler
{
public:
	COraclePasswordHandler(BOOL bAdmin, const CString& sDSN, const BOOL bSetDelete);
	~COraclePasswordHandler();

	COraclePasswordHandler(const COraclePasswordHandler& rhs) = delete;
	COraclePasswordHandler& operator=(const COraclePasswordHandler& rhs) = delete;
	COraclePasswordHandler(COraclePasswordHandler&& rhs) = delete;
	COraclePasswordHandler& operator=(COraclePasswordHandler&& rhs) = delete;

public:
	BOOL IsPasswordForDelete() const { return m_bDeletePassword; }
	void SetDeletePassword(const BOOL bSetDelete) { m_bDeletePassword = bSetDelete; }

private:
	BOOL m_bAdmin{ FALSE };
	CString m_sDSN;
	BOOL m_bDeletePassword{ TRUE };
};

/////////////////////////////////////////////////////////////////////////////
// CDataSourceDlg dialog

class CDataSourceDlg : public CDialog
{
// Construction
public:
	CDataSourceDlg(CDatabaseExplorerDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataSourceDlg)
	enum { IDD = IDD_DIALOG_DATASOURCE };
	CComboBoxExt m_ComboDSN;
	//}}AFX_DATA

private:
	const CString GetKeyData(const CString& sKeyName) const;
	const DatabaseType DecodeDatabaseType(CString sData) const;
	void PopulateDSN(const BOOL bSystemDSN);
	const CString GetComboSelection();
	int GetDSNIndex(const CString& sName) const;

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

protected:
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

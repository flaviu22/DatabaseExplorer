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
// CDataSourceDlg dialog

class CDataSourceDlg : public CDialog
{
	/////////////////////////////////////////////////////////////////////////////
	// CRestoreConnectionSettings class

	class CRestoreConnectionSettings
	{
	public:
		CRestoreConnectionSettings(CDatabaseExplorerDoc* pDoc);
		~CRestoreConnectionSettings();
		CRestoreConnectionSettings(const CRestoreConnectionSettings& rhs) = delete;
		CRestoreConnectionSettings& operator=(const CRestoreConnectionSettings& rhs) = delete;
		CRestoreConnectionSettings(CRestoreConnectionSettings&& rhs) = delete;
		CRestoreConnectionSettings& operator=(CRestoreConnectionSettings&& rhs) = delete;

	public:
		void GiveUpRestoreDSNOrg() { m_bRestore = FALSE; }

	private:
		CString m_sDSN;
		BOOL m_bRestore{ TRUE };
		UINT m_nRSType{ CRecordset::dynaset };
		CDatabaseExplorerDoc* m_pDoc{ nullptr };
	};

	/////////////////////////////////////////////////////////////////////////////
	// CPasswordHandler class

	class CPasswordHandler
	{
	public:
		CPasswordHandler(BOOL bAdmin, const CString& sDSN, const BOOL bSetDelete);
		~CPasswordHandler();

		CPasswordHandler(const CPasswordHandler& rhs) = delete;
		CPasswordHandler& operator=(const CPasswordHandler& rhs) = delete;
		CPasswordHandler(CPasswordHandler&& rhs) = delete;
		CPasswordHandler& operator=(CPasswordHandler&& rhs) = delete;

	public:
		void GiveUpDeletePassword() { m_bDeletePassword = FALSE; }

	private:
		BOOL m_bAdmin{ FALSE };
		CString m_sDSN;
		BOOL m_bDeletePassword{ TRUE };
	};

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
	const CString GetComboSelection(const BOOL bUpdateData = FALSE);
	int GetDSNIndex(const CString& sName) const;
	CString GetMsSQLAuthenticationRequiredUser(const CString& sDSN) const;

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
	BOOL IsSystemDsnSelected() const;
	UINT GetSelectedRSType() const;
	void Test(CDatabaseExt* pDB, const DatabaseType DBType) const;

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

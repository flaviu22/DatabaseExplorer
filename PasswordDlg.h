#pragma once

// CPasswordDlg dialog

class CPasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(BOOL bAdmin, const CString& sDSN, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPasswordDlg();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Dialog Data
	enum { IDD = IDD_DIALOG_PASSWORD };
	CString	m_sPassword;

protected:
	HICON m_hIcon{ nullptr };
	BOOL m_bAdmin{ FALSE };
	CString m_sDSN;


protected:
	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

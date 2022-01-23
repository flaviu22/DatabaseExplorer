// PasswordDlg.cpp : implementation file
//

#include "pch.h"
#include "DatabaseExplorer.h"
#include "PasswordDlg.h"
#include "afxdialogex.h"

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Text(pDX, IDC_EDIT_PASS, m_sPass);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialogEx)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CPasswordDlg dialog

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialogEx)

CPasswordDlg::CPasswordDlg(BOOL bAdmin, const CString& sDSN, CWnd* pParent/* = nullptr*/)
	:CDialogEx(IDD_DIALOG_PASSWORD, pParent)
{
	m_bAdmin = bAdmin;
	m_sDSN = sDSN;
	m_sPass = _T("");
}

CPasswordDlg::~CPasswordDlg()
{
}

// CPasswordDlg message handlers

BOOL CPasswordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: Add extra initialization here

	m_hIcon = AfxGetApp()->LoadIcon(IDI_KEY);
	SetIcon(m_hIcon, FALSE);

	SendDlgItemMessage(IDC_EDIT_PASS, EM_SETCUEBANNER,
		(WPARAM)FALSE, // show only if the edit control has no focus
		(LPARAM)L" <<Type your password>>"); // cue banner text

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::OnOK()
{
	// TODO: Add extra validation here

	UpdateData();

	CString sPath;
	sPath.Format(_T("SOFTWARE\\ODBC\\ODBC.INI\\%s"), m_sDSN);
	CSettingsStore ss(m_bAdmin, FALSE);
	if (! ss.Open(sPath))
	{
		MessageBox(_T("The registry location could not been open !"), NULL, MB_ICONERROR);
		GetDlgItem(IDC_EDIT_PASS)->SetFocus();
		return;
	}

	if (! ss.Write(_T("Password"), m_sPass))
	{

		MessageBox(_T("The password could not been saved !"), NULL, MB_ICONERROR);
		GetDlgItem(IDC_EDIT_PASS)->SetFocus();
		return;
	}

	CDialogEx::OnOK();
}

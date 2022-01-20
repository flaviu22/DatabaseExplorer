// FileDialogEx.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	StartVisualGroup(IDS_GROUP_SEPARATOR, CString(_T("CSV S&eparator")));
	AddEditBox(IDS_EDIT_SEPARATOR, 
		AfxGetApp()->GetProfileString(_T("Settings"), _T("CSVSeparator"), _T(",")));
	EndVisualGroup();
}

BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
	//{{AFX_MSG_MAP(CFileDialogEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFileDialogEx::OnFileNameOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CString sText;
	GetEditBoxText(IDS_EDIT_SEPARATOR, sText);

	if (sText.IsEmpty())
		return TRUE;

	const CString sTextOrg = AfxGetApp()->GetProfileString(_T("Settings"), _T("CSVSeparator"), _T(","));
	if (sText != sTextOrg)
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("CSVSeparator"), sText);

	return CFileDialog::OnFileNameOK();
}

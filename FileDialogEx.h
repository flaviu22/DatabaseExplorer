#if !defined(AFX_FILEDIALOGEX_H__D315C7CE_CD47_4E87_A714_65D8F442BBF2__INCLUDED_)
#define AFX_FILEDIALOGEX_H__D315C7CE_CD47_4E87_A714_65D8F442BBF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDialogEx.h : header file
//

#define IDS_GROUP_SEPARATOR						1900
#define IDS_EDIT_SEPARATOR						1901

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx dialog

class CFileDialogEx : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDialogEx)

public:
	CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
				LPCTSTR lpszDefExt = NULL,
				LPCTSTR lpszFileName = NULL,
				DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				LPCTSTR lpszFilter = NULL,
				CWnd* pParentWnd = NULL);

protected:
	virtual BOOL OnFileNameOK();

protected:
	//{{AFX_MSG(CFileDialogEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDIALOGEX_H__D315C7CE_CD47_4E87_A714_65D8F442BBF2__INCLUDED_)

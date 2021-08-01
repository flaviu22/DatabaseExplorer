// MDIChildWndExt.h : interface of the CMDIChildWndExt class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__B8C7DCC8_2FE6_4489_972C_67E45A39C09F__INCLUDED_)
#define AFX_CHILDFRM_H__B8C7DCC8_2FE6_4489_972C_67E45A39C09F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMDIChildWndExt : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CMDIChildWndExt)
public:
	CMDIChildWndExt();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIChildWndExt)
	protected:
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMDIChildWndExt();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void DockPane(CBasePane* pBar, UINT nDockBarID = 0, LPCRECT lpRect = NULL, DWORD dwAlignment = CBRS_ALIGN_ANY);

// Generated message map functions
protected:
	//{{AFX_MSG(CMDIChildWndExt)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__B8C7DCC8_2FE6_4489_972C_67E45A39C09F__INCLUDED_)

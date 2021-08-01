#if !defined(AFX_EDITEXT_H__C2305B06_158A_4D53_AD16_7AD884F85BB0__INCLUDED_)
#define AFX_EDITEXT_H__C2305B06_158A_4D53_AD16_7AD884F85BB0__INCLUDED_

#include "EditList.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditExt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditExt window

class CEditExt : public CEdit
{
// Construction
public:
	CEditExt();
	virtual ~CEditExt();

// Attributes
public:
	enum
	{
		ONLY_CHAR		= 1,
		ONLY_UINT		= 2,
		ONLY_INT		= 3,
		ONLY_UFLOAT		= 4,
		ONLY_FLOAT		= 5
	};

// Operations
public:
	BOOL IsAlertBkg() const {return m_bAlertBkg;};
	BOOL IsAlertText() const {return m_bAlertText;};
	void SetAutoComplete(const BOOL bActive = TRUE);
	void SetFilter(UINT nFilter = 0, UINT nLimit = 0);
	void AlertBkg(const BOOL bAlert = TRUE){m_bAlertBkg = bAlert;Invalidate();};
	void AlertText(const BOOL bAlert = TRUE){m_bAlertText = bAlert;Invalidate();};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditExt)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CString GetClipboardText(void) const;
	void RemoveAll(){m_List.RemoveAll();};
	virtual int AddString(LPCTSTR lpszString){return m_List.AddString(lpszString);}
	void SetAlertColor(const COLORREF crColor);
	COLORREF GetAlertColor() const {return m_crAlert;}

protected:
	int m_nLimit;
	int m_nFilter;
	BOOL m_bAlertBkg;
	BOOL m_bAlertText;
	BOOL m_bCursorMode;
	CString m_sEditText;
	BOOL m_bAutoComplete;
	COLORREF m_crAlert;
	HBRUSH m_hBrushAlert;
	HBRUSH m_hBrushDefault;
	TCHAR m_chDecimalSymbol;
	TCHAR m_chNegationSymbol;
	virtual BOOL HandleKey(UINT nChar, BOOL bFromChild);
	virtual BOOL IsValidText(int nFilter, CString sText);

// Generated message map functions
protected:
	CEditList m_List;
	//{{AFX_MSG(CEditExt)
	afx_msg BOOL OnChange();
	afx_msg BOOL OnKillFocus();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg	LRESULT OnPaste(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateFromList(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITEXT_H__C2305B06_158A_4D53_AD16_7AD884F85BB0__INCLUDED_)

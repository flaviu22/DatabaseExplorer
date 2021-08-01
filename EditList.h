#if !defined(AFX_EDITLIST_H__AF042705_8603_4822_A2E5_69EAB82C9CFB__INCLUDED_)
#define AFX_EDITLIST_H__AF042705_8603_4822_A2E5_69EAB82C9CFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditList.h : header file
//

#define _MAX_ENTRYS_				8
#define ENAC_UPDATE					(WM_USER + 1500)
/////////////////////////////////////////////////////////////////////////////
// CEditList window

class CEditList : public CWnd
{
// Construction
public:
	CEditList();
	virtual ~CEditList();

// Attributes
public:
	CListCtrl m_List;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditList)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowAll();
	void SetProp();
	void SetScroller();
	CString GetString();
	void Init(CWnd* pWnd);
	CString GetNextString(int nChar);
	BOOL SelectItem(int nItem);
	BOOL EnsureVisible(int nItem, BOOL bWait);
	virtual void DrawItem(CDC* pDC, long lItem, long lWidth);
	virtual void SortSearchList(){SortList(m_saSearchList);}
	void RemoveAll(){m_saSearchList.RemoveAll();m_saDisplayList.RemoveAll();}
	virtual int AddString(LPCTSTR lpszString);
	virtual int FindString(int nStartAfter, LPCTSTR lpszString, BOOL bDisplayOnly = FALSE);

private:
	int m_nIDTimer;
	int m_nIDTimerInstall;

protected:
	CEdit* m_pEditParent;
	CRect m_LastSize, m_ParentRect;
	CString m_sPrefix, m_sDisplayString;
	CScrollBar m_VerticalBar, m_HorizontalBar;
	CStringArray m_saDisplayList, m_saSearchList;
	int m_nTopIndex, m_nCount, m_nItemHeight, m_nVisibleItems, m_nSelectItem;

protected:
	long ScrollBarWidth();
	void DoPaintMessageLoop();
	int HitTest(CPoint point);
	void InvalidateAndScroll();
	static int CompareAscending(const void* a, const void* b);
	static int CompareDescending(const void* a, const void* b);
	void SortList(CStringArray& saList, BOOL bDescending = FALSE);

// Generated message map functions
protected:
	//{{AFX_MSG(CEditList)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLIST_H__AF042705_8603_4822_A2E5_69EAB82C9CFB__INCLUDED_)

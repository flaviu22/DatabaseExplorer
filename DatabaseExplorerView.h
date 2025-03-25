// DatabaseExplorerView.h : interface of the CDatabaseExplorerView class
//

#pragma once

#include "ColorListView.h"
#include "DBRecord.h"

#include <vector>

class CDatabaseExplorerView : public CColorListView
{
protected: // create from serialization only
	CDatabaseExplorerView() noexcept = default;
	DECLARE_DYNCREATE(CDatabaseExplorerView)

// Attributes
public:
	CDatabaseExplorerDoc* GetDocument() const;
	BOOL GetPopulateMode() const { return m_bPopulateMode; }

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// Implementation
public:
	virtual ~CDatabaseExplorerView() = default;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void ExecuteSQL(CDatabaseExplorerDoc& doc, const CString& sSQL);
	void ExecuteSelect(CDatabaseExplorerDoc& doc, const CString& sSQL);
	void DeleteAllColumns();

private:
	BOOL m_bPopulateMode{ FALSE };
	std::vector<std::unique_ptr<CDBRecord>> m_arrRows{};

// Generated message map functions
protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditRefresh();
	afx_msg void OnEditRun();
	afx_msg LRESULT OnPostInit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnOdcachehint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg LRESULT OnIsPopulateMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRestoreQueries(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDarkMode(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DatabaseExplorerView.cpp
inline CDatabaseExplorerDoc* CDatabaseExplorerView::GetDocument() const
   { return reinterpret_cast<CDatabaseExplorerDoc*>(m_pDocument); }
#endif


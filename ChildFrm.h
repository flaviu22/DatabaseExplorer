// ChildFrm.h : interface of the CChildFrame class
//

#pragma once

#include "MDIChildWndExt.h"
#include "QueryPane.h"
#include "MessagePane.h"
#include "DatabasePane.h"

constexpr int PANEQUERYID = 22;
constexpr int PANEMESSAGEID = 32;
constexpr int PANEDATABASEID = 33;
constexpr int DoInitDatabase = 1;
constexpr int DoListTable = 2;
constexpr int UpdateFrameText = 3;

enum class Color
{
	black = RGB(0, 0, 0),
	red = RGB(250, 7, 7)
};

class CDatabaseExplorerView;

class CChildFrame : public CMDIChildWndExt
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame() noexcept;

// Attributes
protected:
	CSplitterWnd m_wndSplitter;

// Operations
public:
	CString GetQueryText();
	CString GetMessageText();
	CQueryPane* GetQueryPane() { return m_pQueryPane.get(); }
	CMessagePane* GetMessagePane() { return m_pMessagePane.get(); }
	CDatabasePane* GetDatabasePane() { return m_pDatabasePane.get(); }
	void SetMessage(const CString& sMessage, const Color& color);

protected:
	std::unique_ptr<CQueryPane> m_pQueryPane{ std::make_unique<CQueryPane>() };
	std::unique_ptr<CMessagePane> m_pMessagePane{ std::make_unique<CMessagePane>() };
	std::unique_ptr<CDatabasePane> m_pDatabasePane{ std::make_unique<CDatabasePane>() };

protected:
	void SetDockingWindowIcons(BOOL bHiColorIcons);

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	// Generated message map functions
	//{{AFX_MSG(CReportChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnViewQuery();
	afx_msg void OnViewMessage();
	afx_msg void OnViewDatabase();
	afx_msg LRESULT OnPostInit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIsPopulateMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetWordWrap(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

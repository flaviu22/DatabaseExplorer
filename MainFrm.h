// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

// Attributes
public:
	BOOL IsTabbedMDIChild() const { return NULL != GetSafeHwnd() && AreMDITabs(); }
	const CTypedPtrArray<CPtrArray, CMDIChildWnd*>* GetChildArray() const { return &m_arrChild; }

// Operations
public:

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

private:
	void OnInitData();
	BOOL IsCharsSelected(CRichEditCtrl* pRichEdit) const;

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCStatusBar     m_wndStatusBar;
	CTypedPtrArray<CPtrArray, CMDIChildWnd*> m_arrChild;
	CWnd* m_pWndFocus{ nullptr };

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnChildFrameAdded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChildFrameRemoved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChildFrameActivate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChildFrameActivated(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnEditSelectall();
	afx_msg void OnEditSelectline();
	afx_msg void OnUpdateEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnViewVirtualmode();
	afx_msg void OnUpdateViewVirtualmode(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnGetTabToolTip(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

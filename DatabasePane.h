#pragma once
#include "NonClosableDockablePane.h"
#include "PaneTreeCtrl.h"
#include <memory>

constexpr int IDC_TREE_DATABASE = 21;

class CDatabasePane : public CNonClosableDockablePane
{
	DECLARE_DYNAMIC(CDatabasePane)

public:
	CDatabasePane();
	~CDatabasePane() override;
	CPaneTreeCtrl* GetTreeCtrl() { return m_pTreeCtrl.get(); }
	CString GetSelection() const;
	CString GetDatabaseSelection() const;
	void SetItemAsDatabase(const CString& sDatabase);
	void SetDarkMode(const BOOL bSet);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void AdjustLayout();
	void InitPropList();

	// Attributes
protected:
	std::unique_ptr<CImageList> m_pIL{};
	std::unique_ptr<CPaneTreeCtrl> m_pTreeCtrl{};

private:
	HTREEITEM m_hItemSelected{ nullptr };

private:
	void SetItemAsDatabase(const HTREEITEM& hItemOld, const HTREEITEM& hItemNew);

protected:
	//{{AFX_MSG(CDatabasePane)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTvnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPostInit(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

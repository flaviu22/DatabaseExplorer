#pragma once

#include "Data.h"
#include "NonClosableDockablePane.h"
#include "PaneTreeCtrl.h"

#include <memory>

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
	COLORREF GetLineColor() const { return m_crLineColor; }
	COLORREF GetTextColor() const { return m_crTextColor; }
	COLORREF GetBkColor() const { return m_crBkColor; }
	void SetLineColor(const COLORREF crLineColor) { m_crLineColor = crLineColor; }
	void SetTextColor(const COLORREF crTextColor) { m_crTextColor = crTextColor; }
	void SetBkColor(const COLORREF crBkColor) { m_crBkColor = crBkColor; }

	// Attributes
protected:
	std::unique_ptr<CImageList> m_pIL{};
	std::unique_ptr<CPaneTreeCtrl> m_pTreeCtrl{};

private:
	COLORREF m_crLineColor{ g_crDummy };
	COLORREF m_crTextColor{ g_crDummy };
	COLORREF m_crBkColor{ g_crDummy };
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

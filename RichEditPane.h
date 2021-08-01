#pragma once
#include "NonClosableDockablePane.h"
#include <memory>

constexpr int IDC_RICHEDIT_QUERY = 33;
constexpr int IDC_RICHEDIT_MESSAGE = 34;

// https://stackoverflow.com/questions/7184620/how-can-i-split-a-cdockablepane

class CRichEditPane : public CNonClosableDockablePane
{
	DECLARE_DYNAMIC(CRichEditPane)

public:
	CRichEditPane();
	virtual ~CRichEditPane() override;
	CRichEditCtrl* GetRichEditCtrl() { return m_pRichEditCtrl.get(); }

public:
	CString GetText() const;

protected:
	std::unique_ptr<CRichEditCtrl> m_pRichEditCtrl;

protected:
	void AdjustLayout();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
};

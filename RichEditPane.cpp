#include "pch.h"
#include "DatabaseExplorer.h"
#include "RichEditPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CRichEditPane, CNonClosableDockablePane)

CRichEditPane::CRichEditPane()
{
}

CRichEditPane::~CRichEditPane()
{
}

BEGIN_MESSAGE_MAP(CRichEditPane, CNonClosableDockablePane)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CRichEditPane message handlers

void CRichEditPane::OnSize(UINT nType, int cx, int cy)
{
	CNonClosableDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CRichEditPane::AdjustLayout()
{
	if (NULL == GetSafeHwnd())
		return;

	CRect rectClient;
	GetClientRect(&rectClient);
	m_pRichEditCtrl->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CRichEditPane::OnClose()
{
	// TODO: Add your specialized code here and/or call the base class

	::PostMessage(m_pRichEditCtrl->GetParentOwner()->GetSafeHwnd(), WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, 0), 0);
	return;

	CNonClosableDockablePane::OnClose();
}

void CRichEditPane::OnSetFocus(CWnd* pOldWnd)
{
	CNonClosableDockablePane::OnSetFocus(pOldWnd);

	m_pRichEditCtrl->SetFocus();
}

CString CRichEditPane::GetText() const
{
	CString sText;
	m_pRichEditCtrl->GetWindowText(sText);
	return sText;
}

void CRichEditPane::SetDarkMode(const BOOL bSet)
{
	CHARFORMAT cf{};
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;  // Set color mask

	m_pRichEditCtrl->SetBackgroundColor(bSet ? FALSE : TRUE, bSet ? g_crColorDark : g_crColorWhite);
	cf.crTextColor = bSet ? GetSysColor(COLOR_WINDOW) : GetSysColor(COLOR_WINDOWTEXT); // Set desired color
	m_pRichEditCtrl->SetSel(-1, -1);
	m_pRichEditCtrl->SetDefaultCharFormat(cf);
}

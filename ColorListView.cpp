// ColorListView.cpp : implementation file
//

#include "pch.h"
#include "ColorListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CColorListView

IMPLEMENT_DYNCREATE(CColorListView, CListView)

BEGIN_MESSAGE_MAP(CColorListView, CListView)
END_MESSAGE_MAP()

// CColorListView diagnostics

#ifdef _DEBUG
void CColorListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CColorListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

// CColorListView message handlers

void CColorListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	if (!m_pHeaderCtrl->GetSafeHwnd())
	{
		HWND hWnd = reinterpret_cast<HWND>(::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0));
		m_pHeaderCtrl->SubclassWindow(hWnd);
		m_clrDefText = GetListCtrl().GetTextColor();
		m_clrDefBack = GetListCtrl().GetTextBkColor();
	}
}

BOOL CColorListView::SetTextColor(const COLORREF cr)
{
	m_clrDefText = cr;
	return GetListCtrl().SetTextColor(cr);
}

BOOL CColorListView::SetTextBkColor(const COLORREF cr)
{
	m_clrDefBack = cr;
	return GetListCtrl().SetTextBkColor(cr);
}

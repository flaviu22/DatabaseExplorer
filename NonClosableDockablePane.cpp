// NonClosableDockablePane.cpp : implementation file
//

#include "pch.h"
#include "NonClosableDockablePane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNonClosableDockablePane

IMPLEMENT_DYNAMIC(CNonClosableDockablePane, CDockablePane)

BEGIN_MESSAGE_MAP(CNonClosableDockablePane, CDockablePane)
	//{{AFX_MSG_MAP(CNonClosableDockablePane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CNonClosableDockablePane::CNonClosableDockablePane()
{
}

CNonClosableDockablePane::~CNonClosableDockablePane()
{
}

// CNonClosableDockablePane message handlers

CTabbedPane* CNonClosableDockablePane::CreateTabbedPane()
{
	CTabbedPane* pTabbedPane = CDockablePane::CreateTabbedPane();
	ASSERT_VALID(pTabbedPane);
	// remove AFX_CBRS_CLOSE flag;
	pTabbedPane->SetControlBarStyle(AFX_NON_CLOSE_DOCKING_PANE_STYLE);
	return pTabbedPane;
}

BOOL CNonClosableDockablePane::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(WM_SYSKEYDOWN == pMsg->message && VK_F4 == pMsg->wParam)
		return TRUE;

	return CDockablePane::PreTranslateMessage(pMsg);
}

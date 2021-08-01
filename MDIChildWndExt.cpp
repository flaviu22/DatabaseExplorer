// ChildFrm.cpp : implementation of the CMDIChildWndExt class
//

#include "pch.h"
#include "DatabaseExplorer.h"

#include "MDIChildWndExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWndExt

IMPLEMENT_DYNCREATE(CMDIChildWndExt, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CMDIChildWndExt, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CMDIChildWndExt)
	ON_WM_GETMINMAXINFO()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWndExt construction/destruction

CMDIChildWndExt::CMDIChildWndExt()
{
	// TODO: add member initialization code here
	
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_CHILDFRAMEADDED, 0, reinterpret_cast<LPARAM>(this));
}

CMDIChildWndExt::~CMDIChildWndExt()
{
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_CHILDFRAMEREMOVED, 0, reinterpret_cast<LPARAM>(this));
}

BOOL CMDIChildWndExt::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if(! CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWndExt diagnostics

#ifdef _DEBUG
void CMDIChildWndExt::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CMDIChildWndExt::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWndExt message handlers

void CMDIChildWndExt::ActivateFrame(int nCmdShow)
{
	// TODO: Add your specialized code here and/or call the base class

	if (-1 == nCmdShow && NULL == GetNextWindow() && NULL == GetNextWindow(GW_HWNDPREV))
		nCmdShow = SW_SHOWMAXIMIZED;

	CMDIChildWndEx::ActivateFrame(nCmdShow);
}

void CMDIChildWndExt::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default

	CMDIChildWndEx::OnGetMinMaxInfo(lpMMI);

	// [Fix for MFC 8.0 MDI Maximizing Child Window bug on Vista]
	// https://groups.google.com/forum/#!topic/microsoft.public.vc.mfc/iajCdW5DzTM
#if _MFC_VER >= 0x0800
	lpMMI->ptMaxTrackSize.x = max(lpMMI->ptMaxTrackSize.x, lpMMI->ptMaxSize.x);
	lpMMI->ptMaxTrackSize.y = max(lpMMI->ptMaxTrackSize.y, lpMMI->ptMaxSize.y);
#endif
}

void CMDIChildWndExt::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	// call the base class to let standard processing switch to
	// the top-level menu associated with this window
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if (bActivate)
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_CHILDFRAMEACTIVATED, 0, reinterpret_cast<LPARAM>(this));
}

void CMDIChildWndExt::DockPane(CBasePane* pBar, UINT nDockBarID/* = 0*/, LPCRECT lpRect/* = NULL*/, DWORD dwAlignment/* = CBRS_ALIGN_ANY*/)
{
	ASSERT_VALID(this);

	if (pBar->CanFloat() && ! CMDIChildWndEx::m_bEnableFloatingBars)
	{
		// bar can't be floating
//		pBar->m_dwControlBarStyle &= ~AFX_CBRS_FLOAT;
		DWORD dwStyle = pBar->GetControlBarStyle();
		pBar->SetControlBarStyle(dwStyle &= ~AFX_CBRS_FLOAT);
	}

	if (pBar->CanBeResized() || pBar->CanFloat())
	{
		pBar->EnableDocking(dwAlignment);
		m_dockManager.DockPane(pBar, nDockBarID);
	}
	else
	{
		AddPane(pBar, TRUE);
	}
}

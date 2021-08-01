#include "pch.h"
#include "PaneTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaneTreeCtrl

CPaneTreeCtrl::CPaneTreeCtrl()
{
}

CPaneTreeCtrl::~CPaneTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CPaneTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaneTreeCtrl message handlers

BOOL CPaneTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	const BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (NULL != pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips())
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

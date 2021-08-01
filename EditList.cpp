// EditList.cpp : implementation file
//

#include "pch.h"
#include "EditList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditList

CEditList::CEditList()
	:m_nCount(0)
	,m_nIDTimer(0)
	,m_nIDTimerInstall(10)
	,m_nTopIndex(0)
	,m_nItemHeight(16)
	,m_nSelectItem(-1)
	,m_nVisibleItems(0)
	,m_pEditParent(NULL)
{
	m_sPrefix.Empty();
	m_LastSize.SetRectEmpty();
}

CEditList::~CEditList()
{
	m_saSearchList.RemoveAll();
	m_saDisplayList.RemoveAll();
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CEditList, CWnd)
	//{{AFX_MSG_MAP(CEditList)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_KEYDOWN()
	ON_WM_NCCALCSIZE()
	ON_WM_VSCROLL()
	ON_WM_ACTIVATEAPP()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditList message handlers

void CEditList::DoPaintMessageLoop()
{
	MSG message;
	while(::PeekMessage(&message, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

void CEditList::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	// TODO: Add your message handler code here

#if(_MSC_VER > 1200)
	CWnd::OnActivateApp(bActive, dwThreadID); //vc7    FIX 1.2
#else
	CWnd::OnActivateApp(bActive, (HTASK)dwThreadID); //vc6        FIX 1.2
#endif

	ShowWindow(FALSE);
}

void CEditList::Init(CWnd* pWnd)
{
	VERIFY(m_VerticalBar.Create(WS_VISIBLE | SBS_VERT | SBS_LEFTALIGN, 
		CRect(0, 0, GetSystemMetrics(SM_CYVSCROLL), 100), this,	0));

	SetScroller();
	m_pEditParent = (CEdit*)pWnd;

	m_nCount = static_cast<int>(m_saDisplayList.GetSize());
	m_VerticalBar.SetScrollPos(0, FALSE);
	SetProp();

	CDC* pDC = GetDC();
	if(NULL != pDC)
	{
		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		CSize size = pDC->GetOutputTextExtent(_T("Hg"));
		m_nItemHeight = size.cy;
		ReleaseDC(pDC);
	}
}

void CEditList::SetScroller()
{
	CRect rcWnd,rcBar;
	GetClientRect(rcWnd);

	if(m_VerticalBar.GetSafeHwnd())
	{
		rcBar = rcWnd;
		rcBar.top = -1;
		rcBar.left = (rcWnd.Width() - GetSystemMetrics(SM_CYVSCROLL));
		rcBar.bottom -= GetSystemMetrics(SM_CYHSCROLL);
		m_VerticalBar.MoveWindow(rcBar);
		rcBar.top = rcWnd.bottom - 20;
		rcBar.bottom = rcWnd.bottom;
		m_VerticalBar.SetScrollPos(m_nTopIndex, TRUE);
	}	
}

void CEditList::SetProp()
{
	CRect rcWnd,rcBar;

	if(! m_nCount)
		return;

	CWnd::GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask =  SIF_PAGE | SIF_RANGE;
	si.nMin = 0;
	si.nMax =  m_nCount - 1;
	m_nVisibleItems = si.nPage = rcWnd.Height() / m_nItemHeight;
	si.nTrackPos = 2;
	m_VerticalBar.SetScrollRange(0, m_nCount - 1);
	m_VerticalBar.SetScrollInfo(&si);

	if(m_nVisibleItems > m_nCount - 1)
		m_VerticalBar.ShowWindow(FALSE);
	else
		m_VerticalBar.ShowWindow(TRUE);

	if(m_nTopIndex + m_nVisibleItems > m_nCount)
	{
		m_nTopIndex = m_nCount - m_nVisibleItems;
		if(m_nTopIndex < 0)
			m_nTopIndex = 0;
		m_VerticalBar.SetScrollPos(m_nTopIndex, TRUE);
	}
}

BOOL CEditList::EnsureVisible(int nItem, BOOL bWait)
{
	if(nItem > m_nTopIndex && nItem < m_nTopIndex + m_nVisibleItems)
		return FALSE; // is visible

	if(nItem > m_nTopIndex)	// scroll down
	{
		long length = nItem;
		for(int i = m_nTopIndex;i < length;++i)
		{
			if(i >= m_nCount - m_nVisibleItems)
				break;
			if(i >= m_nCount - m_nVisibleItems || i + m_nVisibleItems > nItem)
				break;
			m_nTopIndex++;
			if(bWait)
			{
				InvalidateAndScroll();
				Sleep(10);
				DoPaintMessageLoop();
			}
		}
		InvalidateAndScroll();
		return TRUE;
	}

	if(nItem < m_nTopIndex)	// scroll up
	{
		while(nItem < m_nTopIndex)
		{
			if(m_nTopIndex > 0)
				m_nTopIndex--;
			else
				break;
			if(bWait)
			{
				InvalidateAndScroll();
				Sleep(10);
				DoPaintMessageLoop();
			}
		}
		InvalidateAndScroll();
		return TRUE;
	}

	return FALSE;
}

void CEditList::InvalidateAndScroll()
{
	m_VerticalBar.SetScrollPos(m_nTopIndex, TRUE);
	Invalidate();
	DoPaintMessageLoop();
}

BOOL CEditList::SelectItem(int nItem)
{
	if(nItem > m_nCount)
		return FALSE;

	if(nItem == -1)
	{
		EnsureVisible(m_nSelectItem, FALSE);
		Invalidate();
		return FALSE;
	}

	m_nSelectItem = nItem;
	if(! EnsureVisible(nItem, TRUE))
		Invalidate();

	return TRUE;
}

int CEditList::HitTest(CPoint point)
{
	CRect rcItem, rcWnd;
	GetClientRect(rcWnd);
	long y, lWidth = rcWnd.Width() - ScrollBarWidth();

	for(int i = m_nTopIndex;i < m_nCount;++i)
	{
		y = i - m_nTopIndex;
		rcItem.SetRect(2, y * m_nItemHeight, lWidth, (y + 1) * m_nItemHeight);
		if(PtInRect(&rcItem, point))
			return m_nSelectItem = y + m_nTopIndex;
	}

	return -1;
}

long CEditList::ScrollBarWidth()
{
	if(m_VerticalBar.IsWindowVisible())
		return GetSystemMetrics(SM_CYVSCROLL);

	return 0;
}

int CEditList::AddString(LPCTSTR lpszString)
{
	if(! lpszString || ! *lpszString)	// rajesh subramanian
		return -1;
//	if(! lpszString || ! lpszString[0])	// rob grainger
//		return -1;

	int nCount = static_cast<int>(m_saSearchList.GetSize());
	for(int i = 0;i < nCount;++i)
	{
		if(0 == m_saSearchList.GetAt(i).Compare(lpszString))
			return -1;
	}

	return static_cast<int>(m_saSearchList.Add(lpszString));
}

int CEditList::FindString(int nStartAfter, LPCTSTR lpszString, BOOL bDisplayOnly)
{
	long lActCount = static_cast<long>(m_saDisplayList.GetSize());

	if(! bDisplayOnly)
	{
		CString sTemp1, sTemp2 = lpszString;
		if(! m_pEditParent)
		{
			ShowWindow(FALSE);
			return -1;
		}
		if(nStartAfter > m_saSearchList.GetSize())
		{
			ShowWindow(FALSE);
			return -1;
		}
		if(sTemp2.IsEmpty())
		{
			ShowWindow(FALSE);
			return -1;
		}
		m_saDisplayList.RemoveAll();
		sTemp2.MakeUpper();
		for(int i = nStartAfter + 1;i < m_saSearchList.GetSize();++i)
		{
			if(! m_sPrefix.IsEmpty())
				sTemp1 = m_sPrefix;
			else
				sTemp1.Empty();
			sTemp1.AppendFormat(_T("%s"), m_saSearchList.GetAt(i));
			sTemp1.MakeUpper();
			if(0 == sTemp1.Find(sTemp2))
				m_saDisplayList.Add(m_saSearchList.GetAt(i));
		}
	}

	m_nCount = static_cast<int>(m_saDisplayList.GetSize());

	if(m_nCount < 1)
		ShowWindow(FALSE);
	else
	{
		CRect rcWnd;
		int nHeight, nWidth;
		m_pEditParent->GetWindowRect(rcWnd);
		SetScroller();
		SetProp();
		ShowWindow(TRUE);
		Invalidate();
		nHeight = m_nCount * m_nItemHeight + (GetSystemMetrics(SM_CYBORDER) * 2);
		if(m_nCount > _MAX_ENTRYS_)
			nHeight = _MAX_ENTRYS_ * m_nItemHeight + (GetSystemMetrics(SM_CYBORDER) * 2);
		if(m_LastSize.IsRectEmpty())
			SetWindowPos(&CWnd::wndTopMost, rcWnd.left, rcWnd.top + rcWnd.Height(), rcWnd.Width(), nHeight, 0);
		else
		{
			nWidth = m_LastSize.Width();
			nHeight = m_LastSize.Height();
			rcWnd.top += rcWnd.Height();
			rcWnd.right = rcWnd.left + nWidth;
			rcWnd.bottom = rcWnd.top + nHeight;
			SetWindowPos(&CWnd::wndTopMost, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), 0);
		}
		if(lActCount != m_saDisplayList.GetSize())
			m_nSelectItem = -1;
		SortList(m_saDisplayList);
	}

	return 1;
}

CString CEditList::GetString()
{
	int i = static_cast<int>(m_saDisplayList.GetSize());

	if(! i)
		return _T("");
	if(i <= m_nSelectItem || m_nSelectItem == -1)
		i = 0;
	else
		i = m_nSelectItem;

	return m_saDisplayList.GetAt(i);
}

CString CEditList::GetNextString(int nChar)
{
	switch(nChar)
	{
	case VK_DOWN:
		m_nSelectItem++;
		break;
	case VK_UP:
		m_nSelectItem--;
		break;
	case VK_PRIOR:
		m_nSelectItem -= m_nVisibleItems;
		if(m_nSelectItem < 0)
			m_nSelectItem = 0;
		break;
	case VK_NEXT:
		m_nSelectItem += m_nVisibleItems;
		if(m_nSelectItem >= m_nCount - 1)
			m_nSelectItem = m_nCount - 1;
		break;
	case VK_HOME:
		m_nSelectItem = 0;
		break;
	case VK_END:
		m_nSelectItem = m_nCount - 1;
		break;
	}

	if(m_nSelectItem < 0)
		m_nSelectItem = m_nCount - 1;
	if(m_nSelectItem >= m_nCount)
		m_nSelectItem = 0;
	if(EnsureVisible(m_nSelectItem, (m_nCount > 50) ? FALSE : TRUE))
		InvalidateAndScroll();

	return GetString();
}

void CEditList::DrawItem(CDC* pDC, long lItem, long lWidth)
{
	long y = lItem - (long)m_nTopIndex;
	CRect rcLabel(2, y * (long)m_nItemHeight, lWidth, (y + 1) * (long)m_nItemHeight);

	pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

	if(lItem == (long)m_nSelectItem)
	{
		rcLabel.left = 0;
		pDC->FillSolidRect(rcLabel, ::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		rcLabel.left = 2;
	}

	if(m_sPrefix)
		m_sDisplayString = m_sPrefix + m_saDisplayList.GetAt((int)lItem);
	else
		m_sDisplayString = m_saDisplayList.GetAt((int)lItem);

	pDC->DrawText(m_sDisplayString, -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
}

void CEditList::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CWnd::OnPaint() for painting messages

	CRect rcWnd,rect1, rect2;
	CDC MemDC, *pDC = NULL;
	CBitmap bitmap, *pOldBitmap = NULL;

	GetClientRect(rect2);
	rcWnd = rect1 = rect2;

	rect2.left = rect2.right - GetSystemMetrics(SM_CXHSCROLL);
	rect2.top = rect2.bottom - GetSystemMetrics(SM_CYVSCROLL);

	rect1.right -= ScrollBarWidth();

	MemDC.CreateCompatibleDC(&dc);

	bitmap.CreateCompatibleBitmap(&dc, rect1.Width(), rect1.Height());
	pOldBitmap = MemDC.SelectObject(&bitmap);

	MemDC.SetWindowOrg(rect1.left, rect1.top);

	long lWidth = rcWnd.Width() - ScrollBarWidth();

	MemDC.FillSolidRect(rcWnd, ::GetSysColor(COLOR_WINDOW));
	MemDC.SelectObject(GetStockObject(DEFAULT_GUI_FONT));
	MemDC.SetBkMode(TRANSPARENT);

	for(int i = m_nTopIndex;i < m_nCount;++i)
		DrawItem(&MemDC, i, lWidth);

	CPen m_Pen1(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOW));
	CPen m_Pen2(PS_SOLID, 1, ::GetSysColor(COLOR_BTNFACE));
	CPen m_Pen3(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));

	pDC = &dc;

	if(m_VerticalBar.IsWindowVisible())
		dc.FillSolidRect(rect2,::GetSysColor(COLOR_BTNFACE));
	else
		pDC = &MemDC;

	CPen *pOldPen = (CPen*)pDC->SelectObject(&m_Pen1);
	int a = 1, nBottom;

	lWidth = GetSystemMetrics(SM_CXHSCROLL);
	nBottom = (rcWnd.bottom - GetSystemMetrics(SM_CXHSCROLL)) - 1;

	//gripper
	for(int j = 0;j < 20;++j, ++a)
	{
		if(1 == a)
			pDC->SelectObject(&m_Pen1);
		if(2 == a)
			pDC->SelectObject(&m_Pen2);
		if(3 == a)
			pDC->SelectObject(&m_Pen3);
		if(a > 3)
			a = 0;

		pDC->MoveTo(rect2.left + j - 1, rcWnd.bottom);
		pDC->LineTo(rect2.left + j + lWidth, nBottom);
	}

	dc.BitBlt(rect1.left, rect1.top, rect1.Width(), rect1.Height(), 
		&MemDC, rect1.left, rect1.top, SRCCOPY);

	pDC->SelectObject(pOldPen);
	MemDC.SelectObject(pOldBitmap);
}

void CEditList::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	SetScroller();
	SetProp();

	if(! m_LastSize.IsRectEmpty())
		GetWindowRect(m_LastSize);
}

BOOL CEditList::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	return FALSE;
//	return CWnd::OnEraseBkgnd(pDC);
}

void CEditList::OnNcPaint() 
{
	// TODO: Add your message handler code here

	// Do not call CWnd::OnNcPaint() for painting messages

	CWindowDC dc(this);
	CRect rectClient, rectWindow, rcWnd;
	GetClientRect(rectClient);
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-(rectWindow.left), -(rectWindow.top));
	dc.ExcludeClipRect(rectClient);
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.FillSolidRect(rectWindow, ::GetSysColor(COLOR_WINDOWTEXT));
}

void CEditList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

	if(VK_ESCAPE == nChar)
		ShowWindow(FALSE);
}

void CEditList::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	::InflateRect(lpncsp->rgrc, -GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER));
}

void CEditList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	long m_oldlTopIndex = m_nTopIndex;

	switch(nSBCode)
	{
	case SB_ENDSCROLL:
		break;
	case SB_PAGEUP:
		m_nTopIndex -= m_nVisibleItems;
		if(m_nTopIndex < 0)
			m_nTopIndex = 0;
		break;
	case SB_PAGEDOWN:
		m_nTopIndex += m_nVisibleItems;
		if(m_nTopIndex >= m_nCount - m_nVisibleItems)
			m_nTopIndex = m_nCount - m_nVisibleItems;
		break;
	case SB_LINEUP:
		m_nTopIndex--;
		if(m_nTopIndex < 0)
			m_nTopIndex = 0;
		break;
	case SB_LINEDOWN:
		m_nTopIndex++;
		if(m_nTopIndex >= m_nCount - m_nVisibleItems)
			m_nTopIndex = m_nCount - m_nVisibleItems;
		break;
	case SB_THUMBTRACK:
		m_nTopIndex = nPos;
		break;
	}

	m_VerticalBar.SetScrollPos(m_nTopIndex, TRUE);

	if(m_oldlTopIndex != m_nTopIndex)
		Invalidate();
}

LRESULT CEditList::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CRect rectClient;
	GetWindowRect(rectClient);

	rectClient.left = rectClient.right - GetSystemMetrics(SM_CYVSCROLL);
	rectClient.top = rectClient.bottom - GetSystemMetrics(SM_CXVSCROLL);

	if(rectClient.PtInRect(point))
		return (LRESULT)HTBOTTOMRIGHT;
	else
		return (LRESULT)HTCLIENT;

//	return CWnd::OnNcHitTest(point);
}

void CEditList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonDown(nFlags, point);

	int nSel = HitTest(point);

	if(nSel >= 0)
	{
		if(! EnsureVisible(nSel, TRUE))
			Invalidate();
		m_nSelectItem = nSel;
		m_pEditParent->SendMessage(ENAC_UPDATE, WM_KEYDOWN, GetDlgCtrlID());
		DoPaintMessageLoop();
		Sleep(150);
		ShowWindow(FALSE);
	}
	else
	{
		CRect rc;
		GetClientRect(rc);
		if(! rc.PtInRect(point))
			ShowWindow(FALSE);
	}
}

void CEditList::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonDown(nFlags, point);

	ShowWindow(FALSE);
}

BOOL CEditList::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default

	CPoint ptCursor;
	CRect rectClient;

	GetWindowRect(rectClient);
	ScreenToClient(&rectClient);

	rectClient.left = rectClient.right - GetSystemMetrics(SM_CYVSCROLL);
	rectClient.top = rectClient.bottom - GetSystemMetrics(SM_CXVSCROLL);

	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	if(rectClient.PtInRect(ptCursor)) // Magnifying cursor
	{
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	return TRUE;
}

void CEditList::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if(bShow)
	{
		m_nIDTimer = SetTimer(m_nIDTimerInstall, 200, NULL);
		m_pEditParent->GetParent()->GetWindowRect(m_ParentRect);
	}
	else
	{
		if(m_nIDTimer)
			KillTimer(m_nIDTimerInstall);
		m_nIDTimer = 0;
		m_nSelectItem = -1;
		m_nTopIndex = 0;
	}

	CWnd::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CEditList::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if(OnNcHitTest(point) == HTBOTTOMRIGHT)
		GetWindowRect(m_LastSize);

	CWnd::OnNcLButtonDown(nHitTest, point);
}

void CEditList::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseMove(nFlags, point);

	int nSel = HitTest(point);
	if(nSel >= 0)
		Invalidate();
}

void CEditList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnTimer(nIDEvent);

	CRect rectParent;
	m_pEditParent->GetParent()->GetWindowRect(rectParent);
	if(! rectParent.EqualRect(m_ParentRect))
		ShowWindow(FALSE);
}

void CEditList::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default

	if(NULL != GetSafeHwnd())
	{
		long m_lMinY1 = GetSystemMetrics(SM_CYHSCROLL) * 2 + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXHTHUMB), 
			m_lMinY2 = m_nCount * m_nItemHeight + (GetSystemMetrics(SM_CYBORDER) * 2);

		if(m_nVisibleItems > m_nCount - 1 &&  m_lMinY2 < m_lMinY1)
			lpMMI->ptMinTrackSize.y = m_lMinY2;
		else
			lpMMI->ptMinTrackSize.y = m_lMinY1;

		lpMMI->ptMinTrackSize.x = GetSystemMetrics(SM_CXHSCROLL) * 4;

		if(m_pEditParent != NULL) 
		{
			RECT rect;
			m_pEditParent->GetWindowRect(&rect);
			lpMMI->ptMinTrackSize.x = rect.right - rect.left;
		}
	}
	else
	{
		CWnd::OnGetMinMaxInfo(lpMMI);
	}
}

void CEditList::SortList(CStringArray& saList, BOOL bDescending/* = FALSE*/)
{
	int nArraySize = static_cast<int>(saList.GetSize());
	if(nArraySize < 1)
		return;

	int nCStringSize = sizeof(CString*);
	void* pArrayStart = (void*)&saList[0];

	if(bDescending)
		qsort(pArrayStart, nArraySize, nCStringSize, CompareDescending);
	else
		qsort(pArrayStart, nArraySize, nCStringSize, CompareAscending);
}

int CEditList::CompareAscending(const void* a, const void* b)
{
	CString* pA = (CString*)a;
	CString* pB = (CString*)b;

	return pA->CompareNoCase(*pB);
}

int CEditList::CompareDescending(const void* a, const void* b)
{
	CString* pA = (CString*)a;
	CString* pB = (CString*)b;

	return -1 * (pA->CompareNoCase(*pB));
}

void CEditList::ShowAll()
{
	if(m_saSearchList.GetSize() < 1)
		return;
	m_saDisplayList.RemoveAll();

	for(int i = 0;i < m_saSearchList.GetSize();++i)
		m_saDisplayList.Add(m_saSearchList.GetAt(i));

	CRect rcWnd;
	int nHeight, nWidth;
	m_pEditParent->GetWindowRect(rcWnd);
	ShowWindow(TRUE);
	Invalidate();
	m_nCount = static_cast<int>(m_saDisplayList.GetSize());
	nHeight = m_nCount * m_nItemHeight + (GetSystemMetrics(SM_CYBORDER) * 2);
	if(m_nCount > _MAX_ENTRYS_)
		nHeight = _MAX_ENTRYS_ * m_nItemHeight + (GetSystemMetrics(SM_CYBORDER) * 2);
	if(m_LastSize.IsRectEmpty())
		SetWindowPos(&CWnd::wndTopMost, rcWnd.left, rcWnd.top + rcWnd.Height(), rcWnd.Width(), nHeight, 0);
	else
	{
		nWidth = m_LastSize.Width();
		nHeight = m_LastSize.Height();
		rcWnd.top += rcWnd.Height();
		rcWnd.right = rcWnd.left + nWidth;
		rcWnd.bottom = rcWnd.top + nHeight;
		SetWindowPos(&CWnd::wndTopMost, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), 0);
		SetProp();
	}

	SortList(m_saDisplayList);
}

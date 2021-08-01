// EditExt.cpp : implementation file
//

#include "pch.h"
#include "EditExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditExt

CEditExt::CEditExt()
	:m_nLimit(0)
	,m_nFilter(0)
	,m_bAlertBkg(FALSE)
	,m_bAlertText(FALSE)
	,m_bCursorMode(FALSE)
	,m_bAutoComplete(FALSE)
	,m_chDecimalSymbol(_T('.'))
	,m_chNegationSymbol(_T('-'))
{
	m_crAlert = RGB(255, 0, 0);
	m_hBrushAlert =	CreateSolidBrush(m_crAlert);
	m_hBrushDefault = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}

CEditExt::~CEditExt()
{
	DeleteObject(m_hBrushAlert);
	DeleteObject(m_hBrushDefault);
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CEditExt, CEdit)
	//{{AFX_MSG_MAP(CEditExt)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChange)
	ON_CONTROL_REFLECT_EX(EN_KILLFOCUS, OnKillFocus)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PASTE, OnPaste)
	ON_MESSAGE(ENAC_UPDATE, OnUpdateFromList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditExt message handlers

void CEditExt::SetAutoComplete(BOOL bActive/* = TRUE*/)
{
	if(m_nFilter > 0 || m_nLimit > 0)
		m_bAutoComplete = FALSE;
	else
		m_bAutoComplete = bActive;
}

void CEditExt::SetFilter(UINT nFilter/* = 0*/, UINT nLimit/* = 0*/)
{
	if(nFilter != ONLY_CHAR && nFilter != ONLY_UINT && nFilter != ONLY_INT && nFilter != ONLY_UFLOAT && nFilter != ONLY_FLOAT)
		nFilter = 0;
	m_nFilter = nFilter;
	m_nLimit = nLimit;
	if(m_nFilter > 0 || m_nLimit > 0)
		m_bAutoComplete = FALSE;
}

CString CEditExt::GetClipboardText() const
{
	CString	sClipoardText;
	if(::IsClipboardFormatAvailable(CF_TEXT))
	{
		if(::OpenClipboard(m_hWnd))
		{
			HANDLE hClipoardData = NULL;
			if((hClipoardData = ::GetClipboardData(CF_TEXT)) != NULL)
			{
				LPTSTR lpClipoardText = (LPTSTR)::GlobalLock(hClipoardData);
				if(lpClipoardText)
				{
					sClipoardText = lpClipoardText;
					::GlobalUnlock(hClipoardData);
				}
			}
			VERIFY(::CloseClipboard());
		}
	}

	return sClipoardText;
}

LRESULT CEditExt::OnUpdateFromList(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);

	if(WM_KEYDOWN == wParam)
		HandleKey(VK_DOWN, TRUE);

	return 0;
}

BOOL CEditExt::HandleKey(UINT nChar, BOOL bFromChild)
{
	if(nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		m_List.ShowWindow(FALSE);
		return TRUE;
	}

	if(nChar == VK_DOWN || nChar == VK_UP || nChar == VK_PRIOR || nChar == VK_NEXT || nChar == VK_HOME || nChar == VK_END)
	{
		if(m_List.IsWindowVisible())
		{
			int nPos;
			m_bCursorMode = TRUE;
			if(! bFromChild)
				m_sEditText = m_List.GetNextString(nChar);
			else
				m_sEditText = m_List.GetString();
			m_List.SelectItem(-1);
			SetWindowText(m_sEditText);
			nPos = m_sEditText.GetLength();
			((CEdit*)this)->SetSel(nPos, nPos, TRUE);
			((CEdit*)this)->SetModify(TRUE);
			GetParent()->SendMessage(ENAC_UPDATE, WM_KEYDOWN, GetDlgCtrlID());
			m_bCursorMode = FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CEditExt::OnKillFocus() 
{
	if(m_List.GetSafeHwnd())
		m_List.ShowWindow(FALSE);

	return (BOOL)GetParent()->SendMessage(ENAC_UPDATE, WM_KILLFOCUS, GetDlgCtrlID());
}

void CEditExt::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	if(! HandleKey(nChar, FALSE))
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

HBRUSH CEditExt::CtlColor(CDC* pDC, UINT nCtlColor)
{
	HBRUSH hbr = m_hBrushDefault;

	// TODO: Change any attributes of the DC here

	if(! m_bAlertText)
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	else
		pDC->SetTextColor(m_crAlert);
	if(! m_bAlertBkg)
		pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
	else
	{
		pDC->SetBkColor(m_crAlert);
		hbr = m_hBrushAlert;
	}
	pDC->SetBkMode(TRANSPARENT);

	// TODO: Return a different brush if the default is not desired

	if(IsWindowEnabled() && ! (ES_READONLY & GetStyle()))
		return hbr;
	else
		return NULL;
}

BOOL CEditExt::OnChange() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEdit::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here

	if(! m_bAutoComplete)
		return FALSE;

	GetWindowText(m_sEditText);
	if(! m_bCursorMode)
		m_List.FindString(-1, m_sEditText);
	GetParent()->SendMessage(ENAC_UPDATE, EN_UPDATE, GetDlgCtrlID());

	return FALSE;
}

void CEditExt::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CString sEditText;
	GetWindowText(sEditText);
	if(0 == m_nFilter && 0 == m_nLimit && sEditText.IsEmpty() && m_bAutoComplete)
		m_List.ShowAll();

	CEdit::OnLButtonDblClk(nFlags, point);
}

BOOL CEditExt::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(WM_KEYDOWN == pMsg->message && m_List.IsWindowVisible() &&
		(VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
	{
		if(HandleKey(static_cast<UINT>(pMsg->wParam), FALSE))
			return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CEditExt::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	CEdit::PreSubclassWindow();

	CRect rcWnd;
	CString sTemp;
	int nResult = 0;
	GetWindowRect(rcWnd);
	CString szClassName = AfxRegisterWndClass(CS_CLASSDC | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_WINDOW), 0);
	VERIFY(m_List.CreateEx(WS_EX_TOOLWINDOW, szClassName, NULL, 
		WS_CHILD | WS_CLIPSIBLINGS | WS_OVERLAPPED, 
		CRect(rcWnd.left, rcWnd.top + 20, rcWnd.left + 200, rcWnd.top + 200), GetDesktopWindow(), 0x3E8, NULL));
	nResult = ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, sTemp.GetBufferSetLength(2), 2);
	if(nResult)
		m_chDecimalSymbol = sTemp[0];
	sTemp.ReleaseBuffer();
	nResult = ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SNEGATIVESIGN, sTemp.GetBufferSetLength(2), 2);
	if(nResult)
		m_chNegationSymbol = sTemp[0];
	sTemp.ReleaseBuffer();
	m_List.Init(this);
}

void CEditExt::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	CString sEditText;
	int nIndex = 0, nStart = 0, nContor = 0;

	GetWindowText(sEditText);
	DWORD dwGetSel = GetSel();
	WORD wStart = LOWORD(dwGetSel);
	WORD wEnd	= HIWORD(dwGetSel);

	if(nChar > 0x20 && 0 == (wEnd - wStart) && m_nLimit > 0 && sEditText.GetLength() > m_nLimit - 1)
		return;

	switch(m_nFilter)
	{
	case ONLY_CHAR:
		if(nChar <= 0x20 || IsCharAlpha((TCHAR)nChar))
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		break;
	case ONLY_UINT:
		if(nChar < 0x20 || isdigit((TCHAR)nChar))
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		break;
	case ONLY_INT:
		if(nChar < 0x20 || isdigit((TCHAR)nChar) || nChar == (UINT)m_chNegationSymbol)
		{
			nIndex = sEditText.Find(m_chNegationSymbol);
			if(nIndex != -1 && wEnd <= nIndex)
				nContor = 2;
			if(nChar == (UINT)m_chNegationSymbol && wStart != 0 && wEnd != 0)
				nContor = 2;
			if(nContor < 1)
				CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
		break;
	case ONLY_UFLOAT:
		if(nChar < 0x20 || isdigit((TCHAR)nChar) || nChar == (UINT)m_chDecimalSymbol)
		{
			if(nChar == (UINT)m_chDecimalSymbol)
			{
				GetWindowText(sEditText);
				while(nIndex > -1)
				{
					nIndex = sEditText.Find(m_chDecimalSymbol, nStart);
					if(nIndex > -1)
					{
						nStart = nIndex + 1;
						nContor++;
					}
				}
			}
			if(nContor < 1)
				CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
		break;
	case ONLY_FLOAT:
		if(nChar < 0x20 || isdigit((TCHAR)nChar) || nChar == (UINT)m_chNegationSymbol || nChar == (UINT)m_chDecimalSymbol)
		{
			nIndex = sEditText.Find(m_chNegationSymbol);
			if(nIndex != -1 && wEnd <= nIndex)
				nContor = 2;
			if(nChar == (UINT)m_chNegationSymbol && wStart != 0 && wEnd != 0)
				nContor = 2;
			if(nChar == (UINT)m_chDecimalSymbol)
			{
				nIndex = 0;
				while(nIndex > -1)
				{
					nIndex = sEditText.Find(m_chDecimalSymbol, nStart);
					if(nIndex > -1)
					{
						nStart = nIndex + 1;
						nContor++;
					}
				}
			}
			if(nContor < 1)
				CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
		break;
	default:
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

LRESULT CEditExt::OnPaste(WPARAM wParam, LPARAM lParam)
{
	UNUSED_ALWAYS(wParam);
	UNUSED_ALWAYS(lParam);

	CString sBegin, sEnd, sEditText, sClipboardText = GetClipboardText();
	if(sClipboardText.IsEmpty())
		return 0;
	GetWindowText(sEditText);
	DWORD dwGetSel = GetSel();
	WORD wStart = LOWORD(dwGetSel);
	WORD wEnd	= HIWORD(dwGetSel);
	if(! IsValidText(m_nFilter, sEditText.Left(wStart) + sClipboardText + sEditText.Mid(wEnd)))
		return 0;
	sEditText = sEditText.Left(wStart) + sClipboardText + sEditText.Mid(wEnd);
	SetWindowText(sEditText);
	wEnd = (WORD)sEditText.Left(wStart).GetLength() + (WORD)sClipboardText.GetLength();
	SetSel(wEnd, wEnd);

	return 0;
}

BOOL CEditExt::IsValidText(int nFilter, CString sText)
{
	BOOL bRet = TRUE;
	int i, nContor = 0, nLength = sText.GetLength();

	if(m_nLimit > 0 && nLength > m_nLimit)
		return FALSE;

	switch(nFilter)
	{
	case ONLY_CHAR:
		for(i = 0;i < nLength;++i)
		{
			if(! IsCharAlpha(sText.GetAt(i)))
			{
				bRet = FALSE;
				break;
			}
		}
		break;
	case ONLY_UINT:
		for(i = 0;i < nLength;++i)
		{
			if(! isdigit(sText.GetAt(i)))
			{
				bRet = FALSE;
				break;
			}
		}
		break;
	case ONLY_INT:
		for(i = 0;i < nLength;++i)
		{
			if(i == 0)
			{
				if(! isdigit(sText.GetAt(i)) && m_chNegationSymbol != sText.GetAt(i))
				{
					bRet = FALSE;
					break;
				}
			}
			else
			{
				if(! isdigit(sText.GetAt(i)))
				{
					bRet = FALSE;
					break;
				}
			}
		}
		break;
	case ONLY_UFLOAT:
		for(i = 0;i < nLength;++i)
		{
			if(! isdigit(sText.GetAt(i)))
			{
				if(m_chDecimalSymbol == sText.GetAt(i))
				{
					nContor++;
					if(nContor > 1)
					{
						bRet = FALSE;
						break;
					}
				}
				else
				{
					bRet = FALSE;
					break;
				}
			}
		}
		break;
	case ONLY_FLOAT:
		for(i = 0;i < nLength;++i)
		{
			if(i == 0)
			{
				if(! isdigit(sText.GetAt(i)) && m_chNegationSymbol != sText.GetAt(i) && m_chDecimalSymbol != sText.GetAt(i))
				{
					bRet = FALSE;
					break;
				}
				if(! isdigit(sText.GetAt(i)) && m_chDecimalSymbol == sText.GetAt(i))
					nContor++;
			}
			else
			{
				if(! isdigit(sText.GetAt(i)))
				{
					if(m_chDecimalSymbol == sText.GetAt(i))
					{
						nContor++;
						if(nContor > 1)
						{
							bRet = FALSE;
							break;
						}
					}
					else
					{
						bRet = FALSE;
						break;
					}
				}
			}
		}
		break;
	}

	return bRet;
}

void CEditExt::SetAlertColor(const COLORREF crColor)
{
	m_crAlert = crColor;
	DeleteObject(m_hBrushAlert);
	m_hBrushAlert =	CreateSolidBrush(m_crAlert);
}

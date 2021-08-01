#include "pch.h"
#include "MessagePane.h"

BEGIN_MESSAGE_MAP(CMessagePane, CRichEditPane)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CMessagePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (-1 == CRichEditPane::OnCreate(lpCreateStruct))
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	constexpr DWORD dwStyle = WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN;
	m_pRichEditCtrl = std::make_unique<CRichEditCtrl>();
	if (! m_pRichEditCtrl->Create(dwStyle, rectDummy, this, IDC_RICHEDIT_MESSAGE))
	{
		TRACE(_T("Failed to create rich edit control\n"));
		return -1;      // fail to create
	}

	AdjustLayout();

	m_pRichEditCtrl->PostMessage(EM_SETEVENTMASK, 0, ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS);
	m_pRichEditCtrl->PostMessage(EM_SETREADONLY, TRUE, 0);

	return 0;
}

void CMessagePane::AppendToLogAndScroll(const CString& str, const COLORREF& color)
{
	CHARFORMAT cf;
	long nVisible = 0;
	long nInsertionPoint = 0;

	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0; // To disable CFE_AUTOCOLOR

	cf.crTextColor = color;

	// Set insertion point to end of text
	nInsertionPoint = m_pRichEditCtrl->GetWindowTextLength();
	m_pRichEditCtrl->SetSel(nInsertionPoint, -1);

	// Set the character format
	m_pRichEditCtrl->SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing
	// selected, this will simply insert
	// the string at the current caret position.
	m_pRichEditCtrl->ReplaceSel(str + _T("\n"));

	// Get number of currently visible lines or maximum number of visible lines
	// (We must call GetNumVisibleLines() before the first call to LineScroll()!)
	nVisible = GetNumVisibleLines();

	// Now this is the fix of CRichEditCtrl's abnormal behaviour when used
	// in an application not based on dialogs. Checking the focus prevents
	// us from scrolling when the CRichEditCtrl does so automatically,
	// even though ES_AUTOxSCROLL style is NOT set.
	if (m_pRichEditCtrl.get() != m_pRichEditCtrl->GetFocus())
	{
		m_pRichEditCtrl->LineScroll(INT_MAX);
		m_pRichEditCtrl->LineScroll(1 - nVisible);
	}
}

int CMessagePane::GetNumVisibleLines()
{
	CRect rect;
	long nFirstChar, nLastChar;
	long nFirstLine, nLastLine;

	// Get client rect of rich edit control
	m_pRichEditCtrl->GetClientRect(rect);

	// Get character index close to upper left corner
	nFirstChar = m_pRichEditCtrl->CharFromPos(CPoint(0, 0));

	// Get character index close to lower right corner
	nLastChar = m_pRichEditCtrl->CharFromPos(CPoint(rect.right, rect.bottom));
	if (nLastChar < 0)
	{
		nLastChar = m_pRichEditCtrl->GetTextLength();
	}

	// Convert to lines
	nFirstLine = m_pRichEditCtrl->LineFromChar(nFirstChar);
	nLastLine = m_pRichEditCtrl->LineFromChar(nLastChar);

	return nLastLine - nFirstLine;
}

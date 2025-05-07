#include "pch.h"
#include "QueryPane.h"
#include "DatabaseExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CQueryPane, CRichEditPane)
	//{{AFX_MSG_MAP(CQueryPane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_RICHEDIT_QUERY, &CQueryPane::OnEnChangeRichEditCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CQueryPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (-1 == CRichEditPane::OnCreate(lpCreateStruct))
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	constexpr DWORD dwStyle = WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN;
	m_pRichEditCtrl = std::make_unique<CRichEditCtrl>();
	if (! m_pRichEditCtrl->Create(dwStyle, rectDummy, this, IDC_RICHEDIT_QUERY))
	{
		TRACE(_T("Failed to create rich edit control\n"));
		return -1;      // fail to create
	}

	AdjustLayout();

	::PostMessage(m_pRichEditCtrl->GetSafeHwnd(), 
		EM_SETEVENTMASK, 0, ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS | ENM_CHANGE);

	const DWORD dwNominator = static_cast<DWORD>(AfxGetApp()->GetProfileInt(_T("Settings"), 
		_T("QueryPaneZoomNominator"), 120));
	const DWORD dwDenominator = static_cast<DWORD>(AfxGetApp()->GetProfileInt(_T("Settings"), 
		_T("QueryPaneZoomDenominator"), 100));
	if (dwNominator > 0 || dwDenominator > 0)
		::PostMessage(m_pRichEditCtrl->GetSafeHwnd(), EM_SETZOOM, 
			static_cast<WPARAM>(dwNominator), static_cast<LPARAM>(dwDenominator));

	if (theApp.m_bWordWrap)
		m_pRichEditCtrl->SetTargetDevice(nullptr, 0);

	return 0;
}

void CQueryPane::OnDestroy()
{
	CRichEditPane::OnDestroy();

	// TODO: Add your message handler code here

	DWORD dwNominator = NULL;
	DWORD dwDenominator = NULL;
	::SendMessage(m_pRichEditCtrl->GetSafeHwnd(), EM_GETZOOM, reinterpret_cast<WPARAM>(&dwNominator), reinterpret_cast<LPARAM>(&dwDenominator));
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("QueryPaneZoomNominator"), static_cast<int>(dwNominator));
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("QueryPaneZoomDenominator"), static_cast<int>(dwDenominator));
}

CString CQueryPane::GetSelText() const
{
	return m_pRichEditCtrl->GetSelText();
}

void CQueryPane::SetText(std::vector<CString>&& text)
{
	for (const auto& it : text)
		m_pRichEditCtrl->ReplaceSel(it + _T("\n"), TRUE);
}

void CQueryPane::SetDarkMode(const BOOL bSet)
{
	CHARFORMAT cf{};
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;  // Set color mask

	m_pRichEditCtrl->SetBackgroundColor(bSet ? FALSE : TRUE, bSet ? g_crColorDark : g_crColorWhite);
	cf.crTextColor = bSet ? GetSysColor(COLOR_WINDOW) : GetSysColor(COLOR_WINDOWTEXT); // Set desired color
	m_pRichEditCtrl->SetSel(-1, -1);
	m_pRichEditCtrl->SetDefaultCharFormat(cf);
}

void CQueryPane::OnEnChangeRichEditCtrl()
{
	::PostMessage(GetOwner()->GetSafeHwnd(), WMU_QUERYCHANGED, 0, 0);
}

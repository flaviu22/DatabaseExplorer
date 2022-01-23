#include "pch.h"
#include "QueryPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CQueryPane, CRichEditPane)
	//{{AFX_MSG_MAP(CQueryPane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
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

	m_pRichEditCtrl->PostMessage(EM_SETEVENTMASK, 0, ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS);

	const DWORD dwNominator = static_cast<DWORD>(AfxGetApp()->GetProfileInt(_T("Settings"), _T("QueryPaneZoomNominator"), 0));
	const DWORD dwDenominator = static_cast<DWORD>(AfxGetApp()->GetProfileInt(_T("Settings"), _T("QueryPaneZoomDenominator"), 0));
	if (dwNominator > 0 || dwDenominator > 0)
		::PostMessage(m_pRichEditCtrl->GetSafeHwnd(), EM_SETZOOM, static_cast<WPARAM>(dwNominator), static_cast<LPARAM>(dwDenominator));

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

#include "pch.h"
#include "QueryPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CQueryPane, CRichEditPane)
	//{{AFX_MSG_MAP(CQueryPane)
	ON_WM_CREATE()
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

	return 0;
}

CString CQueryPane::GetSelText() const
{
	return m_pRichEditCtrl->GetSelText();
}

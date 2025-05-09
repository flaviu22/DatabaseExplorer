// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "pch.h"
#include "framework.h"
#include "DatabaseExplorer.h"

#include "ChildFrm.h"
//#include "DatabaseExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndExt)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndExt)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_QUERY, &CChildFrame::OnViewQuery)
	ON_COMMAND(ID_VIEW_MESSAGE, &CChildFrame::OnViewMessage)
	ON_COMMAND(ID_VIEW_DATABASE, &CChildFrame::OnViewDatabase)
	ON_MESSAGE(WMU_POSTINIT, &CChildFrame::OnPostInit)
	ON_MESSAGE(WMU_ISPOPULATEMODE, &CChildFrame::OnIsPopulateMode)
	ON_MESSAGE(WMU_SETWORDWRAP, &CChildFrame::OnSetWordWrap)
	ON_MESSAGE(WMU_DARKMODE, &CChildFrame::OnDarkMode)
	ON_MESSAGE(WMU_STATECHANGED, &CChildFrame::OnStateChanged)
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CChildFrame::CChildFrame() noexcept
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if(! CMDIChildWndExt::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndExt::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndExt::Dump(dc);
}
#endif //_DEBUG

// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndExt::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here

	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), FALSE);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	CMDIChildWndEx::m_bEnableFloatingBars = TRUE;

	CString sTitle(_T("Queries"));

	if (! m_pQueryPane->Create(sTitle, this, CSize(300, 200), TRUE, PANEQUERYID,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI,
		AFX_CBRS_REGULAR_TABS, AFX_NON_CLOSE_DOCKING_PANE_STYLE))
	{
		TRACE(_T("Failed to create Query window\n"));
		return FALSE; // failed to create
	}
	sTitle.Format(_T("Messages"));
	if (! m_pMessagePane->Create(sTitle, this, CSize(700, 200), TRUE, PANEMESSAGEID,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI,
		AFX_CBRS_REGULAR_TABS, AFX_NON_CLOSE_DOCKING_PANE_STYLE))
	{
		TRACE(_T("Failed to create Message window\n"));
		return FALSE; // failed to create
	}

	sTitle.Format(_T("Databases"));
	if (! m_pDatabasePane->Create(sTitle, this, CSize(250, 250), TRUE, PANEDATABASEID,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI,
		AFX_CBRS_REGULAR_TABS, AFX_NON_CLOSE_DOCKING_PANE_STYLE))
	{
		TRACE(_T("Failed to create Database window\n"));
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(TRUE);

	m_pQueryPane->EnableDocking(CBRS_ALIGN_ANY);
	m_pMessagePane->EnableDocking(CBRS_ALIGN_ANY);
	m_pDatabasePane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(m_pQueryPane.get(), AFX_IDW_DOCKBAR_BOTTOM);
	m_pMessagePane->DockToWindow(m_pQueryPane.get(), CBRS_ALIGN_RIGHT);
	DockPane(m_pDatabasePane.get(), AFX_IDW_DOCKBAR_RIGHT);

	m_dockManager.LoadState(theApp.GetRegSectionPath(_T("ChildFramePane")));
	m_dockManager.SetDockState();

//	if (! m_pQueryPane->IsPaneVisible())
		m_pQueryPane->ShowPane(TRUE, FALSE, FALSE);
//	if (! m_pMessagePane->IsPaneVisible())
		m_pMessagePane->ShowPane(TRUE, FALSE, FALSE);
//	if (! m_pDatabasePane->IsPaneVisible())
		m_pDatabasePane->ShowPane(TRUE, FALSE, FALSE);

	return 0;
}

void CChildFrame::OnDestroy()
{
	if ((nullptr != m_pQueryPane->GetSafeHwnd() && ! m_pQueryPane->IsFloating())
		&& (nullptr != m_pMessagePane->GetSafeHwnd() && ! m_pMessagePane->IsFloating())
		&& (nullptr != m_pDatabasePane->GetSafeHwnd() && ! m_pDatabasePane->IsFloating()))
		m_dockManager.SaveState(theApp.GetRegSectionPath(_T("ChildFramePane")));

	CMDIChildWndExt::OnDestroy();

	// TODO: Add your message handler code here
}

void CChildFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hBarIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_pQueryPane->SetIcon(hBarIcon, FALSE);
	hBarIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_pMessagePane->SetIcon(hBarIcon, FALSE);
	hBarIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_pDatabasePane->SetIcon(hBarIcon, FALSE);
}

CString CChildFrame::GetQueryText() const
{
	CString sText;
	m_pQueryPane->GetRichEditCtrl()->GetWindowText(sText);
	return sText;
}

CString CChildFrame::GetMessageText() const
{
	CString sText;
	m_pQueryPane->GetRichEditCtrl()->GetWindowText(sText);
	return sText;
}

void CChildFrame::OnViewQuery()
{
	// TODO: Add your command handler code here

	m_pQueryPane->ShowPane(TRUE, FALSE, TRUE);
	if (m_pQueryPane->IsDocked())
		m_pQueryPane->Slide(TRUE);
	m_pQueryPane->GetRichEditCtrl()->SetFocus();
}

void CChildFrame::OnViewMessage()
{
	// TODO: Add your command handler code here

	m_pMessagePane->ShowPane(TRUE, FALSE, TRUE);
	if (m_pMessagePane->IsDocked())
		m_pMessagePane->Slide(TRUE);
	m_pMessagePane->GetRichEditCtrl()->SetFocus();
}

void CChildFrame::OnViewDatabase()
{
	// TODO: Add your command handler code here

	m_pDatabasePane->ShowPane(TRUE, FALSE, TRUE);
	if (m_pDatabasePane->IsDocked())
		m_pDatabasePane->Slide(TRUE);
	m_pDatabasePane->SetFocus();
}

void CChildFrame::SetMessage(const CString& sMessage, const Color& color)
{
	m_pMessagePane->AppendToLogAndScroll(sMessage, static_cast<COLORREF>(color));
}

LRESULT CChildFrame::OnPostInit(WPARAM wParam, LPARAM lParam)
{
	if (DoInitDatabase == static_cast<int>(wParam))
		GetActiveDocument()->UpdateAllViews(nullptr, CDatabaseExplorerApp::UH_INITDATABASE);

	if (DoListTable == static_cast<int>(wParam))
		GetActiveDocument()->UpdateAllViews(nullptr, 
			CDatabaseExplorerApp::UH_LISTTABLE, 
			reinterpret_cast<CObject*>(&m_pDatabasePane->GetSelection()));

	return 1;
}

LRESULT CChildFrame::OnIsPopulateMode(WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(GetActiveView()->GetSafeHwnd(), WMU_ISPOPULATEMODE, 0, 0);
}

LRESULT CChildFrame::OnSetWordWrap(WPARAM wParam, LPARAM lParam)
{
	return m_pQueryPane->GetRichEditCtrl()->SetTargetDevice(nullptr, static_cast<long>(wParam));
}

LRESULT CChildFrame::OnDarkMode(WPARAM wParam, LPARAM lParam)
{
	m_pQueryPane->SetDarkMode(static_cast<BOOL>(wParam));
	m_pMessagePane->SetDarkMode(static_cast<BOOL>(wParam));
	m_pDatabasePane->SetDarkMode(static_cast<BOOL>(wParam));

	return 1;
}

LRESULT CChildFrame::OnStateChanged(WPARAM wParam, LPARAM lParam)
{
	::PostMessage(GetParentFrame()->GetSafeHwnd(), WMU_STATECHANGED, 0, 0);
	return 1;
}

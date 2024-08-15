// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "DatabaseExplorer.h"

#include "MainFrm.h"
#include "WindowsManagerDialog.h"
#include "ChildFrm.h"
#include "DatabaseExplorerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_MESSAGE(WMU_CHILDFRAMEADDED, &CMainFrame::OnChildFrameAdded)
	ON_MESSAGE(WMU_CHILDFRAMEREMOVED, &CMainFrame::OnChildFrameRemoved)
	ON_MESSAGE(WMU_CHILDFRAMEACTIVATE, &CMainFrame::OnChildFrameActivate)
	ON_MESSAGE(WMU_CHILDFRAMEACTIVATED, &CMainFrame::OnChildFrameActivated)
	ON_WM_TIMER()
	ON_WM_TIMECHANGE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_EDIT_CUT, &CMainFrame::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, &CMainFrame::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CMainFrame::OnEditRedo)
	ON_COMMAND(ID_EDIT_SELECTALL, &CMainFrame::OnEditSelectall)
	ON_COMMAND(ID_EDIT_SELECTLINE, &CMainFrame::OnEditSelectline)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CMainFrame::OnUpdateCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMainFrame::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CMainFrame::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CMainFrame::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, &CMainFrame::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTLINE, &CMainFrame::OnUpdateEdit)
	ON_COMMAND(ID_VIEW_VIRTUALMODE, &CMainFrame::OnViewVirtualmode)
	ON_COMMAND(ID_VIEW_WORDWRAP, &CMainFrame::OnViewWordwrap)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VIRTUALMODE, &CMainFrame::OnUpdateViewVirtualmode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORDWRAP, &CMainFrame::OnUpdateViewWordwrap)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_GET_TAB_TOOLTIP, &CMainFrame::OnGetTabToolTip)
	ON_MESSAGE(WMU_SETMESSAGETEXT, &CMainFrame::OnSetMessageText)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here

	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (! CMDIFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (! m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);
	if (! m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);
	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);
	// Enable toolbar and docking window menu replacement
	CString strCustomize;
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	OnInitData();

	return 0;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (WM_KEYDOWN == pMsg->message && VK_TAB == pMsg->wParam && GetAsyncKeyState(VK_CONTROL) < 0 && m_arrChild.GetSize() > 1)
	{
		CWindowsManagerDialog* pDlg = new CWindowsManagerDialog;
		pDlg->Create(CWindowsManagerDialog::IDD, this);
		pDlg->ShowWindow(SW_SHOW);
		return TRUE;
	}

	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}
// base class does the real work
BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	if (! CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		return FALSE;

	return TRUE;
}

std::vector<CString> CMainFrame::GetTabsNames()
{
	CString sLabel;
	std::vector<CString> data{};
	CMFCTabCtrl& MFCTabCtrl = GetMDITabs();
	const int nCount = MFCTabCtrl.GetTabsNum();
	for (int i = 0; i < nCount; ++i)
	{
		MFCTabCtrl.GetTabLabel(i, sLabel);
		const CString sLabelNormalized = theApp.GetTitleNormalized(sLabel);
		const auto found = std::find(data.cbegin(), data.cend(), sLabelNormalized);
		if (theApp.HasValidDocumentTitle(sLabelNormalized) && found == data.cend())
			data.push_back(sLabelNormalized);
	}

	return data;
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	if (theApp.m_bWordWrap)
	{
		OnViewWordwrap();
		theApp.m_bWordWrap = TRUE;
	}
	theApp.UpdateBackupFiles();

	CMDIFrameWndEx::OnClose();
}

void CMainFrame::OnInitData()
{
	EnableMDITabs(TRUE, TRUE, CMFCBaseTabCtrl::LOCATION_TOP, TRUE, 
					CMFCTabCtrl::STYLE_3D_SCROLLED, TRUE, TRUE);

	CMFCTabCtrl& MFCTabCtrl = GetMDITabs();
	MFCTabCtrl.SetTabBorderSize(0);
	MFCTabCtrl.SetFlatFrame(TRUE);
	MFCTabCtrl.EnableTabSwap(TRUE);
	MFCTabCtrl.EnableTabDocumentsMenu(TRUE);
	MFCTabCtrl.EnableAutoColor(FALSE);

	m_wndMenuBar.SetRecentlyUsedMenus(FALSE);
}

LRESULT CMainFrame::OnChildFrameAdded(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (m_arrChild.GetAt(i) == reinterpret_cast<CMDIChildWnd*>(lParam))
		{
			return 0;
		}
	}

	m_arrChild.InsertAt(0, (CMDIChildWnd*)lParam);

	return 1;
}

LRESULT CMainFrame::OnChildFrameRemoved(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (m_arrChild.GetAt(i) == reinterpret_cast<CMDIChildWnd*>(lParam))
		{
			m_arrChild.RemoveAt(i);
			break;
		}
	}

	return 1;
}

LRESULT CMainFrame::OnChildFrameActivate(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (m_arrChild.GetAt(i) == reinterpret_cast<CMDIChildWnd*>(lParam))
		{
			CMDIChildWnd* pMDIChild = m_arrChild.GetAt(i);
			if (pMDIChild->IsIconic())
				pMDIChild->ShowWindow(SW_RESTORE);
			MDIActivate(pMDIChild);
			CMDIChildWndEx* pMDIChildEx = DYNAMIC_DOWNCAST(CMDIChildWndEx, pMDIChild);
			if (NULL != pMDIChildEx->GetSafeHwnd() && IsTabbedMDIChild())
			{
				CMFCTabCtrl& TabCtrl = GetMDITabs();
				const int nTab = TabCtrl.GetTabFromHwnd(pMDIChildEx->GetSafeHwnd());
				TabCtrl.SetActiveTab(nTab);
			}
			break;
		}
	}

	return 1;
}

LRESULT CMainFrame::OnChildFrameActivated(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (m_arrChild.GetAt(i) == reinterpret_cast<CMDIChildWnd*>(lParam))
		{
			m_arrChild.RemoveAt(i);
			break;
		}
	}

	m_arrChild.InsertAt(0, reinterpret_cast<CMDIChildWnd*>(lParam));

	return 1;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CMDIFrameWndEx::OnTimer(nIDEvent);

	if (ID_TIMER_RESETSTATUSBAR == nIDEvent)
	{
		KillTimer(nIDEvent);
		PostMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);
	}
}

void CMainFrame::OnTimeChange()
{
	CMDIFrameWndEx::OnTimeChange();

	// TODO: Add your message handler code here

	theApp.DisconnectAllDatabases();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWndEx::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here

	if (! nState)
		m_pWndFocus = GetFocus();
	else if (nullptr != m_pWndFocus)
		m_pWndFocus->SetFocus();
}

LRESULT CMainFrame::OnSetMessageText(WPARAM wParam, LPARAM lParam)
{
	if (AFX_IDS_IDLEMESSAGE == lParam)
	{
		SetMessageText(AFX_IDS_IDLEMESSAGE);
	}
	else
	{
		if (lParam)
			SetMessageText(reinterpret_cast<LPCTSTR>(lParam));
	}

	if (wParam > 0)
		SetTimer(ID_TIMER_RESETSTATUSBAR, static_cast<UINT>(wParam), nullptr);

	return 0;
}

LRESULT CMainFrame::OnGetTabToolTip(WPARAM wParam, LPARAM lParam)
{
	CMFCTabToolTipInfo* pInfo = (CMFCTabToolTipInfo*)lParam;
	ASSERT(nullptr != pInfo);

	if (pInfo)
	{
		CMFCBaseTabCtrl* tabControl = pInfo->m_pTabWnd;
		ASSERT_VALID(tabControl);
		if (tabControl->IsMDITab())
		{
			CWnd* tabPaneWnd = tabControl->GetTabWndNoWrapper(pInfo->m_nTabIndex);
			if (tabPaneWnd->IsKindOf(RUNTIME_CLASS(CChildFrame)))
			{
				CChildFrame* pChild = STATIC_DOWNCAST(CChildFrame, tabPaneWnd);
				CDatabaseExplorerDoc* pDoc = static_cast<CDatabaseExplorerDoc*>(pChild->GetActiveDocument());
				if (! pDoc->GetDSN().second.IsEmpty())
					pInfo->m_strText = pDoc->GetDSN().second;
			}
		}
	}

	return 0;
}

void CMainFrame::OnEditCut()
{
	// TODO: Add your command handler code here

	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (nullptr != pChild->GetSafeHwnd())
	{
		pChild->GetQueryPane()->GetRichEditCtrl()->Cut();
	}
}

void CMainFrame::OnEditCopy()
{
	// TODO: Add your command handler code here

	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (nullptr != pChild->GetSafeHwnd())
	{
		CWnd* pWnd = GetFocus();
		if (pWnd == pChild->GetQueryPane()->GetRichEditCtrl())
			pChild->GetQueryPane()->GetRichEditCtrl()->Copy();
		if (pWnd == pChild->GetMessagePane()->GetRichEditCtrl())
			pChild->GetMessagePane()->GetRichEditCtrl()->Copy();
	}
}

void CMainFrame::OnEditPaste()
{
	// TODO: Add your command handler code here

	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (nullptr != pChild->GetSafeHwnd())
	{
		pChild->GetQueryPane()->GetRichEditCtrl()->Paste();
	}
}

void CMainFrame::OnEditUndo()
{
	// TODO: Add your command handler code here

	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (nullptr != pChild->GetSafeHwnd())
	{
		pChild->GetQueryPane()->GetRichEditCtrl()->Undo();
	}
}

void CMainFrame::OnEditRedo()
{
	// TODO: Add your command handler code here

	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (nullptr != pChild->GetSafeHwnd())
	{
		pChild->GetQueryPane()->GetRichEditCtrl()->Redo();
	}
}

void CMainFrame::OnEditSelectall()
{
	// TODO: Add your command handler code here

	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (nullptr != pChild->GetSafeHwnd())
	{
		pChild->GetQueryPane()->GetRichEditCtrl()->SetSel(0, -1);
	}
}

void CMainFrame::OnEditSelectline()
{
	// TODO: Add your command handler code here

	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (nullptr != pChild->GetSafeHwnd())
	{
		CRichEditCtrl* pRichEditCtrl = pChild->GetQueryPane()->GetRichEditCtrl();
		const long nLineNum = pRichEditCtrl->LineFromChar(pRichEditCtrl->LineIndex(-1));
		const int nBegin = pRichEditCtrl->LineIndex(nLineNum);
		const int nEnd = pRichEditCtrl->LineLength(nBegin);
		pRichEditCtrl->SetSel(nBegin, nBegin + nEnd);
	}
}

void CMainFrame::OnUpdateEdit(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	CWnd* pWnd = GetFocus();
	BOOL bMaximized = FALSE;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));

	pCmdUI->Enable(nullptr != pWnd && nullptr != pChild && 
		pWnd == pChild->GetQueryPane()->GetRichEditCtrl());
}

void CMainFrame::OnUpdateCut(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	BOOL bEnable = FALSE;
	CWnd* pWnd = GetFocus();
	BOOL bMaximized = FALSE;
	CRichEditCtrl* pRichEdit = nullptr;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));

	do
	{
		if (nullptr == pWnd)
			break;
		if (nullptr == pChild)
			break;
		if (pWnd == pChild->GetQueryPane()->GetRichEditCtrl())
			pRichEdit = pChild->GetQueryPane()->GetRichEditCtrl();
		bEnable = IsCharsSelected(pRichEdit);
	} while (FALSE);

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnUpdateCopy(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	BOOL bEnable = FALSE;
	CWnd* pWnd = GetFocus();
	BOOL bMaximized = FALSE;
	long nStart = 0, nEnd = 0;
	CRichEditCtrl* pRichEdit = nullptr;
	CChildFrame* pChild = static_cast<CChildFrame*>(MDIGetActive(&bMaximized));

	do
	{
		if (nullptr == pWnd)
			break;
		if (nullptr == pChild)
			break;
		if (pWnd == pChild->GetQueryPane()->GetRichEditCtrl())
			pRichEdit = pChild->GetQueryPane()->GetRichEditCtrl();
		if (pWnd == pChild->GetMessagePane()->GetRichEditCtrl())
			pRichEdit = pChild->GetMessagePane()->GetRichEditCtrl();
		bEnable = IsCharsSelected(pRichEdit);
	} while (FALSE);

	pCmdUI->Enable(bEnable);
}

BOOL CMainFrame::IsCharsSelected(CRichEditCtrl* pRichEdit) const
{
	if (nullptr == pRichEdit->GetSafeHwnd())
		return FALSE;
	long nStart = 0, nEnd = 0;
	pRichEdit->GetSel(nStart, nEnd);
	return (nStart != nEnd);
}

void CMainFrame::OnViewVirtualmode()
{
	// TODO: Add your command handler code here

	if (theApp.GetOpenDocumentCount() > 0)
	{
		if (IDYES != MessageBox(_T("In order to change this feature you need to close all documents. Are you sure you want this ?"),
			NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
			return;
		theApp.CloseAllDocuments(FALSE);
	}

	theApp.m_bVirtualMode = ! theApp.m_bVirtualMode;
}

void CMainFrame::OnViewWordwrap()
{
	// TODO: Add your command handler code here

	theApp.m_bWordWrap = ! theApp.m_bWordWrap;

	POSITION pos = theApp.GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pDocTemplate = static_cast<CDocTemplate*>(theApp.GetNextDocTemplate(pos));
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			CDatabaseExplorerDoc* pDoc = static_cast<CDatabaseExplorerDoc*>(pDocTemplate->GetNextDoc(posDoc));
			POSITION posView = pDoc->GetFirstViewPosition();
			while (posView)
			{
				CView* pView = pDoc->GetNextView(posView);
				::SendMessage(pView->GetParentFrame()->GetSafeHwnd(), WMU_SETWORDWRAP, static_cast<WPARAM>(! theApp.m_bWordWrap), 0);
			}
		}
	}
}

void CMainFrame::OnUpdateViewVirtualmode(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(theApp.m_bVirtualMode);
}

void CMainFrame::OnUpdateViewWordwrap(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(theApp.m_bWordWrap);
}

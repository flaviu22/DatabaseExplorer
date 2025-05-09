#include "pch.h"

#include "DatabaseExplorer.h"
#include "DatabasePane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatabasePane

IMPLEMENT_DYNAMIC(CDatabasePane, CNonClosableDockablePane)

CDatabasePane::CDatabasePane()
{
	m_pIL = std::make_unique<CImageList>();
	m_pTreeCtrl = std::make_unique<CPaneTreeCtrl>();
}

CDatabasePane::~CDatabasePane()
{
	m_pIL->DeleteImageList();
}

BEGIN_MESSAGE_MAP(CDatabasePane, CNonClosableDockablePane)
	//{{AFX_MSG_MAP(CDatabasePane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATABASE, &CDatabasePane::OnTvnSelchangedTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DATABASE, &CDatabasePane::OnNMDblclkTree)
	ON_MESSAGE(WMU_POSTINIT, &CDatabasePane::OnPostInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CDatabasePane message handlers

int CDatabasePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (-1 == CNonClosableDockablePane::OnCreate(lpCreateStruct))
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tree:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	if (!m_pTreeCtrl->Create(dwViewStyle, rectDummy, this, IDC_TREE_DATABASE))
	{
		TRACE(_T("Failed to create database pane view\n"));
		return -1;      // fail to create
	}

	AdjustLayout();
	InitPropList();

	return 0;
}

void CDatabasePane::OnSize(UINT nType, int cx, int cy)
{
	CNonClosableDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CDatabasePane::OnDestroy()
{
	CNonClosableDockablePane::OnDestroy();

	// TODO: Add your message handler code here
}

void CDatabasePane::AdjustLayout()
{
	if (NULL == GetSafeHwnd())
		return;

	CRect rectClient;
	GetClientRect(rectClient);

	m_pTreeCtrl->SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDatabasePane::InitPropList()
{
	m_pIL->Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);
	m_pIL->Add(AfxGetApp()->LoadIcon(IDI_DATABASE));
	m_pIL->Add(AfxGetApp()->LoadIcon(IDI_TABLE));
	m_pTreeCtrl->SetImageList(m_pIL.get(), LVSIL_NORMAL);

	const int nIndex = m_pIL->Add(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_SELECTED)));
	if (nIndex >= 0)
		m_pIL->SetOverlayImage(nIndex, 1);
}

void CDatabasePane::OnSetFocus(CWnd* pOldWnd)
{
	CNonClosableDockablePane::OnSetFocus(pOldWnd);

	m_pTreeCtrl->SetFocus();
}

BOOL CDatabasePane::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			::PostMessage(m_pTreeCtrl->GetParentOwner()->GetSafeHwnd(), WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, 0), 0);
		}

		if (VK_RETURN == pMsg->wParam)
		{
			PostMessage(WMU_POSTINIT, DoListTable, 0);
		}
	}

	return CNonClosableDockablePane::PreTranslateMessage(pMsg);
}

LRESULT CDatabasePane::OnPostInit(WPARAM wParam, LPARAM lParam)
{
	if (DoListTable == wParam)
	{
		HTREEITEM hItemSel = m_pTreeCtrl->GetSelectedItem();
		if (nullptr != hItemSel && 0 == m_pTreeCtrl->GetItemData(hItemSel))
		{
			HTREEITEM hParent = m_pTreeCtrl->GetParentItem(hItemSel);
			if (nullptr != hParent)
			{
				if (m_hItemSelected != hParent)
				{
					SetItemAsDatabase(m_hItemSelected, hParent);
					::PostMessage(GetOwner()->GetSafeHwnd(), WMU_POSTINIT, DoInitDatabase, 0);
				}
				::PostMessage(GetOwner()->GetSafeHwnd(), WMU_POSTINIT, DoListTable, 0);
			}
		}
	}

	return 1;
}

CString CDatabasePane::GetSelection() const
{
	HTREEITEM hItem = m_pTreeCtrl->GetSelectedItem();
	if (nullptr != hItem)
		return m_pTreeCtrl->GetItemText(hItem);

	return _T("");
}

CString CDatabasePane::GetDatabaseSelection() const
{
	TVITEM item{};
	item.hItem = m_hItemSelected;
	item.mask = TVIF_PARAM;
	if (::SendMessage(m_pTreeCtrl->GetSafeHwnd(), TVM_GETITEM, 0, (LPARAM)&item))
		return m_pTreeCtrl->GetItemText(m_hItemSelected);

	return _T("");
}

void CDatabasePane::SetItemAsDatabase(const CString& sDatabase)
{
	HTREEITEM hItem = m_pTreeCtrl->GetRootItem();
	while (nullptr != hItem)
	{
		if (0 == sDatabase.CompareNoCase(m_pTreeCtrl->GetItemText(hItem)))
		{
			m_pTreeCtrl->SelectItem(hItem);
			SetItemAsDatabase(m_hItemSelected, hItem);
			break;
		}
		hItem = m_pTreeCtrl->GetNextItem(hItem, TVGN_NEXT);
	}
}

void CDatabasePane::SetItemAsDatabase(const HTREEITEM& hItemOld, const HTREEITEM& hItemNew)
{
	if (m_hItemSelected != hItemNew)
	{
		m_pTreeCtrl->SetItemState(hItemOld, INDEXTOOVERLAYMASK(0), TVIS_OVERLAYMASK);
		m_pTreeCtrl->SetItemState(hItemNew, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK);
		m_hItemSelected = hItemNew;
	}
}

void CDatabasePane::OnTvnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	*pResult = 0;

	if (::SendMessage(GetParentFrame()->GetSafeHwnd(), WMU_ISPOPULATEMODE, 0, 0))
	{
		m_hItemSelected = nullptr;
	}
	else
	{
		if (m_pTreeCtrl->GetItemData(pNMTreeView->itemNew.hItem) > 0)
		{
			SetItemAsDatabase(m_hItemSelected, pNMTreeView->itemNew.hItem);
			::PostMessage(GetOwner()->GetSafeHwnd(), WMU_POSTINIT, DoInitDatabase, 0);
		}
	}
}

void CDatabasePane::OnNMDblclkTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	UINT nFlags;
	CPoint curPoint;
	GetCursorPos(&curPoint);
	m_pTreeCtrl->ScreenToClient(&curPoint);
	HTREEITEM hItemSel = m_pTreeCtrl->HitTest(curPoint, &nFlags);

	if (nullptr != hItemSel && 0 == m_pTreeCtrl->GetItemData(hItemSel))
	{
		HTREEITEM hParent = m_pTreeCtrl->GetParentItem(hItemSel);
		if (nullptr != hParent)
		{
			if (m_hItemSelected != hParent)
			{
				SetItemAsDatabase(m_hItemSelected, hParent);
				::PostMessage(GetOwner()->GetSafeHwnd(), WMU_POSTINIT, DoInitDatabase, 0);
			}
			::PostMessage(GetOwner()->GetSafeHwnd(), WMU_POSTINIT, DoListTable, 0);
		}
	}

	*pResult = 0;
}

void CDatabasePane::SetDarkMode(const BOOL bSet)
{
	if (g_crDummy == GetLineColor())
	{
		SetLineColor(m_pTreeCtrl->GetLineColor());
		SetTextColor(m_pTreeCtrl->GetTextColor());
		SetBkColor(m_pTreeCtrl->GetBkColor());
	}

	m_pTreeCtrl->SetLineColor(bSet ? GetSysColor(COLOR_WINDOW) : GetLineColor());
	m_pTreeCtrl->SetTextColor(bSet ? GetSysColor(COLOR_WINDOW) : GetTextColor());
	m_pTreeCtrl->SetBkColor(bSet ? g_crColorDark : GetBkColor());
}

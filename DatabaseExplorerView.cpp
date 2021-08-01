// DatabaseExplorerView.cpp : implementation of the CDatabaseExplorerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DatabaseExplorer.h"
#endif

#include "DatabaseExplorerDoc.h"
#include "DatabaseExplorerView.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDatabaseExplorerView

IMPLEMENT_DYNCREATE(CDatabaseExplorerView, CListView)

BEGIN_MESSAGE_MAP(CDatabaseExplorerView, CListView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CDatabaseExplorerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_RUN, &CDatabaseExplorerView::OnEditRun)
	ON_MESSAGE(WMU_POSTINIT, &CDatabaseExplorerView::OnPostInit)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CDatabaseExplorerView::OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, &CDatabaseExplorerView::OnLvnOdcachehint)
END_MESSAGE_MAP()

// CDatabaseExplorerView construction/destruction

CDatabaseExplorerView::CDatabaseExplorerView() noexcept
{
	// TODO: add construction code here

}

CDatabaseExplorerView::~CDatabaseExplorerView()
{
}

BOOL CDatabaseExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= LVS_REPORT;

	if (theApp.m_bVirtualMode)
	{
		cs.style &= ~LVS_TYPEMASK;
		cs.style |= LVS_REPORT | LVS_OWNERDATA;
	}

	return CListView::PreCreateWindow(cs);
}

void CDatabaseExplorerView::OnDraw(CDC* pDC)
{
	CDatabaseExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

void CDatabaseExplorerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().

	CListCtrl& ListCtrl = GetListCtrl();
	if (ListCtrl.GetHeaderCtrl()->GetItemCount() > 0)
		return;

	ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

	PostMessage(WMU_POSTINIT);
}

// CDatabaseExplorerView printing

void CDatabaseExplorerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CDatabaseExplorerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDatabaseExplorerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
}

void CDatabaseExplorerView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add cleanup after printing
}

void CDatabaseExplorerView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDatabaseExplorerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CDatabaseExplorerView diagnostics

#ifdef _DEBUG
void CDatabaseExplorerView::AssertValid() const
{
	CListView::AssertValid();
}

void CDatabaseExplorerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CDatabaseExplorerDoc* CDatabaseExplorerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDatabaseExplorerDoc)));
	return (CDatabaseExplorerDoc*)m_pDocument;
}
#endif //_DEBUG

// CDatabaseExplorerView message handlers

LRESULT CDatabaseExplorerView::OnPostInit(WPARAM wParam, LPARAM lParam)
{
	CDatabaseExplorerDoc* pDoc = GetDocument();
	pDoc->UpdateAllViews(NULL, CDatabaseExplorerApp::UH_POPULATEDATABASEPANEL);

	return 1;
}

void CDatabaseExplorerView::OnEditRun()
{
	// TODO: Add your command handler code here

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->SetMessageText(AFX_IDS_IDLEMESSAGE);

	GetDocument()->UpdateAllViews(NULL, CDatabaseExplorerApp::UH_RUNSQLSATEMENTS);
}

void CDatabaseExplorerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class

	CDatabaseExplorerDoc* pDoc = GetDocument();

	if (CDatabaseExplorerApp::UH_INITDATABASE == lHint)
	{
		const CString sDatabase = pDoc->InitDatabase();
		if (! sDatabase.IsEmpty())
			pDoc->LogMessage(sDatabase + _T(" database is selected"), MessageType::info);

		return;
	}

	if (CDatabaseExplorerApp::UH_SELECTDATABASE == lHint && nullptr != pHint)
	{
		pDoc->GetDatabasePane()->SetItemAsDatabase(*reinterpret_cast<CString*>(pHint));

		return;
	}

	if (CDatabaseExplorerApp::UH_POPULATEDATABASEPANEL == lHint)
	{
		CChildFrame* pChild = static_cast<CChildFrame*>(GetParentFrame());
		if (nullptr != pChild->GetSafeHwnd())
		{
			if (! pDoc->PopulateDatabasePanel(*pChild->GetDatabasePane()->GetTreeCtrl()))
			{
				CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
				pFrame->SetMessageText(pDoc->m_sState);
				pFrame->SetTimer(ID_TIMER_RESETSTATUSBAR, 12000, nullptr);
			}
		}

		return;
	}

	if (CDatabaseExplorerApp::UH_RUNSQLSATEMENTS == lHint)
	{
		CChildFrame* pChild = static_cast<CChildFrame*>(GetParentFrame());
		if (nullptr != pChild->GetSafeHwnd())
		{
			CWaitCursor Wait;
			const CString sSelected = pChild->GetQueryPane()->GetSelText();
			const auto sql = pDoc->GetSQLStatements(! sSelected.IsEmpty() ? sSelected : pChild->GetQueryPane()->GetText());
			if (0 == sql.size())
			{
				pDoc->LogMessage(_T("You have no SQL statement"), MessageType::info);
				return;
			}

			const CString sDatabase = pChild->GetDatabasePane()->GetDatabaseSelection();
			const int nDatabaseCountOffline = pChild->GetDatabasePane()->GetDatabaseCount();

			for (auto it = sql.begin(); it != sql.end(); ++it)
			{
				if (! pDoc->IsSelect(*it))
					ExecuteSQL(pDoc, *it);
				else
					ExecuteSelect(pDoc, *it);
			}

			const int nDatabaseCountOnline = pDoc->GetDatabaseCount();
			if (nDatabaseCountOffline != nDatabaseCountOnline)
			{
				pDoc->PopulateDatabasePanel(*pChild->GetDatabasePane()->GetTreeCtrl());
				pDoc->GetDatabasePane()->SetItemAsDatabase(sDatabase);
			}
		}

		return;
	}

	if (CDatabaseExplorerApp::UH_LISTTABLE == lHint && nullptr != pHint)
	{
		CWaitCursor Wait;
		ExecuteSelect(pDoc, _T("SELECT * FROM ") + *reinterpret_cast<CString*>(pHint));

		return;
	}
}

void CDatabaseExplorerView::DeleteAllColumns()
{
	CListCtrl& ListCtrl = GetListCtrl();
	ListCtrl.DeleteAllItems();
	int nCount = ListCtrl.GetHeaderCtrl()->GetItemCount();
	while (nCount > 0)
	{
		ListCtrl.DeleteColumn(0);
		nCount--;
	}
}

void CDatabaseExplorerView::ExecuteSQL(CDatabaseExplorerDoc* pDoc, const CString& sSQL)
{
	pDoc->RunSQL(sSQL);
}

void CDatabaseExplorerView::ExecuteSelect(CDatabaseExplorerDoc* pDoc, const CString& sSQL)
{
	if (theApp.m_bVirtualMode)
	{
		m_arrRows.clear();
		GetListCtrl().SetItemCountEx(0);
	}
	else
	{
		const int nRows = pDoc->GetRecordCount(sSQL);
		if (nRows > 33000)
		{
			CString sMessage;
			sMessage.Format(_T("This select will get %d rows, continue ?"), nRows);
			if (IDYES != MessageBox(sMessage, NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
				return;
		}
	}

	SetRedraw(FALSE);
	DeleteAllColumns();
	pDoc->PopulateListCtrl(this, sSQL);
	SetRedraw();
}

void CDatabaseExplorerView::OnLvnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here

	if (theApp.m_bVirtualMode)
	{
		LV_ITEM* pItem = &pDispInfo->item;
		if (LVIF_TEXT & pItem->mask && pItem->iItem < static_cast<int>(m_arrRows.size()))
		{
			const CDBRecord& record = *m_arrRows.at(pItem->iItem);
			_tcscpy_s(pItem->pszText, pItem->cchTextMax, record.m_arrValue.at(pItem->iSubItem));
		}
	}

	*pResult = 0;
}

void CDatabaseExplorerView::OnLvnOdcachehint(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

	if (theApp.m_bVirtualMode)
	{
		CString sValue;
		CDatabaseExplorerDoc* pDoc = GetDocument();
		CRecordset* pRecordset = pDoc->GetRecordset();
		LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);

		const DWORD dwToRow = pCacheHint->iTo;
		const DWORD dwFetchedRows = static_cast<DWORD>(m_arrRows.size());

		if (dwToRow >= dwFetchedRows)
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (DWORD dwRow = dwFetchedRows; dwRow <= dwToRow; ++dwRow)
			{
				std::unique_ptr<CDBRecord> record = std::make_unique<CDBRecord>();
				const short nColCount = pDoc->GetRecordset()->GetODBCFieldCount();
				for (short nCol = 0; nCol < nColCount; ++nCol)
				{
					pRecordset->GetFieldValue(nCol, sValue);
					record->m_arrValue.push_back(sValue);
				}
				m_arrRows.push_back(std::move(record));
				pRecordset->MoveNext();
			}
			auto end = std::chrono::high_resolution_clock::now();
			if (pDoc->IsLoggedPopulateList())
				pDoc->LogMessage(_T("Cache retrieved in: ") + pDoc->GetTimeAsString(start, end), MessageType::info);
		}
	}

	*pResult = 0;
}

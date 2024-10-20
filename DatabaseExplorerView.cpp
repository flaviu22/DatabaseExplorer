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

#include "ChildFrm.h"
#include "FileDialogEx.h"
#include "RedrawHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDatabaseExplorerView

IMPLEMENT_DYNCREATE(CDatabaseExplorerView, CListView)

BEGIN_MESSAGE_MAP(CDatabaseExplorerView, CListView)
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_RUN, &CDatabaseExplorerView::OnEditRun)
	ON_MESSAGE(WMU_POSTINIT, &CDatabaseExplorerView::OnPostInit)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CDatabaseExplorerView::OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, &CDatabaseExplorerView::OnLvnOdcachehint)
	ON_COMMAND(ID_FILE_SAVE, &CDatabaseExplorerView::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CDatabaseExplorerView::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CDatabaseExplorerView::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CDatabaseExplorerView::OnUpdateFileSave)
	ON_MESSAGE(WMU_ISPOPULATEMODE, &CDatabaseExplorerView::OnIsPopulateMode)
	ON_MESSAGE(WMU_RESTOREQUERIES, &CDatabaseExplorerView::OnRestoreQueries)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CDatabaseExplorerView::OnFilePrintPreview)
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

	auto& ListCtrl = GetListCtrl();
	if (ListCtrl.GetHeaderCtrl()->GetItemCount() > 0)
		return;

	ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

	PostMessage(WMU_POSTINIT);
	if (! GetDocument()->GetPathName().IsEmpty())
		PostMessage(WMU_RESTOREQUERIES);
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

void CDatabaseExplorerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (ID_TIMER_DBDISCONNECT == nIDEvent)
	{
		KillTimer(nIDEvent);
		CDatabaseExplorerDoc* pDoc = GetDocument();
		if (pDoc->GetDB()->IsOpen())
		{
			pDoc->GetDB()->Close();
			pDoc->LogMessage(_T("Disconnected from the current database (due to inactivity)"), MessageType::info, static_cast<CChildFrame*>(GetParentFrame()));
		}
	}

	CListView::OnTimer(nIDEvent);
}

void CDatabaseExplorerView::OnEditRun()
{
	// TODO: Add your command handler code here

	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_SETMESSAGETEXT, 0, AFX_IDS_IDLEMESSAGE);
	GetDocument()->UpdateAllViews(NULL, CDatabaseExplorerApp::UH_RUNSQLSATEMENTS);
}

void CDatabaseExplorerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class

	CDatabaseExplorerDoc* pDoc = GetDocument();

	if (lHint && CDatabaseExplorerApp::UH_POPULATEDATABASEPANEL != lHint)
		SetTimer(ID_TIMER_DBDISCONNECT, 33 * TIME_ONEMINUTE, nullptr);

	if (CDatabaseExplorerApp::UH_INITDATABASE == lHint)
	{
		const CString sDatabase = pDoc->InitDatabase();
		if (! sDatabase.IsEmpty())
		{
			if (DatabaseType::POSTGRE != pDoc->GetDatabaseType())
				pDoc->LogMessage(sDatabase + _T(" database is selected"), MessageType::info);
			else
				pDoc->LogMessage(pDoc->GetPostgreDB() + _T(" database is selected"), MessageType::info);
		}

		return;
	}

	if (CDatabaseExplorerApp::UH_SELECTDATABASE == lHint && nullptr != pHint)
	{
		pDoc->GetDatabasePane()->SetItemAsDatabase(*reinterpret_cast<CString*>(pHint));

		return;
	}

	if (CDatabaseExplorerApp::UH_POPULATEDATABASEPANEL == lHint)
	{
		m_bPopulateMode = TRUE;
		CRaiiSupport rs(m_bPopulateMode);
		GetListCtrl().DeleteAllItems();
		DeleteAllColumns();
		CChildFrame* pChild = static_cast<CChildFrame*>(GetParentFrame());
		if (nullptr != pChild->GetSafeHwnd())
		{
			if (pDoc->PopulateDatabasePanel(*pChild->GetDatabasePane()->GetTreeCtrl()))
				pDoc->SetTitle(pDoc->GetDSN().first);
			else
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_SETMESSAGETEXT, 7000, reinterpret_cast<LPARAM>(pDoc->m_sState.GetString()));
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

			BOOL bIsTableOperation = FALSE;
			BOOL bIsDatabaseOperation = FALSE;
			const CString sDatabase = pChild->GetDatabasePane()->GetDatabaseSelection();

			for (const auto& it : sql)
			{
				if (! bIsTableOperation && pDoc->IsTableOperation(it))
					bIsTableOperation = TRUE;
				if (! bIsDatabaseOperation && pDoc->IsDatabaseOperation(it))
					bIsDatabaseOperation = TRUE;
				if (! pDoc->IsSelect(it))
					ExecuteSQL(pDoc, it);
				else
					ExecuteSelect(pDoc, it);
			}

			if (bIsTableOperation || bIsDatabaseOperation)
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
	auto count = ListCtrl.GetHeaderCtrl()->GetItemCount();
	while (count > 0)
	{
		ListCtrl.DeleteColumn(0);
		count--;
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
		if (nRows > 20000)
		{
			CString sMessage;
			sMessage.Format(_T("This SELECT will get %d rows, continue ?"), nRows);
			if (IDYES != MessageBox(sMessage, NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
				return;
		}
	}

	CRedrawHelper rh(this);
	DeleteAllColumns();
	pDoc->PopulateListCtrl(GetListCtrl(), sSQL);
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
		LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);

		const DWORD dwToRow = pCacheHint->iTo;
		const DWORD dwFetchedRows = static_cast<DWORD>(m_arrRows.size());

		if (dwToRow >= dwFetchedRows)
		{
			CString sValue;
			CDatabaseExplorerDoc* pDoc = GetDocument();
			auto recordset = pDoc->GetRecordset();

			auto start = std::chrono::high_resolution_clock::now();

			for (auto dwRow = dwFetchedRows; dwRow <= dwToRow; ++dwRow)
			{
				std::unique_ptr<CDBRecord> record = std::make_unique<CDBRecord>();
				const short nColCount = recordset->GetODBCFieldCount();
				for (auto nCol = 0; nCol < nColCount; ++nCol)
				{
					recordset->GetFieldValue(nCol, sValue);
					record->m_arrValue.push_back(sValue);
				}
				m_arrRows.emplace_back(std::move(record));
				recordset->MoveNext();
			}
			auto end = std::chrono::high_resolution_clock::now();
			if (pDoc->IsLoggedPopulateList())
				pDoc->LogMessage(_T("Cache retrieved in: ") + pDoc->GetTimeAsString(start, end), MessageType::info);
		}
	}

	*pResult = 0;
}

void CDatabaseExplorerView::OnFileSave()
{
	// TODO: Add your command handler code here

	CDatabaseExplorerDoc* pDoc = GetDocument();

	if (theApp.m_bVirtualMode)
	{
		pDoc->LogMessage(_T("Saving list data is available in non virtual mode only"), MessageType::error);
		return;
	}

	CFileDialogEx dlg(FALSE, nullptr, nullptr, OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
					_T("CSV files (*.csv)|*.csv|All Files (*.*)|*.*||"));

	if (IDOK != dlg.DoModal())
		return;

	CString sPathName = dlg.GetPathName();	// if the use didn't put the extension
	if (sPathName.Find('.') < sPathName.GetLength() - 5)
		sPathName.AppendFormat(_T(".csv"));	// put .csv extension

	CWaitCursor wait;
	if (! pDoc->SaveListContentToCSV(GetListCtrl(), sPathName))
		pDoc->LogMessage(pDoc->m_sState, MessageType::error);
}

void CDatabaseExplorerView::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->Enable(GetListCtrl().GetItemCount() > 0);
}

LRESULT CDatabaseExplorerView::OnIsPopulateMode(WPARAM wParam, LPARAM lParam)
{
	return static_cast<LRESULT>(m_bPopulateMode);
}

LRESULT CDatabaseExplorerView::OnRestoreQueries(WPARAM wParam, LPARAM lParam)
{
	CDatabaseExplorerDoc* pDoc = GetDocument();
	CChildFrame* pChild = static_cast<CChildFrame*>(GetParentFrame());
	if (nullptr != pDoc && nullptr != pChild->GetSafeHwnd())
		pDoc->RestoreQueries(pChild->GetQueryPane());

	return 1;
}

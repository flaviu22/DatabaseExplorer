// DatabaseExplorerDoc.cpp : implementation of the CDatabaseExplorerDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DatabaseExplorer.h"
#endif

#include "DatabaseExplorerDoc.h"
#include "DataSourceDlg.h"
#include "DatabaseExplorerView.h"

#include <propkey.h>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDatabaseExplorerDoc

IMPLEMENT_DYNCREATE(CDatabaseExplorerDoc, CDocument)

BEGIN_MESSAGE_MAP(CDatabaseExplorerDoc, CDocument)
	ON_COMMAND(ID_EDIT_DATASOURCE, &CDatabaseExplorerDoc::OnEditDatasource)
	ON_COMMAND(ID_EDIT_LOGPOPULATELIST, &CDatabaseExplorerDoc::OnLogPopulateList)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LOGPOPULATELIST, &CDatabaseExplorerDoc::OnUpdateEditLogpopulatelist)
END_MESSAGE_MAP()

// CDatabaseExplorerDoc construction/destruction

CDatabaseExplorerDoc::CDatabaseExplorerDoc() noexcept
{
	// TODO: add one-time construction code here

	m_pDB = std::make_shared<CDatabaseExt>();
	m_pRecordset = std::make_unique<CRecordset>(m_pDB.get());
}

CDatabaseExplorerDoc::~CDatabaseExplorerDoc()
{
	theApp.WriteProfileInt(_T("Settings"), _T("LogPopulateList"), m_bLogPopulateList);
}

BOOL CDatabaseExplorerDoc::OnNewDocument()
{
	if (! CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_bLogPopulateList = theApp.GetProfileInt(_T("Settings"), _T("LogPopulateList"), m_bLogPopulateList);

	return TRUE;
}

// CDatabaseExplorerDoc serialization

void CDatabaseExplorerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CDatabaseExplorerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CDatabaseExplorerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CDatabaseExplorerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CDatabaseExplorerDoc diagnostics

#ifdef _DEBUG
void CDatabaseExplorerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDatabaseExplorerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CDatabaseExplorerDoc commands

CString CDatabaseExplorerDoc::InitDatabase()
{
	CDatabasePane* pDatabasePane = GetDatabasePane();
	if (nullptr != pDatabasePane)
	{
		CString sDatabase = pDatabasePane->GetDatabaseSelection();
		if (! sDatabase.IsEmpty())
		{
			if (m_pRecordset->IsOpen())
				m_pRecordset->Close();
			m_pDB->SetReadConectionDataFromRegistry(FALSE);
			m_pDB->SetDatabase(sDatabase);
			m_pDB->InitConnectionString();
			return sDatabase;
		}
	}

	return _T("");
}

BOOL CDatabaseExplorerDoc::IsSelect(const CString& sQuery) const
{
	CString sTemp(sQuery);
	sTemp.MakeLower();
	sTemp.TrimLeft();

	return (0 == sTemp.Find(_T("select")));
}

CChildFrame* CDatabaseExplorerDoc::GetChildFrame() const
{
	CMDIFrameWnd* pFrame = static_cast<CMDIFrameWnd*>(AfxGetMainWnd());
	if (nullptr != pFrame->GetSafeHwnd())
	{
		return static_cast<CChildFrame*>(pFrame->MDIGetActive());
	}

	return nullptr;
}

size_t CDatabaseExplorerDoc::TokenizeString(const CString& sText, const CString& sToken, std::vector<CString>& result) const
{
	int nPos = 0;
	CString sTemp = sText.Tokenize(sToken, nPos);

	while (! sTemp.IsEmpty())
	{
		result.push_back(sTemp);
		sTemp = sText.Tokenize(sToken, nPos);
	}

	return result.size();
}

CQueryPane* CDatabaseExplorerDoc::GetQueryPane() const
{
	CChildFrame* pChild = GetChildFrame();
	if (nullptr != pChild->GetSafeHwnd())
		return pChild->GetQueryPane();

	return nullptr;
}

CMessagePane* CDatabaseExplorerDoc::GetMessagePane() const
{
	CChildFrame* pChild = GetChildFrame();
	if (nullptr != pChild->GetSafeHwnd())
		return pChild->GetMessagePane();

	return nullptr;
}

CDatabasePane* CDatabaseExplorerDoc::GetDatabasePane() const
{
	CChildFrame* pChild = GetChildFrame();
	if (nullptr != pChild->GetSafeHwnd())
		return pChild->GetDatabasePane();

	return nullptr;
}

std::vector<CString> CDatabaseExplorerDoc::GetSQLStatements(const CString& sText) const
{
	std::vector<CString> sql;

	int curPos = 0;
	CString resToken = sText.Tokenize(_T(";\n"), curPos);
	while (! resToken.IsEmpty())
	{
		resToken.TrimLeft();
		resToken.TrimRight();
		if (! resToken.IsEmpty())
			sql.push_back(resToken);
		resToken = sText.Tokenize(_T(";\n"), curPos);
	}

	return sql;
}

CString CDatabaseExplorerDoc::GetTimeAsString(const std::chrono::high_resolution_clock::time_point& t1,
										const std::chrono::high_resolution_clock::time_point& t2) const
{
	const auto dur = t2 - t1;
	std::chrono::hours h = std::chrono::duration_cast<std::chrono::hours>(dur);
	std::chrono::minutes m = std::chrono::duration_cast<std::chrono::minutes>(dur);
	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(dur);
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);

	CString sRet;

	if (h.count() > 0)
	{
		sRet.Format(_T("%.2d:%.2d:%.2I64d hours"), h.count(), m.count() % (h.count() * 60), s.count() % (m.count() * 3600));
		return sRet;
	}

	if (m.count() > 0)
	{
		sRet.Format(_T("%d:%.2I64d minutes"), m.count(), s.count() % (m.count() * 60));
		return sRet;
	}

	if (s.count() > 0)
	{
		sRet.Format(_T("%I64d.%.3I64d seconds"), s.count(), (ms.count() % (s.count() * 1000)));
		return sRet;
	}

	sRet.Format(_T("%I64d miliseconds"), ms.count());

	return sRet;
}

void CDatabaseExplorerDoc::PopulateHeader(CListCtrl& ListCtrl, CRecordset& recordset)
{
	CString sText;
	CODBCFieldInfo fieldinfo;
	const short nColumns = recordset.GetODBCFieldCount();
	for (short nColumn = 0; nColumn < nColumns; ++nColumn)
	{
		recordset.GetODBCFieldInfo(nColumn, fieldinfo);
		if(fieldinfo.m_strName.IsEmpty())
			sText.Format(_T("   "));
		else
			sText.Format(_T("%s"), fieldinfo.m_strName);
		ListCtrl.InsertColumn(nColumn, sText, LVCFMT_LEFT, 200);
	}
}

BOOL CDatabaseExplorerDoc::PopulateListCtrl(CDatabaseExplorerView* pView, const CString& sSQL)
{
	m_sState.Empty();
	std::chrono::high_resolution_clock::time_point start, end;

	if (nullptr == pView->GetSafeHwnd())
	{
		m_sState.Format(_T("View pointer is null"));
		return FALSE;
	}

	if (m_pRecordset->IsOpen())
		m_pRecordset->Close();

	long nTotalCount = 0;
	CListCtrl& ListCtrl = pView->GetListCtrl();

	do
	{
		LogMessage(sSQL, MessageType::info);

		start = std::chrono::high_resolution_clock::now();

		if (! m_pDB->OpenRecordset(m_pRecordset.get(), sSQL))
		{
			m_sState = m_pDB->GetError();
			break;
		}

		end = std::chrono::high_resolution_clock::now();

		LogMessage(_T("SQL select took: ") + GetTimeAsString(start, end), MessageType::info);

		if (m_pRecordset->GetODBCFieldCount() < 1)
		{
			m_sState.Format(_T("SQL result has no field"));
			break;
		}

		PopulateHeader(ListCtrl, *m_pRecordset.get());

		if (m_bLogPopulateList)
			start = std::chrono::high_resolution_clock::now();

		if (theApp.m_bVirtualMode)
		{
			nTotalCount = GetRecordCount(sSQL);
			if (nTotalCount < 0)
				break;
			ListCtrl.SetItemCountEx(nTotalCount, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
		}
		else	// not in virtual mode
		{
			int nRow = 0;
			CString sValue;
			while (! m_pRecordset->IsEOF())
			{
				for (int i = 0; i < m_pRecordset->GetODBCFieldCount(); ++i)
				{
					m_pRecordset->GetFieldValue(i, sValue);
					if (0 == i)
						ListCtrl.InsertItem(nRow, sValue);
					else
						ListCtrl.SetItemText(nRow, i, sValue);
				}
				m_pRecordset->MoveNext();
				nRow++;
			}
		}

		if (m_bLogPopulateList)
			end = std::chrono::high_resolution_clock::now();
	} while (FALSE);

	if (! theApp.m_bVirtualMode)
		m_pRecordset->Close();

	if (! m_sState.IsEmpty())
	{
		LogMessage(m_sState, MessageType::error);
	}
	else if (m_bLogPopulateList)
	{
		CString sMessage;
		if (theApp.m_bVirtualMode)
			sMessage.Format(_T("%d rows, populated in %s"), nTotalCount, GetTimeAsString(start, end));
		else
			sMessage.Format(_T("%d rows, populated in %s"), ListCtrl.GetItemCount(), GetTimeAsString(start, end));
		LogMessage(sMessage, MessageType::info);
	}

	return m_sState.IsEmpty();
}

BOOL CDatabaseExplorerDoc::PopulateDatabasePanel(CTreeCtrl& tree)
{
	m_sState.Empty();

	tree.DeleteAllItems();

	if (GetDatabaseMSSQL(tree) || GetDatabaseOracle(tree) ||
		GetDatabaseMySql(tree) || GetDatabasePostgre(tree) ||
		GetDatabaseInformix(tree) || GetDatabaseMariaDB(tree))
		return TRUE;

	return FALSE;
}

BOOL CDatabaseExplorerDoc::GetDatabaseMSSQL(CTreeCtrl& tree)
{
	std::vector<std::string> database{};
	if (! m_pDB->GetRecordsetVector(database, _T("SELECT database_id, name FROM sys.databases ORDER BY 2")))
	{
		m_sState.Format(_T("%s"), m_pDB->GetError());
	}
	else
	{
		m_DatabaseType = DatabaseType::MSSQL;

		std::vector<CString> table{};

		for (auto it = database.begin(); it != database.end(); it += 2)
		{
			table.clear();
			HTREEITEM hItem = tree.InsertItem(CString((it + 1)->c_str()), 0, 0);
			tree.SetItemData(hItem, std::atoi(it->c_str()));
			if (! m_pDB->GetRecordsetVectorV(table, 
				_T("SELECT table_name FROM %s.information_schema.tables ORDER BY 1"), CString((it + 1)->c_str()))
				|| table.empty())
				continue;
			for (auto it_table = table.begin(); it_table != table.end(); ++it_table)
			{
				HTREEITEM hChild = tree.InsertItem(*it_table, 1, 1, hItem);
				tree.SetItemData(hChild, 0);
			}
		}
	}

	return m_sState.IsEmpty();
}

BOOL CDatabaseExplorerDoc::GetDatabaseOracle(CTreeCtrl& tree)
{
	m_DatabaseType = DatabaseType::ORACLE;

	return m_sState.IsEmpty();
}

BOOL CDatabaseExplorerDoc::GetDatabaseMySql(CTreeCtrl& tree)
{
	m_DatabaseType = DatabaseType::MYSQL;

	return m_sState.IsEmpty();
}

BOOL CDatabaseExplorerDoc::GetDatabasePostgre(CTreeCtrl& tree)
{
	m_DatabaseType = DatabaseType::POSTGRE;

	return m_sState.IsEmpty();
}

BOOL CDatabaseExplorerDoc::GetDatabaseInformix(CTreeCtrl& tree)
{
	m_DatabaseType = DatabaseType::INFORMIX;

	return m_sState.IsEmpty();
}

BOOL CDatabaseExplorerDoc::GetDatabaseMariaDB(CTreeCtrl& tree)
{
	m_DatabaseType = DatabaseType::MARIADB;

	return m_sState.IsEmpty();
}

void CDatabaseExplorerDoc::OnEditDatasource()
{
	// TODO: Add your command handler code here

	m_pDB->SetReadConectionDataFromRegistry();
	CDataSourceDlg dlg(this);
	if (IDOK == dlg.DoModal())
	{
		UpdateAllViews(NULL, CDatabaseExplorerApp::UH_POPULATEDATABASEPANEL);
		UpdateAllViews(NULL, CDatabaseExplorerApp::UH_SELECTDATABASE, reinterpret_cast<CObject*>(const_cast<CString*>(&m_pDB->GetDatabase())));
		UpdateAllViews(NULL, CDatabaseExplorerApp::UH_INITDATABASE);
	}
}

void CDatabaseExplorerDoc::OnLogPopulateList()
{
	m_bLogPopulateList = ! m_bLogPopulateList;
}

void CDatabaseExplorerDoc::OnUpdateEditLogpopulatelist(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(m_bLogPopulateList);
}

void CDatabaseExplorerDoc::LogMessage(const CString& sMessage, const MessageType& type)
{
	CString sText;
	COleDateTime dtNow = COleDateTime::GetCurrentTime();
	sText.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d - %s"), 
		dtNow.GetYear(), dtNow.GetMonth(), dtNow.GetDay(),
		dtNow.GetHour(), dtNow.GetMinute(), dtNow.GetSecond(),
		sMessage);

	CMDIFrameWnd* pFrame = static_cast<CMDIFrameWnd*>(AfxGetMainWnd());
	if (nullptr != pFrame->GetSafeHwnd())
	{
		CChildFrame* pChild = static_cast<CChildFrame*>(pFrame->MDIGetActive());
		if (nullptr != pChild->GetSafeHwnd())
		{
			pChild->SetMessage(sText, MessageType::error == type ? Color::red : Color::black);
		}
	}
}

BOOL CDatabaseExplorerDoc::RunSQL(const CString sSQL)
{
	m_sState.Empty();

	auto start = std::chrono::high_resolution_clock::now();

	if (! m_pDB->Execute(sSQL))
		m_sState = m_pDB->GetError();

	auto end = std::chrono::high_resolution_clock::now();

	LogMessage(sSQL, MessageType::info);
	if (m_sState.IsEmpty())
		LogMessage(_T("SQL statement took: ") + GetTimeAsString(start, end), MessageType::info);
	else
		LogMessage(m_sState, MessageType::error);

	return m_sState.IsEmpty();
}

inline CString CDatabaseExplorerDoc::ConvertDataAsString(const CDBVariant& variant)
{
	CString sRet;

	switch (variant.m_dwType)
	{
	case DBVT_LONG:
		sRet.Format(_T("%d"), variant.m_lVal);
		break;
	case DBVT_DOUBLE:
		sRet.Format(_T("%.2f"), variant.m_dblVal);
		break;
	case DBVT_DATE:
		{
			COleDateTime date(variant.m_pdate->year, variant.m_pdate->month,
				variant.m_pdate->day, variant.m_pdate->hour,
				variant.m_pdate->minute, variant.m_pdate->second);
			sRet = date.Format();
		}
		break;
	case DBVT_STRING:
	case DBVT_ASTRING:
		sRet = *variant.m_pstring;
		break;
	case DBVT_WSTRING:
		sRet = CString(*variant.m_pstringW);
		break;
	default:
		break;
	}

	return sRet;
}

int CDatabaseExplorerDoc::GetDatabaseCount() const
{
	std::vector<CDBVariant> val;
	if (! m_pDB->GetRecordsetVector(val, _T("SELECT count(*) FROM sys.databases")))
		return -1;

	if (1 == val.size())
		return static_cast<int>(val.at(0).m_lVal);

	return 0;
}

long CDatabaseExplorerDoc::GetRecordCount(const CString& sSQL)
{
	std::vector<CDBVariant> val;

	if (! m_pDB->GetRecordsetVector(val, PrepareSQLForCountAll(sSQL)))
	{
		m_sState = m_pDB->GetError();
		return -1;
	}

	if (1 == val.size())
		return static_cast<long>(val.at(0).m_lVal);

	return 0;
}

CString CDatabaseExplorerDoc::PrepareSQLForCountAll(const CString& sSQL)
{
	CString sTemp(sSQL);
	sTemp.TrimLeft();
	sTemp.TrimRight();
	sTemp.MakeLower();

	const int nSelect = sTemp.Find(_T("select"));
	const int nFrom = sTemp.Find(_T("from"), 6);

	if (0 != nSelect || nFrom < 6)
	{
		ASSERT(_T("SELECT statement is not valid"));
		return sSQL;
	}

	CString sSQLNew;
	sSQLNew.Format(_T("SELECT count(*) %s"), sSQL.Right(sSQL.GetLength() - nFrom));

	CString sSQLTemp(sSQLNew);
	sSQLTemp.MakeLower();

	const int nIndexOrderBy = sSQLNew.Find(_T("order by"));

	if (nIndexOrderBy > 0)	// remove 'ORDER BY' condition
	{
		int nIndexAfterOrderBy = sSQLTemp.Find(_T("limit"), nIndexOrderBy);
		if (nIndexAfterOrderBy < 0)
		{
			nIndexAfterOrderBy = sSQLTemp.Find(_T("offset"), nIndexOrderBy);
			if (nIndexAfterOrderBy < 0)
			{
				nIndexAfterOrderBy = sSQLTemp.Find(_T("fetch"), nIndexOrderBy);
				if (nIndexAfterOrderBy < 0)
				{
					nIndexAfterOrderBy = sSQLTemp.Find(_T("option"), nIndexOrderBy);
				}
			}
		}

		CString sLeft = sSQLNew.Left(nIndexOrderBy);

		if (nIndexAfterOrderBy > 0)
		{
			CString sRight = sSQLNew.Right(sSQLNew.GetLength() - nIndexAfterOrderBy);
			sSQLNew = sLeft + sRight;
		}
		else
		{
			sSQLNew = sLeft;
		}
	}

	return sSQLNew;
}

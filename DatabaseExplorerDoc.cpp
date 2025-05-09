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

#include "ChildFrm.h"

#include <propkey.h>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////
// CRaiiSupport

CRaiiSupport::CRaiiSupport(BOOL& bFlag)
	:m_bFlag(bFlag)
{
}

CRaiiSupport::~CRaiiSupport()
{
	m_bFlag = ! m_bFlag;
}

// CDatabaseExplorerDoc

IMPLEMENT_DYNCREATE(CDatabaseExplorerDoc, CDocument)

BEGIN_MESSAGE_MAP(CDatabaseExplorerDoc, CDocument)
	ON_COMMAND(ID_EDIT_DATASOURCE, &CDatabaseExplorerDoc::OnEditDatasource)
	ON_COMMAND(ID_EDIT_LOGPOPULATELIST, &CDatabaseExplorerDoc::OnLogPopulateList)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LOGPOPULATELIST, &CDatabaseExplorerDoc::OnUpdateEditLogpopulatelist)
	ON_COMMAND(ID_EDIT_DISCONNECT, &CDatabaseExplorerDoc::OnEditDisconnect)
END_MESSAGE_MAP()

// CDatabaseExplorerDoc construction/destruction

CDatabaseExplorerDoc::CDatabaseExplorerDoc() noexcept
{
	// TODO: add one-time construction code here

	m_pDB = std::make_unique<CDatabaseExt>();
	m_pRecordset = std::make_unique<CRecordset>(m_pDB.get());

	m_bLogPopulateList = theApp.GetProfileInt(_T("Settings"), _T("LogPopulateList"), m_bLogPopulateList);

	SetDSN(theApp.GetProfileString(_T("Settings"), _T("DSN")));
	m_bDSNSource = static_cast<BOOL>(theApp.GetProfileInt(_T("Settings"), _T("DSNSource"), 0));
	m_DatabaseType = static_cast<DatabaseType>(theApp.GetProfileInt(_T("Settings"), _T("DatabaseType"), static_cast<int>(DatabaseType::MSSQL)));
	m_pDB->SetRecordsetType(theApp.GetProfileInt(_T("Settings"), _T("RSType"), CRecordset::dynaset));
	m_bMsSqlAuthenticationRequired = theApp.GetProfileInt(_T("Settings"), _T("MsSqlAuthenticationRequired"), m_bMsSqlAuthenticationRequired);
}

CDatabaseExplorerDoc::~CDatabaseExplorerDoc()
{
	theApp.WriteProfileInt(_T("Settings"), _T("LogPopulateList"), m_bLogPopulateList);
	theApp.WriteProfileInt(_T("Settings"), _T("DSNSource"), m_bDSNSource);
	theApp.WriteProfileInt(_T("Settings"), _T("DatabaseType"), static_cast<int>(m_DatabaseType));
	theApp.WriteProfileString(_T("Settings"), _T("DSN"), m_DSN.first);
	theApp.WriteProfileInt(_T("Settings"), _T("MsSqlAuthenticationRequired"), m_bMsSqlAuthenticationRequired);
}

BOOL CDatabaseExplorerDoc::OnNewDocument()
{
	if (! CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	SetConnectionString();

	return TRUE;
}

BOOL CDatabaseExplorerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (! CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO: Add your specialized code here and/or call the base class

	SetDSN(theApp.GetFileNameFrom(lpszPathName));
	m_bDSNSource = static_cast<BOOL>(theApp.GetProfileInt(_T("Backup"), m_DSN.first + STR_DSNSOURCE, 0));
	m_DatabaseType = static_cast<DatabaseType>(theApp.GetProfileInt(_T("Backup"), m_DSN.first + STR_DATABASETYPE, static_cast<int>(m_DatabaseType)));
	m_pDB->SetRecordsetType(theApp.GetProfileInt(_T("Backup"), m_DSN.first + STR_RSTYPE, CRecordset::dynaset));
	m_bMsSqlAuthenticationRequired = theApp.GetProfileInt(_T("Backup"), m_DSN.first + STR_MSSQLAUTHENTICATIONREQUIRED, m_bMsSqlAuthenticationRequired);

	SetConnectionString();

	return TRUE;
}

void CDatabaseExplorerDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class

	CDocument::OnCloseDocument();
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

auto CDatabaseExplorerDoc::GetDSN() const -> std::pair<CString, CString>
{
	return m_DSN;
}

void CDatabaseExplorerDoc::SetDSN(const CString& sName)
{
	m_DSN.first = sName;

	CSettingsStoreEx ss(m_bDSNSource, TRUE);
	if (ss.Open(_T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources")))
	{
#ifdef _UNICODE
		std::unordered_map<std::wstring, std::wstring> keys;
#else
		std::unordered_map<std::string, std::string> keys;
#endif // _UNICODE
		ss.EnumValues(keys);
		const auto it = keys.find(m_DSN.first.GetString());
		if (it != keys.end())
			m_DSN.second = it->second.c_str();
	}
}

void CDatabaseExplorerDoc::SetConnectionString() const
{
	if (m_pDB)
	{
		if (! m_bMsSqlAuthenticationRequired)
		{
			m_pDB->SetConnectionString(_T("DSN=") + m_DSN.first + _T(";"));
		}
		else
		{
			const auto credential = GetMsSqlAuthenticationCredential();
			m_pDB->SetConnectionString(_T("DSN=") + m_DSN.first + 
										_T(";UID=") + credential.first + 
										_T(";PWD=") + credential.second + _T(";"));
		}
	}
}

void CDatabaseExplorerDoc::OnEditDisconnect()
{
	// TODO: Add your command handler code here

	if (m_pDB->IsOpen())
	{
		m_pDB->Close();
		LogMessage(_T("Disconnected from the current database."), MessageType::info, GetChildFrame());
	}
}

const CString CDatabaseExplorerDoc::DecodePostGreDatabase(const CString& sConnectionString) const
{
	CString sRet(_T("postgres"));
	CString sConn(sConnectionString);
	sConn.MakeLower();
	const CString sSearch(_T("database="));
	int nIndex1 = sConn.Find(sSearch);
	if (nIndex1 > 0)
	{
		nIndex1 += sSearch.GetLength();
		const int nIndex2 = sConn.Find(';', nIndex1);
		if (nIndex2 > nIndex1)
			sRet = sConnectionString.Mid(nIndex1, nIndex2 - nIndex1);
	}

	return sRet;
}

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
			m_pDB->ExecuteV(_T("USE %s;"), sDatabase);
			return sDatabase;
		}
	}

	return _T("");
}

BOOL CDatabaseExplorerDoc::IsSelect(CString sQuery) const
{
	sQuery.MakeLower();
	sQuery.TrimLeft();

	return (0 == sQuery.Find(_T("select")));
}

BOOL CDatabaseExplorerDoc::IsTableOperation(CString sQuery) const
{
	sQuery.MakeLower();
	sQuery.TrimLeft();

	return (-1 != sQuery.Find(_T("table")) || - 1 != sQuery.Find(_T("exec")));
}

BOOL CDatabaseExplorerDoc::IsDatabaseOperation(CString sQuery) const
{
	sQuery.MakeLower();
	sQuery.TrimLeft();

	return (-1 != sQuery.Find(_T("database")) || -1 != sQuery.Find(_T("create")));
}

CChildFrame* CDatabaseExplorerDoc::GetChildFrame() const
{
	CMDIFrameWnd* pFrame = static_cast<CMDIFrameWnd*>(AfxGetMainWnd());
	if (nullptr != pFrame->GetSafeHwnd())
		return static_cast<CChildFrame*>(pFrame->MDIGetActive());

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
		if (fieldinfo.m_strName.IsEmpty())
			sText.Format(_T("(%d) -"), nColumn);
		else
			sText.Format(_T("(%d) %s"), nColumn, fieldinfo.m_strName);
		ListCtrl.InsertColumn(nColumn, sText, LVCFMT_LEFT, 200);
	}
}

BOOL CDatabaseExplorerDoc::PopulateListCtrl(CListCtrl& ListCtrl, const CString& sSQL)
{
	m_sError.Empty();
	std::chrono::high_resolution_clock::time_point start{}, end{};

	if (m_pRecordset->IsOpen())
		m_pRecordset->Close();

	long nColumns{};
	long nTotalCount{};

	do
	{
		LogMessage(sSQL, MessageType::info);

		start = std::chrono::high_resolution_clock::now();

		if (! m_pDB->OpenRecordset(m_pRecordset.get(), sSQL))
		{
			m_sError = m_pDB->GetError();
			break;
		}

		end = std::chrono::high_resolution_clock::now();

		LogMessage(_T("SQL select have took: ") + GetTimeAsString(start, end), MessageType::info);

		if (m_pRecordset->GetODBCFieldCount() < 1)
		{
			m_sError.Format(_T("SQL result has no field"));
			break;
		}

		nColumns = m_pRecordset->GetODBCFieldCount();
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
			int nRow{ 0 };
			CDBVariant var{};
			const short zero{ 0 };
			while (!m_pRecordset->IsEOF())
			{
				for (int i = 0; i < m_pRecordset->GetODBCFieldCount(); ++i)
				{
					var.Clear();
					m_pRecordset->GetFieldValue(static_cast<short>(i), var);
					if (zero == i)
						ListCtrl.InsertItem(nRow, ConvertToCString(var));
					else
						ListCtrl.SetItemText(nRow, i, ConvertToCString(var));
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

	if (! m_sError.IsEmpty())
	{
		LogMessage(m_sError, MessageType::error);
	}
	else if (m_bLogPopulateList)
	{
		CString sMessage;
		if (theApp.m_bVirtualMode)
			sMessage.Format(_T("%d rows, %d columns, populated in %s"), nTotalCount, nColumns, GetTimeAsString(start, end));
		else
			sMessage.Format(_T("%d rows, %d columns, populated in %s"), ListCtrl.GetItemCount(), nColumns, GetTimeAsString(start, end));
		LogMessage(sMessage, MessageType::info);
	}

	return m_sError.IsEmpty();
}

BOOL CDatabaseExplorerDoc::PopulateDatabasePanel(CTreeCtrl& tree)
{
	m_sError.Empty();
	tree.DeleteAllItems();

	BOOL bReturn{ FALSE };

	switch (m_DatabaseType)
	{
	case DatabaseType::SQLITE:
		bReturn = PopulateSQLite(tree);
		break;
	case DatabaseType::ORACLE:
		bReturn = PopulateOracle(tree);
		break;
	case DatabaseType::MYSQL:
	case DatabaseType::MARIADB:
		bReturn = PopulateMySql(tree);
		break;
	case DatabaseType::POSTGRE:
		bReturn = PopulatePostgre(tree);
		break;
	case DatabaseType::MSSQL:
	default:
		bReturn = PopulateMSSQL(tree);
		break;
	}

	if (1 == GetRootCount(tree))
		tree.Expand(tree.GetRootItem(), TVE_EXPAND);

	return bReturn;
}

BOOL CDatabaseExplorerDoc::PopulateMSSQL(CTreeCtrl& tree)
{
	const auto database = m_pDB->GetData(_T("SELECT database_id, name FROM sys.databases ORDER BY 2"));
	if (! m_pDB->GetError().IsEmpty())
	{
		m_sError.Format(_T("%s"), m_pDB->GetError());
	}
	else
	{
		for (auto it = database.begin(); it != database.end(); it += 2)
		{
			HTREEITEM hItem = tree.InsertItem(CString((it + 1)->c_str()), 0, 0);
			tree.SetItemData(hItem, std::atoi(it->c_str()));
			const auto table = m_pDB->GetDataV(_T("SELECT table_name FROM %s.information_schema.tables ORDER BY 1"), CString((it + 1)->c_str()));
			if (! m_pDB->GetError().IsEmpty() || table.empty())
				continue;
			for (auto it_table = table.begin(); it_table != table.end(); ++it_table)
				tree.SetItemData(tree.InsertItem(CString(it_table->c_str()), 1, 1, hItem), 0);
		}
	}

	return m_sError.IsEmpty();
}

BOOL CDatabaseExplorerDoc::PopulateOracle(CTreeCtrl& tree)
{
	const auto database = m_pDB->GetData(_T("SELECT 1, global_name FROM global_name ORDER BY 2"));
	if (! m_pDB->GetError().IsEmpty())
	{
		m_sError.Format(_T("%s"), m_pDB->GetError());
	}
	else
	{
		const CString sUserID = GetOracleUserID(TRUE);
		for (auto& it = database.begin(); it != database.end(); it += 2)
		{
			HTREEITEM hItem = tree.InsertItem(CString((it + 1)->c_str()), 0, 0);
			tree.SetItemData(hItem, std::atoi(it->c_str()));
			const auto table = m_pDB->GetDataV(_T("SELECT table_name FROM dba_tables WHERE owner = '%s' AND tablespace_name = '%s' ORDER BY 1"), sUserID, sUserID);
			if (! m_pDB->GetError().IsEmpty() || table.empty())
				continue;
			for (auto it_table = table.begin(); it_table != table.end(); ++it_table)
				tree.SetItemData(tree.InsertItem(CString(it_table->c_str()), 1, 1, hItem), 0);
		}
	}
	return m_sError.IsEmpty();
}

BOOL CDatabaseExplorerDoc::PopulateSQLite(CTreeCtrl& tree)
{
	const auto database = m_pDB->GetData(_T("SELECT 1, file FROM pragma_database_list WHERE name='main'"));
	if (! m_pDB->GetError().IsEmpty())
	{
		m_sError.Format(_T("%s"), m_pDB->GetError());
	}
	else
	{
		for (auto it = database.cbegin(); it != database.cend(); it += 2)
		{
			HTREEITEM hItem = tree.InsertItem(CString((it + 1)->c_str()), 0, 0);
			tree.SetItemData(hItem, std::atoi(it->c_str()));
			const auto table = m_pDB->GetData(_T("SELECT tbl_name FROM sqlite_master WHERE type = 'table' AND name NOT LIKE 'sqlite_%' ORDER BY 1"));
			if (! m_pDB->GetError().IsEmpty() || table.empty())
				continue;
			for (auto& it_table = table.begin(); it_table != table.end(); ++it_table)
				tree.SetItemData(tree.InsertItem(CString(it_table->c_str()), 1, 1, hItem), 0);
		}
	}

	return m_sError.IsEmpty();
}

BOOL CDatabaseExplorerDoc::PopulateMySql(CTreeCtrl& tree)
{
	const auto database = m_pDB->GetData(_T("SELECT 1, schema_name FROM information_schema.schemata ORDER BY 2"));
	if (! m_pDB->GetError().IsEmpty())
	{
		m_sError.Format(_T("%s"), m_pDB->GetError());
	}
	else
	{
		for (auto it = database.begin(); it != database.end(); it += 2)
		{
			HTREEITEM hItem = tree.InsertItem(CString((it + 1)->c_str()), 0, 0);
			tree.SetItemData(hItem, std::atoi(it->c_str()));
			const auto table = m_pDB->GetDataV(_T("SELECT table_name FROM information_schema.tables WHERE table_schema = '%s' AND table_type = 'BASE TABLE' ORDER BY 1"), CString((it + 1)->c_str()));
			if (! m_pDB->GetError().IsEmpty() || table.empty())
				continue;
			for (auto it_table = table.begin(); it_table != table.end(); ++it_table)
				tree.SetItemData(tree.InsertItem(CString(it_table->c_str()), 1, 1, hItem), 0);
		}
	}
	return m_sError.IsEmpty();
}

BOOL CDatabaseExplorerDoc::PopulatePostgre(CTreeCtrl& tree)
{
	const auto database = m_pDB->GetDataV(_T("SELECT 1, datname FROM pg_database WHERE datistemplate = false ORDER BY 2"));
	if (! m_pDB->GetError().IsEmpty())
	{
		m_sError.Format(_T("%s"), m_pDB->GetError());
	}
	else
	{
		SetPostgreDB(DecodePostGreDatabase(m_pDB->GetConnect()));
		for (auto it = database.begin(); it != database.end(); it += 2)
		{
			HTREEITEM hItem = tree.InsertItem(CString((it + 1)->c_str()), 0, 0);
			tree.SetItemData(hItem, std::atoi(it->c_str()));
			if (m_sPostgreDB != CString((it + 1)->c_str()))
				continue;
			const auto table = m_pDB->GetData(_T("SELECT tablename FROM pg_catalog.pg_tables WHERE schemaname != 'pg_catalog' AND schemaname != 'information_schema' ORDER BY 1"));
			if (! m_pDB->GetError().IsEmpty() || table.empty())
				continue;
			for (auto it_table = table.begin(); it_table != table.end(); ++it_table)
				tree.SetItemData(tree.InsertItem(CString(it_table->c_str()), 1, 1, hItem), 0);
		}
	}
	return m_sError.IsEmpty();
}

void CDatabaseExplorerDoc::OnEditDatasource()
{
	// TODO: Add your command handler code here

	CDatabasePane* pDatabasePane = GetDatabasePane();
	if (nullptr == pDatabasePane->GetSafeHwnd())
		return;

	CDataSourceDlg dlg(this);
	const auto ret = dlg.DoModal();

	SetConnectionString();
	CString sDatabase = pDatabasePane->GetDatabaseSelection();
	if (IDOK == ret)
		UpdateAllViews(NULL, CDatabaseExplorerApp::UH_POPULATEDATABASEPANEL);
	UpdateAllViews(NULL, CDatabaseExplorerApp::UH_SELECTDATABASE, reinterpret_cast<CObject*>(&sDatabase));
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

void CDatabaseExplorerDoc::LogMessage(const CString& sMessage, const MessageType& type, CChildFrame* pChild/* = nullptr*/) const
{
	if (nullptr == pChild)
		pChild = GetChildFrame();

	CString sText;
	COleDateTime dtNow = COleDateTime::GetCurrentTime();
	sText.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d - %s"), 
		dtNow.GetYear(), dtNow.GetMonth(), dtNow.GetDay(),
		dtNow.GetHour(), dtNow.GetMinute(), dtNow.GetSecond(),
		sMessage);

	if (nullptr != pChild && nullptr != pChild->GetSafeHwnd())
		pChild->SetMessage(sText, MessageType::error == type ? Color::red : Color::black);
}

BOOL CDatabaseExplorerDoc::RunSQL(const CString sSQL) const
{
	m_sError.Empty();

	const auto start = std::chrono::high_resolution_clock::now();

	if (! m_pDB->Execute(sSQL))
		m_sError = m_pDB->GetError();

	const auto end = std::chrono::high_resolution_clock::now();

	LogMessage(sSQL, MessageType::info);
	if (m_sError.IsEmpty())
		LogMessage(_T("SQL statement took: ") + GetTimeAsString(start, end), MessageType::info);
	else
		LogMessage(m_sError, MessageType::error);

	return m_sError.IsEmpty();
}

int CDatabaseExplorerDoc::GetDatabaseCount() const
{
	CString sSQL;
	switch (m_DatabaseType)
	{
	case DatabaseType::MSSQL:
		sSQL.Format(_T("SELECT count(*) FROM sys.databases"));
		break;
	case DatabaseType::ORACLE:
		sSQL.Format(_T("SELECT count(*) FROM GLOBAL_NAME"));
		break;
	case DatabaseType::SQLITE:
		return 1;
	case DatabaseType::MYSQL:
	case DatabaseType::MARIADB:
		sSQL.Format(_T("SELECT count(*) FROM information_schema.schemata"));
		break;
	case DatabaseType::POSTGRE:
		sSQL.Format(_T("SELECT count(*) FROM pg_database"));
		break;
	default:
		sSQL.Format(_T("SELECT 0"));
		break;
	}

	const auto val = m_pDB->GetData(sSQL);
	if (! m_pDB->GetError().IsEmpty())
		return -1;

	if (1 == val.size())
		return std::atoi(val.at(0).c_str());

	return 0;
}

long CDatabaseExplorerDoc::GetRecordCount(const CString& sSQL) const
{
	const auto val = m_pDB->GetData(PrepareSQLForCountAll(sSQL));

	if (! m_pDB->GetError().IsEmpty())
	{
		m_sError = m_pDB->GetError();
		return -1;
	}

	if (1 == val.size())
		return std::atol((val.at(0).c_str()));

	return 0;
}

CString CDatabaseExplorerDoc::PrepareSQLForCountAll(const CString& sSQL) const
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

CString CDatabaseExplorerDoc::GetText(CHeaderCtrl& header, int nItem) const
{
	HD_ITEM hdi = { 0 };
	hdi.mask = HDI_TEXT;
	std::vector<wchar_t> text(127);
	hdi.pszText = text.data();
	hdi.cchTextMax = static_cast<int>(text.size());
	header.GetItem(nItem, &hdi);
	return hdi.pszText;
}

std::vector<CString> CDatabaseExplorerDoc::GetHeaderItems(CListCtrl& ListCtrl)
{
	std::vector<CString> items;

	CHeaderCtrl* pHeader = ListCtrl.GetHeaderCtrl();
	if (pHeader)
	{
		const int nCount = pHeader->GetItemCount();
		for (int i = 0; i < nCount; ++i)
			items.push_back(GetText(*pHeader, i));
	}

	return items;
}
// write first line with the header items
void CDatabaseExplorerDoc::WriteHeaderLine(CListCtrl& ListCtrl, CStdioFile& file, const CString& sSeparator)
{
	CString sLine;
	const auto header_items = GetHeaderItems(ListCtrl);
	for (auto& it = header_items.cbegin(); it != header_items.end(); ++it)
	{
		if (it != header_items.end() - 1)
			sLine.AppendFormat(_T("%s%s"), *it, sSeparator);
		else
			sLine.AppendFormat(_T("%s"), *it);
	}
	sLine.AppendFormat(_T("\n"));
	file.WriteString(sLine);
}
// write every line with the list items
void CDatabaseExplorerDoc::WriteListLines(CListCtrl& ListCtrl, CStdioFile& file, const CString& sSeparator)
{
	CString sLine;
	const int nCount = ListCtrl.GetItemCount();
	const int nHeaderCount = ListCtrl.GetHeaderCtrl()->GetItemCount();

	for (int i = 0; i < nCount; ++i)
	{
		sLine.Empty();
		for (int c = 0; c < nHeaderCount; ++c)
		{
			if (c < nHeaderCount - 1)
				sLine.AppendFormat(_T("%s%s"), ListCtrl.GetItemText(i, c), sSeparator);
			else
				sLine.AppendFormat(_T("%s"), ListCtrl.GetItemText(i, c));
		}
		sLine.AppendFormat(_T("\n"));
		file.WriteString(sLine);
	}
}

BOOL CDatabaseExplorerDoc::SaveListContentToCSV(CListCtrl& ListCtrl, const CString& sPathName)
{
	m_sError.Empty();

	const CString sSeparator = AfxGetApp()->GetProfileString(_T("Settings"), _T("CSVSeparator"), _T(","));

	do
	{
		CStdioFile file;
		CFileException ex;
		if (! file.Open(sPathName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText, &ex))
		{
			ex.GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), MAX_PATH);
			m_sError.ReleaseBuffer();
			break;
		}
		WriteHeaderLine(ListCtrl, file, sSeparator);
		WriteListLines(ListCtrl, file, sSeparator);
	} while (FALSE);

	return m_sError.IsEmpty();
}

CString CDatabaseExplorerDoc::GetOracleUserID(const BOOL bMakeUpper/* = FALSE*/) const
{
	CString sUserID, sPath;
	sPath.Format(_T("SOFTWARE\\ODBC\\ODBC.INI\\%s"), m_DSN.first);
	CSettingsStore ss(m_bDSNSource, TRUE);
	if (ss.Open(sPath))
		ss.Read(_T("UserID"), sUserID);

	if (bMakeUpper)
		return sUserID.MakeUpper();

	return sUserID;
}

std::pair<CString, CString> CDatabaseExplorerDoc::GetMsSqlAuthenticationCredential() const
{
	CString sPath;
	std::pair<CString, CString> credential;
	sPath.Format(_T("SOFTWARE\\ODBC\\ODBC.INI\\%s"), m_DSN.first);
	CSettingsStore ss(m_bDSNSource, TRUE);
	if (ss.Open(sPath))
	{
		CString sUser, sPass;
		ss.Read(_T("LastUser"), sUser);
		ss.Read(_T("Password"), sPass);
		credential.first = sUser;
		credential.second = sPass;
	}
	return credential;
}

void CDatabaseExplorerDoc::RestoreQueries(CQueryPane* pPane) const
{
	if (nullptr == pPane->GetSafeHwnd())
		return;

	const CString sBackupFile = theApp.GetAppPath() + _T("Backup\\") + m_DSN.first;
	if (! theApp.FileExist(sBackupFile))
		return;

	pPane->SetText(GetQueries(sBackupFile));
}

std::vector<CString> CDatabaseExplorerDoc::GetQueries(const CString& sFile) const
{
	CStdioFile file;
	CFileException ex;
	std::vector<CString> queries;

	if (! file.Open(sFile, CFile::modeRead | CFile::typeText, &ex))
	{
		ex.GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
		m_sError.ReleaseBuffer();
		return queries;
	}

	CString sLine;
	while (file.ReadString(sLine))
		queries.push_back(sLine);

	return queries;
}

void CDatabaseExplorerDoc::GetQueries(CRichEditCtrl* pRichEdit, std::vector<CString>& queries) const
{
	if (nullptr == pRichEdit)
		return;

	CString sLine;
	for (int i = 0; i < pRichEdit->GetLineCount(); ++i)
	{
		const int nLineLength = pRichEdit->LineLength(pRichEdit->LineIndex(i));
		if (nLineLength > 1)
		{
			sLine.Empty();
			pRichEdit->GetLine(i, sLine.GetBuffer(nLineLength), nLineLength);
			sLine.ReleaseBuffer();
			const auto found = std::find(queries.cbegin(), queries.cend(), sLine);
			if (found == queries.cend())
				queries.push_back(sLine);
		}
	}
}

std::vector<CString> CDatabaseExplorerDoc::GetDocumentQueries() const
{
	std::vector<CString> queries;
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CDatabaseExplorerView* pView = static_cast<CDatabaseExplorerView*>(GetNextView(pos));
		if (pView)
		{
			CChildFrame* pChild = static_cast<CChildFrame*>(pView->GetParentFrame());
			if (pChild)
			{
				CQueryPane* pQuery = static_cast<CQueryPane*>(pChild->GetQueryPane());
				if (pQuery)
					GetQueries(pQuery->GetRichEditCtrl(), queries);
			}
		}
	}

	return queries;
}

BOOL CDatabaseExplorerDoc::HasHarmfulQueries(const std::vector<CString>& queries) const
{
	for (const auto& it : queries)
	{
		CString sTemp(it);
		sTemp.MakeLower();
		if (ContainHarmfulKeyword(sTemp))
			return TRUE;
	}

	return FALSE;
}

BOOL CDatabaseExplorerDoc::ContainHarmfulKeyword(const CString& sSQL) const
{
	return (-1 != sSQL.Find(_T("delete")) ||
		-1 != sSQL.Find(_T("drop")) ||
		-1 != sSQL.Find(_T("alter")) ||
		-1 != sSQL.Find(_T("truncate")) ||
		-1 != sSQL.Find(_T("table")));
}

size_t CDatabaseExplorerDoc::GetRootCount(const CTreeCtrl& tree) const
{
	size_t count{};
	auto item = tree.GetRootItem();
	while (item)
	{
		count++;
		item = tree.GetNextSiblingItem(item);
	}

	return count;
}

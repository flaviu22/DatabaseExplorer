// DatabaseExplorerDoc.h : interface of the CDatabaseExplorerDoc class
//

#include "ChildFrm.h"
#include "QueryPane.h"
#include "MessagePane.h"
#include "DatabasePane.h"
#include "DatabaseExt.h"
#include "SettingsStoreEx.h"

#include <vector>
#include <chrono>

#pragma once

enum class MessageType
{
	info = 0,
	error = 1
};

enum class DatabaseType
{
	MSSQL = 1,
	ORACLE,
	SQLITE,
	MYSQL,
	MARIADB,
	POSTGRE,
	UNKNOWN
};

class CRaiiSupport
{
public:
	CRaiiSupport(BOOL& bFlag);
	~CRaiiSupport();

	CRaiiSupport(const CRaiiSupport& rhs) = delete;
	CRaiiSupport& operator=(const CRaiiSupport& rhs) = delete;
	CRaiiSupport(CRaiiSupport&& rhs) = delete;
	CRaiiSupport* operator=(CRaiiSupport&& rhs) = delete;

private:
	BOOL& m_bFlag;
};

struct SDocData
{
	BOOL m_bDSNSource{ FALSE };
	DatabaseType m_DBType{ DatabaseType::MSSQL };
	UINT m_nRecordsetType{ AFX_DB_USE_DEFAULT_TYPE };
	BOOL m_bMsSqlAuthenticationRequired{ FALSE };
	std::vector<CString> m_queries{};

	SDocData(BOOL bDSNSource, DatabaseType DBType, UINT nRecordsetType, BOOL bMsSqlAuthenticationRequired, std::vector<CString>&& queries)
		:m_bDSNSource(bDSNSource)
		,m_DBType(DBType)
		,m_nRecordsetType(nRecordsetType)
		,m_bMsSqlAuthenticationRequired(bMsSqlAuthenticationRequired)
		,m_queries(std::move(queries))
	{}
	SDocData(const SDocData& rhs) = delete;
	SDocData& operator=(const SDocData& rhs) = delete;
	SDocData(SDocData&& rhs) = default;
	SDocData& operator=(SDocData&& rhs) = default;
	void AddQueries(std::vector<CString>&& queries)
	{
		m_queries.insert(m_queries.begin(), queries.begin(), queries.end());
	}
};

class CDatabaseExplorerView;

class CDatabaseExplorerDoc : public CDocument
{
protected: // create from serialization only
	CDatabaseExplorerDoc() noexcept;
	DECLARE_DYNCREATE(CDatabaseExplorerDoc)

// Attributes
public:
	mutable CString m_sState;
	inline CDatabaseExt* GetDB() { return m_pDB.get(); }
	inline CRecordset* GetRecordset() const { return m_pRecordset.get(); }
	BOOL GetDSNSource() const { return m_bDSNSource; }
	void SetDSNSource(const BOOL bSet) { m_bDSNSource = bSet; }
	BOOL IsLoggedPopulateList() const { return m_bLogPopulateList; }
	DatabaseType GetDatabaseType() const { return m_DatabaseType; }
	void SetDatabaseType(const DatabaseType type) { m_DatabaseType = type; }
	CString GetPostgreDB() const { return m_sPostgreDB; }
	void SetPostgreDB(const CString& sName) { m_sPostgreDB = sName; }
	const CString DecodePostGreDatabase(const CString& sConnectionString) const;
	BOOL GetMsSqlAuthenticationRequired() const { return m_bMsSqlAuthenticationRequired; }
	void SetMsSqlAuthenticationRequired(const BOOL bSet) { m_bMsSqlAuthenticationRequired = bSet; }

// Operations
public:
	void SetConnectionString() const;
	auto GetDSN() const -> std::pair<CString, CString>;
	void SetDSN(const CString& sName);
	std::vector<CString> GetSQLStatements(const CString& sText) const;
	CQueryPane* GetQueryPane() const;
	CMessagePane* GetMessagePane() const;
	CDatabasePane* GetDatabasePane() const;
	CString GetTimeAsString(const std::chrono::high_resolution_clock::time_point& point1,
							const std::chrono::high_resolution_clock::time_point& point2) const;
	BOOL PopulateListCtrl(CListCtrl& ListCtrl, const CString& sSQL);
	BOOL IsSelect(CString sQuery) const;
	BOOL IsTableOperation(CString sQuery) const;
	BOOL IsDatabaseOperation(CString sQuery) const;
	BOOL PopulateDatabasePanel(CTreeCtrl& tree);
	void LogMessage(const CString& sMessage, const MessageType& type, CChildFrame* pChild = nullptr) const;
	BOOL RunSQL(const CString sSQL) const;
	CString InitDatabase();
	long GetRecordCount(const CString& sSQL) const;
	int GetDatabaseCount() const;
	BOOL SaveListContentToCSV(CListCtrl& ListCtrl, const CString& sPathName);
	void RestoreQueries(CQueryPane* pPane) const;
	std::vector<CString> GetDocumentQueries() const;

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CDatabaseExplorerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bDSNSource{ FALSE };				// user dsn
	std::pair<CString, CString> m_DSN{};	// dsn name - dsn type
	CString m_sPostgreDB{ _T("postgres") };
	std::unique_ptr<CDatabaseExt> m_pDB{ nullptr };
	std::unique_ptr<CRecordset> m_pRecordset{ nullptr };
	DatabaseType m_DatabaseType{ DatabaseType::MSSQL };
	BOOL m_bMsSqlAuthenticationRequired{ FALSE };

protected:
	size_t TokenizeString(const CString& sText, const CString& sToken, std::vector<CString>& result) const;
	void PopulateHeader(CListCtrl& ListCtrl, CRecordset& recordset);
	BOOL GetMSSQLDatabases(CTreeCtrl& tree);
	BOOL GetOracleDatabases(CTreeCtrl& tree);
	BOOL GetSQLiteDatabases(CTreeCtrl& tree);
	BOOL GetMySqlDatabases(CTreeCtrl& tree);
	BOOL GetPostgreDatabases(CTreeCtrl& tree);
	CChildFrame* GetChildFrame() const;
	CString PrepareSQLForCountAll(const CString& sSQL) const;
	std::vector<CString> GetHeaderItems(CListCtrl& ListCtrl);
	void WriteHeaderLine(CListCtrl& ListCtrl, CStdioFile& file, const CString& sSeparator);
	void WriteListLines(CListCtrl& ListCtrl, CStdioFile& file, const CString& sSeparator);
	CString GetText(CHeaderCtrl& header, int nItem) const;
	CString GetOracleUserID(const BOOL bMakeUpper = FALSE) const;
	std::pair<CString, CString> GetMsSqlAuthenticationCredential() const;

	inline CString ConvertToCString(const CDBVariant& variant)
	{
		CString sRet;

		switch (variant.m_dwType)
		{
		case DBVT_LONG:
			sRet = std::to_string(variant.m_lVal).c_str();
			break;
		case DBVT_DOUBLE:
			sRet = std::to_string(variant.m_dblVal).c_str();
			break;
		case DBVT_DATE:
			{
				COleDateTime date(variant.m_pdate->year, variant.m_pdate->month,
					variant.m_pdate->day, variant.m_pdate->hour,
					variant.m_pdate->minute, variant.m_pdate->second);
				if (0 == variant.m_pdate->hour && 0 == variant.m_pdate->minute && 0 == variant.m_pdate->second)
					sRet = date.Format(_T("%Y-%m-%d"));
				else
					sRet = date.Format(_T("%Y-%m-%d %H:%M:%S"));
			}
			break;
		case DBVT_STRING:
			sRet = variant.m_pstring->GetString();
			break;
		case DBVT_ASTRING:
			sRet = variant.m_pstringA->GetString();
			break;
		case DBVT_WSTRING:
			sRet = variant.m_pstringW->GetString();
			break;
		default:
			break;
		}

		return sRet;
	}

private:
	BOOL m_bLogPopulateList{ TRUE };

private:
	void GetQueries(CRichEditCtrl* pRichEdit, std::vector<CString>& queries) const;
	std::vector<CString> GetQueries(const CString& sFile) const;
	CChildFrame* GetChild(LPCTSTR lpszTitle = NULL) const;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CDatabaseExplorerDoc)
	afx_msg void OnEditDatasource();
	afx_msg void OnLogPopulateList();
	afx_msg void OnUpdateEditLogpopulatelist(CCmdUI* pCmdUI);
	//}}AFX_MSG

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

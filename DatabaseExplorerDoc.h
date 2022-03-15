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
#include <set>

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
	DatabaseType m_DBType{ DatabaseType::MSSQL };
	UINT m_nRecordsetType{ AFX_DB_USE_DEFAULT_TYPE };
	std::set<CString> m_queries{};
	BOOL m_bMsSqlAuthenticationRequired{ FALSE };

	SDocData(DatabaseType DBType, UINT nRecordsetType, BOOL bMsSqlAuthenticationRequired, std::set<CString>&& queries)
		:m_DBType(DBType)
		,m_nRecordsetType(nRecordsetType)
		,m_bMsSqlAuthenticationRequired(bMsSqlAuthenticationRequired)
		,m_queries(std::move(queries))
	{}
	SDocData(const SDocData& rhs) = default;
	SDocData& operator=(const SDocData& rhs) = default;
	SDocData(SDocData&& rhs) = default;
	SDocData& operator=(SDocData&& rhs) = default;
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
	void LogMessage(const CString& sMessage, const MessageType& type);
	BOOL RunSQL(const CString sSQL);
	CString InitDatabase();
	long GetRecordCount(const CString& sSQL);
	int GetDatabaseCount() const;
	BOOL SaveListContentToCSV(CListCtrl& ListCtrl, const CString& sPathName);
	void RestoreQueries(CQueryPane* pPane) const;
	std::set<CString> GetDocumentQueries() const;
	BOOL HasValidDocumentTitle(const CString& sTitle) const;
	CString GetTitleNormalized() const;

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
	CString PrepareSQLForCountAll(const CString& sSQL);
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
				sRet = date.Format();
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
	void GetQueries(CRichEditCtrl* pRichEdit, std::set<CString>& queries) const;
	std::vector<CString> GetQueries(const CString& sFile) const;

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

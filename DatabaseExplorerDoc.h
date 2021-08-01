// DatabaseExplorerDoc.h : interface of the CDatabaseExplorerDoc class
//

#include "ChildFrm.h"
#include "QueryPane.h"
#include "MessagePane.h"
#include "DatabasePane.h"
#include "DatabaseExt.h"

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
	MYSQL,
	POSTGRE,
	INFORMIX,
	MARIADB
};

class CDatabaseExplorerView;

class CDatabaseExplorerDoc : public CDocument
{
protected: // create from serialization only
	CDatabaseExplorerDoc() noexcept;
	DECLARE_DYNCREATE(CDatabaseExplorerDoc)

// Attributes
public:
	CString m_sState;
	DatabaseType m_DatabaseType{ DatabaseType::MSSQL };
	inline CDatabaseExt* GetDB() { return m_pDB.get(); }
	inline CRecordset* GetRecordset() const { return m_pRecordset.get(); }
	BOOL IsLoggedPopulateList() const { return m_bLogPopulateList; }

// Operations
public:
	std::vector<CString> GetSQLStatements(const CString& sText) const;
	CQueryPane* GetQueryPane() const;
	CMessagePane* GetMessagePane() const;
	CDatabasePane* GetDatabasePane() const;
	CString GetTimeAsString(const std::chrono::high_resolution_clock::time_point& point1,
							const std::chrono::high_resolution_clock::time_point& point2) const;
	BOOL PopulateListCtrl(CDatabaseExplorerView* pView, const CString& sSQL);
	BOOL IsSelect(const CString& sQuery) const;
	BOOL PopulateDatabasePanel(CTreeCtrl& tree);
	void LogMessage(const CString& sMessage, const MessageType& type);
	BOOL RunSQL(const CString sSQL);
	CString InitDatabase();
	long GetRecordCount(const CString& sSQL);
	int GetDatabaseCount() const;

// Overrides
public:
	virtual BOOL OnNewDocument();
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
	std::shared_ptr<CDatabaseExt> m_pDB{ nullptr };
	std::unique_ptr<CRecordset> m_pRecordset{ nullptr };

protected:
	size_t TokenizeString(const CString& sText, const CString& sToken, std::vector<CString>& result) const;
	void PopulateHeader(CListCtrl& ListCtrl, CRecordset& recordset);
	BOOL GetDatabaseMSSQL(CTreeCtrl& tree);
	BOOL GetDatabaseOracle(CTreeCtrl& tree);
	BOOL GetDatabaseMySql(CTreeCtrl& tree);
	BOOL GetDatabasePostgre(CTreeCtrl& tree);
	BOOL GetDatabaseInformix(CTreeCtrl& tree);
	BOOL GetDatabaseMariaDB(CTreeCtrl& tree);
	CString ConvertDataAsString(const CDBVariant& variant);
	CChildFrame* GetChildFrame() const;
	CString PrepareSQLForCountAll(const CString& sSQL);

private:
	BOOL m_bLogPopulateList{ TRUE };

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

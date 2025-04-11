#pragma once

#include "pch.h"
#include "resource.h"
#include <afxdb.h>

#define WMU_SETMESSAGETEXT					(WM_APP + 39)
#define WMU_CHILDFRAMEADDED					(WM_APP + 40)
#define WMU_CHILDFRAMEREMOVED				(WM_APP + 41)
#define WMU_CHILDFRAMEACTIVATE				(WM_APP + 42)
#define WMU_CHILDFRAMEACTIVATED				(WM_APP + 43)
#define WMU_ISPOPULATEMODE					(WM_APP + 44)
#define WMU_RESTOREQUERIES					(WM_APP + 45)
#define WMU_SETWORDWRAP						(WM_APP + 46)
#define WMU_DARKMODE						(WM_APP + 47)
#define WMU_POSTINIT						(WM_APP + 80)

#define STR_DSNSOURCE						_T("DSNSource")
#define STR_DATABASETYPE					_T("DatabaseType")
#define STR_RSTYPE							_T("RsType")
#define STR_MSSQLAUTHENTICATIONREQUIRED		_T("MsSqlAuthenticationRequired")

constexpr UINT TIME_SECOND = 1000;
constexpr UINT TIME_MINUTE = 60 * TIME_SECOND;

constexpr COLORREF g_crColorDark = RGB(44, 44, 44);
constexpr COLORREF g_crColorWhite = RGB(254, 255, 255);
constexpr COLORREF g_crHeaderColorDark = RGB(44, 44, 44);
constexpr COLORREF g_crHeaderColorDarkHot = RGB(64, 64, 64);
constexpr COLORREF g_crHeaderColorDarkPressed = RGB(144, 144, 144);

constexpr int IDC_TREE_DATABASE = 21;
constexpr COLORREF g_crDummy = RGB(77, 77, 77);

constexpr int PANEQUERYID = 22;
constexpr int PANEMESSAGEID = 32;
constexpr int PANEDATABASEID = 33;
constexpr int DoInitDatabase = 1;
constexpr WPARAM DoListTable = 2;
constexpr int UpdateFrameText = 3;

constexpr int IDC_RICHEDIT_QUERY = 33;
constexpr int IDC_RICHEDIT_MESSAGE = 34;

enum class Color
{
	black = RGB(0, 0, 0),
	red = RGB(250, 7, 7)
};

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
		, m_DBType(DBType)
		, m_nRecordsetType(nRecordsetType)
		, m_bMsSqlAuthenticationRequired(bMsSqlAuthenticationRequired)
		, m_queries(std::move(queries))
	{
	}
	SDocData(const SDocData& rhs) = delete;
	SDocData& operator=(const SDocData& rhs) = delete;
	SDocData(SDocData&& rhs) = default;
	SDocData& operator=(SDocData&& rhs) = default;
	void AddQueries(std::vector<CString>&& queries)
	{
		m_queries.insert(m_queries.begin(), queries.begin(), queries.end());
	}
};

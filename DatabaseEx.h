// DatabaseEx.h: interface for the CDatabaseEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DatabaseEx_H__0ED7B212_C215_452C_AA8F_054809CF5F07__INCLUDED_)
#define AFX_DatabaseEx_H__0ED7B212_C215_452C_AA8F_054809CF5F07__INCLUDED_

#include <afxdb.h>
#include <afxtempl.h>
#include <vector>
#include <string>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDatabaseEx

class CDatabaseEx : public CDatabase 
{
public:
	CDatabaseEx();
	explicit CDatabaseEx(const CString& sPassword);
	explicit CDatabaseEx(LPCTSTR lpszConnectionString);
	explicit CDatabaseEx(LPCTSTR lpszConnectionString, UINT nRecordsetType);
	virtual ~CDatabaseEx()/* override*/;

protected:
	CString m_sDSN;
	CString m_sDriver;
	CString m_sHost;
	CString m_sServer;
	CString m_sService;
	CString m_sProtocol;
	CString m_sDatabase;
	CString m_sUser;
	CString m_sPassword;

protected:
	CString m_sError;
	UINT m_nRecordsetType;
	BOOL m_bUseTransaction;
	CString m_sConnectionString;
	CMapStringToString m_mapRegistryEntry;
	BOOL m_bReadConectionDataFromRegistry;

protected:
	void PopulateRegistryEntry();

// Operations
public:
	CString GetError() const;
	void SetError(LPCTSTR lpszFormat, ...);
	void ResetError();
	void InitConnectionString(int nModel = 0);
	BOOL GetUseTransaction() const;
	void SetUseTransaction(BOOL bUse);
	UINT GetRecordsetType() const;
	void SetRecordsetType(UINT nRecordsetType = CRecordset::dynaset);
	void SetConnectionString(const CString& sConnectionString);
	CString GetRegistryEntry(const CString& sKey) const;
	void SetRegistryEntry(const CString& sKey, const CString& sValue);

public:
	const CString GetDSN() const;
	void SetDSN(const CString& sDSN);
	const CString GetDriver() const;
	void SetDriver(const CString& sDriver);
	const CString GetHost() const;
	void SetHost(const CString& sHost);
	const CString GetServer() const;
	void SetServer(const CString& sServer);
	const CString GetService() const;
	void SetService(const CString& sService);
	const CString GetProtocol() const;
	void SetProtocol(const CString& sProtocol);
	const CString GetDatabase() const;
	void SetDatabase(const CString& sDatabase);
	const CString GetUser() const;
	void SetUser(const CString& sUser);
	const CString GetPassword() const;
	void SetPassword(const CString& sPassword);
	BOOL GetReadConectionDataFromRegistry() const;
	void SetReadConectionDataFromRegistry(BOOL bReadConectionDataFromRegistry = TRUE);
	void ReadConectionDataFromRegistry();
	void WriteConectionDataToRegistry(const CString& sDSN, const CString& sDriver, const CString& sHost,
									const CString& sServer, const CString& sService, const CString& sProtocol,
									const CString& sDatabase, const CString& sUser);

public:
	BOOL OpenConnection(DWORD dwOptions = CDatabase::noOdbcDialog);
	BOOL OpenRecordsetV(CRecordset* pRecordset, LPCTSTR lpszFormat, ...);
	BOOL OpenRecordset(CRecordset* pRecordset, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);

public:
	BOOL ExecuteV(LPCTSTR lpszFormat, ...);
	BOOL Execute(LPCTSTR lpszSQL, DWORD dwOptions = CDatabase::noOdbcDialog);
	BOOL Execute(const CStringArray& saSQL, DWORD dwOptions = CDatabase::noOdbcDialog);
	BOOL Execute(const std::vector<CString>& saSQL, DWORD dwOptions = CDatabase::noOdbcDialog);
	BOOL Execute(const std::vector<std::string>& saSQL, DWORD dwOptions = CDatabase::noOdbcDialog);
};

#endif // !defined(AFX_DatabaseEx_H__0ED7B212_C215_452C_AA8F_054809CF5F07__INCLUDED_)

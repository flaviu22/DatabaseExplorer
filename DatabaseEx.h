// DatabaseEx.h: interface for the CDatabaseEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DatabaseEx_H__0ED7B212_C215_452C_AA8F_054809CF5F07__INCLUDED_)
#define AFX_DatabaseEx_H__0ED7B212_C215_452C_AA8F_054809CF5F07__INCLUDED_

#include <afxdb.h>
#include <memory>
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
	explicit CDatabaseEx(LPCTSTR lpszConnectionString);
	explicit CDatabaseEx(LPCTSTR lpszConnectionString, UINT nRecordsetType);
	virtual ~CDatabaseEx()/* override*/;

protected:
	CString m_sError;
	UINT m_nRecordsetType;
	BOOL m_bUseTransaction;
	CString m_sConnectionString;

// Operations
public:
	CString GetError() const;
	void SetError(LPCTSTR lpszFormat, ...);
	void ResetError();
	BOOL GetUseTransaction() const;
	void SetUseTransaction(BOOL bUse);
	UINT GetRecordsetType() const;
	void SetRecordsetType(UINT nRecordsetType = CRecordset::dynaset);
	void SetConnectionString(const CString& sConnectionString);

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

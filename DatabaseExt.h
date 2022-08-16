// DatabaseExt.h: interface for the CDatabaseExt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DatabaseExt_H__14C57009_F177_4ACF_A3BC_AF52CDCB528F__INCLUDED_)
#define AFX_DatabaseExt_H__14C57009_F177_4ACF_A3BC_AF52CDCB528F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DatabaseEx.h"

class CDatabaseExt : public CDatabaseEx
{
public:
	CDatabaseExt();
#if(_MSC_VER <= 1200)
	~CDatabaseExt() {}
#else
	~CDatabaseExt()/* override*/ = default;
#endif

protected:
	int m_nFieldCount;

public:
	int GetFieldCount() const;
	BOOL GetDataV(CStringArray& saResult, LPCTSTR lpszFormat, ...);
	BOOL GetDataV(CStringArray& saResult, CString(*FormatData)(short nIndex, CDBVariant*), LPCTSTR lpszFormat, ...);
	BOOL GetData(CStringArray& saResult, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetData(CStringArray& saResult, CString(*FormatData)(short nIndex, CDBVariant*), LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetDataV(std::vector<CString>& data, LPCTSTR lpszFormat, ...);
	BOOL GetData(std::vector<CString>& data, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	std::vector<std::string> GetDataV(LPCTSTR lpszFormat, ...);
	std::vector<std::string> GetData(LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetData(std::vector<CString>& data, CString(*FormatData)(short nIndex, CDBVariant*), LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
};

#endif // !defined(AFX_DatabaseExt_H__14C57009_F177_4ACF_A3BC_AF52CDCB528F__INCLUDED_)

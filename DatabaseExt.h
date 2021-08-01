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
	BOOL GetRecordsetV(CStringArray& saResult, LPCTSTR lpszFormat, ...);
	BOOL GetRecordsetV(CTypedPtrArray<CPtrArray, CDBVariant*>& arrResult, LPCTSTR lpszFormat, ...);
	BOOL GetRecordsetV(CStringArray& saResult, CString(*FormatData)(const short& nIndex, CDBVariant*), LPCTSTR lpszFormat, ...);
	BOOL GetRecordset(CStringArray& saResult, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetRecordset(CTypedPtrArray<CPtrArray, CDBVariant*>& arrResult, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetRecordset(CStringArray& saResult, CString(*FormatData)(const short& nIndex, CDBVariant* pVariant), LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetRecordsetVectorV(std::vector<CString>& result, LPCTSTR lpszFormat, ...);
	BOOL GetRecordsetVector(std::vector<CString>& result, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetRecordsetVectorV(std::vector<std::string>& result, LPCTSTR lpszFormat, ...);
	BOOL GetRecordsetVector(std::vector<std::string>& result, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetRecordsetVectorV(std::vector<CDBVariant>& result, LPCTSTR lpszFormat, ...);
	BOOL GetRecordsetVector(std::vector<CDBVariant>& result, LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
	BOOL GetRecordsetVector(std::vector<CString>& result, CString(*FormatData)(const short& nIndex, CDBVariant*), LPCTSTR lpszSQL = NULL, DWORD dwRecordsetOptions = CRecordset::none);
};

#endif // !defined(AFX_DatabaseExt_H__14C57009_F177_4ACF_A3BC_AF52CDCB528F__INCLUDED_)

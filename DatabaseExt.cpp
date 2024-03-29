// DatabaseExt.cpp: implementation of the CDatabaseExt class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "DatabaseExt.h"

#include <memory>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

CDatabaseExt::CDatabaseExt()
	:m_nFieldCount(0)
{
}

int CDatabaseExt::GetFieldCount() const
{
	return m_nFieldCount;
}

BOOL CDatabaseExt::GetDataV(CStringArray& saResult, LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetData(saResult, sSQL);
}

BOOL CDatabaseExt::GetDataV(CStringArray& saResult, CString(*FormatData)(short nIndex, CDBVariant*), LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetData(saResult, FormatData, sSQL);
}

BOOL CDatabaseExt::GetData(CStringArray& saResult, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
{
	m_nFieldCount = 0;
	if (! OpenConnection())
		return FALSE;

	std::unique_ptr<CRecordset> recordset = std::make_unique<CRecordset>(this);

	do
	{
		if (! OpenRecordset(recordset.get(), lpszSQL, dwRecordsetOptions))
			break;

		m_nFieldCount = recordset->GetODBCFieldCount();

		CString sTemp;

		while (! recordset->IsEOF())
		{
			for (int i = 0; i < m_nFieldCount; ++i)
			{
				try
				{
					recordset->GetFieldValue(static_cast<short>(i), sTemp);
					saResult.Add(sTemp);
				}
				catch (CDBException* pDBException)
				{
					pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pDBException->Delete();
					break;
				}
				catch (CMemoryException* pMemException)
				{
					pMemException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pMemException->Delete();
					break;
				}
			}
			if (m_sError.IsEmpty())
				recordset->MoveNext();
			else
				break;
		}

		recordset->Close();

	} while (FALSE);

	return m_sError.IsEmpty();
}

BOOL CDatabaseExt::GetData(CStringArray& saResult, CString(*FormatData)(short nIndex, CDBVariant*), LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
{
	m_nFieldCount = 0;
	if (! OpenConnection())
		return FALSE;

	std::unique_ptr<CRecordset> recordset = std::make_unique<CRecordset>(this);

	do
	{
		if (! OpenRecordset(recordset.get(), lpszSQL, dwRecordsetOptions))
			break;

		m_nFieldCount = recordset->GetODBCFieldCount();

		CDBVariant variant;

		while (! recordset->IsEOF())
		{
			for (int i = 0; i < m_nFieldCount; ++i)
			{
				try
				{
					recordset->GetFieldValue(static_cast<short>(i), variant);
					saResult.Add(FormatData(static_cast<short>(i), &variant));
				}
				catch (CDBException* pDBException)
				{
					pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pDBException->Delete();
					break;
				}
				catch (CMemoryException* pMemException)
				{
					pMemException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pMemException->Delete();
					break;
				}
			}
			if (m_sError.IsEmpty())
				recordset->MoveNext();
			else
				break;
		}

		recordset->Close();

	} while (FALSE);

	return m_sError.IsEmpty();
}

BOOL CDatabaseExt::GetDataV(std::vector<CString>& data, LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetData(data, sSQL);
}

std::vector<std::string> CDatabaseExt::GetDataV(LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetData(sSQL);
}

BOOL CDatabaseExt::GetData(std::vector<CString>& data, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
{
	m_nFieldCount = 0;
	if (! OpenConnection())
		return FALSE;

	std::unique_ptr<CRecordset> recordset = std::make_unique<CRecordset>(this);

	do
	{
		if (! OpenRecordset(recordset.get(), lpszSQL, dwRecordsetOptions))
			break;

		m_nFieldCount = recordset->GetODBCFieldCount();

		CString sTemp;

		while (! recordset->IsEOF())
		{
			for (int i = 0; i < m_nFieldCount; ++i)
			{
				try
				{
					recordset->GetFieldValue(static_cast<short>(i), sTemp);
					data.push_back(sTemp);
				}
				catch (CDBException* pDBException)
				{
					pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pDBException->Delete();
					break;
				}
				catch (CMemoryException* pMemException)
				{
					pMemException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pMemException->Delete();
					break;
				}
			}
			if (m_sError.IsEmpty())
				recordset->MoveNext();
			else
				break;
		}

		recordset->Close();

	} while (FALSE);

	return m_sError.IsEmpty();
}

std::vector<std::string> CDatabaseExt::GetData(LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
{
	std::vector<std::string> result;

	m_nFieldCount = 0;
	if (! OpenConnection())
		return result;

	std::unique_ptr<CRecordset> recordset = std::make_unique<CRecordset>(this);

	do
	{
		if (! OpenRecordset(recordset.get(), lpszSQL, dwRecordsetOptions))
			break;

		m_nFieldCount = recordset->GetODBCFieldCount();

		CString sTemp;

		while (! recordset->IsEOF())
		{
			for (int i = 0; i < m_nFieldCount; ++i)
			{
				try
				{
					recordset->GetFieldValue(static_cast<short>(i), sTemp);
#ifdef _UNICODE
					result.push_back(std::string(CW2A(sTemp.GetString(), CP_UTF8), sTemp.GetLength()));
#else
					result.push_back(std::string(sTemp.GetString(), sTemp.GetLength()));
#endif
				}
				catch (CDBException* pDBException)
				{
					pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pDBException->Delete();
					break;
				}
				catch (CMemoryException* pMemException)
				{
					pMemException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pMemException->Delete();
					break;
				}
			}
			if (m_sError.IsEmpty())
				recordset->MoveNext();
			else
				break;
		}

		recordset->Close();

	} while (FALSE);

	return result;
}

BOOL CDatabaseExt::GetData(std::vector<CString>& data, CString(*FormatData)(short nIndex, CDBVariant*), LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
{
	m_nFieldCount = 0;
	if (! OpenConnection())
		return FALSE;

	std::unique_ptr<CRecordset> recordset = std::make_unique<CRecordset>(this);

	do
	{
		if (! OpenRecordset(recordset.get(), lpszSQL, dwRecordsetOptions))
			break;

		m_nFieldCount = recordset->GetODBCFieldCount();

		CDBVariant variant;

		while (! recordset->IsEOF())
		{
			for (int i = 0; i < m_nFieldCount; ++i)
			{
				try
				{
					recordset->GetFieldValue(static_cast<short>(i), variant);
					data.push_back(FormatData(static_cast<short>(i), &variant));
				}
				catch (CDBException* pDBException)
				{
					pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pDBException->Delete();
					break;
				}
				catch (CMemoryException* pMemException)
				{
					pMemException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
					m_sError.ReleaseBuffer();
					pMemException->Delete();
					break;
				}
			}
			if (m_sError.IsEmpty())
				recordset->MoveNext();
			else
				break;
		}

		recordset->Close();

	} while (FALSE);

	return m_sError.IsEmpty();
}

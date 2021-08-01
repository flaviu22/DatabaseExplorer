// DatabaseExt.cpp: implementation of the CDatabaseExt class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "DatabaseExt.h"

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

BOOL CDatabaseExt::GetRecordsetV(CStringArray& saResult, LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetRecordset(saResult, sSQL);
}

BOOL CDatabaseExt::GetRecordsetV(CTypedPtrArray<CPtrArray, CDBVariant*>& arrResult, LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetRecordset(arrResult, sSQL);
}

BOOL CDatabaseExt::GetRecordsetV(CStringArray& saResult, CString(*FormatData)(const short& nIndex, CDBVariant*), LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetRecordset(saResult, FormatData, sSQL);
}

BOOL CDatabaseExt::GetRecordset(CStringArray& saResult, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
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
					recordset->GetFieldValue(i, sTemp);
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
				saResult.Add(sTemp);
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

BOOL CDatabaseExt::GetRecordset(CTypedPtrArray<CPtrArray, CDBVariant*>& arrResult, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
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

		while (! recordset->IsEOF())
		{
			for (int i = 0; i < m_nFieldCount; ++i)
			{
				try
				{
					CDBVariant* pDBVariant = new CDBVariant();
					recordset->GetFieldValue(i, *pDBVariant);
					arrResult.Add(pDBVariant);
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

BOOL CDatabaseExt::GetRecordset(CStringArray& saResult, CString(*FormatData)(const short& nIndex, CDBVariant* pVariant), LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
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
					recordset->GetFieldValue(i, variant);
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
				saResult.Add(FormatData(i, &variant));
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

BOOL CDatabaseExt::GetRecordsetVectorV(std::vector<CString>& result, LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetRecordsetVector(result, sSQL);
}

BOOL CDatabaseExt::GetRecordsetVectorV(std::vector<std::string>& result, LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetRecordsetVector(result, sSQL);
}

BOOL CDatabaseExt::GetRecordsetVectorV(std::vector<CDBVariant>& result, LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return GetRecordsetVector(result, sSQL);
}

BOOL CDatabaseExt::GetRecordsetVector(std::vector<CString>& result, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
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
					recordset->GetFieldValue(i, sTemp);
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
				result.push_back(sTemp);
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

BOOL CDatabaseExt::GetRecordsetVector(std::vector<std::string>& result, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
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
					recordset->GetFieldValue(i, sTemp);
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
#ifdef _UNICODE
				result.push_back(std::string(CW2A(sTemp.GetString(), CP_UTF8), sTemp.GetLength()));
#else
				result.push_back(std::string(sTemp.GetString(), sTemp.GetLength()));
#endif
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

BOOL CDatabaseExt::GetRecordsetVector(std::vector<CDBVariant>& result, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
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
					recordset->GetFieldValue(i, variant);
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
				result.push_back(variant);
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

BOOL CDatabaseExt::GetRecordsetVector(std::vector<CString>& result, CString(*FormatData)(const short& nIndex, CDBVariant*), LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
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
					recordset->GetFieldValue(i, variant);
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
				result.push_back(FormatData(i, &variant));
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

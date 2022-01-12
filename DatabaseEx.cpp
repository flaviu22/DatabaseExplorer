// DatabaseEx.cpp: implementation of the CDatabaseEx class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "DatabaseEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDatabaseEx::CDatabaseEx()
	:m_bUseTransaction(FALSE)
	,m_nRecordsetType(AFX_DB_USE_DEFAULT_TYPE)
{
}

CDatabaseEx::CDatabaseEx(LPCTSTR lpszConnectionString)
	:m_bUseTransaction(FALSE)
	,m_nRecordsetType(AFX_DB_USE_DEFAULT_TYPE)
{
	if (lpszConnectionString && *lpszConnectionString)
		m_sConnectionString.Format(_T("%s"), lpszConnectionString);
}

CDatabaseEx::CDatabaseEx(LPCTSTR lpszConnectionString, UINT nRecordsetType)
	:m_bUseTransaction(FALSE)
	,m_nRecordsetType(AFX_DB_USE_DEFAULT_TYPE)
{
	ASSERT(nRecordsetType == AFX_DB_USE_DEFAULT_TYPE	||
		nRecordsetType == CRecordset::dynaset			|| 
		nRecordsetType == CRecordset::snapshot			|| 
		nRecordsetType == CRecordset::dynamic			|| 
		nRecordsetType == CRecordset::forwardOnly);

	m_nRecordsetType = nRecordsetType;
	if (lpszConnectionString && *lpszConnectionString)
		m_sConnectionString.Format(_T("%s"), lpszConnectionString);
}

CDatabaseEx::~CDatabaseEx()
{
	Close();
}

CString CDatabaseEx::GetError() const
{
	return m_sError;
}

void CDatabaseEx::SetError(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	m_sError.FormatV(lpszFormat, args);
}

void CDatabaseEx::ResetError()
{
	m_sError.Empty();
}

BOOL CDatabaseEx::GetUseTransaction() const
{
	return m_bUseTransaction;
}

void CDatabaseEx::SetUseTransaction(BOOL bUse)
{
	m_bUseTransaction = bUse;
}

UINT CDatabaseEx::GetRecordsetType() const
{
	return m_nRecordsetType;
}

void CDatabaseEx::SetConnectionString(const CString& sConnectionString)
{
	m_sConnectionString = sConnectionString;
}

void CDatabaseEx::SetRecordsetType(UINT nRecordsetType/* = CRecordset::dynaset*/)
{
	ASSERT(nRecordsetType == AFX_DB_USE_DEFAULT_TYPE	|| 
		nRecordsetType == CRecordset::dynaset			|| 
		nRecordsetType == CRecordset::snapshot			|| 
		nRecordsetType == CRecordset::dynamic			|| 
		nRecordsetType == CRecordset::forwardOnly);

	m_nRecordsetType = nRecordsetType;
}

BOOL CDatabaseEx::OpenConnection(DWORD dwOptions/* = CDatabase::noOdbcDialog*/)
{
	m_sError.Empty();

	if (IsOpen())
		return TRUE;

	try
	{
		OpenEx(m_sConnectionString, dwOptions);
	}
	catch(CDBException* pDBException)
	{
		Close();
		pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
		m_sError.ReleaseBuffer();
		pDBException->Delete();
		return FALSE;
	}
	catch(CMemoryException* pMemException)
	{
		Close();
		pMemException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
		m_sError.ReleaseBuffer();
		pMemException->Delete();
		return FALSE;
	}

	if (! IsOpen() && m_sError.IsEmpty())
		m_sError.Format(_T("The connection to the database could not be established"));

	return IsOpen();
}

BOOL CDatabaseEx::OpenRecordsetV(CRecordset* pRecordset, LPCTSTR lpszFormat, ...)
{
	if (NULL == pRecordset)
		return FALSE;

	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return OpenRecordset(pRecordset, sSQL);
}

BOOL CDatabaseEx::OpenRecordset(CRecordset* pRecordset, LPCTSTR lpszSQL/* = NULL*/, DWORD dwRecordsetOptions/* = CRecordset::none*/)
{
	if (NULL == pRecordset)
		return FALSE;

	if (! IsOpen() && ! OpenConnection())
		return FALSE;

	m_sError.Empty();

	do
	{
		try
		{
			VerifyConnect();
			pRecordset->Open(m_nRecordsetType, lpszSQL, dwRecordsetOptions);
		}
		catch(CDBException* pDBException)
		{
			pRecordset->Close();
			pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
			m_sError.ReleaseBuffer();
			pDBException->Delete();
			break;
		}
		catch(CMemoryException* pMemException)
		{
			pRecordset->Close();
			pMemException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
			m_sError.ReleaseBuffer();
			pMemException->Delete();
			break;
		}

		if (! pRecordset->IsOpen())
		{
			m_sError.Format(_T("The recordset could not be opened"));
		}
	} while (FALSE);

	return m_sError.IsEmpty();
}

BOOL CDatabaseEx::ExecuteV(LPCTSTR lpszFormat, ...)
{
	CString sSQL;
	va_list args;
	va_start(args, lpszFormat);
	sSQL.FormatV(lpszFormat, args);

	return Execute(sSQL);
}

BOOL CDatabaseEx::Execute(LPCTSTR lpszSQL, DWORD dwOptions/* = CDatabase::noOdbcDialog*/)
{
	do
	{
		if (! lpszSQL || ! *lpszSQL)
		{
			m_sError.Format(_T("The sql string is null or empty"));
			break;
		}

		if (! OpenConnection(dwOptions))
			break;

		if (m_bUseTransaction && (! CanTransact() || ! BeginTrans()))
		{
			m_sError.Format(_T("Can not perform transaction"));
			break;
		}

		try
		{
			VerifyConnect();
			ExecuteSQL(lpszSQL);
			if (m_bUseTransaction)
				CommitTrans();
		}
		catch (CDBException* pDBException)
		{
			if (m_bUseTransaction)
				Rollback();
			pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
			m_sError.ReleaseBuffer();
			pDBException->Delete();
			break;
		}
	} while (FALSE);

	return m_sError.IsEmpty();
}

BOOL CDatabaseEx::Execute(const CStringArray& saSQL, DWORD dwOptions/* = CDatabase::noOdbcDialog*/)
{
	do
	{
		if (! OpenConnection(dwOptions))
			break;

		if (m_bUseTransaction && (! CanTransact() || ! BeginTrans()))
		{
			m_sError.Format(_T("Can not perform transaction"));
			break;
		}

		for (int i = 0; i < saSQL.GetSize(); ++i)
		{
			try
			{
				VerifyConnect();
				ExecuteSQL(saSQL.GetAt(i));
			}
			catch (CDBException* pDBException)
			{
				if (m_bUseTransaction)
					Rollback();
				pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
				m_sError.ReleaseBuffer();
				pDBException->Delete();
				break;
			}
		}

		if (m_bUseTransaction)
			CommitTrans();
	} while (FALSE);

	return m_sError.IsEmpty();
}

BOOL CDatabaseEx::Execute(const std::vector<CString>& saSQL, DWORD dwOptions/* = CDatabase::noOdbcDialog*/)
{
	do
	{
		if (! OpenConnection(dwOptions))
			break;

		if (m_bUseTransaction && (! CanTransact() || ! BeginTrans()))
		{
			m_sError.Format(_T("Can not perform transaction"));
			break;
		}

		for (std::vector<CString>::const_iterator it = saSQL.cbegin(); it != saSQL.cend(); ++it)
		{
			try
			{
				VerifyConnect();
				ExecuteSQL(*it);
			}
			catch (CDBException* pDBException)
			{
				if (m_bUseTransaction)
					Rollback();
				pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
				m_sError.ReleaseBuffer();
				pDBException->Delete();
				break;
			}
		}

		if (m_bUseTransaction)
			CommitTrans();
	} while (FALSE);

	return m_sError.IsEmpty();
}

BOOL CDatabaseEx::Execute(const std::vector<std::string>& saSQL, DWORD dwOptions/* = CDatabase::noOdbcDialog*/)
{
	do
	{
		if (! OpenConnection(dwOptions))
			break;

		if (m_bUseTransaction && (! CanTransact() || ! BeginTrans()))
		{
			m_sError.Format(_T("Can not perform transaction"));
			break;
		}

		for (std::vector<std::string>::const_iterator it = saSQL.cbegin(); it != saSQL.cend(); ++it)
		{
			try
			{
				VerifyConnect();
				ExecuteSQL(CString(it->c_str()));
			}
			catch (CDBException* pDBException)
			{
				if (m_bUseTransaction)
					Rollback();
				pDBException->GetErrorMessage(m_sError.GetBuffer(_MAX_PATH), _MAX_PATH);
				m_sError.ReleaseBuffer();
				pDBException->Delete();
				break;
			}
		}

		if (m_bUseTransaction)
			CommitTrans();
	} while (FALSE);

	return m_sError.IsEmpty();
}

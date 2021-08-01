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
	,m_bReadConectionDataFromRegistry(TRUE)
{
	PopulateRegistryEntry();
}

CDatabaseEx::CDatabaseEx(const CString& sPassword)
	:m_bUseTransaction(FALSE)
	,m_nRecordsetType(AFX_DB_USE_DEFAULT_TYPE)
	,m_sPassword(sPassword)
	,m_bReadConectionDataFromRegistry(TRUE)
{
	PopulateRegistryEntry();
}

CDatabaseEx::CDatabaseEx(LPCTSTR lpszConnectionString)
	:m_bUseTransaction(FALSE)
	,m_nRecordsetType(AFX_DB_USE_DEFAULT_TYPE)
	,m_bReadConectionDataFromRegistry(TRUE)
{
	PopulateRegistryEntry();
	if (lpszConnectionString && *lpszConnectionString)
		m_sConnectionString.Format(_T("%s"), lpszConnectionString);
}

CDatabaseEx::CDatabaseEx(LPCTSTR lpszConnectionString, UINT nRecordsetType)
	:m_bUseTransaction(FALSE)
	,m_nRecordsetType(AFX_DB_USE_DEFAULT_TYPE)
	,m_bReadConectionDataFromRegistry(TRUE)
{
	ASSERT(nRecordsetType == AFX_DB_USE_DEFAULT_TYPE	||
		nRecordsetType == CRecordset::dynaset			|| 
		nRecordsetType == CRecordset::snapshot			|| 
		nRecordsetType == CRecordset::dynamic			|| 
		nRecordsetType == CRecordset::forwardOnly);

	PopulateRegistryEntry();
	m_nRecordsetType = nRecordsetType;
	if (lpszConnectionString && *lpszConnectionString)
		m_sConnectionString.Format(_T("%s"), lpszConnectionString);
}

void CDatabaseEx::PopulateRegistryEntry()
{
	m_mapRegistryEntry.SetAt(_T("RegistryEntry"), _T("Settings"));
	m_mapRegistryEntry.SetAt(_T("DSNModel"), _T("DSNModel"));
	m_mapRegistryEntry.SetAt(_T("RSType"), _T("RsType"));
	m_mapRegistryEntry.SetAt(_T("DSNName"), _T("DSNName"));
	m_mapRegistryEntry.SetAt(_T("DSNDriver"), _T("DSNDriver"));
	m_mapRegistryEntry.SetAt(_T("DSNHost"), _T("DSNHost"));
	m_mapRegistryEntry.SetAt(_T("DSNServer"), _T("DSNServer"));
	m_mapRegistryEntry.SetAt(_T("DSNService"), _T("DSNService"));
	m_mapRegistryEntry.SetAt(_T("DSNProtocol"), _T("DSNProtocol"));
	m_mapRegistryEntry.SetAt(_T("DSNDatabase"), _T("DSNDatabase"));
	m_mapRegistryEntry.SetAt(_T("DSNUser"), _T("DSNUser"));
	m_mapRegistryEntry.SetAt(_T("DSNPass"), _T("DSNPass"));
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

const CString CDatabaseEx::GetDSN() const
{
	return m_sDSN;
}

void CDatabaseEx::SetDSN(const CString& sDSN)
{
	m_sDSN = sDSN;
}

const CString CDatabaseEx::GetDriver() const
{
	return m_sDriver;
}

void CDatabaseEx::SetDriver(const CString& sDriver)
{
	m_sDriver = sDriver;
}

const CString CDatabaseEx::GetHost() const
{
	return m_sHost;
}

void CDatabaseEx::SetHost(const CString& sHost)
{
	m_sHost = sHost;
}

const CString CDatabaseEx::GetServer() const
{
	return m_sServer;
}

void CDatabaseEx::SetServer(const CString& sServer)
{
	m_sServer = sServer;
}

const CString CDatabaseEx::GetService() const
{
	return m_sService;
}

void CDatabaseEx::SetService(const CString& sService)
{
	m_sService = sService;
}

const CString CDatabaseEx::GetProtocol() const
{
	return m_sProtocol;
}

void CDatabaseEx::SetProtocol(const CString& sProtocol)
{
	m_sProtocol = sProtocol;
}

const CString CDatabaseEx::GetDatabase() const
{
	return m_sDatabase;
}

void CDatabaseEx::SetDatabase(const CString& sDatabase)
{
	m_sDatabase = sDatabase;
}

const CString CDatabaseEx::GetUser() const
{
	return m_sUser;
}

void CDatabaseEx::SetUser(const CString& sUser)
{
	m_sUser = sUser;
}

const CString CDatabaseEx::GetPassword() const
{
	return m_sPassword;
}

void CDatabaseEx::SetPassword(const CString& sPassword)
{
	m_sPassword = sPassword;
}

BOOL CDatabaseEx::GetReadConectionDataFromRegistry() const
{
	return m_bReadConectionDataFromRegistry;
}

void CDatabaseEx::SetReadConectionDataFromRegistry(BOOL bReadConectionDataFromRegistry/* = TRUE*/)
{
	m_bReadConectionDataFromRegistry = bReadConectionDataFromRegistry;
}

void CDatabaseEx::ReadConectionDataFromRegistry()
{
	const CString sRegistryEntry = GetRegistryEntry(_T("RegistryEntry"));
	m_sDSN = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNName")));
	m_sDriver = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNDriver")));
	m_sHost = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNHost")));
	m_sServer = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNServer")));
	m_sService = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNService")));
	m_sProtocol = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNProtocol")));
	m_sDatabase = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNDatabase")));
	m_sUser = AfxGetApp()->GetProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNUser")));
}

void CDatabaseEx::WriteConectionDataToRegistry(const CString& sDSN, const CString& sDriver, const CString& sHost,
												const CString& sServer, const CString& sService, const CString& sProtocol,
												const CString& sDatabase, const CString& sUser)
{
	const CString sRegistryEntry = GetRegistryEntry(_T("RegistryEntry"));
	if (! sDSN.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNName")), sDSN);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNName")), NULL);
	if (! sDriver.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNDriver")), sDriver);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNDriver")), NULL);
	if (! sHost.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNHost")), sHost);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNHost")), NULL);
	if (! sServer.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNServer")), sServer);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNServer")), NULL);
	if (! sService.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNService")), sService);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNService")), NULL);
	if (! sProtocol.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNProtocol")), sProtocol);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNProtocol")), NULL);
	if (! sDatabase.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNDatabase")), sDatabase);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNDatabase")), NULL);
	if (! sUser.IsEmpty())
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNUser")), sUser);
	else
		AfxGetApp()->WriteProfileString(sRegistryEntry, GetRegistryEntry(_T("DSNUser")), NULL);
}

void CDatabaseEx::SetConnectionString(const CString& sConnectionString)
{
	m_sConnectionString = sConnectionString;
}

CString CDatabaseEx::GetRegistryEntry(const CString& sKey) const
{
	CString sRet;
	m_mapRegistryEntry.Lookup(sKey, sRet);
	return sRet;
}

void CDatabaseEx::SetRegistryEntry(const CString& sKey, const CString& sValue)
{
	m_mapRegistryEntry.SetAt(sKey, sValue);
}

void CDatabaseEx::SetRecordsetType(UINT nRecordsetType/* = CRecordset::dynaset*/)
{
	ASSERT(nRecordsetType == AFX_DB_USE_DEFAULT_TYPE	|| 
		nRecordsetType == CRecordset::dynaset			|| 
		nRecordsetType == CRecordset::snapshot			|| 
		nRecordsetType == CRecordset::dynamic			|| 
		nRecordsetType == CRecordset::forwardOnly);

	m_nRecordsetType = nRecordsetType;
	AfxGetApp()->WriteProfileInt(GetRegistryEntry(_T("RegistryEntry")), GetRegistryEntry(_T("RSType")), m_nRecordsetType);
}
// 0 - read mode, 1, 2, 3 write mode
void CDatabaseEx::InitConnectionString(int nModel/* = 0*/)
{
	Close();

	if (nModel < 0 || nModel > 3)
		return;

	const CString sRegistryEntry = GetRegistryEntry(_T("RegistryEntry"));

	if (0 == nModel)
		nModel = AfxGetApp()->GetProfileInt(sRegistryEntry, GetRegistryEntry(_T("DSNModel")), nModel);
	else
		AfxGetApp()->WriteProfileInt(sRegistryEntry, GetRegistryEntry(_T("DSNModel")), nModel);

	if (0 == nModel)
		return;

	if (m_bReadConectionDataFromRegistry)
		ReadConectionDataFromRegistry();

	m_sConnectionString.Empty();
	if (! m_sDSN.IsEmpty())
		m_sConnectionString.AppendFormat(_T("DSN=%s;"), m_sDSN);
	if (! m_sDriver.IsEmpty())
		m_sConnectionString.AppendFormat(_T("DRIVER=%s;"), m_sDriver);
	if (! m_sHost.IsEmpty())
		m_sConnectionString.AppendFormat(_T("HOST=%s;"), m_sHost);
	if (! m_sServer.IsEmpty())
	{
		switch (nModel)
		{
		case 3:
			m_sConnectionString.AppendFormat(_T("SERVERNAME=%s;"), m_sServer);
			break;
		default:
			m_sConnectionString.AppendFormat(_T("SERVER=%s;"), m_sServer);
		}
	}
	if (! m_sService.IsEmpty())
		m_sConnectionString.AppendFormat(_T("SERVICE=%s;"), m_sService);
	if (! m_sProtocol.IsEmpty())
		m_sConnectionString.AppendFormat(_T("PROTOCOL=%s;"), m_sProtocol);
	if (! m_sDatabase.IsEmpty())
		m_sConnectionString.AppendFormat(_T("DATABASE=%s;"), m_sDatabase);
	if (! m_sUser.IsEmpty())
	{
		switch (nModel)
		{
		case 2:
			m_sConnectionString.AppendFormat(_T("USER=%s;"), m_sUser);
			break;
		default:
			m_sConnectionString.AppendFormat(_T("UID=%s;"), m_sUser);
		}
	}
	if (! m_sPassword.IsEmpty())
	{
		switch (nModel)
		{
		case 2:
			m_sConnectionString.AppendFormat(_T("PASSWORD=%s;"), m_sPassword);
			break;
		default:
			m_sConnectionString.AppendFormat(_T("PWD=%s;"), m_sPassword);
		}
	}

	m_nRecordsetType = AfxGetApp()->GetProfileInt(sRegistryEntry, GetRegistryEntry(_T("RegistryEntryRSType")), CRecordset::dynaset);
}

BOOL CDatabaseEx::OpenConnection(DWORD dwOptions/* = CDatabase::noOdbcDialog*/)
{
	if (m_sConnectionString.IsEmpty())
		InitConnectionString();

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

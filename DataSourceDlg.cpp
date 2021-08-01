// DataSourceDlg.cpp : implementation file
//

#include "pch.h"
#include "DatabaseExplorer.h"
#include "DataSourceDlg.h"

#include "Crypt.h"
#include "MainFrm.h"
#include "DatabaseExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_KEY_LENGTH					255
#define MAX_VALUE_NAME					16383

/////////////////////////////////////////////////////////////////////////////
// CDataSourceDlg dialog

CDataSourceDlg::CDataSourceDlg(CDatabaseExplorerDoc* pDoc, CWnd* pParent /*=NULL*/)
	:CDialog(CDataSourceDlg::IDD, pParent)
	,m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CDataSourceDlg)
	m_sDSN = _T("");
	m_sDatabase = _T("");
	m_sDriver = _T("");
	m_sHost = _T("");
	m_sPassword = _T("");
	m_sProtocol = _T("");
	m_sServer = _T("");
	m_sService = _T("");
	m_sUser = _T("");
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDI_DATABASE);
}

void CDataSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataSourceDlg)
	DDX_Control(pDX, IDC_EDIT_DSN, m_editDSN);
	DDX_Text(pDX, IDC_EDIT_DSN, m_sDSN);
	DDX_Text(pDX, IDC_EDIT_DATABASE, m_sDatabase);
	DDX_Text(pDX, IDC_EDIT_DRIVER, m_sDriver);
	DDX_Text(pDX, IDC_EDIT_HOST, m_sHost);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_EDIT_PROTOCOL, m_sProtocol);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_sServer);
	DDX_Text(pDX, IDC_EDIT_SERVICE, m_sService);
	DDX_Text(pDX, IDC_EDIT_USER, m_sUser);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDataSourceDlg, CDialog)
	//{{AFX_MSG_MAP(CDataSourceDlg)
	ON_BN_CLICKED(IDC_RADIO_USERDSN, OnRadioUserDsn)
	ON_BN_CLICKED(IDC_RADIO_SYSTEMDSN, OnRadioSystemDsn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataSourceDlg message handlers

BOOL CDataSourceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	SetIcon(m_hIcon, FALSE);

	CDatabaseExt* pDB = m_pDoc->GetDB();

	m_editDSN.SetAutoComplete();
	const CString sRegistryEntry = pDB->GetRegistryEntry(_T("RegistryEntry"));

	switch(theApp.GetProfileInt(sRegistryEntry, RegistryEntryDSNSource, 0))
	{
	case 1:
		CheckRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN, IDC_RADIO_SYSTEMDSN);
		m_nDSNSource = 1;
		break;
	default:
		CheckRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN, IDC_RADIO_USERDSN);
		m_nDSNSource = 0;
		break;
	}

	switch (theApp.GetProfileInt(sRegistryEntry, pDB->GetRegistryEntry(_T("RSType")), CRecordset::dynaset))
	{
	case CRecordset::snapshot:
		CheckRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC, IDC_RADIO_SNAPSHOT);
		m_nRSTypeInit = CRecordset::snapshot;
		break;
	case CRecordset::dynamic:
		CheckRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC, IDC_RADIO_DYNAMIC);
		m_nRSTypeInit = CRecordset::dynamic;
		break;
	default:
		CheckRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC, IDC_RADIO_DYNASET);
		m_nRSTypeInit = CRecordset::dynaset;
		break;
	}

	switch (theApp.GetProfileInt(sRegistryEntry, RegistryEntryDSNFormatDate, 0))
	{
	case 1:
		CheckRadioButton(IDC_RADIO_YMD, IDC_RADIO_DMY, IDC_RADIO_DMY);
		m_nFormatDateInit = 1;
		break;
	default:
		CheckRadioButton(IDC_RADIO_YMD, IDC_RADIO_DMY, IDC_RADIO_YMD);
		m_nFormatDateInit = 0;
		break;
	}

	pDB->ReadConectionDataFromRegistry();

	m_sDSNInit = m_sDSN = pDB->GetDSN();
	m_sDriverInit = m_sDriver = pDB->GetDriver();
	m_sHostInit = m_sHost = pDB->GetHost();
	m_sServerInit = m_sServer = pDB->GetServer();
	m_sServiceInit = m_sService = pDB->GetService();
	m_sProtocolInit = m_sProtocol = pDB->GetProtocol();
	m_sDatabaseInit = m_sDatabase = pDB->GetDatabase();
	m_sUserInit = m_sUser = pDB->GetUser();

	HKEY hKey;
	CCrypt crypt;
	CString sTemp;
	LPBYTE lpByte;
	CByteArray arrBytes;
	DWORD dwLength, dwType = REG_BINARY;
	crypt.DeriveKey(g_sAppDSNPasswordKey);

	sTemp.Format(_T("Software\\%s\\%s\\%s\\"), AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName, sRegistryEntry);
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, sTemp, 0, KEY_ALL_ACCESS, &hKey))
	{
		if(ERROR_SUCCESS == RegQueryValueEx(hKey, pDB->GetRegistryEntry(_T("DSNPass")), NULL, &dwType, NULL, &dwLength))
		{
			arrBytes.SetSize(dwLength);
			lpByte = arrBytes.GetData();
			RegQueryValueEx(hKey, pDB->GetRegistryEntry(_T("DSNPass")), NULL, &dwType, lpByte, &dwLength);
			crypt.Decrypt(arrBytes, m_sPassword);
			m_sPasswordInit = m_sPassword;
		}
		RegCloseKey(hKey);
	}

	m_nDSModel = theApp.GetProfileInt(sRegistryEntry, pDB->GetRegistryEntry(_T("DSNModel")), 0);

	CStringArray saODBC;
	if(1 == m_nDSNSource)
		GetKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources"), saODBC);
	else
		GetKey(HKEY_CURRENT_USER, _T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources"), saODBC);

	for(int i = 0;i < saODBC.GetSize();++i)
		m_editDSN.AddString(saODBC.GetAt(i));

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataSourceDlg::OnRadioUserDsn() 
{
	// TODO: Add your control notification handler code here

	m_editDSN.RemoveAll();

	CStringArray saODBC;
	GetKey(HKEY_CURRENT_USER, _T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources"), saODBC);

	BOOL bFound = FALSE;
	for(int i = 0;i < saODBC.GetSize();++i)
	{
		m_editDSN.AddString(saODBC.GetAt(i));
		if(0 == m_sDSN.Compare(saODBC.GetAt(i)))
			bFound = TRUE;
	}

	if(! bFound)
		m_sDSN.Empty();

	UpdateData(FALSE);
}

void CDataSourceDlg::OnRadioSystemDsn() 
{
	// TODO: Add your control notification handler code here

	m_editDSN.RemoveAll();

	CStringArray saODBC;
	GetKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources"), saODBC);

	BOOL bFound = FALSE;
	for(int i = 0;i < saODBC.GetSize();++i)
	{
		m_editDSN.AddString(saODBC.GetAt(i));
		if(0 == m_sDSN.Compare(saODBC.GetAt(i)))
			bFound = TRUE;
	}

	if(! bFound)
		m_sDSN.Empty();

	UpdateData(FALSE);
}

void CDataSourceDlg::RestoreInitValues()
{
	HKEY hKey;
	CCrypt crypt;
	CString sTemp;
	LPBYTE lpByte;
	DWORD dwLength;
	CByteArray arrBytes;

	m_pDoc->GetDB()->WriteConectionDataToRegistry(m_sDSNInit, m_sDriverInit, m_sHostInit, 
												m_sServerInit, m_sServiceInit, m_sProtocolInit, 
												m_sDatabaseInit, m_sUserInit);

	crypt.DeriveKey(g_sAppDSNPasswordKey);
	crypt.Encrypt(m_sPasswordInit, arrBytes);
	lpByte = arrBytes.GetData();
	dwLength = static_cast<DWORD>(arrBytes.GetSize());

	const CString sRegistryEntry = m_pDoc->GetDB()->GetRegistryEntry(_T("RegistryEntry"));
	sTemp.Format(_T("Software\\%s\\%s\\%s\\"), AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName, sRegistryEntry);
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, sTemp, 0, KEY_ALL_ACCESS, &hKey))
	{
		RegSetValueEx(hKey, m_pDoc->GetDB()->GetRegistryEntry(_T("DSNPass")), 0, REG_BINARY, lpByte, dwLength);
		RegCloseKey(hKey);
	}
	theApp.WriteProfileInt(sRegistryEntry, RegistryEntryDSNSource, m_nDSNSource);
	theApp.WriteProfileInt(sRegistryEntry, m_pDoc->GetDB()->GetRegistryEntry(_T("DSNModel")), m_nDSModel);
	theApp.WriteProfileInt(sRegistryEntry, m_pDoc->GetDB()->GetRegistryEntry(_T("RSType")), m_nRSTypeInit);
	theApp.WriteProfileInt(sRegistryEntry, RegistryEntryDSNFormatDate, m_nFormatDateInit);
	m_pDoc->GetDB()->InitConnectionString(m_nDSModel);
	m_pDoc->GetDB()->SetRecordsetType(m_nRSTypeInit);
}

void CDataSourceDlg::GetKey(HKEY hKey, LPCTSTR lpSubKey, CStringArray& saResult)
{
	if (ERROR_SUCCESS != RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hKey))
		return;

	TCHAR    achKey[MAX_KEY_LENGTH];	// buffer for subkey name
	DWORD    cbName;					// size of name string
	TCHAR    achClass[MAX_PATH] = TEXT("");	// buffer for class name
	DWORD    cchClassName = MAX_PATH;	// size of class string
	DWORD    cSubKeys = 0;				// number of subkeys
	DWORD    cbMaxSubKey;				// longest subkey size
	DWORD    cchMaxClass;				// longest class string
	DWORD    cValues;					// number of values for key
	DWORD    cchMaxValue;				// longest value name
	DWORD    cbMaxValueData;			// longest value data
	DWORD    cbSecurityDescriptor;		// size of security descriptor
	FILETIME ftLastWriteTime;			// last write time

	DWORD i, retCode;

	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	// Get the class name and the value count
	retCode = RegQueryInfoKey(
		hKey,                    // key handle
		achClass,                // buffer for class name
		&cchClassName,           // size of class string
		NULL,                    // reserved
		&cSubKeys,               // number of subkeys
		&cbMaxSubKey,            // longest subkey size
		&cchMaxClass,            // longest class string
		&cValues,                // number of values for this key
		&cchMaxValue,            // longest value name
		&cbMaxValueData,         // longest value data
		&cbSecurityDescriptor,   // security descriptor
		&ftLastWriteTime);       // last write time

	// Enumerate the subkeys, until RegEnumKeyEx fails.
	if(cSubKeys)
	{
		TRACE(_T("\nNumber of subkeys: %d\n"), cSubKeys);
		for(i = 0;i < cSubKeys;++i)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i, 
				achKey, 
				&cbName, 
				NULL, 
				NULL, 
				NULL, 
				&ftLastWriteTime);
			if(retCode == ERROR_SUCCESS)
			{
				TRACE(_T("(%d) %s\n"),i + 1, achKey);
				saResult.Add(achKey);
			}
		}
	}
	// Enumerate the key values.
	if(cValues)
	{
		TRACE(_T("\nNumber of values: %d\n"), cValues);
		for(i = 0,retCode = ERROR_SUCCESS;i < cValues;++i)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			retCode = RegEnumValue(hKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				NULL, 
				NULL, 
				NULL);
			if(retCode == ERROR_SUCCESS)
			{
				TRACE(_T("(%d) %s\n"), i + 1, achValue);
				saResult.Add(achValue);
			}
		}
	}

	RegCloseKey(hKey);
}

void CDataSourceDlg::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData();

	BOOL bOK = FALSE;
	CDatabaseExt* pDB = m_pDoc->GetDB();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	const CString sRegistryEntry = pDB->GetRegistryEntry(_T("RegistryEntry"));

	if (IDC_RADIO_SYSTEMDSN == GetCheckedRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN))
		theApp.WriteProfileInt(sRegistryEntry, RegistryEntryDSNSource, 1);
	else
		theApp.WriteProfileInt(sRegistryEntry, RegistryEntryDSNSource, 0);

	switch(GetCheckedRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC))
	{
	case IDC_RADIO_SNAPSHOT:
		pDB->SetRecordsetType(CRecordset::snapshot);
		break;
	case IDC_RADIO_DYNAMIC:
		pDB->SetRecordsetType(CRecordset::dynamic);
		break;
	default:
		pDB->SetRecordsetType(CRecordset::dynaset);
	}

	if(IDC_RADIO_DMY == GetCheckedRadioButton(IDC_RADIO_YMD, IDC_RADIO_DMY))
	{
		theApp.WriteProfileInt(sRegistryEntry, RegistryEntryDSNFormatDate, 1);
	}
	else
	{
		theApp.WriteProfileInt(sRegistryEntry, RegistryEntryDSNFormatDate, 0);
	}

	pDB->WriteConectionDataToRegistry(m_sDSN, m_sDriver, m_sHost,
												m_sServer, m_sService, m_sProtocol,
												m_sDatabase, m_sUser);

	HKEY hKey;
	CCrypt crypt;
	CString sTemp;
	LPBYTE lpByte;
	DWORD dwLength;
	CByteArray arrBytes;

	crypt.DeriveKey(g_sAppDSNPasswordKey);
	crypt.Encrypt(m_sPassword, arrBytes);
	lpByte = arrBytes.GetData();
	dwLength = static_cast<DWORD>(arrBytes.GetSize());

	sTemp.Format(_T("Software\\%s\\%s\\%s\\"), AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName, sRegistryEntry);
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, sTemp, 0, KEY_ALL_ACCESS, &hKey))
	{
		RegSetValueEx(hKey, pDB->GetRegistryEntry(_T("DSNPass")), 0, REG_BINARY, lpByte, dwLength);
		RegCloseKey(hKey);

		pDB->SetPassword(m_sPassword);
	}

	CWaitCursor Wait;
//	pFrame->Login(NULL);
	for (int i = 1; i <= 3; ++i)
	{
		pDB->InitConnectionString(i);
///*/////// replace this select with one of yours ////////////////
		pDB->Execute(_T("SELECT count(*) FROM information_schema.tables"));
///*//////////////////////////////////////////////////////////////
		if(pDB->GetError().IsEmpty())
		{
			bOK = TRUE;
			theApp.WriteProfileInt(sRegistryEntry, pDB->GetRegistryEntry(_T("DSNModel")), i);
			break;
		}
	}

	if(! bOK)
	{
		if(! pDB->GetError().IsEmpty())
			MessageBox(pDB->GetError(), NULL, MB_ICONERROR);
		else
			MessageBox(_T("Setup data source name failed"), NULL, MB_ICONERROR);
		RestoreInitValues();
		return;
	}
	else
	{
///*/////// replace this select with one of yours ////////////////
		CStringArray saRes;
		pDB->GetRecordset(saRes, _T("SELECT count(*) FROM information_schema.tables"));
///*//////////////////////////////////////////////////////////////
		if (pDB->GetError().IsEmpty())
		{
			pFrame->SetMessageText(_T("You have successfully set up the datasource"));
		}
		else
		{
			MessageBox(pDB->GetError(), NULL, MB_ICONERROR);
			RestoreInitValues();
			return;
		}
	}

	pDB->Close();

	CDialog::OnOK();
}

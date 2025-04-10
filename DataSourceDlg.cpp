// DataSourceDlg.cpp : implementation file
//

#include "pch.h"
#include "DatabaseExplorer.h"
#include "DataSourceDlg.h"

#include "DatabaseExt.h"

#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRestoreConnectionSettings class

CDataSourceDlg::CRestoreConnectionSettings::CRestoreConnectionSettings(CDatabaseExplorerDoc* pDoc)
	:m_pDoc(pDoc)
{
	m_sDSN = m_pDoc->GetDSN().first;
	m_nRSType = m_pDoc->GetDB()->GetRecordsetType();
}

CDataSourceDlg::CRestoreConnectionSettings::~CRestoreConnectionSettings()
{
	if (m_bRestore)
	{
		m_pDoc->SetDSN(m_sDSN);
		m_pDoc->GetDB()->SetRecordsetType(m_nRSType);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPasswordHandler class

CDataSourceDlg::CPasswordHandler::CPasswordHandler(BOOL bAdmin, const CString& sDSN, const BOOL bSetDelete)
{
	m_bAdmin = bAdmin;
	m_sDSN = sDSN;
	m_bDeletePassword = bSetDelete;
}

CDataSourceDlg::CPasswordHandler::~CPasswordHandler()
{
	if (m_bDeletePassword)
	{
		CString sPath;
		sPath.Format(_T("SOFTWARE\\ODBC\\ODBC.INI\\%s"), m_sDSN);
		CSettingsStore ss(m_bAdmin, FALSE);
		if (ss.Open(sPath))
			ss.Write(_T("Password"), _T(""));	// reset password
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDataSourceDlg dialog

CDataSourceDlg::CDataSourceDlg(CDatabaseExplorerDoc* pDoc, CWnd* pParent /*=NULL*/)
	:CDialog(CDataSourceDlg::IDD, pParent)
	,m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CDataSourceDlg)
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDI_DATABASE);
}

void CDataSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataSourceDlg)
	DDX_Control(pDX, IDC_COMBO_DSN, m_ComboDSN);
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

	m_ComboDSN.AdjustDroppedWidth();
	m_ComboDSN.SetEditTooltip(TRUE, TRUE);
	m_ComboDSN.SetListTooltip(TRUE, TRUE);

	switch(m_pDoc->GetDSNSource())
	{
	case 1:
		CheckRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN, IDC_RADIO_SYSTEMDSN);
		PopulateDSN(TRUE);
		break;
	default:
		CheckRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN, IDC_RADIO_USERDSN);
		PopulateDSN(FALSE);
		break;
	}

	switch (m_pDoc->GetDB()->GetRecordsetType())
	{
	case CRecordset::snapshot:
		CheckRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC, IDC_RADIO_SNAPSHOT);
		break;
	case CRecordset::dynamic:
		CheckRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC, IDC_RADIO_DYNAMIC);
		break;
	default:
		CheckRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC, IDC_RADIO_DYNASET);
		break;
	}

	m_ComboDSN.SetCurSel(GetDSNIndex(m_pDoc->GetDSN().first));

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataSourceDlg::PopulateDSN(const BOOL bSystemDSN)
{
	m_ComboDSN.ResetContent();

	CSettingsStoreEx ss(bSystemDSN, TRUE);
	if (ss.Open(_T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources")))
	{
#ifdef _UNICODE
		std::unordered_map<std::wstring, std::wstring> keys;
#else
		std::unordered_map<std::string, std::string> keys;
#endif // _UNICODE
		ss.EnumValues(keys);
		for (const auto& it : keys)
			m_ComboDSN.AddStringWithInfo(it.first.c_str(), it.second.c_str());
	}
}

void CDataSourceDlg::OnRadioUserDsn() 
{
	// TODO: Add your control notification handler code here

	m_ComboDSN.SetCurSel(CB_ERR);
	PopulateDSN(FALSE);
	UpdateData(FALSE);
}

void CDataSourceDlg::OnRadioSystemDsn() 
{
	// TODO: Add your control notification handler code here

	m_ComboDSN.SetCurSel(CB_ERR);
	PopulateDSN(TRUE);
	UpdateData(FALSE);
}

int CDataSourceDlg::GetDSNIndex(const CString& sName) const
{
	CString sText;
	for (int i = 0; i < m_ComboDSN.GetCount(); ++i)
	{
		sText.Empty();
		m_ComboDSN.GetLBText(i, sText);
		if (sName == sText)
			return i;
	}

	return CB_ERR;
}

CString CDataSourceDlg::GetMsSQLAuthenticationRequiredUser(const CString& sDSN) const
{
	CString sUser, sPath;
	const BOOL bSystemDSN = (IDC_RADIO_SYSTEMDSN == GetCheckedRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN));
	CSettingsStoreEx ss(bSystemDSN, TRUE);
	sPath.Format(_T("SOFTWARE\\ODBC\\ODBC.INI\\%s"), sDSN);
	if (ss.Open(sPath))
		ss.Read(_T("LastUser"), sUser);

	return sUser;
}

const CString CDataSourceDlg::GetComboSelection(const BOOL bUpdateData/* = FALSE*/)
{
	if (bUpdateData)
		UpdateData();

	CString sText;
	m_ComboDSN.GetLBText(m_ComboDSN.GetCurSel(), sText);
	return sText;
}

const CString CDataSourceDlg::GetKeyData(const CString& sKeyName) const
{
	CString sValue;
	const BOOL bSystemDSN = (IDC_RADIO_SYSTEMDSN == GetCheckedRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN));
	CSettingsStoreEx ss(bSystemDSN, TRUE);

	if (ss.Open(_T("SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources")))
	{
#ifdef _UNICODE
		std::unordered_map<std::wstring, std::wstring> keys;
#else
		std::unordered_map<std::string, std::string> keys;
#endif // _UNICODE
		ss.EnumValues(keys);
		const auto found = keys.find(sKeyName.GetString());
		if (found != keys.end())
			sValue = found->second.c_str();
	}
	return sValue;
}

const DatabaseType CDataSourceDlg::DecodeDatabaseType(CString sData) const
{
	sData.MakeLower();

	if (-1 != sData.Find(_T("sql server")))
		return DatabaseType::MSSQL;

	if (-1 != sData.Find(_T("oracle")))
		return DatabaseType::ORACLE;

	if (-1 != sData.Find(_T("sqlite")))
		return DatabaseType::SQLITE;

	if (-1 != sData.Find(_T("mysql")))
		return DatabaseType::MYSQL;

	if (-1 != sData.Find(_T("maria")))
		return DatabaseType::MARIADB;

	if (-1 != sData.Find(_T("postgre")))
		return DatabaseType::POSTGRE;

	return DatabaseType::UNKNOWN;
}

BOOL CDataSourceDlg::IsSystemDsnSelected() const
{
	return (IDC_RADIO_SYSTEMDSN == GetCheckedRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN));
}

UINT CDataSourceDlg::GetSelectedRSType() const
{
	UINT nRSType = CRecordset::dynaset;

	switch (GetCheckedRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC))
	{
	case IDC_RADIO_SNAPSHOT:
		nRSType = CRecordset::snapshot;
		break;
	case IDC_RADIO_DYNAMIC:
		nRSType = CRecordset::dynamic;
		break;
	default:
		break;	// CRecordset::dynaset
	}

	return nRSType;
}

void CDataSourceDlg::Test(CDatabaseExt* pDB, const DatabaseType DBType) const
{
	switch (DBType)
	{
	case DatabaseType::MSSQL:
		if (! pDB->Execute(_T("SELECT count(*) FROM information_schema.tables")))
			break;
		pDB->GetData(_T("SELECT count(*) FROM information_schema.tables"));
		break;
	case DatabaseType::ORACLE:
		if (! pDB->Execute(_T("SELECT count(*) FROM GLOBAL_NAME")))
			break;
		pDB->GetData(_T("SELECT count(*) FROM GLOBAL_NAME"));
		break;
	case DatabaseType::SQLITE:
		if (! pDB->Execute(_T("SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT LIKE 'sqlite_%'")))
			break;
		pDB->GetData(_T("SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT LIKE 'sqlite_%'"));
		break;
	case DatabaseType::MYSQL:
	case DatabaseType::MARIADB:
		if (! pDB->Execute(_T("SELECT count(*) FROM information_schema.schemata")))
			break;
		pDB->GetData(_T("SELECT count(*) FROM information_schema.schemata"));
		break;
	case DatabaseType::POSTGRE:
		if (! pDB->Execute(_T("SELECT count(*) FROM pg_database")))
			break;
		pDB->GetData(_T("SELECT count(*) FROM pg_database"));
		if (! pDB->GetError().IsEmpty())
			break;
		m_pDoc->SetPostgreDB(m_pDoc->DecodePostGreDatabase(pDB->GetConnect()));
		break;
	default:
		pDB->SetError(_T("This version of application is supporting:\n\tMicrosoft SQL\n\tOracle\n\tSQLite\n\tMySQL\n\tMariaDB\n\tPostgreSQL\nFor other databases types contact the developer"));
	}

	pDB->Close();
}

void CDataSourceDlg::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData();

	if (m_ComboDSN.GetCurSel() < 0)
	{
		MessageBox(_T("Choose a data source name first!"), NULL, MB_ICONERROR);
		m_ComboDSN.SetFocus();
		m_ComboDSN.ShowDropDown();
		return;
	}

	CRestoreConnectionSettings rcs(m_pDoc);
	const BOOL bIsSystemDsnSelected = IsSystemDsnSelected();
	const UINT nRSType = GetSelectedRSType();
	const CString sSelectedDSN = GetComboSelection();
	const DatabaseType DBType = DecodeDatabaseType(GetKeyData(sSelectedDSN));

	CDatabaseExt* pDB = m_pDoc->GetDB();
	pDB->Close();

	CString sMsSQLAuthenticationUser;
	if (DatabaseType::MSSQL == DBType)
		sMsSQLAuthenticationUser = GetMsSQLAuthenticationRequiredUser(sSelectedDSN);

	CString sSuffix;
	if (DatabaseType::ORACLE == DBType || 
		(DatabaseType::MSSQL == DBType && ! sMsSQLAuthenticationUser.IsEmpty()))
	{
		CPasswordDlg dlg(bIsSystemDsnSelected, sSelectedDSN);
		if (IDOK != dlg.DoModal())
			return;
		sSuffix.Format(_T("UID=%s;PWD=%s;"), sMsSQLAuthenticationUser, dlg.GetPassword());
	}
	// only for this temporary connection
	pDB->SetConnectionString(_T("DSN=") + sSelectedDSN + _T(";") + sSuffix);
	pDB->SetRecordsetType(nRSType);

	CPasswordHandler ph(bIsSystemDsnSelected, sSelectedDSN, DatabaseType::ORACLE == DBType);

	CWaitCursor Wait;
	Test(pDB, DBType);
	if(! pDB->GetError().IsEmpty())
	{
		MessageBox(pDB->GetError(), NULL, MB_ICONERROR);
		return;
	}
	m_pDoc->SetDSNSource(bIsSystemDsnSelected);
	// save recordset type into registry
	theApp.WriteProfileInt(_T("Settings"), _T("RSType"), nRSType);
	// setup document data
	m_pDoc->SetDatabaseType(DBType);
	m_pDoc->SetDSN(sSelectedDSN);
	m_pDoc->SetMsSqlAuthenticationRequired(! sSuffix.IsEmpty());
	// give feedback to user
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_SETMESSAGETEXT, 3000, reinterpret_cast<LPARAM>(_T("You have successfully set up the datasource")));
	// everything is ok
	rcs.GiveUpRestoreDSNOrg();
	ph.GiveUpDeletePassword();

	CDialog::OnOK();
}

// DataSourceDlg.cpp : implementation file
//

#include "pch.h"
#include "DatabaseExplorer.h"
#include "DataSourceDlg.h"

#include "MainFrm.h"
#include "DatabaseExt.h"

#include "SettingsStoreEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

	switch(theApp.GetProfileInt(_T("Settings"), _T("DSNSource"), 0))
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

	switch (theApp.GetProfileInt(_T("Settings"), _T("RSType"), CRecordset::dynaset))
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

	m_ComboDSN.SetCurSel(GetDSNIndex(m_pDoc->GetDSNName()));

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

const CString CDataSourceDlg::GetComboSelection()
{
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

	if (-1 != sData.Find(_T("sql server native")))
		return DatabaseType::MSSQL;

	if (-1 != sData.Find(_T("sqlite")))
		return DatabaseType::SQLITE;

	if (-1 != sData.Find(_T("oracle")))
		return DatabaseType::ORACLE;

	if (-1 != sData.Find(_T("mysql")))
		return DatabaseType::MYSQL;

	if (-1 != sData.Find(_T("maria")))
		return DatabaseType::MARIADB;

	return DatabaseType::MSSQL;
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

	CDatabaseExt* pDB = m_pDoc->GetDB();

	pDB->Close();

	int nDSNSource = 0;	// User DSN
	if (IDC_RADIO_SYSTEMDSN == GetCheckedRadioButton(IDC_RADIO_USERDSN, IDC_RADIO_SYSTEMDSN))
		nDSNSource = 1;

	UINT nRSType = CRecordset::dynaset;
	const UINT nRSTypeOrg = pDB->GetRecordsetType();

	switch(GetCheckedRadioButton(IDC_RADIO_DYNASET, IDC_RADIO_DYNAMIC))
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

	pDB->SetConnectionString(_T("DSN=") + GetComboSelection() + _T(";"));
	pDB->SetRecordsetType(nRSType);

	const DatabaseType DBType = DecodeDatabaseType(GetKeyData(GetComboSelection()));

	CWaitCursor Wait;

	switch (DBType)
	{
	case DatabaseType::SQLITE:
		pDB->Execute(_T("SELECT name FROM sqlite_master WHERE type = 'table' AND name NOT LIKE 'sqlite_%'"));
		break;
	case DatabaseType::MYSQL:
	case DatabaseType::MARIADB:
		pDB->Execute(_T("SELECT count(*) FROM information_schema.schemata"));
		break;
	default:	// DatabaseType::MSSQL
		pDB->Execute(_T("SELECT count(*) FROM information_schema.tables"));
		break;
	}

	if(! pDB->GetError().IsEmpty())
	{
		pDB->SetRecordsetType(nRSTypeOrg);
		MessageBox(pDB->GetError(), NULL, MB_ICONERROR);
		return;
	}

	switch (DBType)
	{
	case DatabaseType::SQLITE:
		pDB->GetDataAsStdString(_T("SELECT count(*) FROM sqlite_master"));
		break;
	case DatabaseType::MYSQL:
	case DatabaseType::MARIADB:
		pDB->GetDataAsStdString(_T("SELECT count(*) FROM information_schema.schemata"));
		break;
	default:	// DatabaseType::MSSQL
		pDB->GetDataAsStdString(_T("SELECT count(*) FROM information_schema.tables"));
		break;
	}

	if (! pDB->GetError().IsEmpty())
	{
		pDB->SetRecordsetType(nRSTypeOrg);
		MessageBox(pDB->GetError(), NULL, MB_ICONERROR);
		return;
	}

	pDB->Close();

	theApp.WriteProfileInt(_T("Settings"), _T("DSNSource"), nDSNSource);
	theApp.WriteProfileInt(_T("Settings"), _T("RSType"), nRSType);

	m_pDoc->SetDatabaseType(DBType);
	m_pDoc->SetDSNName(GetComboSelection());

	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pFrame->SetMessageText(_T("You have successfully set up the datasource"));

	CDialog::OnOK();
}

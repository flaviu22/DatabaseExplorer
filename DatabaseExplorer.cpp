// DatabaseExplorer.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "DatabaseExplorer.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "DatabaseExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CDatabaseExplorerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDatabaseExplorerApp

BEGIN_MESSAGE_MAP(CDatabaseExplorerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CDatabaseExplorerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

// CDatabaseExplorerApp construction

CDatabaseExplorerApp::CDatabaseExplorerApp() noexcept
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DatabaseExplorer.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CDatabaseExplorerApp object

CDatabaseExplorerApp theApp;

// CDatabaseExplorerApp initialization

BOOL CDatabaseExplorerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (! AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
//	AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Sinis"));
	LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

	InitContextMenuManager();
	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	m_bVirtualMode = GetProfileInt(_T("Settings"), _T("VirtualMode"), 0);
	m_bWordWrap = GetProfileInt(_T("Settings"), _T("WordWrap"), 0);
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_DatabaseExplorerTYPE,
		RUNTIME_CLASS(CDatabaseExplorerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CDatabaseExplorerView));
	if (! pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (! pMainFrame || ! pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// suppress automatic creation of child window
	if (CCommandLineInfo::FileNew == cmdInfo.m_nShellCommand)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (! ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	const auto data = GetDocsOrder();
	for (const auto& it : data)
	{
		const CString sFile = GetBackupPath() + reinterpret_cast<LPCTSTR>(it.c_str());
		if (FileExist(sFile))
			OpenDocumentFile(sFile, FALSE);
	}

	return TRUE;
}

int CDatabaseExplorerApp::ExitInstance()
{
	WriteProfileInt(_T("Settings"), _T("VirtualMode"), m_bVirtualMode);
	WriteProfileInt(_T("Settings"), _T("WordWrap"), m_bWordWrap);

	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CDatabaseExplorerApp customization load/save methods

void CDatabaseExplorerApp::PreLoadState()
{
	CString strName;
	const BOOL bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CDatabaseExplorerApp::LoadCustomState()
{
}

void CDatabaseExplorerApp::SaveCustomState()
{
}

// CDatabaseExplorerApp message handlers

CString CDatabaseExplorerApp::GetAppPath() const
{
	CString sFullPath;
	GetModuleFileName(NULL, sFullPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	sFullPath.ReleaseBuffer();

	CString sDrive, sFolder, sPath;
	_wsplitpath(sFullPath, sDrive.GetBuffer(_MAX_PATH), sFolder.GetBuffer(_MAX_PATH), NULL, NULL);
	sDrive.ReleaseBuffer();
	sFolder.ReleaseBuffer();
	sPath.Format(_T("%s%s"), sDrive, sFolder);

	return sPath;
}

CString CDatabaseExplorerApp::GetAppPathTemp() const
{
	CString sTempPath, sTemp;
	GetTempPath(_MAX_PATH, sTempPath.GetBuffer(_MAX_PATH));
	sTemp.Format(_T("%s%s\\"), sTempPath, AfxGetApp()->m_pszAppName);

	if (! PathIsDirectory(sTemp) &&
		! CreateDirectory(sTemp, NULL) &&
		ERROR_ALREADY_EXISTS != GetLastError())
		return sTempPath;

	return sTemp;
}

CString CDatabaseExplorerApp::GetFileNameFrom(const CString& sPath) const
{
	CString sFile;
	_wsplitpath(sPath, NULL, NULL, sFile.GetBuffer(_MAX_PATH), NULL);
	sFile.ReleaseBuffer();
	return sFile;
}

CString CDatabaseExplorerApp::GetTitleNormalized(const CString& sTitle) const
{
	CString sText(sTitle);
	const int nIndex = sText.Find(':');
	if (-1 == nIndex)
		return sText;

	return sText.Left(nIndex);
}

BOOL CDatabaseExplorerApp::HasValidDocumentTitle(CString sTitle) const
{
	if (-1 != sTitle.Find(AfxGetAppName()))
	{
		sTitle.MakeReverse();
		if (isdigit(sTitle.GetAt(0)))
			return FALSE;
	}

	return TRUE;
}

std::vector<std::wstring> CDatabaseExplorerApp::GetBackupFiles() const
{
	CFileFind ff{};
	std::vector<std::wstring> data{};
	BOOL bMoreFiles = ff.FindFile(GetBackupPath() + _T("*.*"));

	while (bMoreFiles)
	{
		bMoreFiles = ff.FindNextFile();

		if (ff.IsDots() || ff.IsHidden())
			continue;
		if (! ff.IsDirectory() && -1 == ff.GetFileName().Find('.'))
			data.push_back(std::wstring(ff.GetFileName()));
	}

	return data;
}

void CDatabaseExplorerApp::UpdateBackupFiles()
{
	auto docdata = GetDocumentsData();
	if (docdata.size() > 0)
	{
		const CString sBackupPath = GetAppPath() + _T("Backup");
		if (! PathIsDirectory(sBackupPath))
			CreateDirectory(sBackupPath, NULL);
	}

	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(m_pMainWnd);
	SaveDocsOrder(pFrame->GetTabsNames());

	RemoveOldBackup(docdata);
	SaveNewBackup(std::move(docdata));
}

std::unordered_map<std::wstring, SDocData> CDatabaseExplorerApp::GetDocumentsData() const
{
	std::unordered_map<std::wstring, SDocData> docdata{};

	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pDocTemplate = static_cast<CDocTemplate*>(GetNextDocTemplate(pos));
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			CDatabaseExplorerDoc* pDoc = static_cast<CDatabaseExplorerDoc*>(pDocTemplate->GetNextDoc(posDoc));
			if (nullptr != pDoc && HasValidDocumentTitle(GetTitleNormalized(pDoc->GetTitle())))
			{
				const auto& it = docdata.find(pDoc->GetDSN().first.GetString());
				if (it == docdata.end())
					docdata.emplace(pDoc->GetDSN().first,
									SDocData(pDoc->GetDSNSource(),
									pDoc->GetDatabaseType(),
									pDoc->GetDB()->GetRecordsetType(),
									pDoc->GetMsSqlAuthenticationRequired(),
									pDoc->GetDocumentQueries()));
				else
					it->second.AddQueries(pDoc->GetDocumentQueries());
			}
		}
	}

	return docdata;
}

void CDatabaseExplorerApp::RemoveOldBackup(const std::unordered_map<std::wstring, SDocData>& docdata)
{
	CString sKey;
	const auto data = GetBackupFiles();
	for (const auto& it : data)
	{
		if (docdata.find(it) == docdata.end())
		{
			const CString sFile = GetBackupPath() + it.c_str();
			if (FileExist(sFile))
				DeleteFile(sFile);
			sKey.Format(_T("%s%s"), it.c_str(), STR_DSNSOURCE);
			WriteProfileString(_T("Backup"), sKey, nullptr);
			sKey.Format(_T("%s%s"), it.c_str(), STR_DATABASETYPE);
			WriteProfileString(_T("Backup"), sKey, nullptr);
			sKey.Format(_T("%s%s"), it.c_str(), STR_RSTYPE);
			WriteProfileString(_T("Backup"), sKey, nullptr);
			sKey.Format(_T("%s%s"), it.c_str(), STR_MSSQLAUTHENTICATIONREQUIRED);
			WriteProfileString(_T("Backup"), sKey, nullptr);
		}
	}
}

void CDatabaseExplorerApp::SaveDocsOrder(std::vector<CString>&& names) const
{
	const CString sFileName = GetAppPathTemp() + _T("DocsOrder");
	if (names.empty() && FileExist(sFileName))
	{
		DeleteFile(sFileName);
		return;
	}

	CStdioFile file;
	if (file.Open(sFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
	{
		for (const auto& it : names)
			file.WriteString(it + _T("\n"));
	}
}

std::vector<std::wstring> CDatabaseExplorerApp::GetDocsOrder() const
{
	CStdioFile file;
	std::vector<std::wstring> data{};
	if (file.Open(GetAppPathTemp() + _T("DocsOrder"), CFile::modeRead | CFile::typeText))
	{
		CString sLine;
		while (file.ReadString(sLine))
			data.push_back(sLine.GetString());
	}

	return data;
}

void CDatabaseExplorerApp::SaveNewBackup(std::unordered_map<std::wstring, SDocData>&& docdata)
{
	CString sKey;
	for (auto& it : docdata)
	{
		SaveQueries(it.first, std::move(it.second.m_queries));
		sKey.Format(_T("%s%s"), it.first.c_str(), STR_DSNSOURCE);
		WriteProfileInt(_T("Backup"), sKey, static_cast<int>(it.second.m_bDSNSource));
		sKey.Format(_T("%s%s"), it.first.c_str(), STR_DATABASETYPE);
		WriteProfileInt(_T("Backup"), sKey, static_cast<int>(it.second.m_DBType));
		sKey.Format(_T("%s%s"), it.first.c_str(), STR_RSTYPE);
		WriteProfileInt(_T("Backup"), sKey, it.second.m_nRecordsetType);
		sKey.Format(_T("%s%s"), it.first.c_str(), STR_MSSQLAUTHENTICATIONREQUIRED);
		WriteProfileInt(_T("Backup"), sKey, it.second.m_bMsSqlAuthenticationRequired);
	}
}

void CDatabaseExplorerApp::SaveQueries(const std::wstring& filename, std::vector<CString>&& queries) const
{
	CStdioFile file;
	if (! file.Open(GetBackupPath() + filename.c_str(),
		CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
		return;

	for (const auto it : queries)
		file.WriteString(it + _T("\n"));
}

void CDatabaseExplorerApp::DisconnectAllDatabases() const
{
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pDocTemplate = static_cast<CDocTemplate*>(GetNextDocTemplate(pos));
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			CDatabaseExplorerDoc* pDoc = static_cast<CDatabaseExplorerDoc*>(pDocTemplate->GetNextDoc(posDoc));
			if (pDoc)
			{
				POSITION posView = pDoc->GetFirstViewPosition();
				while (posView)
				{
					CView* pView = pDoc->GetNextView(posView);
					if (nullptr != pView->GetSafeHwnd())
					{
						pDoc->GetDB()->Close();
						pDoc->LogMessage(_T("Disconnected from the current database"), MessageType::info, static_cast<CChildFrame*>(pView->GetParentFrame()));
					}
				}				
			}
		}
	}
}

/* Copyright 2002-2016 Nikolay Avrionov. All Rights Reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#include "stdafx.h"
#include "ExplorerXP.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ExplorerXPDoc.h"
#include "ExplorerXPView.h"
#include "globals.h"
#include "hyperlink.h"

#include "DirSize.h"
#include "Themes.h"
#include "Options.h"
#include "FolderStateManager.h"
#include "ShortcutManager.h"



extern CDirSize dirs;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CShortcutManager gShortcutManager (FALSE);

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPApp

BEGIN_MESSAGE_MAP(CExplorerXPApp, CWinApp)
	//{{AFX_MSG_MAP(CExplorerXPApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_WINDOWS_CLOSE_ALL, OnWindowCloseAll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPApp construction

CExplorerXPApp::CExplorerXPApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CExplorerXPApp object

CExplorerXPApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPApp initialization


void LoadDefaultTabs ()
{
	InOut log (_T("LoadDefaultTabs"));

	CString lastFolder;
	DisableSync();
	switch (gOptions.m_OnStart) 
	{
		case ONSTART_NOTHING:
			break;

		case ONSTART_MYCOMP:	
			OpenFolder (CONST_MYCOMPUTER);
			break;

		case ONSTART_LASTSAVED:			
			for (unsigned int i = 0 ; i < gOptions.m_LastTabs.size (); i++)
			//if (IsDirectory (gOptions.m_LastTabs[i].m_Name))
			{
				SureBackSlashEx (gOptions.m_LastTabs[i].m_Name);
				OpenFolder (gOptions.m_LastTabs[i].m_Name);
				lastFolder = gOptions.m_LastTabs[i].m_Name;				
			}

			if (!gOptions.m_LastTabsActiveView.IsEmpty())
				lastFolder = gOptions.m_LastTabsActiveView;

			break;

		case ONSTART_DEFAULT:
			for (unsigned int i = 0 ; i < gOptions.m_DefaultTabs.size (); i++)			
			{
				SureBackSlashEx (gOptions.m_DefaultTabs[i].m_Name);
				OpenFolder (gOptions.m_DefaultTabs[i].m_Name);
				lastFolder = gOptions.m_DefaultTabs[i].m_Name;				
			}
			break;
	}

	EnableSync();


	if (!lastFolder.IsEmpty())
	{			
		SureBackSlashEx (lastFolder);
		OpenFolder (lastFolder);
		SyncUI (lastFolder);
	}
}

BOOL CExplorerXPApp::InitInstance()
{
	InOut log (TEXT("CExplorerXPApp::InitInstance"));

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	AfxOleInit();

	AfxEnableControlContainer();

	InitContextMenuManager();
	InitShellManager();

	InitThemes();
	COleMessageFilter *pfilter = AfxOleGetMessageFilter();
	if (pfilter) 
	{
		pfilter->EnableBusyDialog(FALSE);
		pfilter->EnableNotRespondingDialog(FALSE);
	}
	
	RegisterShellClipboardFormats ();
	
	/*HINSTANCE hInstance = LoadLibrary(_T("Bulgarian.dll"));
	if( hInstance )
	{
		AfxSetResourceHandle( hInstance );
	}
	*/
	InitSoftwareLogPath ();

	gOptions.Load ();
	gFolderStateMan.Load();
	gGroupManager.Load ();
	dirs.Load();
	
	// Parse command line for standard shell commands, DDE, file open
	
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	ParseCommandLine(cmdInfo);

	if ( !m_SingleInstance.Create( IDR_MAINFRAME, cmdInfo) )	
	    return FALSE;
	

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("ExplorerXP"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_EXPLORTYPE,
		RUNTIME_CLASS(CExplorerXPDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CExplorerXPView));
	AddDocTemplate(pDocTemplate);

	CMainFrame* pMainFrame = new CMainFrame;
	{
		InOut log2 (_T("Mainframe:Load"));
		// create main MDI Frame window
	
		if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
			return FALSE;
	}

	m_pMainWnd = pMainFrame;

	// Dispatch commands specified on the command line
	
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);

	pMainFrame->UpdateWindow();

	{
		InOut log2(_T("MainFrame->FillTree "));
//		pMainFrame->FillTree ();
	}

	LoadFilters ();
	dirs.Start ();

	LoadDefaultTabs();
	
	gShortcutManager.Initialize ();
	gShortcutManager.LoadAccel(IDR_MAINFRAME);

	if (!cmdInfo.m_strFileName.IsEmpty ())
	{
	//	//AfxMessageBox (cmdInfo.m_strFileName);
		if (cmdInfo.m_strFileName[cmdInfo.m_strFileName.GetLength() -1] == _T('\"'))
			cmdInfo.m_strFileName.Delete (cmdInfo.m_strFileName.GetLength() -1);
		OpenFolder (cmdInfo.m_strFileName);
	}

	pMainFrame->RepositionMDIChildsInitial ();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_Version;
	CString	m_Time;
	//}}AFX_DATA
	CHyperLink	m_HyperLink;
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_Version = EMPTYSTR;
	m_Time = EMPTYSTR;
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VERSION, m_Version);
	DDX_Control(pDX, IDC_HARMONDALE, m_HyperLink);
	DDX_Text(pDX, IDC_TIME, m_Time);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CExplorerXPApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
	//HICON hIcon = LoadIcon (IDR_EXPLORTYPE);
	//ShellAbout (AfxGetMainWnd()->GetSafeHwnd (), "ExplorerXP", "Nikolay Avrionov", hIcon);
}

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPApp message handlers



int CExplorerXPApp::ExitInstance() 
{
	SaveFilters ();
	FinThemes ();
	gOptions.Save ();
	gFolderStateMan.Save();
	gGroupManager.Save ();
	dirs.Save();
	return CWinApp::ExitInstance();
}

BOOL CExplorerXPApp::OnIdle(LONG lCount) 
{	
	return CWinApp::OnIdle(lCount);
}


CDocument* CExplorerXPApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	ASSERT(m_pDocManager != NULL);
    POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate( pos);
    return pTemplate->OpenDocumentFile (lpszFileName);	
}


char *data = __DATE__;


BOOL CAboutDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_Version = "Version 1.10";
  m_Time = data;
  UpdateData (FALSE);

  return TRUE; 	
}




void CExplorerXPApp::OnWindowCloseAll()
{
	CloseAllDocuments (FALSE);
}

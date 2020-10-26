/* Copyright 2002-2020 Nikolay Avrionov. All Rights Reserved.
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
#include "globals.h"
#include "ClipboardFiles.h"
#include "globals.h"
#include "dirsize.h"
#include "ExplorerXPDoc.h"
#include "ExplorerXPView.h"
#include "Viewers\RecBinViewer.h"
#include "Viewers\DriveViewer.h"
#include "DBT.H"
#include "OptionsDialog.h"
#include "Options.h"
#include "SelectDialog.h"
#include "ShellContextMenu.h"
#include "KeyboardPage.h"
#include "ShortCutManager.h"
#include "FolderSizeDlg.h"
#include "TabOptionsDlg.h"
#include "Themes.h"
#include "Shell32.h"

extern CDirSize dirs;
extern CExplorerXPApp theApp;
extern CShortcutManager gShortcutManager;
#define MAIN_UPDATETIMER 2705
#define MAIN_SAVETABSTIMER 2710


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMainFolder m_DriveTree;

#define NUM_FIXED_ITEMS 9
#define NUM_WINDOWS_ITEMS 19


static UINT indicators[] =
{	
	ID_INDICATOR_OBJECTS,
	ID_INDICATOR_SIZE,
	ID_INDICATOR_OBJECTS_SELECTED,
	ID_INDICATOR_SIZE_SELECTERD,
	ID_INDICATOR_DRIVESINFO
	//ID_SEPARATOR,           // status line indicator	
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CPersistMDIFrame )

BEGIN_MESSAGE_MAP(CMainFrame, CPersistMDIFrame )
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FOLDERTREE, OnFoldertree)	
	ON_WM_CHANGECBCHAIN()
	ON_WM_DRAWCLIPBOARD()
	ON_WM_DESTROY()	
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_INITMENUPOPUP()
	ON_WM_SETTINGCHANGE()
	//	ON_WM_DEVMODECHANGE()
	
	ON_COMMAND(ID_DESKTOP, OnDesktop)
	ON_COMMAND(ID_TEMPFOLDER, OnTempFolder)
	ON_COMMAND(ID_MYCOMPUTER, OnMycomputer)
	ON_COMMAND(ID_RECYCLEBIN, OnRecyclebin)
	ON_COMMAND(ID_MYCOMPUTER_PROPERTIES, OnMycomputerProperties)
	ON_COMMAND(ID_RECYCLE_PROPERTIES, OnRecycleProperties)
	ON_COMMAND(ID_RECYCLE_EMPTY, OnRecycleEmpty)
	ON_COMMAND(ID_RECYCLE_RESTOREALL, OnRecycleRestoreall)
	ON_COMMAND(ID_MYDOCUMENTS, OnMydocuments)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_WINDOWS_NEXT, OnWindowNext)
	ON_COMMAND(ID_WINDOWS_PREVIOUS, OnWindowPrevious)
	ON_COMMAND(ID_WINDOWS_TILE_HORZ, OnWindowTileHorz)
	ON_COMMAND(ID_WINDOWS_TILE_VERT, OnWindowTileVert)
	ON_COMMAND(ID_WINDOWS_CASCADE, OnWindowCascade)
	ON_COMMAND(ID_VIEW_ADDRESSBAR, OnViewAddressbar)	
	ON_UPDATE_COMMAND_UI(ID_RECYCLE_EMPTY, OnUpdateRecycleEmpty)	
	ON_UPDATE_COMMAND_UI(ID_RECYCLE_RESTOREALL, OnUpdateRecycleRestoreall)		
	ON_UPDATE_COMMAND_UI(ID_VIEW_ADDRESSBAR, OnUpdateViewAddressbar)		
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SIZE,    OnUpdateSize)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OBJECTS, OnUpdateObjects)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OBJECTS_SELECTED, OnUpdateObjectsSelected)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SIZE_SELECTERD, OnUpdateSizeSelected)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DRIVESINFO, OnUpdateDrivesInfo)
	ON_UPDATE_COMMAND_UI(ID_RECYCLEBIN, OnUpdateRecyclebin)
	ON_UPDATE_COMMAND_UI(ID_FOLDERTREE, OnUpdateFoldertree)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)	
	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHELLNOTIFY, OnShellNotify)
	ON_MESSAGE(WM_SHELLNOTIFYRBINDIR, OnNotifyRBinDir)
	ON_MESSAGE(WM_EXP_OPENFROMSHELL, OnWmOpenFromShell)	
	ON_MESSAGE(MSG_SHELLCHANGE, OnChangeDesktop)
	
	ON_COMMAND(ID_VIEW_TABSONTOP, OnViewTabsontop)
	ON_COMMAND(ID_VIEW_TABSONBOTTOM, OnViewTabsonbottom)	
	ON_COMMAND(ID_FILE_OPTIONS, OnFileOptions)	
	ON_COMMAND(ID_SAVEDEFAULT, OnSavedefault)
	ON_COMMAND(ID_MOVETOADDRESSBAR, OnMovetoaddressbar)
	ON_COMMAND(ID_ADDRESSSHOWDROPDOWN, OnAddressshowdropdown)
	ON_COMMAND(ID_HELP_COMMUNITY, OnHelpCommunity)
	ON_COMMAND(ID_HELP_EXPLORERXPHOMEPAGE, OnHelpExplorerxphomepage)
	ON_COMMAND(ID_XX_CLOSEOTHERTABS, OnXxCloseothertabs)
	ON_COMMAND(ID_FAVS, OnFavs)
	ON_COMMAND(ID_VIEW_CUSTOMIZETOOLBAR, OnViewCustomizetoolbar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	
	ON_COMMAND(ID_GROUPS_ADDANEWGROUP, OnGroupsAddanewgroup)
	ON_COMMAND_RANGE (GROUP_BASE_CMD, GROUP_BASE_CMD_LAST, OnOpenGroup)	
	ON_COMMAND_RANGE (MIN_ID, MAX_ID, OnNewMenuDummy)	
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)

	ON_UPDATE_COMMAND_UI(ID_FAVS, OnUpdateFavs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TABSONBOTTOM, OnUpdateViewTabsonbottom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TABSONTOP, OnUpdateViewTabsontop)	

	ON_UPDATE_COMMAND_UI(ID_WINDOWS_CASCADE, OnUpdateWindowsCascade)

	ON_WM_THEMECHANGED()
	ON_WM_WININICHANGE()
	ON_COMMAND(ID_WINDOWS_MORE, OnWindowsMore)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
:m_bAcc (true),
m_bEmptyRecycle (true)
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];

        ASSERT(pInfo != NULL);

        int nDockedCount = pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {            
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPersistMDIFrame ::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	if ( !m_wndReBar.Create( this, AFX_IDW_CONTROLBAR_LAST - 1,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_SIZE_DYNAMIC ) 
          )
    {
        return -1;
    }


    if ( !m_wndToolBar.Create( &m_wndReBar, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_FLYBY ) )
    {
        TRACE0("Failed to create toolbar(s)\n");
        return -1;      // fail to create
    }

    m_wndToolBar.ModifyStyle( 0, CCS_ADJUSTABLE );

		
	if (!m_wndDlgBar.Create(&m_wndReBar, IDR_MAINFRAME, CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	m_wndReBar.AddBar(&m_wndToolBar, 0, 0, RBBS_GRIPPERALWAYS, _T("&Toolbar"), false) ;
	m_wndReBar.AddBar(&m_wndDlgBar, 0, 0, RBBS_GRIPPERALWAYS, _T("Address Bar"), false);


	m_wndToolBar.Init (); 
	//m_wndReBar.AddBar(&m_wndToolBar, _T("&Toolbar"), NULL, RBBS_USECHEVRON |RBBS_GRIPPERALWAYS ) ;
	//m_wndReBar.AddBar(&m_wndDlgBar,_T("Address Bar"));
	
	
	 
	 //m_wndReBar.GetReBarCtrl().MaximizeBand(0);
	m_wndReBar.GetReBarCtrl().RestoreBand(0);
	
	
	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	gpStatusBar = &m_wndStatusBar;

	EnableDocking (CBRS_ALIGN_ANY);
	
	
	UINT style =   WS_CHILD|WS_VISIBLE| TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |  TVS_SHOWSELALWAYS | WS_BORDER;

	if (!m_FolderBar.Create(_T("Folders"), this, 127))
	{
		TRACE0("Failed to create instant bar\n");
		return -1;		// fail to create
	}

	m_FolderBar.SetSCBStyle(m_FolderBar.GetSCBStyle() | SCBS_SIZECHILD);

	/*
	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | WS_BORDER;

	if (!m_wndTree.Create(dwTreeStyle, rectDummy, &m_FolderBar, ID_DRIVETREE))
	{
		TRACE0("Failed to create Folder bar child\n");
		return -1;		// fail to create
	}*/

	
	if (!m_DriveTree.Create(style, CRect(0, 0, 0, 0), &m_FolderBar, ID_DRIVETREE))
	{
		TRACE0("Failed to create Folder bar child\n");
		return -1;		// fail to create
	}

    //m_DriveTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	
	
	m_FolderBar.SetBarStyle(m_FolderBar.GetBarStyle() | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_FolderBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	DockControlBar(&m_FolderBar, AFX_IDW_DOCKBAR_LEFT);
	RecalcLayout();

	if (!m_Favorities.Create(_T("Groups"), this, 128))
	{
		TRACE0("Failed to create Groups bar\n");
		return -1;		// fail to create
	}

	m_Favorities.SetSCBStyle(m_Favorities.GetSCBStyle() | SCBS_SIZECHILD);
	m_Favorities.SetBarStyle(m_Favorities.GetBarStyle() | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	if (!gFavs.Create(style, CRect(0, 0, 0, 0), &m_Favorities, ID_DRIVETREE))
	{
		TRACE0("Failed to create Favs Tree\n");
		return -1;		// fail to create
	}

    //gFavs.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	gFavs.Reload();

	m_Favorities.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);

	/*if (!m_ShellBar.Create(_T("Shell"), this, 128))
	{
		TRACE0("Failed to create Shell bar\n");
		return -1;		// fail to create
	}

	m_ShellBar.SetSCBStyle(m_Favorities.GetSCBStyle() | SCBS_SIZECHILD);
	m_ShellBar.SetBarStyle(m_Favorities.GetBarStyle() | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	if (!m_ShellTree.Create(style, CRect(0, 0, 0, 0), &m_ShellBar, ID_DRIVETREE))
	{
		TRACE0("Failed to create Shell Tree\n");
		return -1;		// fail to create
	}

	m_ShellTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_ShellTree.InitializeTree();

	m_ShellBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);*/
	
	CRect rect;

	
	m_FolderBar.GetWindowRect(&rect);
	rect.top = rect.bottom-1 ;
	rect.bottom = rect.bottom + 5;

	
	DockControlBar(&m_Favorities, AFX_IDW_DOCKBAR_LEFT, rect);
	//DockControlBar(&m_ShellBar, AFX_IDW_DOCKBAR_LEFT, rect);

//	VERIFY(m_MDIClient.SubclassMDIClient(this, &m_wndViewManager));

	m_wndDocSelector.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | CBRS_TOP,
		         CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR);

	if (gOptions.m_bTabsOnTop == TRUE)
		m_wndDocSelector.SetBarStyle(CBRS_ALIGN_TOP);
	else
		m_wndDocSelector.SetBarStyle(CBRS_ALIGN_BOTTOM);

	CString sProfile(_T("BarState"));
    if (VerifyBarState(sProfile))
    {
        m_FolderBar.LoadState(sProfile);
		m_Favorities.LoadState (sProfile);
		m_wndReBar.LoadState(sProfile);
        LoadBarState(sProfile);
		m_wndToolBar.LoadState(sProfile);
		
    }

	m_bCallingSetClipboardViewer = TRUE;
    m_hNextClipboardViewer = SetClipboardViewer();
    m_bCallingSetClipboardViewer = FALSE;

	//if (VerifyBarState (AfxGetApp()->m_pszProfileName))
	//	LoadBarState (AfxGetApp()->m_pszProfileName);


    // Read the stuff on the clipboard, if there is any.
    OnReadClipboard();
	gRecBinViewer.Install (m_hWnd);
	
	//SetTimer (MAIN_UPDATETIMER, 1500, NULL);
	SetTimer(MAIN_UPDATETIMER, 2500, NULL);

	// save the tabs every 2 minutes
	SetTimer(MAIN_SAVETABSTIMER, 60000 * 2, NULL);

	// install shell notification handler
	{
		LPITEMIDLIST	pidlDesktop	= NULL;
		HRESULT hr = SHGetSpecialFolderLocation (m_hWnd, CSIDL_DESKTOP, &pidlDesktop);
		
		if (FAILED(hr)) return 0;

		SHChangeNotifyEntry cne;
		cne.fRecursive	= TRUE;
		cne.pidl	= pidlDesktop;

		const LONG fEvents = SHCNE_CREATE | SHCNE_DELETE | 
				SHCNE_MKDIR | SHCNE_RMDIR | SHCNE_RENAMEFOLDER | 
				SHCNE_RENAMEITEM | SHCNE_UPDATEITEM | SHCNE_UPDATEDIR |
				SHCNE_ATTRIBUTES | SHCNE_FREESPACE;

			m_ShellNotifyHandle = SHChangeNotifyRegister( m_hWnd, 
				SHCNF_ACCEPT_INTERRUPTS | SHCNF_ACCEPT_NON_INTERRUPTS, 
				fEvents, 
				MSG_SHELLCHANGE, 
				1, 
				&cne );


	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = theApp.m_SingleInstance.GetClassName();
	return CPersistMDIFrame::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CPersistMDIFrame::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CPersistMDIFrame::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// If pHandlerInfo is NULL, then handle the message
	if (pHandlerInfo == NULL)
	{
		// Filter the commands sent to Window menu
		if (nID >= ID_WINDOWS_FIRST && nID <= ID_WINDOWS_LAST)
		{
			if (nCode == CN_COMMAND)
				// Handle WM_COMMAND message
				ActivateFrameFromMenu(nID);
			else 
				if (nCode == CN_UPDATE_COMMAND_UI)
					// Update UI element state
				DoUpdateWindowMenu(nID, (CCmdUI*)pExtra);
			return TRUE;
		}
				
	}

	// If we didn't process the command, call the base class
	// version of OnCmdMsg so the message-map can handle the message
	
	return CPersistMDIFrame ::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnFoldertree() 
{
	ShowControlBar (&m_FolderBar, !m_FolderBar.IsWindowVisible (), FALSE);
}

void CMainFrame::OnUpdateFoldertree(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_FolderBar.IsWindowVisible ());
}



/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

// Handler for WM_CHANGECBCHAIN.
void CMainFrame::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter) 
{
    // If the next window in the chain is being removed, reset our
    // "next window" handle.
    if ( m_hNextClipboardViewer == hWndRemove )
        {
            m_hNextClipboardViewer = hWndAfter;
        }

    // If there is a next clipboard viewer, pass the message on to it.
    if ( NULL != m_hNextClipboardViewer )
        {
            ::SendMessage ( m_hNextClipboardViewer, WM_CHANGECBCHAIN, (WPARAM) hWndRemove, (LPARAM) hWndAfter );
        }

    CPersistMDIFrame::OnChangeCbChain(hWndRemove, hWndAfter);
}

// Handler for WM_DRAWCLIPBOARD.  We get this message when the contents of
// the clipboard changes.

void CMainFrame::OnDrawClipboard() 
{
    // Note the m_bCallingSetClipboardViewer flag.  This flag is set before
    // the call to SetClipboardViewer().  We get a WM_DRAWCLIPBOARD message
    // before SetClipboardViewer() returns, but we must not process it.
    // So we don't do anything if that flag is set.

    if ( !m_bCallingSetClipboardViewer )
        {
			// If there is a next clipboard viewer, pass the message on to it.
			if ( NULL != m_hNextClipboardViewer )
				::SendMessage ( m_hNextClipboardViewer, WM_DRAWCLIPBOARD, 0, 0 );

			// Read the contents of the clipboard.
			OnReadClipboard();

			CPersistMDIFrame::OnDrawClipboard();
        }
}

// Handler for WM_DESTROY.  Tell Windows that we're closing so the clipboard
// viewer chain gets updated properly.
void CMainFrame::OnDestroy() 
{
	dirs.Stop ();
	
    if ( NULL != m_hNextClipboardViewer )
        ChangeClipboardChain ( m_hNextClipboardViewer );
        
	
    CPersistMDIFrame::OnDestroy();
}

void CMainFrame::OnReadClipboard()
{
	gClipboard.ReadClipboard ();	
	dirs.Notify (WM_EXP_CLIPCHANGED, 0, NULL);
}

void CMainFrame::OnUpdateSize(CCmdUI *pCmdUI)
{
	if (!IsNotInFOP())
		return;
	
	pCmdUI->Enable(); 
	CExplorerXPView *pView = GetCurrentView ();
	if (pView == NULL) 
	  pCmdUI->SetText (EMPTYSTR);
	else			
		pCmdUI->SetText (pView->GetSizeOfAll ());	
}

void CMainFrame::OnUpdateSizeSelected(CCmdUI *pCmdUI)
{
	if (!IsNotInFOP())
		return;
	
	pCmdUI->Enable(); 
	CExplorerXPView *pView = GetCurrentView ();
	if (pView == NULL) 
	  pCmdUI->SetText (EMPTYSTR);
	else			
		pCmdUI->SetText (pView->GetSelectedSize());	
}


void CMainFrame::OnUpdateObjects(CCmdUI *pCmdUI)
{
	if (!IsNotInFOP())
		return;
	
	pCmdUI->Enable(); 
	CExplorerXPView *pView = GetCurrentView ();
	if (pView == NULL) 
	  pCmdUI->SetText (EMPTYSTR);
	else			
		pCmdUI->SetText (pView->GetCountAll ());	
}

void CMainFrame::OnUpdateObjectsSelected(CCmdUI *pCmdUI)
{
	if (!IsNotInFOP())
		return;
	
	pCmdUI->Enable(); 
	CExplorerXPView *pView = GetCurrentView ();
	if (pView == NULL) 
	  pCmdUI->SetText (EMPTYSTR);
	else			
		pCmdUI->SetText (pView->GetCountSelection ());	
}

void CMainFrame::OnUpdateDrivesInfo (CCmdUI *pCmdUI)
{
	if (!IsNotInFOP())
		return;
	
	pCmdUI->Enable(); 

	CExplorerXPView *pView = GetCurrentView ();
	if (pView == NULL) 
		pCmdUI->SetText (EMPTYSTR);
	else				
		pCmdUI->SetText (gDriveViewer.GetDriveInfoStr(pView->GetRoot()));	
}

void CMainFrame::OnDropDown( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    NMTOOLBAR* pNMToolBar = ( NMTOOLBAR* )pNotifyStruct;
       
	*pResult =TBDDRET_NODEFAULT;
	
	switch (pNMToolBar->iItem)
	{
		case ID_RECYCLEBIN:
			{
				DoRecycleMenu (pNotifyStruct);
				*pResult = TBDDRET_DEFAULT;	
				return;
			}
		case ID_MYCOMPUTER:
			{
				DoMyComputerMenu (pNotifyStruct);
				*pResult = TBDDRET_DEFAULT;
				return;				
			}
	}		        
}

void CMainFrame::OnMycomputer() 
{
	OpenFolder (CONST_MYCOMPUTER);
}

void CMainFrame::OnRecyclebin() 
{
	OpenFolder (CONST_RECYCLEBIN);
}


void CMainFrame::SetFullRecycle ()
{
	if (m_bEmptyRecycle)
	{
		m_bEmptyRecycle = false;
		m_wndToolBar.ChangeButtonImage (ID_RECYCLEBIN,	11);
	}
}

void CMainFrame::SetEmptyRecycle ()
{
	if (!m_bEmptyRecycle)
	{
		m_bEmptyRecycle = true;
		m_wndToolBar.ChangeButtonImage (ID_RECYCLEBIN,	10);		
	}
}

void SpecialFolderProperties (int iFolder)
{
	LPITEMIDLIST   pidlMFolder;

	SHGetSpecialFolderLocation(NULL, iFolder, &pidlMFolder);

	SHELLEXECUTEINFO sei;

    ZeroMemory(&sei,sizeof(sei));
    sei.cbSize = sizeof(sei);    
	sei.lpIDList = pidlMFolder;
    sei.lpVerb = _T("properties");
    sei.fMask  = SEE_MASK_INVOKEIDLIST;

    ShellExecuteEx(&sei); 	
}

void CMainFrame::OnMycomputerProperties() 
{
	SpecialFolderProperties (CSIDL_DRIVES);	
}

void CMainFrame::OnRecycleProperties() 
{
	SpecialFolderProperties (CSIDL_BITBUCKET);
}


LRESULT CMainFrame::OnNotifyRBinDir (WPARAM wParam, LPARAM lParam)
{
	//LRESULT ret =  gRecBinViewer.OnNotifyRBinDir (wParam, lParam);
	//dirs.Notify (WM_EXP_RECYCLEUPDATE, NULL);
	gRecBinViewer.MarkForUpdate ();
	gDriveViewer.MarkForUpdate ();
	return 0; 
}

LRESULT CMainFrame::OnShellNotify (WPARAM wParam, LPARAM lParam)
{
//	LRESULT ret = gRecBinViewer.OnShellNotify (wParam, lParam);
	//dirs.Notify (WM_EXP_RECYCLEUPDATE, NULL);
	gRecBinViewer.MarkForUpdate ();
	gDriveViewer.MarkForUpdate ();
	return 0;
}
  
void pidlToString( LPCITEMIDLIST lpIL, CString& retStr )
{
	static TCHAR buffer [MAX_PATH*32 + 2];
	if( SHGetPathFromIDList( lpIL, buffer ) )
	{
		retStr = buffer;
	}
}

LRESULT CMainFrame::OnChangeDesktop (WPARAM wParam, LPARAM lParam)
{
	// Unpack string
		if (wParam  == NULL)
		 return NULL;

		CString s1, s2;

		const LPCITEMIDLIST* pidls = (LPCITEMIDLIST*)wParam;
		const LONG wEventId = (LONG)lParam;

		
		pidlToString( pidls[0], s1 );

		// Determine change type
		switch( wEventId )
		{
		case SHCNE_CREATE:
		//	addLocalFile( s1 );
			dirs.Invalidate (s1);
			break;
		case SHCNE_MKDIR:
		//	addLocalFolder( s1 );
			dirs.Invalidate (s1);
			break;
		case SHCNE_RENAMEITEM:
			pidlToString( pidls[1], s2 );		
			//dirs.InvalidateName (s1, s2);
			dirs.Invalidate (s2, true);
			dirs.Invalidate (s1, false);
			break;
		case SHCNE_RENAMEFOLDER:
			pidlToString( pidls[1], s2 );
			// find a better solution here
			//dirs.InvalidateName (s1, s2);
			dirs.Invalidate (s2, true);
			dirs.Invalidate (s1, false);
			break;
		case SHCNE_DELETE:
		//	deleteLocalFile( s1 );
			dirs.Invalidate (s1, true);
			break;
		case SHCNE_RMDIR:
			dirs.Invalidate (s1, true);
//			deleteLocalFolder( s1 );
			break;
		case SHCNE_UPDATEITEM:
		case SHCNE_ATTRIBUTES:
			dirs.Invalidate (s1);
		//	updateLocalFile( s1 );
			break;
		}

		return NULL;
}

void CMainFrame::OnUpdateRecyclebin(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable ();
	static bool recycleFull = false;
	bool full = gRecBinViewer.GetCount () != 0 ;

	if (full)
		SetFullRecycle ();
	else
		SetEmptyRecycle ();

	if (full != recycleFull) {
		recycleFull = full;
		m_wndDocSelector.reDraw();
	}
}

void CMainFrame::OnRecycleEmpty() 
{
	gRecBinViewer.Empty ();
	
}

void CMainFrame::OnUpdateRecycleEmpty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (gRecBinViewer.GetCount () != 0);	
}

void CMainFrame::OnRecycleRestoreall() 
{
	gRecBinViewer.UndeleteAll ();	
}

void CMainFrame::OnUpdateRecycleRestoreall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (gRecBinViewer.GetCount () != 0);
}

void CMainFrame::FillTree()
{
	m_DriveTree.Fill ();	
}

void CMainFrame::OnClose() 
{
	gFavs.GetOpen();

	SaveBarState(AfxGetApp()->m_pszProfileName);	

	saveTabState();
	
	CString sProfile(_T("BarState"));
	m_FolderBar.SaveState(sProfile);
	m_wndReBar.SaveState(sProfile);
	m_wndToolBar.SaveState (sProfile);
    SaveBarState(sProfile);	

	CPersistMDIFrame::OnClose();
}

void CMainFrame::saveTabState()
{
	CStringAr tabs;
	m_wndDocSelector.GetTabs(tabs);
	gOptions.m_LastTabs.clear();
	for (unsigned int i = 0; i < tabs.size(); i++)
	{
		CStrOptions opt;
		opt.m_Name = tabs[i];
		gOptions.m_LastTabs.push_back(opt);
	}

	BOOL maximized;
	CMDIChildWnd* child = MDIGetActive(&maximized);

	if (child)
	{
		if (maximized)
			gOptions.m_LastWindowMode = WINDOW_FULL;

	}

	gOptions.m_LastTabsActiveView.Empty();

	CExplorerXPView* pView = ::GetCurrentView();

	if (pView) {
		gOptions.m_LastTabsActiveView = pView->GetRoot();
		if (!pView->IsSpecialViewer())
			SureBackSlash(gOptions.m_LastTabsActiveView);
	}

	gOptions.Save();
}

void CMainFrame::GetTabs (CStringAr &tabs)
{
	m_wndDocSelector.GetTabs (tabs);	
}

void CMainFrame::UpdateTabs()
{
	m_wndDocSelector.UpdateTabs ();
}

void CMainFrame::OnMydocuments() 
{
	TCHAR path[2094];

	SHGetSpecialFolderPath (m_hWnd, path, CSIDL_PERSONAL, FALSE);
	CString mydocs = path;
	SureBackSlash (mydocs);
	OpenFolder (mydocs);	
}

void CMainFrame::OnTempFolder() 
{
	TCHAR path[2094];

	GetTempPath(MAX_PATH, path);
	CString tempFolder = path;
	SureBackSlash (tempFolder);
	OpenFolder (tempFolder);	
}

void CMainFrame::OnDesktop() 
{
	TCHAR path[2094];

	SHGetSpecialFolderPath (m_hWnd, path, CSIDL_DESKTOP , FALSE);
	CString mydocs = path;
	SureBackSlash (mydocs);
	OpenFolder (mydocs);	
}

void CMainFrame::OnViewAddressbar() 
{
	ShowControlBar (&m_wndDlgBar	, !m_wndDlgBar.IsWindowVisible (), FALSE);
}

void CMainFrame::OnUpdateViewAddressbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndDlgBar.IsWindowVisible ());	
}

void CMainFrame::OnHelpHelp() 
{
	CString cs;
	GetExePath (cs);	
	cs += "keyhelp.html";
	ShellExecute (m_hWnd, NULL, cs, NULL, NULL, SW_SHOWNORMAL);	
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{

	if (pMsg->message == WM_DEVICECHANGE)
	{
		gDriveViewer.MarkForUpdate ();
		m_DriveTree.MarkForUpdate ();		
		TRACE(_T("Drive Update %d\n"), pMsg->wParam);
			/*switch (pMsg->wParam){
				case DBT_DEVICEARRIVAL:				// 
				case DBT_DEVICEREMOVECOMPLETE:		// We only concern about these two cases.
					{
						TRACE("Drive Update\n");
						gDriveViewer.MarkForUpdate ();
					}
			}	*/
	}

//	if ((pMsg->message == WM_WININICHANGE) || (pMsg->message == WM_THEMECHANGED))
//		return CPersistMDIFrame ::PreTranslateMessage(pMsg);

	int nCmdID = gShortcutManager.ProcessMessage(pMsg);

	if (nCmdID)
		SendMessage(WM_COMMAND, nCmdID);
	else
		return CPersistMDIFrame ::PreTranslateMessage(pMsg);

	return TRUE;
}

void CMainFrame::OnWindowNext() 
{
	 ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDINEXT, 0, 1); 		
}

void CMainFrame::OnWindowPrevious() 
{	
	MDINext ();
}

void CMainFrame::RepositionMDIChilds ()
{
	BOOL maximized;
    CMDIChildWnd* child = MDIGetActive(&maximized);
	if (!child)
		return;

	if (maximized)
		gOptions.m_LastWindowMode = WINDOW_FULL;
	
	if (gOptions.m_LastWindowMode == WINDOW_TILEV)
		::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0);

	if (gOptions.m_LastWindowMode == WINDOW_TILEH)
		::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);	
}

void CMainFrame::RepositionMDIChildsInitial () 
{
	switch (gOptions.m_LastWindowMode)
	{
		case WINDOW_TILEV:
			::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0);
			break;

		case WINDOW_TILEH:
			::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);	
			break;

		case WINDOW_CASCADE:
			::SendMessage(m_hWndMDIClient,  WM_MDICASCADE, 0, 0);	
			break;
		case WINDOW_FULL:
			{
				CView *pView = GetActiveView ();
				if (!pView)
					break;
				::SendMessage(m_hWndMDIClient,  WM_MDIMAXIMIZE, (WPARAM)pView->GetParent()->GetSafeHwnd(), 0);	
			}
			break;
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CPersistMDIFrame ::OnSize(nType, cx, cy);

	int count =  sizeof(indicators) / sizeof(UINT);
	UINT nID; 
	UINT nStyle; 
	int cxWidth;
	int totalWidth = 0;
	int separator = -1;

	for (int i = 0; i < count; i++)
	{
		m_wndStatusBar.GetPaneInfo(i, nID, nStyle, cxWidth);
		if (nID != 0)
			totalWidth += cxWidth;
		else
			separator = i;
	}

	if (separator != -1)
	{
		m_wndStatusBar.GetPaneInfo(separator, nID, nStyle, cxWidth);
		CRect rc;
		GetClientRect(rc);
		cxWidth = rc.Width() - totalWidth - count * 3;
		cxWidth -= GetSystemMetrics(SM_CXVSCROLL);
		if (cxWidth <= 0)
			cxWidth = 1;
		m_wndStatusBar.SetPaneInfo(separator, nID, nStyle, cxWidth);
	}
	RepositionMDIChilds ();
}

void CMainFrame::OnWindowTileHorz() 
{
	::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);	
	gOptions.m_LastWindowMode = WINDOW_TILEH;
}

void CMainFrame::OnWindowTileVert() 
{
	::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0);	
	gOptions.m_LastWindowMode = WINDOW_TILEV;
}

void CMainFrame::OnWindowCascade() 
{	
	::SendMessage(m_hWndMDIClient, WM_MDICASCADE, 0, 0);	
	gOptions.m_LastWindowMode = WINDOW_CASCADE;
}

void CMainFrame::OnUpdateViewTabsontop(CCmdUI *pCmdUI)
{
	DWORD dwStyle = m_wndDocSelector.GetBarStyle();
	if ( (dwStyle & CBRS_ALIGN_TOP) == CBRS_ALIGN_TOP)
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnViewTabsontop()
{
	m_wndDocSelector.SetBarStyle(CBRS_ALIGN_TOP);
	gOptions.m_bTabsOnTop = TRUE;
	RecalcLayout ();
}

void CMainFrame::OnViewTabsonbottom()
{
	m_wndDocSelector.SetBarStyle(CBRS_ALIGN_BOTTOM);
	gOptions.m_bTabsOnTop = FALSE;
	RecalcLayout ();
}

void CMainFrame::OnUpdateViewTabsonbottom(CCmdUI *pCmdUI)
{
	DWORD dwStyle = m_wndDocSelector.GetBarStyle();
	if ( (dwStyle & CBRS_ALIGN_BOTTOM) == CBRS_ALIGN_BOTTOM)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);


}

void CMainFrame::OnFileOptions()
{
	/*COptionsDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		dirs.Notify (WM_EXP_OPTIONS, 0, NULL);
		m_DriveTree.MarkAll();
	}*/

	CPropertySheet sheet (IDS_SETTINGS);
	COptionsDialog dlg;
	CFolderSizeDlg sizes;	
	CKeyboardPage keyboard;
	CTabOptionsDlg tabs;

	sheet.AddPage(&dlg);
	sheet.AddPage(&sizes);
	sheet.AddPage(&keyboard);
	sheet.AddPage(&tabs);
	sheet.DoModal();
}


void CMainFrame::OnSavedefault()
{
	CStringAr tabs;
	m_wndDocSelector.GetTabs (tabs);

	gOptions.m_DefaultTabs.clear ();
	for (unsigned int  i = 0 ; i < tabs.size (); i++)
	{
		CStrOptions opt;
		opt.m_Name = tabs[i];
		gOptions.m_DefaultTabs.push_back (opt);
	}

	gOptions.Save ();
}

void CMainFrame::OnMovetoaddressbar()
{
	if (m_wndDlgBar.IsWindowVisible())
		m_wndDlgBar.FocusOnAddress ();
}


void CMainFrame::OnAddressshowdropdown()
{
	if (m_wndDlgBar.IsWindowVisible())
		m_wndDlgBar.ShowDropDown();
}


LRESULT CMainFrame::OnWmOpenFromShell(WPARAM, LPARAM lParam)
{
	LPVOID lpvFile = ::MapViewOfFile((HANDLE)lParam, FILE_MAP_READ, 0, 0, 0);

	if (!lpvFile) 
		return FALSE;

	CString filename = (LPTSTR)lpvFile;

	if (filename[filename.GetLength() -1] == _T('\"'))
		filename.Delete (filename.GetLength() -1);

	::OpenFolder (filename);
	::UnmapViewOfFile(lpvFile);
	::CloseHandle((HANDLE)lParam);
	RepositionMDIChilds ();
	return TRUE;
}
BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CMainFrame::OnViewCustomizetoolbar()
{
	m_wndToolBar.Customize ();
}

void CMainFrame::OnViewToolbar()
{
	ShowControlBar (&m_wndToolBar, !m_wndToolBar.IsWindowVisible (), FALSE);
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (m_wndToolBar.IsWindowVisible ());	
}


void CMainFrame::TabFrom (int idView, int idDirection)
{
	CWnd *wnd[3];
	wnd[0] = GetCurrentView();
	wnd[1] = &m_wndDlgBar;
	wnd[2] = &m_DriveTree;

	CWnd *pStart = wnd[idView];
	int next = 0;
	if (FORWARD == idDirection)
		next = 1;
	else
		next = -1;

	int nextView  = idView;
	CWnd *pNext = NULL;

calc_next:

	nextView = (nextView + next + 3)  % 3;
	pNext = wnd[nextView];

	if (pNext == pStart)
		return;

	if (!pNext)
		goto calc_next;

	if (!pNext->IsWindowVisible ())
		goto calc_next;

	pNext->SetFocus ();
}

void CMainFrame::OnFavs()
{
	ShowControlBar (&m_Favorities, !m_Favorities.IsWindowVisible (), FALSE);
}

void CMainFrame::OnUpdateFavs(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (m_Favorities.IsWindowVisible ());
}


void CMainFrame::OnTimer(UINT nIDEvent) 
{
	static ULONGLONG skipFirst = 0;
	skipFirst++; 
	if ((nIDEvent == MAIN_UPDATETIMER) && (gOptions.m_FolderSizeMode != FOLDERSIZE_MANUAL))	
	{		
		TRACE (_T("MainFrame Check Timer\n"));
		StartSizeThread ();			
	}

	if (nIDEvent == MAIN_UPDATETIMER)		
	{
		if (!IsNotInFOP())
			return;

		gRecBinViewer.CheckUpdate ();	
		gDriveViewer.CheckUpdate ();
		m_DriveTree.CheckUpdate ();
	}	

	if (nIDEvent == MAIN_SAVETABSTIMER)
		saveTabState();
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CPersistMDIFrame::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);	
		
	CMDIChildWnd *pChild =  MDIGetActive(NULL );

	if (pChild && nIndex != 6)
		return;

	if (!pChild && nIndex != 2)
		return;

	for (int i = 0; i < MAX_GROUPS; i++)
	{
		BOOL ret = pPopupMenu->DeleteMenu (GROUP_BASE_CMD+i, MF_BYCOMMAND);
		if (!ret)
			break;
	}

	for ( unsigned i = 0; i < min (gGroupManager.size(), MAX_GROUPS); i++)
		pPopupMenu->AppendMenu(MF_STRING, GROUP_BASE_CMD+i, gGroupManager[i].m_Name);
}

void CMainFrame::OnGroupsAddanewgroup()
{
	CSelectDialog dlg (_T("Add Group"), NULL);

	if (dlg.DoModal())
	{	
		gGroupManager.Add(dlg.m_Wildcards);
		gFavs.Reload();
		gFavs.Invalidate();
	}
}

void CMainFrame::OnOpenGroup(UINT nID)
{
	int index = nID - GROUP_BASE_CMD;
	if (index >= 0 && index < gGroupManager.size())
	{
		gGroupManager.OpenGroup(index);
	}
}

void CMainFrame::OnHelpCommunity()
{
	ShellExecute(NULL, _T("open"), _T("http://www.explorerxp.com/phpBB2/"), NULL,NULL, SW_SHOW);			
}

void CMainFrame::OnHelpExplorerxphomepage()
{
	ShellExecute(NULL, _T("open"), _T("http://www.explorerxp.com"), NULL,NULL, SW_SHOW);
}

void CMainFrame::OnXxCloseothertabs()
{
	
}

void CMainFrame::OnNewMenuDummy(UINT nID)
{
}

LRESULT CMainFrame::OnThemeChanged()
{
	TRACE(_T("OnThemeChanged"));
	FinThemes ();
	InitThemes ();

	return 1;
}

void CMainFrame::OnWinIniChange(LPCTSTR lpszSection)
{
	CPersistMDIFrame::OnWinIniChange(lpszSection);

	TRACE(_T("OnWinIniChange"));
	FinThemes ();
	InitThemes ();
}



CMenu* CMainFrame::GetWindowMenu()
{

// MENU_POSITION is not reliable because it's different for different views
		
	CMenu* pWindowMenu = NULL;
	CMenu* pTopMenu = GetMenu();
	CString strMenu;
	for (int iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
	{
		pTopMenu->GetMenuString( iPos, strMenu, MF_BYPOSITION );
		if (strMenu == "&Window")
		{
			pWindowMenu = pTopMenu->GetSubMenu(iPos);
			break;
		}
	}
	ASSERT(pWindowMenu != NULL);
	return pWindowMenu;
}

void CMainFrame::UpdateWindowMenu()
{
	CMenu* pWindowMenu = GetWindowMenu();

	//Remove all entries after separator
	for (	int i = pWindowMenu->GetMenuItemCount()- 1 ; i > NUM_FIXED_ITEMS - 1;
			pWindowMenu->DeleteMenu(i--, MF_BYPOSITION) );

	//m_hWndMDIClient - undocumented

	CWnd* pWnd = CWnd::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);

	CString strTitle;
	int i = 0;
	while(pWnd)
	{
		if(pWnd)
			if(pWnd->IsWindowVisible())
			{
			    if (i == NUM_WINDOWS_ITEMS)    //Critical number ow windows (9) reached
				{
					pWindowMenu->AppendMenu( MF_STRING,  ID_WINDOWS_MORE, _T("&Windows..."));
					return;
				}

				CString tabName;
				pWnd->GetWindowText(tabName);
				
				if (i < 9)
					strTitle.Format (_T("&%d "), i+1);
				else
					strTitle.Format (_T("%d "), i+1);

				strTitle += tabName;

				pWindowMenu->AppendMenu( MF_STRING,  ID_WINDOWS_FIRST + i++, strTitle);
			}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

}

void CMainFrame::ActivateFrameFromMenu(int nItem)
{
	CWnd* pWnd = CWnd::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);
	int i = ID_WINDOWS_FIRST;
	BOOL bMaximized;

	while(pWnd)
	{
		if(pWnd)
			if(pWnd->IsWindowVisible())
				if ( i++ == nItem)
				{
					MDIGetActive( &bMaximized );
					if (!bMaximized)
						MDIRestore(pWnd);
					MDIActivate(pWnd);
					return;
				}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CMainFrame::DoUpdateWindowMenu(int nID, CCmdUI* pCCmdUI)
{
	pCCmdUI->Enable(TRUE);
	if (nID == ID_WINDOWS_FIRST)
		pCCmdUI->SetCheck();
}

void CMainFrame::OnUpdateWindowsCascade(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//It is a challange to find a better place to call it from
	UpdateWindowMenu();
}


void CMainFrame::FillWinList(CListBox* pListBox)
{
	CWnd* pWnd = CWnd::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);
	CString strTitle;
	while(pWnd)
	{
		if(pWnd)
			if(pWnd->IsWindowVisible())
			{
				pWnd->GetWindowText(strTitle);
				pListBox->AddString(strTitle);
			}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
	pListBox->SetCurSel(0);
}

/////////////////////////////////////////////////////////////////////////////
// CWinList dialog

CWinList::CWinList(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CWinList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWinList::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinList)
	DDX_Control(pDX, IDC_LIST_WINDOWS, m_lbWinList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWinList, CResizableDialog)
	//{{AFX_MSG_MAP(CWinList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinList message handlers


BOOL CWinList::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	AddAnchor (IDC_LIST_WINDOWS, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDOK, BOTTOM_LEFT);
	AddAnchor (IDCANCEL, BOTTOM_LEFT);

	EnableSaveRestore (_T("CWinList")); 
	// TODO: Add extra initialization here
	((CMainFrame*) AfxGetMainWnd())->FillWinList(&m_lbWinList);
	m_lbWinList.SetCurSel(0);
	m_lbWinList.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWinList::OnOK() 
{
	// TODO: Add extra validation here
	EndDialog(m_lbWinList.GetCurSel() + ID_WINDOWS_FIRST);
	//CDialog::OnOK();
}

void CMainFrame::OnWindowsMore() 
{
	int nItem = CWinList().DoModal();
	if (nItem != IDCANCEL) 
		ActivateFrameFromMenu(nItem);
}

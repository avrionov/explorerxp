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


#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#pragma once

#include "Sizebar\SizableReBar.h"
#include "Bars\docselect.h"
#include "Bars\MainToolbar.h"
#include "MainFolder.h"
#include "AddressBar\AddressBar.h"
#include "Persist\PersistFrames.h"
#include "FavTree.h"
#include "ResizableLib\ResizableDialog.h"
#include "ShellTree.h"


//#include "OXFileWatcher.h"

extern CMainFolder m_DriveTree;

class CMainFrame : public CPersistMDIFrame 
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

	CDocSelector	m_wndDocSelector;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	afx_msg LRESULT OnShellNotify (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyRBinDir (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWmOpenFromShell(WPARAM, LPARAM lParam);
	afx_msg LRESULT OnChangeDesktop (WPARAM wParam, LPARAM lParam);
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	void FillTree();
	CStatusBar  m_wndStatusBar;
	CMainToolBar    m_wndToolBar;
	//CToolBar		m_wndToolBar;
	CSizableReBar	m_wndReBar;
//	CReBar  m_wndReBar;
	CAddressBar      m_wndDlgBar;
	baseCMyBar	m_FolderBar; // instant bar
	baseCMyBar	m_Favorities; //
	baseCMyBar  m_ShellBar;
//	CShellTree  m_ShellTree;

	
	void DisableAcc ();
	void EnableAcc ();

	void SetFullRecycle ();
	void SetEmptyRecycle ();
	BOOL VerifyBarState(LPCTSTR lpszProfileName);
	void RepositionMDIChilds ();
	void RepositionMDIChildsInitial ();

	void TabFrom (int idView, int idDirection);
	void GetTabs (CStringAr &tabs);
	void UpdateTabs ();
	
// Generated message map functions
//protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFoldertree();
	afx_msg void OnUpdateFoldertree(CCmdUI* pCmdUI);
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDrawClipboard();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateSize(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSizeSelected(CCmdUI *pCmdUI);
	afx_msg void OnUpdateObjects(CCmdUI *pCmdUI);
	afx_msg void OnUpdateObjectsSelected(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDrivesInfo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRecyclebin (CCmdUI *pCmdUI);
	afx_msg void OnMycomputer();
	afx_msg void OnRecyclebin();
	afx_msg void OnMycomputerProperties();
	afx_msg void OnRecycleProperties();
	afx_msg void OnRecycleEmpty();
	afx_msg void OnUpdateRecycleEmpty(CCmdUI* pCmdUI);
	afx_msg void OnRecycleRestoreall();
	afx_msg void OnUpdateRecycleRestoreall(CCmdUI* pCmdUI);
	afx_msg void OnClose();	
	afx_msg void OnMydocuments();
	afx_msg void OnDesktop();
	afx_msg void OnTempFolder();
	afx_msg void OnViewAddressbar();
	afx_msg void OnUpdateViewAddressbar(CCmdUI* pCmdUI);
	afx_msg void OnHelpHelp();
	afx_msg void OnWindowNext();
	afx_msg void OnWindowPrevious();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowTileHorz();
	afx_msg void OnWindowTileVert();
	afx_msg void OnWindowCascade();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnOpenGroup(UINT nID);
	afx_msg void OnNewMenuDummy(UINT nID);
	afx_msg void OnDropDown( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnReadClipboard();
	afx_msg void OnUpdateWindowsCascade(CCmdUI* pCmdUI);
	afx_msg void OnWindowsMore();
	DECLARE_MESSAGE_MAP()
	bool m_bAcc;

	HWND m_hNextClipboardViewer;
    BOOL m_bCallingSetClipboardViewer;
	bool  m_bEmptyRecycle;	
	DWORD	m_ShellNotifyHandle;
public:
	afx_msg void OnUpdateViewTabsontop(CCmdUI *pCmdUI);
	afx_msg void OnViewTabsontop();
	afx_msg void OnViewTabsonbottom();
	afx_msg void OnUpdateViewTabsonbottom(CCmdUI *pCmdUI);
	afx_msg void OnFileOptions();	
	afx_msg void OnSavedefault();
	afx_msg void OnMovetoaddressbar();
	afx_msg void OnAddressshowdropdown();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewCustomizetoolbar();
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI *pCmdUI);
	afx_msg void OnFavs();
	afx_msg void OnUpdateFavs(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnGroupsAddanewgroup();
	afx_msg void On32891();
	afx_msg void OnHelpCommunity();
	afx_msg void OnHelpExplorerxphomepage();
	afx_msg void OnXxCloseothertabs();	
	LRESULT OnThemeChanged();
	afx_msg void OnWinIniChange(LPCTSTR lpszSection);

	void FillWinList(CListBox* pListBox);

//windows menu managment
protected:
	CMenu* GetWindowMenu();
	void UpdateWindowMenu();
	void ActivateFrameFromMenu(int nItem);
	void DoUpdateWindowMenu(int nID, CCmdUI* pCCmdUI);
  CShellTree m_wndTree;
};

/////////////////////////////////////////////////////////////////////////////

 /////////////////////////////////////////////////////////////////////////////
// CWinList dialog

class CWinList : public CResizableDialog
{
// Construction
public:
	CWinList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWinList)
	enum { IDD = IDD_DIALOG_WINLIST };
	CListBox	m_lbWinList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWinList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __MAINFRAME_H__

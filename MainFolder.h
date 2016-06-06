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


#ifndef __MANINFOLDER_H__
#define __MANINFOLDER_H__

#pragma once

#include  "Sizebar\SizeBarInclude.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFolder window

#include "DriveTree.h"

#include "MainTreeDropTarget.h"

class CMainFolder : public CDriveTree
{

//	DECLARE_DYNCREATE(CMainFolder)

// Construction
public:
	CMainFolder();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFolder)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFolder();
	void OnSelectionChanged (CString& strPathName);

public:
    //virtual void OnBeginDrag();
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	void StartTimer (HTREEITEM hITem);
	void MarkAll ();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFolder)
		afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnGetTip(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);		
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnUpdateMoveto(CCmdUI* pCmdUI);
		afx_msg void OnUpdateCopyto(CCmdUI* pCmdUI);

		afx_msg void OnMoveto();	
		afx_msg void OnCopyto();
	
		afx_msg void onRename ();
		afx_msg void onNewFolder ();
		afx_msg void onCopy ();
		afx_msg void onCut ();
		afx_msg void onPaste ();
		afx_msg void onDelete ();
		afx_msg void onDeletePerm ();

	//}}AFX_MSG
	LPARAM OnFolderUpdate(WPARAM wp, LPARAM lp);	
	LPARAM OnFileRenamed (WPARAM wp, LPARAM lp);	
	DECLARE_MESSAGE_MAP()
	bool m_bDisableUpdate;
	bool m_arUpdateDrives[1024];

protected:

	virtual UINT AddDirectoryNodes (HTREEITEM hItem, CString& strPathName);
	//virtual CString GetItemStr (HTREEITEM hItem);
	
	CMainTreeDropTarget  m_DropTarget;
	CSelRowArray m_DropFiles;
	HTREEITEM m_hLastDropItem;
	CString m_DropFolder;
	CString m_LastDropFolder;
	bool m_bDragDataAcceptable;
	HTREEITEM m_hTimerItem;	
public:	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __MANINFOLDER_H__

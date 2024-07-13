/* Copyright 2002-2021 Nikolay Avrionov. All Rights Reserved.
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


#ifndef __EXPLORERXP_VIEW_H__
#define __EXPLORERXP_VIEW_H__

#pragma once

#include <vector>
#include "FileInfo.h"
#include "Viewers\Viewer.h"
#include "Viewers\FileViewer.h"
#include "Viewers\DriveViewer.h"
#include "Viewers\RecBinViewer.h"
#include "History.h"


class CGridList;

struct IDropTargetHelper;    // in case the newest PSDK isn't available.

class CExplorerXPDoc;

class CExplorerXPView : public CView
{
protected: // create from serialization only
	CExplorerXPView();
	DECLARE_DYNCREATE(CExplorerXPView)

	enum VIEW_TYPE
	{
		VIEW_NOTHING,
		VIEW_FOLDER,
		VIEW_MYCOMPUTER,
		VIEW_ZIP,
        VIEW_BIN
	};

// Attributes
public:
	CExplorerXPDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExplorerXPView)
	public:
	virtual void OnInitialUpdate(); // called first time after construct	
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragScroll( DWORD dwKeyState, CPoint point );
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC) {} ;  // overridden to draw this view
	
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridLBDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridLBUp (NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBeginDrag (NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBeginEdit (NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnEndEdit (NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRightClick (NMHDR *pNotifyStruct, LRESULT* pResult); 
	
	// Implementation
public:
	void GetSelection (CSelRowArray &ar);
	void GetSelectionFast (CSelRowArray &ar);
	void SaveSelection ();
	void RestoreSelection ();
	void RestoreSelection (CSelRowArray &sel);
	void RestoreSelectionFast (CSelRowArray &sel);
	void Sort();
	void Fill();
	void Fill (const TCHAR *, bool bHistory = true);
	void HardFill ();
	bool FileOperation (CSelRowArray &ar, UINT wFunc, FILEOP_FLAGS fFlags, const TCHAR *pTo);

	bool FileOperation (COleDataObject *object, bool bClipboard = true, DROPEFFECT dwEffect = DROPEFFECT_NONE, const TCHAR *pTo = NULL);
	
	bool Rename (const TCHAR *src, const TCHAR *dst, FILEOP_FLAGS fFlags);
	bool Rename (CSelRowArray &src, CSelRowArray &dst, FILEOP_FLAGS fFlags);

	//DROPEFFECT CalcDropEffect (const TCHAR * root, const TCHAR * drop_source,  DWORD dwKeyState);

	bool IsSelected ();
	bool IsFormatAvaible (UINT cf);	
	CString GetCountAll ();
	CString GetCountSelection();

	CString GetSizeOfAll ();
	CString GetSelectedSize();
	virtual ~CExplorerXPView();
	void SetRoot (const TCHAR *);
	void DoSomething (int iRow, bool &bChange);

	static BOOL CALLBACK GridCallback(GV_DISPINFO *pDispInfo, LPARAM lParam);
	LPARAM OnSync(WPARAM wp, LPARAM lp);
	LPARAM OnFolderUpdate(WPARAM wp, LPARAM lp);
	LPARAM OnClipChanged (WPARAM wp, LPARAM lp);
	LPARAM OnRecBinUpdate (WPARAM wp, LPARAM lp);
	LPARAM OnFileRenamed (WPARAM wp, LPARAM lp);
	LPARAM OnOptions (WPARAM wp, LPARAM lp);
	LPARAM OnHardUpdate (WPARAM wp, LPARAM lp);
	LPARAM OnSetState (WPARAM wp, LPARAM lp);
	LPARAM OnAppCommand(WPARAM wp, LPARAM lp);
	

	void SetGridFromOptions ();
	void InitInitial ();

	bool GetPathFromCell (CCellID &cell, CString &path);
	void DisableUpdates ();
	void EnableUpdates ();
	void MoveBack (int amount);
	void MoveForward (int amount );
	bool IsSpecialViewer();
	CViewer* GetViewer() { return m_pViewer;}
	void Select (CString &wildcards, bool bSelect);
	void SkipResize (BOOL bSkip) { m_bSkipSize = bSkip;};
	CString GetRoot () { return m_Root;	}

	void SortByColumn(int sortColumn);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	
#endif

protected:
	CString m_Root;
	VIEW_TYPE m_ViewType;
	
	CGridList* m_pGridCtrl;
	CSelRowArray m_SelRows;
	CViewer* m_pViewer;
	CFileViewer m_FileViewer;
	//CDriveViewer m_DriveViewer;
    
	COleDropTarget     m_droptarget;

    IDropTargetHelper* m_piDropHelper;
    bool               m_bUseDnDHelper;
	bool m_bDragDataAcceptable;
	CString m_DropFolder;
	CHistory m_History;

	CCellID m_LastDragOverCell;
	CString m_LastDropFolder;
	bool m_bDisable;
	bool m_bToDo;
	CSelRowArray m_DropFiles;
	static int m_iTimerCount;
	BOOL  m_bSkipSize;

	UINT m_iTimerID;
	bool m_bNeedTimerRefresh;
	bool m_bNeedSync;
	void InternalDelete (bool bRecyle);
	
	volatile static DWORD m_ThreadHandle;		


    //BOOL ReadHdropData ( COleDataObject* pDataObject , CSelRowArray &ar);
	void CopyToClipboard (DWORD effect);
// Generated message map functions
protected:
	//{{AFX_MSG(CExplorerXPView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnUp();
	afx_msg void OnUndo();
	afx_msg void OnSearch();
	afx_msg void OnForward();
	afx_msg void OnCopy();
	afx_msg void OnBack();
	afx_msg void OnDelete();
	afx_msg void OnMove();
	afx_msg void OnDown();
	afx_msg void OnDestroy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnCut();
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateForward(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBack(CCmdUI* pCmdUI);
	afx_msg void OnRename();
	afx_msg void OnUpdateRename(CCmdUI* pCmdUI);
	afx_msg void OnNewfolder();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoveTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyTo(CCmdUI* pCmdUI);
	afx_msg void OnCopyTo();
	afx_msg void OnMoveTo();
	afx_msg void OnSelectionRemove();
	afx_msg void OnUpdateSelectionRemove(CCmdUI* pCmdUI);
	afx_msg void OnSelectionWildcarddeselect();
	afx_msg void OnUpdateSelectionWildcarddeselect(CCmdUI* pCmdUI);
	afx_msg void OnSelectionWildcardselect();
	afx_msg void OnUpdateSelectionWildcardselect(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectall();
	afx_msg void OnUpdateEditSelectall(CCmdUI* pCmdUI);
	afx_msg void OnEditInvertselection();
	afx_msg void OnUpdateEditInvertselection(CCmdUI* pCmdUI);	
	afx_msg void OnCommandsSplit();
	afx_msg void OnUpdateCommandsSplit(CCmdUI* pCmdUI);
	afx_msg void OnCommandsMerge();
	afx_msg void OnUpdateCommandsMerge(CCmdUI* pCmdUI);
	afx_msg void OnCommandsClean();
	afx_msg void OnUpdateCommandsClean(CCmdUI* pCmdUI);
	afx_msg void OnCopyclipboard();
	afx_msg void OnWindowNext();
	//}}AFX_MSG
	afx_msg void OnDropDown( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnGetInfoTip( NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProperties();
	afx_msg void OnFoldersResetallfolders();
	afx_msg void OnFoldersMakeasthisfolder();
	afx_msg void OnUpdateFoldersResetallfolders(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFoldersMakeasthisfolder(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNewfolder(CCmdUI *pCmdUI);	
	afx_msg void OnGroupsSaveopenfoldersasgroup();
	afx_msg void OnDeletepermently();
	afx_msg void OnSelectionSelectsimilar();
	afx_msg void OnCommandsCmd();
	afx_msg void OnUpdateCommandsCmd(CCmdUI *pCmdUI);
	afx_msg void OnEditCopyfolderpathtoclipboard();
	afx_msg void OnEditCopyfilenametoclipboard();
	afx_msg void OnUpdateEditCopyfolderpathtoclipboard(CCmdUI *pCmdUI);
	afx_msg void OnEditCopyfullpathnamesastext();
	afx_msg void OnEditCopytoparent();
	afx_msg void OnEditCopytosub();
	afx_msg void OnUpdateEditCopytosub(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopytoparent(CCmdUI *pCmdUI);
	afx_msg void OnEditMovetogroups();
	afx_msg void OnUpdateEditMovetogroups(CCmdUI *pCmdUI);
	afx_msg void OnEditMovetotabs();
	afx_msg void OnUpdateEditMovetotabs(CCmdUI *pCmdUI);
	afx_msg void OnCommandsFindmissing();
	afx_msg void OnCommandsFindduplicates();
	afx_msg void OnCommandsShowcharts();


	afx_msg void  OnCommandToolsSort1();
	afx_msg void  OnCommandToolsSort2();
	afx_msg void  OnCommandToolsSort3();
	afx_msg void  OnCommandToolsSort4();
	afx_msg void  OnCommandToolsSort5();
	afx_msg void  OnCommandToolsSort6();
	afx_msg void  OnUpdateToolsSort(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // debug version in ExplorerXPView.cpp
inline CExplorerXPDoc* CExplorerXPView::GetDocument()
   { return (CExplorerXPDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXPLORERXP_VIEW_H__

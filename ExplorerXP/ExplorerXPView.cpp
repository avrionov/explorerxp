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


#include "stdafx.h"
#include "ExplorerXP.h"
#include "globals.h"
#include "ExplorerXPDoc.h"
#include "ExplorerXPView.h"
#include "GridList.h"
#include "MainFrm.h"
#include "DirSize.h"
#include "ClipboardFiles.h"
#include "RenameDlg.h"
#include "MultiRenameDlg.h"
#include "NewFolderDialog.h"
#include "CopyMoveDlg.h"
#include "SelectDialog.h"
#include "ShellContextMenu.h"
#include "SplitDlg.h"
#include "MergeDlg.h"
#include "CleanDialog.h"
#include "Options.h"
#include "shlwapi.h"
#include "ThreadPool.h"
#include "FavTree.h"
#include "FindMissing.h"
#include "GroupManager.h"
#include "Charts.h"
#include <filesystem>


volatile DWORD CExplorerXPView::m_ThreadHandle = 200;

const TCHAR hex[] = _T("0123456789ABCDEFGHIJKLMNOPRQSTUVWXYZ");

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef COLOR_HOTLIGHT
#define COLOR_HOTLIGHT  26
#endif

extern CDirSize dirs;


IMPLEMENT_DYNCREATE(CExplorerXPView, CView)
BEGIN_MESSAGE_MAP(CExplorerXPView, CView)	
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()	
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_COMMAND(ID_UP, OnUp)
	ON_COMMAND(ID_UNDO, OnUndo)
	ON_COMMAND(ID_SEARCH, OnSearch)
	ON_COMMAND(ID_FORWARD, OnForward)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_COMMAND(ID_BACK, OnBack)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_DOWN, OnDown)		
	ON_COMMAND(ID_PASTE, OnEditPaste)
	ON_COMMAND(ID_CUT, OnCut)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_COMMAND(ID_NEWFOLDER, OnNewfolder)	
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_COPYTO, OnCopyTo)
	ON_COMMAND(ID_MOVETO, OnMoveTo)
	ON_COMMAND(ID_SELECTION_REMOVE, OnSelectionRemove)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)		
	ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCut)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_UPDATE_COMMAND_UI(ID_FORWARD, OnUpdateForward)	
	ON_UPDATE_COMMAND_UI(ID_BACK, OnUpdateBack)		
	ON_UPDATE_COMMAND_UI(ID_RENAME, OnUpdateRename)		
	ON_UPDATE_COMMAND_UI(ID_UP, OnUpdateUp)
	ON_UPDATE_COMMAND_UI(ID_MOVETO, OnUpdateMoveTo)
	ON_UPDATE_COMMAND_UI(ID_COPYTO, OnUpdateCopyTo)			
	ON_UPDATE_COMMAND_UI(ID_SELECTION_REMOVE, OnUpdateSelectionRemove)
	ON_COMMAND(ID_SELECTION_WILDCARDDESELECT, OnSelectionWildcarddeselect)
	//ON_UPDATE_COMMAND_UI(ID_SELECTION_WILDCARDDESELECT, OnUpdateSelectionWildcarddeselect)
	ON_COMMAND(ID_SELECTION_WILDCARDSELECT, OnSelectionWildcardselect)
	//ON_UPDATE_COMMAND_UI(ID_SELECTION_WILDCARDSELECT, OnUpdateSelectionWildcardselect)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnUpdateEditSelectall)
	ON_COMMAND(ID_EDIT_INVERTSELECTION, OnEditInvertselection)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INVERTSELECTION, OnUpdateEditInvertselection)
	ON_COMMAND(ID_COMMANDS_SPLIT, OnCommandsSplit)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_SPLIT, OnUpdateCommandsSplit)
	ON_COMMAND(ID_COMMANDS_MERGE, OnCommandsMerge)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_MERGE, OnUpdateCommandsMerge)
	ON_COMMAND(ID_COMMANDS_CLEAN, OnCommandsClean)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_CLEAN, OnUpdateCommandsClean)
	ON_COMMAND(ID_COPYCLIPBOARD, OnCopyclipboard)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)	
	//ON_COMMAND(ID_WINDOW_NEXT, OnWindowNext)	
	ON_NOTIFY(GVN_DBLCLK, 100, OnGridDblClick)
	ON_NOTIFY(GVN_LBUTTONDOWN, 100, OnGridLBDown)
	ON_NOTIFY(GVN_LBUTTONUP, 100, OnGridLBUp)
	ON_NOTIFY(GVN_BEGINRDRAG, 100, OnBeginDrag)
	ON_NOTIFY(GVN_BEGINLABELEDIT, 100, OnBeginEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT  , 100, OnEndEdit)
	ON_NOTIFY(GVN_RCLICK , 100, OnRightClick)
	ON_MESSAGE(WM_EXP_SYNC, OnSync)
	ON_MESSAGE(WM_EXP_UPDATE, OnFolderUpdate)
	ON_MESSAGE(WM_EXP_CLIPCHANGED, OnClipChanged)
	ON_MESSAGE(WM_EXP_RECYCLEUPDATE, OnRecBinUpdate)
	ON_MESSAGE(WM_EXP_FILERENAMED, OnFileRenamed)
	ON_MESSAGE(WM_EXP_OPTIONS, OnOptions)
	ON_MESSAGE(WM_EXP_HARDUPDATE, OnHardUpdate)
	ON_MESSAGE(WM_EXP_SETSTATE, OnSetState)
	ON_MESSAGE(WM_APPCOMMAND, OnAppCommand)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
	ON_NOTIFY( TBN_GETINFOTIP, AFX_IDW_TOOLBAR, OnGetInfoTip )
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_FOLDERS_RESETALLFOLDERS, OnFoldersResetallfolders)
	ON_COMMAND(ID_FOLDERS_MAKEASTHISFOLDER, OnFoldersMakeasthisfolder)
	ON_UPDATE_COMMAND_UI(ID_FOLDERS_RESETALLFOLDERS, OnUpdateFoldersResetallfolders)
	ON_UPDATE_COMMAND_UI(ID_FOLDERS_MAKEASTHISFOLDER, OnUpdateFoldersMakeasthisfolder)
	ON_UPDATE_COMMAND_UI(ID_NEWFOLDER, OnUpdateNewfolder)	
	ON_COMMAND(ID_GROUPS_SAVEOPENFOLDERSASGROUP, OnGroupsSaveopenfoldersasgroup)
	ON_COMMAND(ID_DELETEPERMENTLY, OnDeletepermently)
	ON_UPDATE_COMMAND_UI(ID_DELETEPERMENTLY, OnUpdateDelete)
	ON_COMMAND(ID_SELECTION_SELECTSIMILAR, OnSelectionSelectsimilar)
	ON_COMMAND(ID_COMMANDS_CMD, OnCommandsCmd)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_CMD, OnUpdateCommandsCmd)
	ON_COMMAND(ID_EDIT_COPYFOLDERPATHTOCLIPBOARD, OnEditCopyfolderpathtoclipboard)
	ON_COMMAND(ID_EDIT_COPYFILENAMETOCLIPBOARD, OnEditCopyfilenametoclipboard)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYFOLDERPATHTOCLIPBOARD, OnUpdateEditCopyfolderpathtoclipboard)
	ON_COMMAND(ID_EDIT_COPYFULLPATHNAMESASTEXT, OnEditCopyfullpathnamesastext)
	ON_COMMAND(ID_EDIT_COPYTOPARENT, OnEditCopytoparent)
	ON_COMMAND(ID_EDIT_COPYTOSUB, OnEditCopytosub)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYTOSUB, OnUpdateEditCopytosub)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYTOPARENT, OnUpdateEditCopytoparent)
	ON_COMMAND(ID_EDIT_MOVETOGROUPS, OnEditMovetogroups)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MOVETOGROUPS, OnUpdateEditMovetogroups)
	ON_COMMAND(ID_EDIT_MOVETOTABS, OnEditMovetotabs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MOVETOTABS, OnUpdateEditMovetotabs)
	ON_COMMAND(ID_COMMANDS_FINDMISSING, OnCommandsFindmissing)
	ON_COMMAND(ID_COMMANDS_FINDDUPLICATES, OnCommandsFindduplicates)
	ON_COMMAND(ID_COMMANDS_SHOWCHARTS, OnCommandsShowcharts)
	ON_COMMAND(ID_COMMANDS_SORTBYCOLUMN1, OnCommandToolsSort1)
	ON_COMMAND(ID_COMMANDS_SORTBYCOLUMN2, OnCommandToolsSort2)
	ON_COMMAND(ID_COMMANDS_SORTBYCOLUMN3, OnCommandToolsSort3)
	ON_COMMAND(ID_COMMANDS_SORTBYCOLUMN4, OnCommandToolsSort4)
	ON_COMMAND(ID_COMMANDS_SORTBYCOLUMN5, OnCommandToolsSort5)
	ON_COMMAND(ID_COMMANDS_SORTBYCOLUMN6, OnCommandToolsSort6)

	ON_UPDATE_COMMAND_UI(ID_COMMANDS_SORTBYCOLUMN1, OnUpdateToolsSort)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_SORTBYCOLUMN2, OnUpdateToolsSort)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_SORTBYCOLUMN3, OnUpdateToolsSort)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_SORTBYCOLUMN4, OnUpdateToolsSort)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_SORTBYCOLUMN5, OnUpdateToolsSort)
	ON_UPDATE_COMMAND_UI(ID_COMMANDS_SORTBYCOLUMN6, OnUpdateToolsSort)
	
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPView construction/destruction

int CExplorerXPView::m_iTimerCount = 100;

CExplorerXPView::CExplorerXPView()
:m_ViewType(VIEW_NOTHING),
m_pGridCtrl (NULL),
m_bSkipSize (FALSE)
{
	m_iTimerID = m_iTimerCount++;
	m_bDragDataAcceptable = false;

	m_LastDragOverCell = CCellID(-1,-1);
	
	m_pViewer = &m_FileViewer;	

	if ( SUCCEEDED( CoCreateInstance ( CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**) &m_piDropHelper ) ))
      m_bUseDnDHelper = true;

	m_bToDo = false;
	m_bDisable = false;
	m_bNeedTimerRefresh = false;
	m_bNeedSync = false;     
	m_bUseDnDHelper = false;
}


CExplorerXPView::~CExplorerXPView()
{
	if (m_pGridCtrl)
		delete m_pGridCtrl;

	 if ( NULL != m_piDropHelper) 
        m_piDropHelper->Release();
}

BOOL CExplorerXPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	if( !CView::PreCreateWindow(cs)) 
		return false;

	cs.dwExStyle &= ~WS_EX_RIGHTSCROLLBAR;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	cs.style |= WS_BORDER;
	return TRUE;
}

COLORREF HLS_TRANSFORM (COLORREF rgb, int percent_L, int percent_S);

void CExplorerXPView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_droptarget.Register ( this );
	
	RegisterWindow (this);
	
	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridList;
		m_FileViewer.FillHeader ();		
		m_FileViewer.SetGrid (m_pGridCtrl);
//		m_DriveViewer.SetGrid (m_pGridCtrl);
        		
		if (!m_pGridCtrl) return;

		// Create the Gridctrl window
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->Create(rect, this, 100, WS_CHILD | WS_TABSTOP | WS_VISIBLE);

		// fill it up with stuff
		m_pGridCtrl->SetVirtualMode(TRUE);
        m_pGridCtrl->SetCallbackFunc(GridCallback , (LPARAM)this);
		
		m_pGridCtrl->SetEditable(TRUE);
		m_pGridCtrl->EnableDragAndDrop(TRUE);
		m_pGridCtrl->SetListMode ();
		m_pGridCtrl->SetFixedColumnSelection (FALSE);		
		SetGridFromOptions ();
		m_pGridCtrl->SetHeaderSort (TRUE);

		m_pGridCtrl->SetFixedRowCount(1);
		m_pGridCtrl->SetFixedColumnCount(0);
		
		m_pGridCtrl->SetImageList (GetSysImageList ());
		m_pGridCtrl->SetFrameFocusCell (FALSE);

		m_pGridCtrl->SetGridBkColor (::GetSysColor (COLOR_WINDOW));

		m_pGridCtrl->SetAutoSizeStyle (GVS_DATA);
		m_pGridCtrl->EnableHiddenColUnhide(FALSE);

		m_pGridCtrl->SetSortColumn (0);
		m_pGridCtrl->SetSortAscending (TRUE);
		m_pGridCtrl->SetGridLineColor(HLS_TRANSFORM(::GetSysColor (COLOR_WINDOWTEXT), +50, 0));
	}
      
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.AddView (GetDocument ()->GetPath (),  (CMDIChildWnd*)GetParent () );
   
	m_pViewer = &gDriveViewer;   
	gDriveViewer.SetGrid (m_pGridCtrl);
    SetTimer (m_iTimerID, 1000, NULL);
	InitInitial();
	//m_bNeedTimerRefresh = true;
	//gPool.StartThread (m_ThreadHandle++, this, CExplorerXPView::InitInitial);	
}


void CExplorerXPView::InitInitial ()
{
	m_bDisable = true;
	Fill (GetDocument ()->GetPath (), false);
	m_bDisable = false;
	//m_pGridCtrl->ExpandLastColumn ();	
}

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPView diagnostics

#ifdef _DEBUG
void CExplorerXPView::AssertValid() const
{
	CView::AssertValid();
}

void CExplorerXPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CExplorerXPDoc* CExplorerXPView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CExplorerXPDoc)));
	return (CExplorerXPDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPView message handlers

void CExplorerXPView::Fill(const TCHAR *root, bool bHistory)
{
    ASSERT (root);

	TRACE (_T("Fill - %s\n"), root);
	
	if (IsSpecialViewer ())
		m_pViewer->SetGrid (NULL);
	
	if (bHistory) {
		CSelRowArray ar;
		GetSelection (ar);
		m_History.Push (m_Root, m_pGridCtrl->GetScrollPos (SB_VERT), ar);
	}
	
    if (_tcsicmp (root, CONST_MYCOMPUTER) == 0)
	{
		m_pViewer = &gDriveViewer;
		gDriveViewer.SetGrid (m_pGridCtrl);
	}	
    else
	    if (_tcsicmp (root, CONST_RECYCLEBIN) == 0)
		{
			m_pViewer = &gRecBinViewer;
			gRecBinViewer.SetGrid (m_pGridCtrl);
		}
		    
        else
            m_pViewer = &m_FileViewer;
	
	if (m_Root == root)	
	 	return;	

	

	m_Root = root;
    m_pGridCtrl->SetVertScroll (0, FALSE);		
	HardFill ();		
}
	
void CExplorerXPView::HardFill ()
{
	CExplorerXPDoc *pDoc = GetDocument ();

	m_pViewer->Fill (m_Root);
	CString title = pDoc->GetTitle ();

	if (title != m_pViewer->GetTitle ())
		pDoc->SetTitle (m_pViewer->GetTitle ());

	CExplorerXPView *pView = GetCurrentView ();

	if (pView == this)
		SyncUI (m_Root);
}

void CExplorerXPView::Fill ()
{
	Fill (NULL);
}


/*
void CExplorerXPView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;

	if (pItem->mask & LVIF_TEXT) //valid text buffer?
		lstrcpy(pItem->pszText, m_Array[pItem->iItem].as_text (pItem->iSubItem));
	if (pItem->mask & LVIF_IMAGE)
	{
		CString csFileName = m_Root + m_Array[pItem->iItem].m_FileInfo.cFileName;
	
		pItem->iImage = GetFileIcon (csFileName);
	}
	*pResult = 0;
}*/

BOOL CExplorerXPView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
    if (m_pGridCtrl && IsWindow(m_pGridCtrl->m_hWnd))
        if (m_pGridCtrl->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
            return TRUE;

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CExplorerXPView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (!m_pGridCtrl)
		return;

	if (m_pGridCtrl->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
	//	m_pGridCtrl->EnableScrollBars(SB_HORZ, FALSE);
		m_pGridCtrl->MoveWindow(rect);
		m_pGridCtrl->ExpandLastColumn ();
		m_pGridCtrl->ResetScrollBars();
	}
}

BOOL CExplorerXPView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


BOOL CALLBACK CExplorerXPView::GridCallback(GV_DISPINFO *pDispInfo, LPARAM lParam) 
{
	CExplorerXPView *pView = (CExplorerXPView *)lParam;

	pView->m_pViewer->GridCallBack (pDispInfo);

	if (pDispInfo->item.row == pView->m_LastDragOverCell.row)
	//if (pDispInfo->item.row == pView->m_LastDragOverCell.row && pDispInfo->item.col == 0)
	{
		pDispInfo->item.lfFont.lfUnderline = TRUE;
		pDispInfo->item.lfFont.lfWeight  = FW_BOLD;
		pDispInfo->item.crBkClr = HLS_TRANSFORM(::GetSysColor (COLOR_WINDOW), 0, 15);
	}

//	if ( (pDispInfo->item.col == pView->m_LastDragOverCell.col) &&  (pDispInfo->item.row == pView->m_LastDragOverCell.row))
//		pDispInfo->item.nState != GVIS_DROPHILITED;

	return TRUE;	
}

void CExplorerXPView::Sort()
{
	m_pViewer->Sort ();		
}


void CExplorerXPView::DoSomething (int iRow, bool &bChange)
{
	if (m_pViewer->CanChangeTo (iRow) && (bChange == false))
	{
		CString cs = m_pViewer->GetPath (iRow);
		SureBackSlash (cs);		
		Fill (cs);		
		bChange = true;
	}    
	else
	{		
		CString cs = m_pViewer->GetPath (iRow);
		CString dir = m_pViewer->GetTitle ();
		//ShellExecute (m_hWnd, NULL, cs, NULL, dir, SW_SHOWNORMAL );
		ShellExecute(m_hWnd, NULL, cs, NULL, dir, SW_SHOW);
		
		SHELLEXECUTEINFO ShExecInfo;

		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = NULL;
		ShExecInfo.hwnd = m_hWnd;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = cs;
		ShExecInfo.lpParameters = NULL;
		ShExecInfo.lpDirectory = dir;
		ShExecInfo.nShow = SW_SHOWNORMAL;
		ShExecInfo.hInstApp = NULL;

		//ShellExecuteEx(&ShExecInfo);

	}
}

void CExplorerXPView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if (! pItem->iRow)
	{	
	//	OnGridClick (pNotifyStruct, pResult);
		return;
	}
		
	int iRow = pItem->iRow-1;	
	bool bChange = false;
	DoSomething (iRow, bChange);
	
}

void CExplorerXPView::OnGridLBUp (NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if ( pItem->iRow)
		return;

	if (!m_pGridCtrl)
		return;

	m_pViewer->SetSortDir (m_pGridCtrl->GetSortAscending () == TRUE);
	m_pViewer->SetSortColumn  ( m_pGridCtrl->GetSortColumn () );
	Sort ();
	RestoreSelection ();
	Invalidate ();
}

void CExplorerXPView::OnGridLBDown (NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if ( pItem->iRow)
		return;

	 //TRACE (_T("Down on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	 SaveSelection ();	
}

void CExplorerXPView::OnRightClick (NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	CPoint pt;		
	GetCursorPos (&pt);

	if (pItem->iRow < 0)
	{
		if (IsSpecialViewer())
		{
			CMenu menu;
			menu.LoadMenu (IDR_EMPTY);
			CMenu *pPopup = menu.GetSubMenu (0);
			pPopup->DeleteMenu(10, MF_BYPOSITION);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
			return;
		}

		CShellContextMenu scm;
		CString path = m_pViewer->GetTitle();
		scm.NewMenu (path, pt,  AfxGetMainWnd());
		return;

	}
	
	if ( pItem->iRow == 0)
	{
		m_pViewer->ContextMenuOnHeader(this, pt, pItem);
		return;
	}

	if (!m_pGridCtrl->IsCellSelected (pItem->iRow, 0))
		 m_pGridCtrl->SetSelectedRange ( pItem->iRow, 0, pItem->iRow, 
										 static_cast<int>(m_pViewer->GetColumnCount ()), 
										 FALSE);
	
	CSelRowArray ar;
	GetSelection (ar);

	if (!ar.size ())
	 return;
		
	m_pViewer->ContextMenu (this, ar, pt);	
}

bool GetParentFolder (CString &tmp)
{	
	int len = tmp.GetLength ();
	if (!len)
		return false;

	if (tmp[len-1] == '\\')
		tmp.Delete (len-1);

	int pos = tmp.ReverseFind ('\\');

	if (pos != -1)
	{		
		tmp.Delete (pos+1, len - pos-1);
		return true;
	}
	
	return false;
}

void CExplorerXPView::OnUp() 
{
	if (m_Root.IsEmpty())
		return;

	CString tmp = m_Root;
	if (GetParentFolder(tmp))
	{
		Fill (tmp);
	}
	else
		Fill (CONST_MYCOMPUTER);

}

void CExplorerXPView::OnUndo() 
{
	// TODO: Add your command handler code here
	
}

void CExplorerXPView::OnSearch() 
{
	// TODO: Add your command handler code here
	
}

void CExplorerXPView::OnForward() 
{
	MoveForward (1);
}


void CExplorerXPView::OnCopy() 
{
	TRACE("OnCopy");
	CopyToClipboard (DROPEFFECT_COPY);
}

void CExplorerXPView::OnBack() 
{
	MoveBack (1);
}

void CExplorerXPView::InternalDelete (bool bRecycle)
{	
	if (m_pViewer == &gDriveViewer)
		return;

	 if (m_pViewer == &gRecBinViewer) 
	 {
		 gRecBinViewer.DeleteSelection ();
		 return;
	 }

	CSelRowArray ar;
	GetSelection (ar);

	FileOperation (ar, FO_DELETE , bRecycle ? FOF_ALLOWUNDO : 0, NULL);	
}

void CExplorerXPView::OnDelete() 
{
	InternalDelete (true);
}

void CExplorerXPView::OnMove() 
{
	// TODO: Add your command handler code here
	
}



LPARAM CExplorerXPView::OnSync(WPARAM wp, LPARAM lp)
{
	CDirStackEl *pEl = (CDirStackEl* )lp;

	if (m_pViewer->Sync (pEl->m_Path, pEl->m_FileName))	
	{		
		Invalidate ();	
		m_bNeedSync = true;		
	}
	
	return 0;
}

struct CompareRows 
{
	bool operator() (const CSelectRow &SR1, const CSelectRow &SR2)
	{
		return SR1.m_nRow <= SR2.m_nRow;
	}
};

void CExplorerXPView::GetSelection (CSelRowArray &ar)
{
	ar.clear ();

	if (!m_pGridCtrl)
		return;

	for (POSITION pos = m_pGridCtrl->m_SelectedCellMap.GetStartPosition(); pos != NULL; )
    {
		DWORD key;
        CCellID cell;
        m_pGridCtrl->m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
		if (cell.col == 0)  
			ar.push_back (CSelectRow(m_pViewer->GetPath (cell.row - 1), m_pViewer->GetName (cell.row - 1), cell.row));
    }

	std::sort (ar.begin(), ar.end(), CompareRows());	
}

void CExplorerXPView::GetSelectionFast (CSelRowArray &ar)
{
	ar.clear ();

	if (!m_pGridCtrl)
		return;

	for (POSITION pos = m_pGridCtrl->m_SelectedCellMap.GetStartPosition(); pos != NULL; )
	{
		DWORD key;
		CCellID cell;
		m_pGridCtrl->m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
		if (cell.col == 0)  
			ar.push_back (CSelectRow(cell.row, m_pViewer->GetName (cell.row - 1)));
	}
	ar.m_Path = m_pViewer->GetTitle();
}

void CExplorerXPView::SaveSelection ()
{
	GetSelectionFast (m_SelRows);
}

void CExplorerXPView::RestoreSelection (CSelRowArray &sel)
{
	if (!m_pGridCtrl)
		return;

	m_pGridCtrl->SetRedraw(FALSE);
	m_pGridCtrl->ClearSelection ();
	
	TRACE (_T("RestoreSel %s\n"), (LPCTSTR)sel.m_FocusCell);

	TRACE ("SEL %d GRID %d\n", sel.size (),  m_pGridCtrl->GetRowCount ()-1);
	
	for (unsigned int i = 0; i < sel.size (); i++)
	{
		for (int j = 0; j < m_pGridCtrl->GetRowCount ()-1; j++)
		{
           if (sel[i].m_Path == m_pViewer->GetPath (j))
			{	
				m_pGridCtrl->SetSelectedRangeFast (j+1, 0, j+1,m_pViewer->GetColumnCount (), TRUE);				
				break;
			}
		}
	}
	m_pGridCtrl->SetRedraw(TRUE);
}

void CExplorerXPView::RestoreSelectionFast (CSelRowArray &sel)
{
	if (!m_pGridCtrl)
		return;

	m_pGridCtrl->SetRedraw(FALSE);
	m_pGridCtrl->ClearSelection ();

	for (unsigned int i = 0; i < sel.size (); i++)
	{
		for (int j = 0; j < m_pGridCtrl->GetRowCount ()-1; j++)
		{
			if (sel[i].m_Name == m_pViewer->GetName (j))
			{	
				m_pGridCtrl->SetSelectedRangeFast (j+1, 0, j+1,m_pViewer->GetColumnCount (), TRUE);				
				break;
			}			
		}
	}
	m_pGridCtrl->SetRedraw(TRUE);
}

void CExplorerXPView::RestoreSelection ()
{
	if (!m_pGridCtrl)
		return;

	if (m_pViewer->GetTitle() != m_SelRows.m_Path)
	{
		m_pGridCtrl->ClearSelection ();
		return;
	}

	RestoreSelectionFast (m_SelRows);	
}

void CExplorerXPView::OnDown() 
{
	bool bChange = false;
	CSelRowArray ar;

	GetSelection (ar);
	
	for (unsigned int i =0; i < ar.size (); i++)
		DoSomething (ar[i].m_nRow-1, bChange);
}

void CExplorerXPView::OnDestroy() 
{
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.RemoveView( ( CMDIChildWnd* )GetParent ());
	m_pViewer->OnClose ();
	m_pViewer->SetGrid (NULL);
	UnRegisterWindow (this);
	CView::OnDestroy();	
}



bool CExplorerXPView::FileOperation (CSelRowArray &ar, UINT wFunc, FILEOP_FLAGS fFlags, const TCHAR *pTo)
{	
	if (ar.size () == 0)
		return false;

	DisableUpdates ();

	::FileOperation (this, ar, wFunc, fFlags, pTo);
	
	EnableUpdates ();

	m_pGridCtrl->SetFocus ();

	return true;
}

bool CExplorerXPView::Rename (CSelRowArray &src, CSelRowArray &dst, FILEOP_FLAGS fFlags)
{
	for (unsigned int i = 0; i < src.size (); i++)
		Rename (src[i].m_Path, dst[i].m_Path, fFlags);
	
	m_pGridCtrl->SetFocus ();
	return true;
}

bool CExplorerXPView::Rename (const TCHAR *src, const TCHAR *dst, FILEOP_FLAGS fFlags)
{
	int size = 0;

	// same ?
	if (_tcscmp (src, dst) == 0)
		return true;

	TCHAR *pFrom = AllocateNullPath (src, size);
	
	SHFILEOPSTRUCT fo;
	fo.hwnd = m_hWnd;
	fo.wFunc = FO_RENAME;
	fo.fFlags = fFlags;
	fo.pFrom = pFrom;
	fo.pTo = dst;
	fo.lpszProgressTitle = NULL;

	int ret = SHFileOperation (&fo);

	if (ret != 0) {
		TRACE(TEXT("Rename Error %xd"), ret);
	}

	delete [] pFrom;
		
	m_pGridCtrl->SetFocus ();

	return true;

}

LPARAM CExplorerXPView::OnFileRenamed(WPARAM wp, LPARAM lp)
{
	RENAMEFILESTRUCT *rnStruct = (RENAMEFILESTRUCT *)lp;

	CString old_path = rnStruct->_oldName;

	CString root = m_Root;

	SureBackSlash (root);

	CString old_path_copy = old_path;
	SureBackSlash (old_path_copy);

	if (old_path_copy.CompareNoCase (root) == 0)
	{
		Fill (rnStruct->_newName);
		return NULL;
	}
	
	CString parent_old, name_old;
	CString parent_new, name_new;

	CString new_path = rnStruct->_newName;

	Split(old_path, parent_old, name_old);
	Split(new_path, parent_new, name_new);

	SureBackSlash (parent_old);
	SureBackSlash (parent_new);


	if (parent_old.CompareNoCase(root) == 0)
	{
		if (parent_new.CompareNoCase(root) == 0)
			if (IsSpecialViewer ()) 
			{
				m_bNeedTimerRefresh = true;
				return NULL;
			}
			else
			{
				m_pViewer->Rename (name_old, name_new);
				m_pGridCtrl->Invalidate();
			}

	}

	
	//if (root.Find (old_path) != -1)
	//{		
	//	if (m_bDisable)
	//		m_bToDo = true;
	//	else
	//		m_bNeedTimerRefresh = true;

	//	return NULL;	
	//}
	//		
	//if (old_path.Find (root) != -1)
	//{
	//	if (m_bDisable)
	//		m_bToDo = true;
	//	else
	//		m_bNeedTimerRefresh = true;

	//	return NULL;
	//}		
	return NULL;
}

LPARAM CExplorerXPView::OnFolderUpdate(WPARAM wp, LPARAM lp)
{
	CString path = (const TCHAR *) lp;

	path.MakeLower ();
	CString root = m_Root;

	root.MakeLower ();

	if (root.Find (path) != -1)
	{		
		if (m_bDisable)
			m_bToDo = true;
		else
			m_bNeedTimerRefresh = true;

		return NULL;	
	}

	if (path.Find (root) != -1)
	{
		if (m_bDisable)
			m_bToDo = true;
		else
			m_bNeedTimerRefresh = true;

		return NULL;
	}		

	return NULL;
}

// drag and drop

/////////////////////////////////////////////////////////////////////////////
// CMultiFilerView drag 'n' drop handlers

DROPEFFECT CExplorerXPView::OnDragScroll( DWORD dwKeyState, CPoint point )
{
	return CView::OnDragScroll (dwKeyState, point);
}

DROPEFFECT CExplorerXPView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	
	m_bDragDataAcceptable = false;
	
	m_DropFiles.clear ();

	m_DropFolder.Empty ();


	if (m_pViewer->CanDropToSubs ())
		m_LastDragOverCell = m_pGridCtrl->GetCellFromPt(point, FALSE);

	m_LastDropFolder = m_pViewer->GetTitle ();

	if (ReadHDropData (pDataObject, m_DropFiles, m_DropFolder))
	{
		m_bDragDataAcceptable = true;

		if (m_pViewer->CanDropToSubs ())
			if (GetPathFromCell (m_LastDragOverCell, m_LastDropFolder)	)		
				m_pGridCtrl->Invalidate ();
		
		dwEffect = m_pViewer->CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
	}

    if ( m_bUseDnDHelper )
        {
			// The DnD helper needs an IDataObject interface, so get one from
			// the COleDataObject.  Note that the FALSE param means that
			// GetIDataObject will not AddRef() the returned interface, so 
			// we do not Release() it.

			IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 

			m_piDropHelper->DragEnter ( GetSafeHwnd(), piDataObj, &point, dwEffect );
        }

    return dwEffect;
}

DROPEFFECT CExplorerXPView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

	if (!m_pGridCtrl)
		return DROPEFFECT_NONE;

	CCellID cell = m_pGridCtrl->GetCellFromPt(point, FALSE);

    
	if (!m_pGridCtrl->IsValid(cell))
    {
		m_LastDragOverCell = CCellID(-1,-1);
		m_LastDropFolder = m_pViewer->GetTitle ();        
    }

//	TRACE (L"Cell [%d, %d] ", cell.row, cell.col);

	     // Have we moved over a different cell than last time?
	if (m_pViewer->CanDropToSubs ())
		//if (cell != m_LastDragOverCell)
		{
			//m_LastDragOverCell = cell;
			//if (!GetPathFromCell ( m_LastDragOverCell, m_LastDropFolder))
			if (!GetPathFromCell ( cell, m_LastDropFolder))
					m_LastDropFolder = m_pViewer->GetTitle ();			    
		}
	
	if (m_bDragDataAcceptable == true)	
		dwEffect = m_pViewer->CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
	
	//TRACE("LDFolder %s effect %X\n", (LPCTSTR)m_LastDropFolder, dwEffect);

	if (m_pViewer->CanDropToSubs ())
	{
		if (dwEffect == DROPEFFECT_NONE)
			cell = CCellID(-1, -1);

		if (cell != m_LastDragOverCell)
		{
			m_LastDragOverCell = cell;
			m_pGridCtrl->Invalidate ();    
		}
	}
	
	CRect rectClient;
	GetClientRect(&rectClient);
	ClientToScreen(rectClient);
	ClientToScreen(&point);

	int nScrollDir = -1;
	if ( point.y >= rectClient.bottom - RECT_BORDER)
		nScrollDir = SB_LINEDOWN;
	else
		if ( (point.y <= rectClient.top + RECT_BORDER) )
			nScrollDir = SB_LINEUP;

	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = m_pGridCtrl->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		m_pGridCtrl->SendMessage(WM_VSCROLL, wParam);
	}

    if ( m_bUseDnDHelper )      
        m_piDropHelper->DragOver ( &point, dwEffect );   

    return dwEffect;
}

BOOL CExplorerXPView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	BOOL bRet;

	m_LastDragOverCell = CCellID(-1,-1);
	
	if ( m_bUseDnDHelper )
        {
        	IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
			m_piDropHelper->Drop ( piDataObj, &point, dropEffect );
        }

	DisableUpdates ();
    bRet = m_pViewer->OnDrop (pDataObject, dropEffect, m_LastDropFolder);
	EnableUpdates ();
    m_pGridCtrl->Invalidate ();
    return bRet;
}

void CExplorerXPView::OnDragLeave() 
{
    if ( m_bUseDnDHelper )
        m_piDropHelper->DragLeave();

	m_LastDragOverCell = CCellID(-1,-1);
	m_pGridCtrl->Invalidate ();
	m_bDragDataAcceptable = false;
	m_DropFolder.Empty ();
        
}

void CExplorerXPView::OnEditPaste() 
{
	TRACE("OnPaste");
	COleDataObject object;
	object.AttachClipboard ();
	FileOperation (&object);	
}


void CExplorerXPView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_pViewer->CanPaste () && IsFormatAvaible ( CF_HDROP);

	pCmdUI->Enable (bEnable);
}


bool CExplorerXPView::IsSelected ()
{
	return m_pGridCtrl->GetSelectedCount () != 0;
}

bool CExplorerXPView::IsFormatAvaible (UINT cf)
{
	return IsClipboardFormatAvailable (cf) == TRUE;
}

void CExplorerXPView::OnUpdateCopy(CCmdUI* pCmdUI) 
{
	BOOL bEnable = IsSelected () && m_pViewer->CanCopy ();

	pCmdUI->Enable (bEnable);
	
}

void CExplorerXPView::OnCut() 
{
	CopyToClipboard (DROPEFFECT_MOVE);
	
}

void CExplorerXPView::OnUpdateCut(CCmdUI* pCmdUI) 
{
	BOOL bEnable = IsSelected () && m_pViewer->CanCut ();

	pCmdUI->Enable (bEnable);
	
}

void CExplorerXPView::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = IsSelected () && m_pViewer->CanDelete ();

	pCmdUI->Enable (bEnable);
}


void CExplorerXPView::CopyToClipboard (DWORD effect)
{

  CSelRowArray ar;
  GetSelection (ar);

  ::CopyToClipboard (ar, effect);	
}

LPARAM CExplorerXPView::OnClipChanged (WPARAM wp, LPARAM lp)
{
	if (gClipboard.IsSameRoot ( m_pViewer->GetTitle ()))
		Invalidate ();

	return NULL;
}


CString CExplorerXPView::GetCountAll()
{
	CString csTemp;

	if (m_bDisable)
		return csTemp;
	
	csTemp.Format (_T("  %d object"), m_pViewer->GetCount ());
	if (m_pViewer->GetCount () != 1)
		csTemp += "s";
	
	return csTemp;
}


CString CExplorerXPView::GetCountSelection()
{
	CString csTemp;

	if (m_bDisable)
		return csTemp;

	CSelRowArray ar;
	GetSelectionFast (ar);

	if (ar.size() > 0) {	
		csTemp.Format (_T("%d selected object"), ar.size ());
		if (ar.size () > 1)
			csTemp += "s";			
	}

	return csTemp;
}

void Split (CString &path, CString &parent, CString &name);

CString CExplorerXPView::GetSizeOfAll ()
{
	CString csTemp ="";

	if (m_bDisable)
	  return csTemp;
	
	ULONGLONG size = 0, sizeOnDisk = 0;
	ULONGLONG tempsize;
	ULONGLONG tempSizeOnDisk;

		
	if (!IsSpecialViewer()) {	
		CString path, folder, name;
		path = m_pViewer->GetTitle();

		Split (path, folder, name);
		FILETIME fTime;
		GetLastWriteTime (path, fTime);
		if (!GetDirSize(folder, name, size, sizeOnDisk, fTime))
			return csTemp;
	} else {
		for ( int i = 0; i < m_pViewer->GetCount (); i++) {
			if (m_pViewer->GetObjectSize (i, tempsize, tempSizeOnDisk)) {
				size += tempsize;
				sizeOnDisk += tempSizeOnDisk;
			} else {
				return csTemp;
			}
		}
	}
	

    csTemp = size_to_string (size);

	return csTemp;
}

CString CExplorerXPView::GetSelectedSize()
{
	CString csTemp ="";

	if (m_bDisable)
	  return csTemp;
	
	CSelRowArray ar;
	GetSelection (ar);

	ULONGLONG size = 0, sizeOnDisk = 0;
	ULONGLONG tempsize;
	ULONGLONG tempSizeOnDisk;

	if (ar.size ())
	{		
		for (unsigned int i = 0; i < ar.size (); i++)
			if (m_pViewer->GetObjectSize (ar[i].m_nRow-1, tempsize, tempSizeOnDisk))
			{
				size += tempsize;
				sizeOnDisk += tempSizeOnDisk;
			} else {
				return csTemp;
			}
	} else {
		return csTemp;		
	}

    csTemp = size_to_string (size);

	return csTemp;
}


bool CExplorerXPView::FileOperation (COleDataObject *object, bool bClipboard, DROPEFFECT dwEffect, const TCHAR *pTo )
{
	CSelRowArray ar;

	CString source_folder;
	
	CString dest_folder;

	if (pTo)
		dest_folder = pTo;
	else
		dest_folder = m_Root;

	if (!ReadHDropData (object, ar, source_folder))
		return false;

	UINT file_oper = FO_COPY;

	DROPEFFECT drop_effect = dwEffect;
	
	if (bClipboard)
		ReadDropEffect (object, drop_effect);

	SureBackSlash(dest_folder);
	SureBackSlash(source_folder);
	switch (drop_effect)
		{
			case DROPEFFECT_COPY:
				file_oper = FO_COPY;
			break;

			case DROPEFFECT_MOVE:
				file_oper = FO_MOVE;
				break;
			case DROPEFFECT_LINK:
				return CreateShortcuts (ar, m_pViewer->GetTitle ());
				
			default:
				TRACE(_T("Invalide file operation %d\n"), drop_effect);
		}					
		
	FILEOP_FLAGS copyOnCollison = 0;
	if (dest_folder.CompareNoCase (source_folder) == 0)
		copyOnCollison = FOF_RENAMEONCOLLISION;

	bool ret = FileOperation (ar, file_oper, FOF_ALLOWUNDO | copyOnCollison  , dest_folder) != 0;

	if (bClipboard &&  (file_oper == FO_MOVE))
	{
		EmptyClipboard ();
	}

	return ret;
	
}

void DoDragDrop (CSelRowArray &ar)
{
  if (ar.size () == 0)
    return;

  HANDLE hGlobalDrop;
  DROPFILES df;
  BYTE*  lpGlobalDrop;
  
  TCHAR *data;
  int size = 0;

  data = AllocateNullPaths (ar, size);
  hGlobalDrop = GlobalAlloc(GHND, size + sizeof(df));

   if (hGlobalDrop != NULL) 
        {
		  
		  lpGlobalDrop = (BYTE*)GlobalLock(hGlobalDrop);
		  if (lpGlobalDrop != NULL) {

			  df.pFiles = sizeof (df);
			  df.fWide = 1;          

			  memcpy (lpGlobalDrop, &df, sizeof (df));
		
			  lpGlobalDrop +=  sizeof(df);

			  memcpy (lpGlobalDrop, data, size);

			  GlobalUnlock(hGlobalDrop);
		  }

          delete  [] data;
        }
       

	COleDataSource *pDataSource = new COleDataSource;

	pDataSource->CacheGlobalData (CF_HDROP, hGlobalDrop);
	CPoint pt;
	GetCursorPos (&pt);

	CRect dragRect (pt, CSize (10,10));
	dragRect.OffsetRect (-5,-5);

	//DROPEFFECT dwEffect = ;
	pDataSource->DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK, &dragRect );   
}

void CExplorerXPView::OnBeginDrag (NMHDR *pNotifyStruct, LRESULT* pResult)
{
	if (m_pViewer->CanDrag ())
	{
		CSelRowArray ar;
		GetSelection (ar);
		DoDragDrop (ar);
	}
}


void CExplorerXPView::OnBeginEdit (NMHDR *pNotifyStruct, LRESULT* pResult)
{

}

void CExplorerXPView::OnEndEdit (NMHDR *pNotifyStruct, LRESULT* pResult)
{

}


void CExplorerXPView::OnUpdateForward(CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_History.CanFore ());
}

void CExplorerXPView::OnUpdateBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_History.CanBack ());
}

void CExplorerXPView::MoveBack (int amount)
{
	CString path;

	int iPos;
	bool bFlag = false;

	CSelRowArray ar, cur_sel;
	GetSelection (cur_sel);
	m_History.MoveBack (amount, path, iPos, ar, m_Root, m_pGridCtrl->GetScrollPos (SB_VERT), cur_sel);

	if (path.IsEmpty ())
		Fill (NULL, bFlag);
	else
		Fill (path, bFlag);

	m_pGridCtrl->SetScrollPos (SB_VERT, iPos);
	RestoreSelection (ar);
	m_pGridCtrl->SetFocus ();
	m_pGridCtrl->Invalidate ();
}


void CExplorerXPView::MoveForward (int amount )
{
	CString path;
	int iPos;
	CSelRowArray ar;

	m_History.MoveForward (amount, path, iPos, ar);

	if (path.IsEmpty ())
		Fill (NULL, false);
	else
		Fill (path, false);

	
	m_pGridCtrl->SetScrollPos (SB_VERT, iPos);
	RestoreSelection (ar);
	m_pGridCtrl->SetFocus ();
	m_pGridCtrl->Invalidate ();
}

void CExplorerXPView::OnDropDown( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    NMTOOLBAR* pNMToolBar = ( NMTOOLBAR* )pNotifyStruct;
	CMenu menu;
    
	VERIFY( menu.CreatePopupMenu () );
 
	CHistoAr ar;
	
    switch ( pNMToolBar->iItem )
    {
        case ID_FORWARD:
            m_History.GetForeAr (ar);
            break;

        case ID_BACK:
            m_History.GetBackAr (ar);
            break;

		case ID_RECYCLEBIN:
			DoRecycleMenu (pNotifyStruct);
			*pResult = TBDDRET_DEFAULT;
			return;
			
		case ID_MYCOMPUTER:
			DoMyComputerMenu (pNotifyStruct);
			*pResult = TBDDRET_DEFAULT;
			return;			
    }
	

	for (unsigned int i = 0; i < ar.size (); i++)
	{
		//TRACE (_T("% i %s\n"), ar[i].m_Text);

		if (ar[i].m_Text.IsEmpty ())
			menu.AppendMenu (MF_STRING, i+1, _T("My Computer"));
		else
			menu.AppendMenu (MF_STRING, i+1, ar[i].m_Text);
	}
	
   
    
    CRect rc;
    ::SendMessage( pNMToolBar->hdr.hwndFrom, 
        TB_GETRECT, pNMToolBar->iItem, ( LPARAM )&rc );
    rc.top = rc.bottom;
    ::ClientToScreen( pNMToolBar->hdr.hwndFrom, &rc.TopLeft() );

    int cmd = menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD , rc.left, rc.top, this );

	if (!cmd)
		return;

	
	if (pNMToolBar->iItem == ID_FORWARD)
		MoveForward (cmd);	
	else
		if (pNMToolBar->iItem == ID_BACK)
			MoveBack (cmd);
	    
}


LPARAM CExplorerXPView::OnRecBinUpdate (WPARAM wp, LPARAM lp)
{
	if (m_pViewer == &gRecBinViewer)
	{		
		SaveSelection ();
		m_pViewer->OnClose ();
		HardFill ();		
		RestoreSelection ();
		Invalidate ();
	}		
	return 0;
}

bool Rename(const CString& src, const CString& dst, FILEOP_FLAGS fFlags)
{
	// int size = 0;

	// same ?
	if (src.Compare(dst) == 0)
		return true;

	std::filesystem::path src1 = src.GetString();
	std::filesystem::path dst1 = dst.GetString();
	try {
		if (std::filesystem::exists(dst1) == false) 
			std::filesystem::rename(src1, dst1);
	}
	catch (std::filesystem::filesystem_error& e) {
		auto errorCode = e.code();		
		//TRACE(_T("Rename Error - %s\n"), errorCode.message().c_str());
	}
	

	return true;
}

/*
bool Rename (const TCHAR *src, const TCHAR *dst, FILEOP_FLAGS fFlags)
{
	int size = 0;

	// same ?
	if (_tcscmp (src, dst) == 0)
		return true;

	std::filesystem::rename(src, dst);

	return true;

	TCHAR *pFrom = AllocateNullPath (src, size);
	
	SHFILEOPSTRUCT fo;
	fo.hwnd = NULL;
	fo.wFunc = FO_RENAME;
	fo.fFlags = fFlags;
	fo.pFrom = pFrom;
	fo.pTo = dst;
	fo.lpszProgressTitle = NULL;

	SHFileOperation (&fo);

	delete [] pFrom;	
	
	return true;
}
*/

void RenameFiles (CSelRowArray &ar, const TCHAR *path)
{
	if (ar.size () == 1)
	{
		CRenameDlg dlg (ar[0].m_Name);
		if (dlg.DoModal () == IDOK)
		{
			CString cs = path+  dlg.m_NewName;
			Rename (ar[0].m_Path, cs, FOF_ALLOWUNDO);			 
		}		
	}
	else
	{
		CMultiRenameDlg dlg (ar);
		if (dlg.DoModal () == IDOK)
		{
			for (unsigned int i = 0; i < ar.size (); i++)
			{
				CString cs = path +  dlg.m_ArNew[i].m_Name;
				Rename (ar[i].m_Path, cs, FOF_ALLOWUNDO);				
			}
		}		
	}
}

void CExplorerXPView::OnRename() 
{		
	CSelRowArray ar;
	GetSelection (ar);	
	DisableUpdates ();
	CString currentPath = m_pViewer->GetTitle ();
	RenameFiles (ar, currentPath);
	m_bDisable = false;	
	m_pGridCtrl->SetFocus ();		
	m_bToDo = false;

}

void CExplorerXPView::OnUpdateRename(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (IsSelected () && !IsSpecialViewer ());	
}


bool CExplorerXPView::GetPathFromCell (CCellID &cell, CString &path)
{
	if (!m_pGridCtrl)
		return false;

	if (m_pGridCtrl->IsValid (cell))
	{
			CString tmp = m_pViewer->GetPath (cell.row - 1);
			if (IsDirectory (tmp) && cell.col == 0)
			{
				path = tmp;
				return true;
			}	
			
		cell = CCellID (-1, -1);
	}

	return false;
}

void CExplorerXPView::OnNewfolder() 
{
	if (IsSpecialViewer ())
		return;

	CSelRowArray ar;
	GetSelection (ar);

	CNewFolderDialog dlg (ar.size () != 0 ? ar[0].m_Name:"");
	
	if (dlg.DoModal () == IDOK)
	{
		//DisableUpdates ();
		CString path = m_pViewer->GetTitle ();
		SureBackSlash	(path);
		path += dlg.m_FolderName;

		BOOL bRet = CreateDirectory (path, NULL);

		if (!bRet)
		  LastErrorMessage();		
		
		OnRefresh ();
		//EnableUpdates ();
	}
}


void CExplorerXPView::DisableUpdates ()
{
	m_bDisable = true;
	m_bToDo = false;
}

void CExplorerXPView::EnableUpdates ()
{
	m_bDisable = false;
	if (m_bToDo)
	{
		SaveSelection ();	
		HardFill ();		
		RestoreSelection ();
		Invalidate ();
	}
	m_bToDo = false;
}



void CExplorerXPView::OnGetInfoTip( NMHDR* pNMHDR, LRESULT* pResult )
{
    NMTBGETINFOTIP* lptbgit = ( NMTBGETINFOTIP* )pNMHDR;
    CString strTip;
	CString strFull;
	CHistoAr ar;

	*pResult = 0;

	switch (lptbgit->iItem)
	{
		case ID_BACK:
			if (!m_History.CanBack ())
			{
				VERIFY( strFull.LoadString( ID_BACK ) );
				VERIFY( AfxExtractSubString( strTip, strFull, 1, _T('\n') ) );
			}
			else
			{
			    m_History.GetBackAr (ar);
				strTip = "Back to ";
				strTip += ar[0].m_Text;
			}
			break;
		case ID_FORWARD:
			if (!m_History.CanFore ())
			{
				VERIFY( strFull.LoadString( ID_FORWARD ) );
				VERIFY( AfxExtractSubString( strTip, strFull, 1, _T('\n') ) );
			}
			else
			{
				m_History.GetForeAr (ar);
				strTip = "Forward to ";
				strTip += ar[0].m_Text;
			}
			break;
		default:
			return;

	}
    //GetButtonTip( lptbgit->iItem, strTip );
    _tcsncpy( lptbgit->pszText, strTip, lptbgit->cchTextMax );
    
}

void CExplorerXPView::OnSetFocus(CWnd* pOldWnd) 
{
	if (!m_pGridCtrl)
		return;

	m_pGridCtrl->SetFocus ();	
}

void CExplorerXPView::OnTimer(UINT_PTR nIDEvent)
{
	if (m_iTimerID == nIDEvent)
	{
		if (!IsWindowVisible())
			return;

		if (m_bNeedSync)
		{						
			SaveSelection ();	
			Sort ();
			RestoreSelection ();
			Invalidate ();
			m_bNeedSync = false;
		}		

		if (m_bNeedTimerRefresh)
		{
			SaveSelection ();	
			m_pViewer->OnClose();
			HardFill ();		
			RestoreSelection ();
			Invalidate ();	
			m_bNeedTimerRefresh = false;
		}			
	}
	
	CView::OnTimer(nIDEvent);
}

void CExplorerXPView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if (bActivate)
	{
		SyncUI (m_Root);
		//m_pGridCtrl->SetGridBkColor (::GetSysColor (COLOR_WINDOW));
	}
	else 
		//m_pGridCtrl->SetGridBkColor (::GetSysColor (COLOR_3DFACE));

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CExplorerXPView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
		switch (pMsg->wParam )
		{
			case VK_TAB:
				{
					if (IsCTRLpressed())
						return FALSE;

					if (IsSHIFTpressed())
						TabFrom (MAIN_VIEW, BACKWARD);
					else
						TabFrom (MAIN_VIEW, FORWARD);					
					return TRUE;
				}
			case VK_APPS: 
				{
					if (!m_pGridCtrl)
						break;

					CSelRowArray ar;
					GetSelection (ar);

					if (!ar.size ())
						break;
					
					CPoint pt;						
					pt.x = 0;

					CCellRange selRange = m_pGridCtrl->GetSelectedCellRange();

					CRect selRect, clientRect;
					GetClientRect (clientRect);
					m_pGridCtrl->GetCellRangeRect (selRange, &selRect);

					if ((clientRect.top < selRect.top ) && (clientRect.bottom < selRect.bottom)) // to big selection
						pt.y = clientRect.CenterPoint().y;
					else
						pt.y =  (clientRect.top < selRect.top) ?  selRect.bottom : selRect.top;

					ClientToScreen (&pt);
					m_pViewer->ContextMenu (this, ar, pt);
				}
				break;

			case VK_RIGHT: 
				{
					bool bChange = false;
					CSelRowArray ar;

					GetSelection (ar);	

					if (ar.size () == 1)
					   if (m_pViewer->CanChangeTo (ar[0].m_nRow-1))
					 	DoSomething (ar[0].m_nRow-1, bChange);
				} break;
			case VK_BACK:
			case VK_LEFT:
				OnUp ();
				return TRUE;
				

			case VK_RETURN:	
				OnDown();
				return TRUE;

	}
	
	return CView::PreTranslateMessage(pMsg);
}


void CExplorerXPView::OnRefresh() 
{
	SaveSelection ();	
	m_pViewer->OnClose();
	m_pViewer->Fill (m_Root);
	RestoreSelection ();
	Invalidate ();	
}

void CExplorerXPView::OnUpdateRefresh(CCmdUI* pCmdUI)
{
	
}

bool CExplorerXPView::IsSpecialViewer()
{
	if ( (m_pViewer == &gDriveViewer) ||  (m_pViewer == &gRecBinViewer))
		  return true;

	return false;
}

void CExplorerXPView::OnUpdateUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ());
}

void CExplorerXPView::OnUpdateMoveTo(CCmdUI* pCmdUI)
{
	BOOL bEnable = IsSelected () && !IsSpecialViewer ();
	pCmdUI->Enable (bEnable);	
}

void CExplorerXPView::OnUpdateCopyTo(CCmdUI* pCmdUI)
{
	BOOL bEnable = IsSelected () && !IsSpecialViewer ();
	pCmdUI->Enable (bEnable);	
}

void CExplorerXPView::OnCopyTo()
{
	if (IsSpecialViewer ())
		return;

	CCopyMoveDlg dlg (_T("Copy To..."), m_pViewer->GetTitle ());
	if (dlg.DoModal () == IDOK)
	{
		CSelRowArray ar;
		GetSelection (ar);
		CString path = dlg.GetPath();
		FileOperation (ar, FO_COPY, FOF_ALLOWUNDO, path);
	}

}

void CExplorerXPView::OnMoveTo()
{
	if (IsSpecialViewer ())
		return;

	CCopyMoveDlg dlg (_T("Move To..."), m_pViewer->GetTitle ());
	if (dlg.DoModal () == IDOK)
	{
		CSelRowArray ar;
		GetSelection (ar);
		CString path = dlg.GetPath();
		FileOperation (ar, FO_MOVE, FOF_ALLOWUNDO, path);
	}
}

void CExplorerXPView::OnSelectionRemove() 
{
	m_pGridCtrl->ClearSelection ();	
}

void CExplorerXPView::OnUpdateSelectionRemove(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CExplorerXPView::OnSelectionWildcarddeselect() 
{
	CSelRowArray ar;
	GetSelection (ar);
	CSelectDialog dlg(_T("Deselect"), ar.size () != 0 ? ar[0].m_Name : EMPTYSTR);
	if (dlg.DoModal() == IDOK)
		Select (dlg.m_Wildcards, false);
	
}

void CExplorerXPView::OnUpdateSelectionWildcarddeselect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CExplorerXPView::OnSelectionWildcardselect() 
{
	CSelRowArray ar;
	GetSelection (ar);
	CSelectDialog dlg(_T("Select"), ar.size () != 0 ? ar[0].m_Name:EMPTYSTR);	
	if (dlg.DoModal() == IDOK)
		Select (dlg.m_Wildcards, true);
	
	
}

void CExplorerXPView::OnUpdateSelectionWildcardselect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}



void CExplorerXPView::Select (CString &wildcards, bool bSelect)
{
	if (!m_pGridCtrl)
		return;

	wildcards.MakeLower();
	for (int i = 0; i < m_pViewer->GetCount (); i++)
	{
		CString cs = m_pViewer->GetName (i);
		cs.MakeLower();

		if (PathMatchSpec(cs, wildcards))
			if (bSelect)				
				m_pGridCtrl->SetSelectedRangeFast (i+1, 0, i+1,m_pViewer->GetColumnCount (), TRUE);				
			else				
				m_pGridCtrl->SetSelectedRangeFast (i+1, 0, i+1,m_pViewer->GetColumnCount (), FALSE);
	}
	m_pGridCtrl->Invalidate ();
}

void CExplorerXPView::OnEditSelectall() 
{
	m_pGridCtrl->SelectAllCells ();	
}

void CExplorerXPView::OnUpdateEditSelectall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CExplorerXPView::OnEditInvertselection() 
{
	if (!m_pGridCtrl)
		return;

	for (int i = 1; i < m_pGridCtrl->GetRowCount ();i++)
	{
		if (m_pGridCtrl->IsCellSelected (i,0))		
			m_pGridCtrl->SetSelectedRangeFast (i, 0, i,m_pViewer->GetColumnCount (), FALSE);
		else
			m_pGridCtrl->SetSelectedRangeFast (i, 0, i,m_pViewer->GetColumnCount (), TRUE);
	}

	m_pGridCtrl->Invalidate ();
}

void CExplorerXPView::OnUpdateEditInvertselection(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here	
}

void CExplorerXPView::OnCommandsSplit() 
{	
	CSelRowArray ar;
	GetSelection (ar);
	if (ar.size () == 1)
	{
		CSplitDlg dlg (ar[0].m_Path);
		dlg.DoModal ();	
	}
}

void CExplorerXPView::OnUpdateCommandsSplit(CCmdUI* pCmdUI) 
{
	BOOL bEnable = IsSelected () && !IsSpecialViewer ();
	pCmdUI->Enable (bEnable);		
}

void CExplorerXPView::OnCommandsMerge() 
{
	CSelRowArray ar;
	GetSelection (ar);
	if (ar.size () >= 1)
	{
		CMergeDlg dlg (ar[0].m_Path);
		dlg.DoModal ();	
	}
}

void CExplorerXPView::OnUpdateCommandsMerge(CCmdUI* pCmdUI) 
{
	BOOL bEnable = IsSelected () && !IsSpecialViewer ();
	pCmdUI->Enable (bEnable);		
}

void AddDir (CString dir, CSelRowArray &toClean, CFilter &filter)
{
  CString tmpDir = dir;

  SureBackSlash (tmpDir);

  CString strFilter;
  CFileFind find;
  BOOL      bWorking;

  for (int i = 0; i < filter.size(); i++)
  {
	  strFilter = tmpDir;	  
	  strFilter += filter[i];
	  if (find.FindFile (strFilter))
	  {	  
		  toClean.push_back (CSelectRow(strFilter, filter[i]));
		  find.Close ();
	  }
  }

  if ( find.FindFile ( tmpDir + _T("*.*") ))
  {
	do
	{
		bWorking = find.FindNextFile();

		if (!ifNotJunctionPoint(find)) 
				continue;

		if ( !find.IsDirectory() || find.IsDots() )
			continue;

		AddDir ( find.GetFilePath() + '\\', toClean, filter);
	} while ( bWorking );
  }
}

void Clean (CSelRowArray &sel, CFilter &filter)
{
	CSelRowArray toClean;
	
	for (unsigned int i = 0; i < sel.size (); i++)
		if (IsDirectory (sel[i].m_Path))
			AddDir (sel[i].m_Path, toClean, filter);

  if (toClean.size ())
	  FileOperation (AfxGetMainWnd(), toClean, FO_DELETE,  FOF_ALLOWUNDO | FOF_FILESONLY | FOF_NOCONFIRMATION, NULL);
}


void CExplorerXPView::OnCommandsClean() 
{
	CSelRowArray ar;
	GetSelection (ar);

	CCleanDialog dlg;
	if (dlg.DoModal () == IDOK)
	{
		Clean (ar, gFilterMan[dlg.m_nSel]);
	}
	
}

void CExplorerXPView::OnUpdateCommandsClean(CCmdUI* pCmdUI) 
{
	BOOL bEnable = false;
	
	if (m_pViewer == &gDriveViewer)
		bEnable = IsSelected ();
	else
		if (m_pViewer == &m_FileViewer)
		bEnable = IsSelected ();

	pCmdUI->Enable (bEnable);	
}



void CExplorerXPView::OnCopyclipboard() 
{	
	CSelRowArray ar;
	GetSelection (ar);
    CString cs;

	if (ar.size () >= 1)	
	{
		for (int  i = ar.size() - 1; i >= 0; i--)
		{
			for (int j = 0; j < m_pViewer->GetColumnCount (); j++)
			{
				CString tmp = m_pViewer->GetText (ar[i].m_nRow - 1,  j);
				tmp.TrimLeft ();
				tmp.TrimRight ();
				cs += tmp;
				if (j!= (m_pViewer->GetColumnCount () -1))
					cs += "\t";
			}
			cs +="\r\n";
		}	
    } 
    else
    {
        for (int i = 0; i < m_pViewer->GetCount (); i++)
        {
		    for (int j = 0; j < m_pViewer->GetColumnCount (); j++)
			    {
				    CString tmp = m_pViewer->GetText (i,  j);
				    tmp.TrimLeft ();
				    tmp.TrimRight ();
				    cs += tmp;
				    if (j!= (m_pViewer->GetColumnCount () -1))
					    cs += "\t";
			    }
		    cs +="\r\n";		   	
        }
    }
    
	CopyTextToClip(cs);
}

void CExplorerXPView::OnWindowNext() 
{
	//AfxGetMainWnd ()->SendMessage (WM_COMMAND, ID_WINDOW_NEXT);
}



void CExplorerXPView::SetGridFromOptions ()
{
	if (!m_pGridCtrl)
		return;

	int gridLines = m_pGridCtrl->GetGridLines();
	int newgridLines = GVL_NONE;

	if (gOptions.m_bVLines && gOptions.m_bHLines)
		newgridLines = GVL_BOTH;
	else 
		if (gOptions.m_bVLines)
			newgridLines = GVL_VERT;
		else
			if (gOptions.m_bHLines)
				newgridLines = GVL_HORZ;

	if (newgridLines != gridLines)
	{
		m_pGridCtrl->SetGridLines(newgridLines);
		m_pGridCtrl->Invalidate();
	}    
}
LPARAM CExplorerXPView::OnOptions (WPARAM wp, LPARAM lp) 
{
	SetGridFromOptions ();
	return 0L;
}
void CExplorerXPView::OnProperties()
{
	CSelRowArray ar;
	GetSelection (ar);

	if (ar.size ())
		m_pViewer->Properties (ar);	
}


LPARAM CExplorerXPView::OnHardUpdate (WPARAM wp, LPARAM lp) 
{
	m_bNeedTimerRefresh = true;
	return 0L;
}

void CExplorerXPView::OnFoldersResetallfolders()
{
	gFolderStateMan.ResetAllFolders ();
	CFolderState state = gFolderStateMan.GetDefault();

	dirs.Notify (WM_EXP_SETSTATE, NULL, (LPARAM)&state);
}

void CExplorerXPView::OnFoldersMakeasthisfolder()
{
	if (!m_pViewer)
		return;

	CFolderState state;
	m_pViewer->GetState (state);	
	gFolderStateMan.MakeAllAs (state);
	dirs.Notify (WM_EXP_SETSTATE, NULL, (LPARAM)&state);
}

void CExplorerXPView::OnUpdateFoldersResetallfolders(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ());	
}

void CExplorerXPView::OnUpdateFoldersMakeasthisfolder(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ());	
}


LPARAM CExplorerXPView::OnSetState (WPARAM wp, LPARAM lp)
{
	if (IsSpecialViewer ())
		return NULL;

	if (!lp)
		return NULL;

	CFolderState *state = (CFolderState *)lp;
	m_pViewer->SetState (*state);
	SaveSelection ();		
	HardFill ();		
	RestoreSelection ();
	Invalidate ();	
	
	return NULL;
}
void CExplorerXPView::OnUpdateNewfolder(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ());	
}


void CExplorerXPView::OnGroupsSaveopenfoldersasgroup()
{
	CSelectDialog dlg (_T("Add Group"), NULL);

	if (dlg.DoModal())
	{		
		CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
		CStringAr  tabs;
		pFrame->GetTabs(tabs);
		gGroupManager.Add(dlg.m_Wildcards, tabs);
		gFavs.Reload();
		gFavs.Invalidate();
	}
}

void CExplorerXPView::OnDeletepermently()
{
	InternalDelete (false);	
}

void CExplorerXPView::OnSelectionSelectsimilar()
{
	CSelRowArray ar;
	GetSelection(ar);
	if (!ar.size ())
		return;

	CString match = ar[0].m_Name;
	
	match.MakeLower();
	int last_dif = -1;
	for (int i = 0; i < m_pViewer->GetCount (); i++)
	{
		CString file = m_pViewer->GetName (i);
		file.MakeLower();

		int dif = 0;
		dif += abs (match.GetLength() - file.GetLength());
		int len = min (match.GetLength(), file.GetLength());
		for (int k = 0; k < len ; k++)
		{
			/*if (match[k] == '.')
				break;

			if (file[k] == '.')
				break;*/

		/*	if (match[k] == '-')
			{
				dif -= abs (match.GetLength() - file.GetLength());
				break;
			}

			if (file[k] == '-')
			{
				dif -= abs (match.GetLength() - file.GetLength());
				break;
			}*/

			if (match[k] != file[k]) 
			{
				dif++;
				if ( (last_dif != -1) && (last_dif != (k-1)))
					dif++;
				last_dif = k;				
			}


				
		}

		if (dif <= 4)
			m_pGridCtrl->SetSelectedRangeFast (i+1, 0, i+1,m_pViewer->GetColumnCount (), TRUE);
		


		/*if (PathMatchSpec(cs, wildcards))
			if (bSelect)				
				m_pGridCtrl->SetSelectedRangeFast (i+1, 0, i+1,m_pViewer->GetColumnCount (), TRUE);				
			else				
				m_pGridCtrl->SetSelectedRangeFast (i+1, 0, i+1,m_pViewer->GetColumnCount (), FALSE);*/
	}
	m_pGridCtrl->Invalidate ();	
}

void CExplorerXPView::OnCommandsCmd()
{
	CString folder = m_pViewer->GetTitle();
	CString params = "/k \"cd ";
	params += folder; 
	params += "\"";
	ShellExecute(NULL, NULL, _T("cmd.exe"), params, folder, SW_SHOWNORMAL);
}

void CExplorerXPView::OnUpdateCommandsCmd(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ());	
}

void CExplorerXPView::OnEditCopyfolderpathtoclipboard()
{
	CString cs = m_pViewer->GetTitle();
	CopyTextToClip(cs);
}

void CExplorerXPView::OnEditCopyfilenametoclipboard()
{
	CSelRowArray ar;
	GetSelection (ar);
	CString cs;

	if (ar.size () >= 1)	
	{
		for (int i = ar.size() - 1; i >= 0; i--)
		{

			CString tmp = m_pViewer->GetText (ar[i].m_nRow - 1,  0);
			tmp.TrimLeft ();
			tmp.TrimRight ();
			cs += tmp;			
			cs +="\r\n";
		}	
	} 
	else
	{
		for (int i = 0; i < m_pViewer->GetCount (); i++)
		{
			CString tmp = m_pViewer->GetText (i,  0);
			tmp.TrimLeft ();
			tmp.TrimRight ();
			cs += tmp;			
			cs +="\r\n";		   	
		}
	}
	CopyTextToClip(cs);
}

void CExplorerXPView::OnUpdateEditCopyfolderpathtoclipboard(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ());
}


void CExplorerXPView::OnEditCopyfullpathnamesastext()
{
	CSelRowArray ar;
	GetSelection (ar);
	CString cs;

	CString path;
	if (!IsSpecialViewer())
	{
		path = m_pViewer->GetTitle();
		SureBackSlash(path);
	}
	

	if (ar.size () >= 1)	
	{
		for (int i = ar.size() - 1; i >= 0; i--)
		{
			CString tmp = m_pViewer->GetText (ar[i].m_nRow - 1,  0);
			tmp.TrimLeft ();
			tmp.TrimRight ();
			cs += path;
			cs += tmp;			
			cs +="\r\n";
		}	
	} 
	else
	{
		for (int i = 0; i < m_pViewer->GetCount (); i++)
		{
			CString tmp = m_pViewer->GetText (i,  0);
			tmp.TrimLeft ();
			tmp.TrimRight ();
			cs += path;
			cs += tmp;			
			cs +="\r\n";		   	
		}
	}
	CopyTextToClip(cs);
}

void CExplorerXPView::OnEditCopytoparent()
{
	CString folder = m_Root;
	if (GetParentFolder(folder))	
	{
		CSelRowArray sel;
		GetSelection(sel);
		FileOperation (sel, FO_MOVE, FOF_ALLOWUNDO, folder);
	}	
}

void CExplorerXPView::OnEditCopytosub()
{	
	CMenu menu;
	menu.CreatePopupMenu();

	POINT pt;
	GetCursorPos(&pt);
	CStringAr folders;
	int j = 0;
	for (int i = 0; i < m_pViewer->GetCount(); i++) 
	{
		CString cs;		
    bool bFolder = false;
		cs += m_pViewer->GetPath(i, bFolder);		
    if (bFolder == true) 
		//if (IsDirectory(cs))
		{
			folders.push_back(cs);			
			CString nm;
			if (i < 36)
			{
				nm += _T("&");
				nm += hex[i];
				nm += _T("  ");
			}
			else
				nm += _T("   ");

			cs = m_pViewer->GetText (i, 0);
			cs.Replace(_T("&"), _T("&&"));
			cs = nm + cs;
			cs += '\t';
			cs += m_pViewer->GetText (i, 1);
			menu.AppendMenu (MF_STRING, j+1, cs);		
			j++;
		}
	}

	int result = menu.TrackPopupMenu (TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, this );
	if ((result > 0) && (result <= static_cast<int>(folders.size())))
	{
		CSelRowArray sel;
		GetSelection(sel);
		FileOperation (sel, FO_MOVE, FOF_ALLOWUNDO, folders[result-1]);
	}
	
}

void CExplorerXPView::OnUpdateEditCopytosub(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer () && IsSelected ());
}

void CExplorerXPView::OnUpdateEditCopytoparent(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ()  && IsSelected ());
}

void CExplorerXPView::OnEditMovetogroups()
{
	CMenu menu;
	menu.CreatePopupMenu();

	POINT pt;
	GetCursorPos(&pt);
	CStringAr folders;
	int count = 0;

	for (unsigned int i = 0; i < gGroupManager.size(); i++)
	{
		if (i != 0)
		{
			menu.AppendMenu(MF_SEPARATOR | MF_GRAYED, ++count, gGroupManager[i].m_Name);
			folders.push_back("");
		}
		//menu.SetDefaultItem (count -1, TRUE);

		for (unsigned int j = 0; j < gGroupManager[i].m_Folders.size(); j++)
		{
			CString cs = gGroupManager[i].m_Folders[j];
			cs.Replace(_T("&"), _T("&&"));
			menu.AppendMenu(MF_STRING, ++count, cs);
			folders.push_back(gGroupManager[i].m_Folders[j]);
		}
	}

	int result = menu.TrackPopupMenu (TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, this );

	if (result != 0)
	{
		CSelRowArray sel;
		GetSelection(sel);
		FileOperation (sel, FO_MOVE, FOF_ALLOWUNDO, folders[result-1]);
	}
}

void CExplorerXPView::OnUpdateEditMovetogroups(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ()  && IsSelected ());
}

void CExplorerXPView::OnEditMovetotabs()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
	CStringAr  tabs;
	pFrame->GetTabs(tabs);
	sort(tabs.begin(), tabs.end());

	CMenu menu;
	menu.CreatePopupMenu();

	POINT pt;
	GetCursorPos(&pt);
	CStringAr folders;
	CString root = m_pViewer->GetTitle ();

	SureBackSlash(root);

	for (unsigned int i = 0; i < tabs.size(); i++) 
	{
		CString cs;

		cs += tabs[i];		
		if (IsDirectory(cs) && cs != root)
		{
			folders.push_back(cs);
			CString nm;
			if (i < 36)
			{
				nm += _T("&");
				nm += hex[i];
				nm += _T("  ");
			}
			else
			nm += _T("   ");
			cs = nm + cs;	
			cs.Replace(_T("&"), _T("&&"));
			menu.AppendMenu (MF_STRING, folders.size(), cs);		
		}
	}

	int result = menu.TrackPopupMenu (TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, this );
	if (result != 0)
	{
		CSelRowArray sel;
		GetSelection(sel);
		FileOperation (sel, FO_MOVE, FOF_ALLOWUNDO, folders[result-1]);
	}
}

void CExplorerXPView::OnUpdateEditMovetotabs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (!IsSpecialViewer ()  && IsSelected ());
}

void CExplorerXPView::OnCommandsFindmissing()
{
	CFindMissing dlg;

	CSelRowArray ar;
	GetSelection(ar);

	if (!ar.size ())
		return;

	dlg.Init(m_pViewer->GetTitle(), ar[0].m_Name);
	
	dlg.DoModal();
}

LRESULT CExplorerXPView::OnAppCommand (WPARAM wp, LPARAM lp)
{
	short	cmd  = GET_APPCOMMAND_LPARAM(lp);

	if (cmd == APPCOMMAND_BROWSER_FORWARD)
		if (m_History.CanFore ())
		{
			OnForward();
			return TRUE;
		}

	if (cmd == APPCOMMAND_BROWSER_BACKWARD)
		if (m_History.CanBack ())
		{
			OnBack();
			return TRUE;
		}

	return FALSE;	
}	

void CExplorerXPView::OnCommandsFindduplicates()
{
	CString exePath;
	GetExePath (exePath);
	exePath += _T("dupfind.exe");


	CString dir = m_pViewer->GetTitle ();

	CString params = _T(" /Folder:\"");
	params += dir;
	params += _T("\"");

	ShellExecute (m_hWnd, NULL, exePath, params, NULL, SW_SHOWNORMAL);
}

void CExplorerXPView::OnCommandsShowcharts()
{
	CCharts charts;

	charts.DoModal ();
}

void CExplorerXPView::SortByColumn(int sortColumn) {
	

	//m_pViewer->SetSortDir(m_pGridCtrl->GetSortAscending() == TRUE);
	
	int currentSortColumn = m_pGridCtrl->GetSortColumn();
    
	SaveSelection();

	if (currentSortColumn == sortColumn) {
		bool bSort = ! m_pGridCtrl->GetSortAscending();
		m_pViewer->SetSortDir( bSort);
		m_pGridCtrl->SetSortAscending(bSort);
	} else {
		m_pViewer->SetSortDir(TRUE);
		m_pGridCtrl->SetSortAscending(TRUE);
	}
	
	m_pViewer->SetSortColumn(sortColumn);
	m_pGridCtrl->SetSortColumn(sortColumn);
	Sort();
	RestoreSelection();
	Invalidate();
}


// sort by name
void  CExplorerXPView::OnCommandToolsSort1() {
	SortByColumn(0);
}

//sort by size
void  CExplorerXPView::OnCommandToolsSort2() {
	SortByColumn(1);
}

//sort by size on disk
void  CExplorerXPView::OnCommandToolsSort3() {
	SortByColumn(2);
}

//sort by type
void  CExplorerXPView::OnCommandToolsSort4() {
	SortByColumn(3);
}

void  CExplorerXPView::OnCommandToolsSort5() {
	SortByColumn(4);
}

void  CExplorerXPView::OnCommandToolsSort6() {
	SortByColumn(5);
}

void  CExplorerXPView::OnUpdateToolsSort(CCmdUI* pCmdUI) {

	// disable the sort order for My Computer folder & Recycle Bin

	// quick and dirty way to identify the file viewer
	if (m_pViewer->CanPaste())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}


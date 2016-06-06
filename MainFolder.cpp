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
#include "MainFolder.h"
#include "globals.h"
#include "ExplorerXPDoc.h"
#include "ExplorerXPView.h"
#include "ShellContextMenu.h"
#include "DirSize.h"
#include "NewFolderDialog.h"
#include "Options.h"
#include "ShortcutManager.h"
#include "FileShellAttributes.h"

extern CShortcutManager gShortcutManager;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFolder

//IMPLEMENT_DYNCREATE(CMainFolder, CDriveTree)

#define EXPAND_TIMER 99
#define REFRESH_TIMER 77


CMainFolder::CMainFolder()
{
	m_hLastDropItem = NULL;
	m_bDisableUpdate = false;

	for (int i = 0; i < 1024; i++)
		m_arUpdateDrives[i] = false;

	m_bDragDataAcceptable = false;
}

void CMainFolder::MarkAll ()
{
	for (int i = 0; i < 1024; i++)
		m_arUpdateDrives[i] = true;
}

CMainFolder::~CMainFolder()
{
}


BEGIN_MESSAGE_MAP(CMainFolder, CDriveTree)	
		ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
		ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
		ON_WM_CREATE()
		ON_NOTIFY_REFLECT (TVN_BEGINDRAG, OnBegindrag)
		ON_WM_TIMER()
//		ON_NOTIFY_REFLECT (TVN_KEYDOWN, OnKeyDown)				
	ON_COMMAND(ID_MOVETO, OnMoveto)
	ON_UPDATE_COMMAND_UI(ID_MOVETO, OnUpdateMoveto)
	ON_COMMAND(ID_COPYTO, OnCopyto)
	ON_UPDATE_COMMAND_UI(ID_COPYTO, OnUpdateCopyto)	
	ON_MESSAGE(WM_EXP_UPDATE, OnFolderUpdate)		
	ON_MESSAGE(WM_EXP_FILERENAMED, OnFileRenamed)		
	ON_COMMAND (ID_RENAME, onRename)
	ON_COMMAND (ID_NEWFOLDER, onNewFolder)
	ON_COMMAND (ID_COPY, onCopy)
	ON_COMMAND (ID_CUT, onCut)
	ON_COMMAND (ID_PASTE, onPaste)
	ON_COMMAND (ID_DELETE, onDelete)
	ON_COMMAND (ID_DELETEPERMENTLY, onDeletePerm)
END_MESSAGE_MAP()


void CMainFolder::OnSelectionChanged (CString& strPathName)
{
	if (!m_bDisableUpdate)
	{
		//SureBackSlash(strPathName);
		::SetRoot (strPathName);	
	}
}

void CMainFolder::OnRclick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{   
  CPoint p(GetCurrentMessage()->pt);
  CPoint pt(p);

  ScreenToClient(&pt);

  HTREEITEM hItem = HitTest (pt);

  if (hItem == NULL)
	  return;

  CString path = GetPathFromItem (hItem);
  HTREEITEM parent = GetParentItem ( hItem);

   CString parent_path = GetPathFromItem (parent);
   CString name = GetItemText (hItem);

    CSelRowArray ar;

    ar.push_back (CSelectRow (path, name, 0));

    CShellContextMenu scm;

	scm.SetObjects (ar);

	ClientToScreen (&pt);

	UINT idCommand = scm.ShowContextMenu (this, pt);
												// therefore CShellContextMenu would delete it
	if (idCommand)
	{
		if (idCommand == CONTEXT_EXPLORE)		
			for (unsigned int i = 0 ; i < ar.size (); i++)
				if (IsDirectory (ar[i].m_Path))
				{
					SureBackSlash (ar[i].m_Path);
					OpenFolder (ar[i].m_Path);
				}						

		if (idCommand == CONTEXT_OPEN)		
			for (unsigned int i = 0 ; i < ar.size (); i++)
				if (IsDirectory (ar[i].m_Path))
				{
					::SetRoot (ar[i].m_Path);
					break;
				}				
		
		if (idCommand == CONTEXT_RENAME)		
			RenameFiles (ar, parent_path);		
	}
}

void CMainFolder::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    TRACE( _T("CMainFolder::Onclick(%p)\n"), pNMHDR );
	
	*pResult = NULL;
	
	UINT uFlags;

//Work out the position of where the context menu should be
  CPoint p(GetCurrentMessage()->pt);
  CPoint pt(p);
  ScreenToClient(&pt);

  HTREEITEM hItem = HitTest (pt, &uFlags);

  if ((hItem == NULL) ||  ( (TVHT_ONITEM & uFlags) == 0))
	  return;

  CString str;
  if (hItem == m_hParent)
	  str = CONST_MYCOMPUTER;
  else
	str = GetPathFromItem ( hItem);
	
  OnSelectionChanged (str);
}



DROPEFFECT CMainFolder::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	UINT uFlags;

	DROPEFFECT dwEffect = DROPEFFECT_NONE;

	m_DropFiles.clear ();

	m_LastDropFolder.Empty ();
	
	m_hLastDropItem = HitTest (point, &uFlags);

	if (m_hLastDropItem )		
		SetItemBold (m_hLastDropItem, TRUE);
	else
		m_hLastDropItem = NULL;

	if (m_hLastDropItem)
	{
		m_LastDropFolder = GetPathFromItem ( m_hLastDropItem);
		SureBackSlash (m_LastDropFolder);
	}	
	
	if (ReadHDropData (pDataObject, m_DropFiles, m_DropFolder))
	{
		m_bDragDataAcceptable = true;		
		if (m_hLastDropItem != NULL)
			dwEffect = CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
	}

	return dwEffect;	
}

DROPEFFECT CMainFolder::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	UINT uFlags;
	HTREEITEM hITem =  HitTest (point, &uFlags);

	//if (hITem && (TVHT_ONITEM & uFlags)
	//Expand (m_hLastDropItem, TVE_EXPAND);
	
	//if (!(TVHT_ONITEM & uFlags)) 
	//	hITem = NULL;

	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	
	if (hITem != m_hLastDropItem)
	{
		if (m_hLastDropItem)
			SetItemBold (m_hLastDropItem, FALSE);

		m_hLastDropItem = hITem;
		if (m_hLastDropItem != NULL)
		{
			//SelectDropTarget (m_hLastDropItem);	
			SetItemBold (m_hLastDropItem, TRUE);
			StartTimer (m_hLastDropItem);
		}
	}

	if (m_hLastDropItem)
	{
		m_LastDropFolder = GetPathFromItem ( m_hLastDropItem);
		SureBackSlash (m_LastDropFolder);
	}	
		
	if ( (m_bDragDataAcceptable == true) && (m_hLastDropItem != NULL))	
		dwEffect = CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
	
	// Scroll Tree control depending on mouse position

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
		int nScrollPos = GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		dwEffect  |= DROPEFFECT_SCROLL;
		SendMessage(WM_VSCROLL, wParam);
	}
	
	nScrollDir = -1;
	if ( point.x <= rectClient.left + RECT_BORDER )
		nScrollDir = SB_LINELEFT;
	else
	if ( point.x >= rectClient.right - RECT_BORDER)
		nScrollDir = SB_LINERIGHT;
	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		dwEffect  |= DROPEFFECT_SCROLL;
		SendMessage(WM_HSCROLL, wParam);
	}

	return dwEffect;
}

void CMainFolder::OnDragLeave()
{	
	m_LastDropFolder.Empty ();
	m_DropFiles.clear ();

	if (m_hLastDropItem)
		SetItemBold (m_hLastDropItem, FALSE);

	m_hLastDropItem = NULL;
	//SelectDropTarget (NULL);	
}

BOOL CMainFolder::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	CSelRowArray ar;
	CString source_folder;	
	CString dest_folder;

	SelectDropTarget (NULL);

	SetItemBold (m_hLastDropItem, FALSE);
	dest_folder = m_LastDropFolder;
	
	if (!ReadHDropData (pDataObject, ar, source_folder))
		return false;

	UINT file_oper = FO_COPY;

	DROPEFFECT drop_effect = dropEffect;

	switch (drop_effect)
		{
			case DROPEFFECT_COPY:
				file_oper = FO_COPY;
			break;

			case DROPEFFECT_MOVE:
				file_oper = FO_MOVE;
				break;
			case DROPEFFECT_LINK:
				return CreateShortcuts (ar,  m_LastDropFolder);
				
			default:
				TRACE(_T("Invalide file operation %d\n"), drop_effect);
				return false;
		}					
		
	return FileOperation (this, ar, file_oper, FOF_ALLOWUNDO  , dest_folder) != 0;	
	
}

int CMainFolder::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDriveTree::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_DropTarget.Register(this);	
	
	RegisterWindow (m_hWnd);

	SetTimer (REFRESH_TIMER, 4500, NULL);

	//CFont* pFont = CFont::FromHandle ((HFONT)::GetStockObject (SYSTEM_FONT));
	//SetFont (pFont);
	return 0;
}

void DoDragDrop (CSelRowArray &ar);

void CMainFolder::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	
	*pResult = 0;
	if (hSelItem == m_hParent)
		return;

	// Highlight selected item
	SelectItem(hSelItem);
	
	Select(hSelItem, TVGN_DROPHILITE);
	
	CString path = GetPathFromItem (hSelItem);

	CSelRowArray ar;

	ar.push_back (CSelectRow (path));
	
	DoDragDrop (ar);	
}

BOOL CMainFolder::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
		switch (pMsg->wParam )
		{
			case VK_TAB:
				{
					if (IsSHIFTpressed())
						TabFrom (MAIN_FOLDER, BACKWARD);
					else
						TabFrom (MAIN_FOLDER, FORWARD);					
					return TRUE;
				}

			/*
			case VK_F2: return onRename ();
			case VK_F7: return onNewFolder ();
				
			case VK_DELETE:
				if (IsSHIFTpressed())
					return onCut();
				else return onDelete ();
					
				
			case VK_INSERT:
				if (IsCTRLpressed())
					return onCopy();
				else
					if (IsSHIFTpressed())
						return onPaste();
				break;

			case 'C':
				if (IsCTRLpressed())				
					return onCopy();
				break;

			case 'V':
				if (IsCTRLpressed())				
					return onPaste();
				break;
				
			case 'X':
				if (IsCTRLpressed())				
					return onCut();
				break;
			*/
		}

	int nCmdID = gShortcutManager.ProcessMessage(pMsg);
	
	if (nCmdID)
		SendMessage(WM_COMMAND, nCmdID);
	else
		return CDriveTree ::PreTranslateMessage(pMsg);
	return TRUE;
}

void CMainFolder::StartTimer (HTREEITEM hITem)
{
	m_hTimerItem = hITem;
	SetTimer (EXPAND_TIMER, 940, NULL);
}

void CMainFolder::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == EXPAND_TIMER)
	{
		KillTimer (nIDEvent);
		CPoint pt;
		GetCursorPos (&pt);
		ScreenToClient (&pt);
		UINT uFlags;

		HTREEITEM hItem = HitTest (pt, &uFlags);
		if (hItem/* && (uFlags &TVHT_ONITEM)*/ )
		{
			CString path = GetPathFromItem (hItem);
			TRACE(_T("Timer Item %s "), (LPCTSTR)path);
			if (hItem == m_hTimerItem)
				Expand (hItem, TVE_EXPAND);
		}
	}

	if (REFRESH_TIMER == nIDEvent) 
	{
		if (!IsNotInFOP())
			return;

		for (int i = 0; i < 26; i++)
			if (m_arUpdateDrives [i]) 
			{
				m_arUpdateDrives [i] = false;
				RefreshDrive (i);
				break;
			}
	}
	CDriveTree::OnTimer (nIDEvent);
}

/*
void CMainFolder::OnGetTip(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVGETINFOTIP *pTip = (NMTVGETINFOTIP*)pNMHDR;
	CString text = GetItemText (pTip->hItem);
	HTREEITEM hParent = GetParentItem (pTip->hItem);

	if (hParent == m_hParent)
		return;

	CString parentPath = GetPathFromItem (hParent);
	CString result = text;

	ULONGLONG size, sizeondisk;
	if (GetDirSizeLight (parentPath, text, size, sizeondisk))
	{
		result.Format ("%s %s KB", (LPCTSTR)text, filesize_to_str (size));
	}

	lstrcpyn(pTip->pszText, result, pTip->cchTextMax);
}*/

void CMainFolder::onRename ()
{
	HTREEITEM hItem = GetSelectedItem ();

	if (!hItem) return;
	if (hItem == m_hParent) return;
			

	CString path = GetPathFromItem (hItem);
	HTREEITEM parent = GetParentItem (hItem);

	if (parent == m_hParent) return;

	CString parent_path = GetPathFromItem (parent);
	CString name = GetItemText (hItem);

	CSelRowArray ar;

	ar.push_back (CSelectRow (path, name, 0));
	SureBackSlash (parent_path);
	RenameFiles (ar, parent_path);
	SetFocus ();	
}

void CMainFolder::onNewFolder ()
{
	HTREEITEM hItem = GetSelectedItem ();

	if (!hItem) return;
	if (hItem == m_hParent) return;
	
	CString path = GetPathFromItem (hItem);
	
	CNewFolderDialog dlg (EMPTYSTR);

	if (dlg.DoModal () == IDOK)
	{		
		SureBackSlash	(path);
		path += dlg.m_FolderName;
		BOOL bRet = CreateDirectory (path, NULL);
		if (!bRet)
		  LastErrorMessage();		
	}
	SetFocus ();
}

void CMainFolder::onCopy ()
{
	HTREEITEM hItem = GetSelectedItem ();

	if (!hItem) return;
	if (hItem == m_hParent) return;

	CSelRowArray ar;

	CString path = GetPathFromItem (hItem);
	CString name = GetItemText (hItem);

	ar.push_back (CSelectRow (path, name, 0));

	CopyToClipboard (ar, DROPEFFECT_COPY);
	SetFocus ();	
}

void CMainFolder::onCut ()
{
	HTREEITEM hItem = GetSelectedItem ();

	if (!hItem) return;
	if (hItem == m_hParent) return;

	CSelRowArray ar;

	CString path = GetPathFromItem (hItem);
	CString name = GetItemText (hItem);
	
	ar.push_back (CSelectRow (path, name, 0));

	CopyToClipboard (ar, DROPEFFECT_MOVE);

	SetFocus ();	
}

void CMainFolder::onPaste ()
{
	HTREEITEM hItem = GetSelectedItem ();

	if (!hItem) return;
	if (hItem == m_hParent) return;
	
	CString dest_folder = GetPathFromItem (hItem);
	
	CSelRowArray ar;

	CString source_folder;
		
	COleDataObject object;

	object.AttachClipboard ();
		
	if (!ReadHDropData (&object, ar, source_folder))
		return;

	UINT file_oper = FO_COPY;

	DROPEFFECT drop_effect;

	ReadDropEffect (&object, drop_effect);

	switch (drop_effect)
		{
			case DROPEFFECT_COPY:
				file_oper = FO_COPY;
			break;

			case DROPEFFECT_MOVE:
				file_oper = FO_MOVE;
			break;

			case DROPEFFECT_LINK:
				CreateShortcuts (ar,  m_LastDropFolder);
				return;
				
			default:
				TRACE(_T("Invalide file operation %d\n"), drop_effect);
				return ;
		}					
		
	FileOperation (this, ar, file_oper, FOF_ALLOWUNDO  , dest_folder) ;

	SetFocus ();	
}
 

void CMainFolder::onDelete ()
{
	HTREEITEM hItem = GetSelectedItem ();

	if (!hItem) return;
	if (hItem == m_hParent) return;

	CSelRowArray ar;

	CString path = GetPathFromItem (hItem);
	CString name = GetItemText (hItem);
	
	ar.push_back (CSelectRow (path, name, 0));

	FileOperation (this, ar, FO_DELETE , FOF_ALLOWUNDO, NULL);
	SetFocus ();	
}

void CMainFolder::onDeletePerm ()
{
	HTREEITEM hItem = GetSelectedItem ();

	if (!hItem) return;
	if (hItem == m_hParent) return;

	CSelRowArray ar;

	CString path = GetPathFromItem (hItem);
	CString name = GetItemText (hItem);

	ar.push_back (CSelectRow (path, name, 0));

	FileOperation (this, ar, FO_DELETE , 0, NULL);
	SetFocus ();	
}

/*CString CMainFolder::GetItemStr (HTREEITEM hItem)
{
	CString cs = GetItemText (hItem);
	int pos = cs.ReverseFind ('[');
	if (pos != -1)
		cs.Delete (pos, cs.GetLength () - pos);
	cs.TrimRight ();
	return cs;
}*/

void padRight (CString &cs, int count) 
{
	while (count>0)
	{
		cs+=' ';
		count--;
	}
}

UINT CMainFolder::AddDirectoryNodes (HTREEITEM hItem, CString& strPathName) 
{
    HANDLE hFind;
    WIN32_FIND_DATA fd;

    UINT nCount = 0;

	SureBackSlash (strPathName);
	CString strFileSpec = strPathName;
	std::vector<HTREEITEM> hitems;
	
    strFileSpec += "*.*";
	unsigned int max_len = 0;
	CString strFileName;

	if ((hFind = FindFirstFileEx (strFileSpec, FindExInfoStandard , &fd, FindExSearchLimitToDirectories, NULL, 0)) == INVALID_HANDLE_VALUE)
    //if ((hFind = ::FindFirstFile ((LPCTSTR) strFileSpec, &fd)) == INVALID_HANDLE_VALUE)
	{
        if (IsDriveNode (hItem))
            AddDummyNode (hItem);
        return 0;
    }

	COLORREF colorWnd = GetSysColor(COLOR_WINDOWTEXT);

    do {
		if (ifNotJunctionPoint(fd.dwFileAttributes)) {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{         
				if (!gOptions.m_bShowHidden)
					if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) == FILE_ATTRIBUTE_HIDDEN)
						continue;

				if (!IsDots (fd.cFileName) )
				{
					strFileName = (LPCTSTR) &fd.cFileName;
					int nIcon = CFileShellAttributes::GetFileIcon (strPathName + strFileName);
					
					max_len = max(max_len, _tcsclen(fd.cFileName));
					HTREEITEM hChild = InsertItem ((LPCTSTR) &fd.cFileName, nIcon, nIcon, hItem, TVI_SORT);
					if (fd.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
						SetItemColor(hChild, RGB(0,0, 255));								
					else
						SetItemColor(hChild, colorWnd);				

					
					CString strNewPathName = strPathName;
					if (strNewPathName.Right (1) != "\\")
						strNewPathName += "\\";
					hitems.push_back (hChild);
					strNewPathName += (LPCTSTR) &fd.cFileName;
					SetButtonState (hChild, strNewPathName);				
					nCount++;
				}
			}
        }
    } while (::FindNextFile (hFind, &fd));

	::FindClose (hFind);
/*
	for (int i = 0; i < hitems.size (); i++)
	{
		ULONGLONG size;
		ULONGLONG sizeondisk;
		CString name = GetItemText (hitems[i]);
		
		 bool hasSize = GetDirSizeLight (strPathName, name, size, sizeondisk);

		if (hasSize)
			{
				padRight (name, max_len - name.GetLength());
				strFileName.Format ("%s [ %s KB]", (LPCTSTR)name, filesize_to_str (size));
				SetItemText (hitems[i], strFileName);
				//hChild = InsertItem (strFileName, nIcon, nIcon, hItem, TVI_SORT);

			}				
				
	}
	
**/

    /*
	if (hasSize)
				{
					strFileName.Format ("%s [ %s KB]", fd.cFileName, filesize_to_str (size));
					hChild = InsertItem (strFileName, nIcon, nIcon, hItem, TVI_SORT);

				}				
				else*/

	//
    return nCount;
}


void CMainFolder::OnMoveto() 
{
	// TODO: Add your command handler code here
	
}

void CMainFolder::OnUpdateMoveto(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CMainFolder::OnCopyto() 
{
	// TODO: Add your command handler code here
	
}

void CMainFolder::OnUpdateCopyto(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}



LPARAM CMainFolder::OnFolderUpdate(WPARAM wp, LPARAM lp)
{
	CString path = (const TCHAR *) lp;	
	//TRACE(_T("MainFolderUpdate %s\n"), lp);
	path.MakeUpper ();	
	if (IsDirectory (path) && path.GetLength ()) 
		 m_arUpdateDrives[path[0] - 0x41] = true;
		//RefreshDrive (path[0] - 0x41);
	return NULL;
}

LPARAM CMainFolder::OnFileRenamed (WPARAM wp, LPARAM lp) 
{
	RENAMEFILESTRUCT *rnStruct = (RENAMEFILESTRUCT *)lp;

	CString old_path = rnStruct->_oldName;
	old_path.MakeUpper ();

	if (old_path.GetLength ())
		m_arUpdateDrives[old_path[0] - 0x41] = true;
		//RefreshDrive (old_path[0] - 0x41);

	CString new_path = rnStruct->_newName;
	new_path.MakeUpper ();

	if (new_path.GetLength ())
		m_arUpdateDrives[new_path[0] - 0x41] = true;
		//RefreshDrive (new_path[0] - 0x41);

	return NULL;
}

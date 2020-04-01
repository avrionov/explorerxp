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
#include "FavTree.h"
#include "GroupManager.h"
#include "ExplorerXPView.h"
#include "ExplorerXPDoc.h"
#include "RenameDlg.h"
#include "BrowseForFolder.h"

// CFavTree

CFavTree gFavs;

CFavTree::CFavTree()
{
	m_bInit = false;
	m_hLastDropItem = NULL;
	m_bDragDataAcceptable = false;
}

CFavTree::~CFavTree()
{
}

void CFavTree::Init ()
{
	if (m_bInit)
		return;
	m_bInit = true;
	m_Target.Register(this);
	SetImageList (GetSysImageList (), TVSIL_NORMAL );
}

void CFavTree::Reload ()
{
	Init ();
	GetOpen();
	DeleteAllItems();

	for (unsigned int i = 0; i < gGroupManager.size(); i++)
	{
		AddGroup(gGroupManager[i]);
	}
}

void CFavTree::AddGroup (CGroup &group)
{
	TCHAR path[2094];
	SHGetSpecialFolderPath (NULL, path, CSIDL_FAVORITES, FALSE);

	//SHGetFileInfo( path, 0, &ssfi, sizeof(SHFILEINFO),  SHGFI_SYSICONINDEX | SHGFI_SMALLICON); 
	int icon = GetFolderIconEx (path);
	
	HTREEITEM hItem = InsertItem(group.m_Name, icon, icon);

	for (unsigned int i = 0; i < group.m_Folders.size(); i++)
	{
		int image = GetFolderIconEx(group.m_Folders[i]);
		InsertItem(group.m_Folders[i], image, image, hItem);
	}

	if (group.m_bOpen)
		Expand (hItem, TVE_EXPAND);
}
void CFavTree::GetOpen ()
{
	HTREEITEM hNext = GetRootItem();

//	HTREEITEM hNext = GetNextItem(hRoot, TVGN_CHILD );
	while (hNext)
	{
		CString name = GetItemText(hNext);
		UINT state = GetItemState (hNext, TVIS_EXPANDED );
		bool bOpen = (state & TVIS_EXPANDED) == TVIS_EXPANDED;
		
		gGroupManager.SetOpen(name, bOpen);
		hNext = GetNextItem(hNext, TVGN_NEXT );
	}
}

BEGIN_MESSAGE_MAP(CFavTree, baseTree)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
END_MESSAGE_MAP()



// CFavTree message handlers


void CFavTree::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{   
	CPoint p(GetCurrentMessage()->pt);
	CPoint pt(p);

	ScreenToClient(&pt);

	HTREEITEM hItem = HitTest (pt);

	if (hItem == NULL)
		return;

	HTREEITEM parent = GetParentItem ( hItem);

	if (parent == NULL) // groups;
	{
		CMenu menu;
		menu.LoadMenu(IDR_GROUPS);

		CMenu *pPopup = menu.GetSubMenu (0);
		CString group = GetItemText(hItem);

		DWORD result = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, p.x, p.y, this);
		switch (result)
		{		
			case ID_NEWGROUP:
				AfxGetMainWnd ()->SendMessage (WM_COMMAND,ID_GROUPS_ADDANEWGROUP);
				break;

			case ID_DELETEGROUP:
				gGroupManager.DeleteGroup(group);
				Reload();
				break;
			
			case ID_RENAMEGROUP:
				{
					CRenameDlg dlg (group);
					dlg.HideExt();
					if (dlg.DoModal() == IDOK)
					{
						gGroupManager.RenameGroup(group, dlg.m_NewName);
						Reload();
					}
				} break;

			case ID_OPENGROUP:
				gGroupManager.OpenGroup(group);
				break;

			case ID_CLOSEALLOPENGROUP:
				gGroupManager.CloseAllAndOpenGroup(group);
				break;
			case ID_ADDFOLDERTOGROUP:
				{
					CString sz;
					CBrowseForFolder bf;	
					bf.strTitle = _T("Add folder to \"");
					bf.strTitle += group;
					bf.strTitle += "\"";
					//bf.strStartupDir = m_OutDirName;
					if (bf.GetFolder(sz))
					{
						gGroupManager.AddToGroup(group, sz);
						Reload();
					}
				}
				break;
		}
		return;
	}
		
	CString full_path = GetItemText (hItem);

	CMenu menu;
	menu.LoadMenu(IDR_GROUP_ITEM);

	CMenu *pPopup = menu.GetSubMenu (0);
	CString group = GetItemText(GetParentItem(hItem));

	DWORD result = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, p.x, p.y, this);
	switch (result)
	{
		case ID_OPENGROUPITEM:
			if (IsDirectory (full_path))
			{
				SureBackSlash (full_path);
				OpenFolder (full_path);
			}	
			break;
		case ID_EXPLOREGROUPITEM:
			if (IsDirectory (full_path))
			{
				::SetRoot (full_path);
				break;
			}	
			break;

		case ID_DELETEGROUPITEM:
			gGroupManager.DeleteFromGroup (group, full_path);
			Reload ();
			break;
	}
}

void CFavTree::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	*pResult = 0;

	if (!hItem)
		return;

	HTREEITEM hParent = GetParentItem(hItem);

	if (!hParent)
		return;

	CString txt = GetItemText(hItem);
	::SetRoot (txt);	
}


DROPEFFECT CFavTree::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{	
	m_FoldersOnly = false;
	UINT uFlags  = 0;

	DROPEFFECT dwEffect = DROPEFFECT_NONE;

	m_DropFiles.clear ();

	m_LastDropFolder.Empty ();

	m_hLastDropItem = HitTest (point, &uFlags);

	if (m_hLastDropItem )		
		SetItemBold (m_hLastDropItem, TRUE);
	else
		m_hLastDropItem = NULL;

	if (m_hLastDropItem)
		if (GetParentItem(m_hLastDropItem) == NULL)
		{
			dwEffect =  DROPEFFECT_LINK;
		}	
		else
		{
			m_LastDropFolder = GetItemText(m_hLastDropItem);
			SureBackSlash (m_LastDropFolder);
		}	

	if (ReadHDropData (pDataObject, m_DropFiles, m_DropFolder))
	{
		m_bDragDataAcceptable = true;	
		int count = 0;
		for (unsigned int i = 0; i < m_DropFiles.size(); i++)
			if (!IsDirectory(m_DropFiles[i].m_Path))
				break;
			else
				count++;

		if (count == static_cast<int>(m_DropFiles.size()))
			m_FoldersOnly = true;

		if ( (m_hLastDropItem != NULL) && (GetParentItem(m_hLastDropItem) != NULL))
			dwEffect = CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
	}

	return dwEffect;
}

DROPEFFECT CFavTree::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	UINT uFlags = 0;

	HTREEITEM hITem =  HitTest (point, &uFlags);
	
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
//			StartTimer (m_hLastDropItem);
		}
	}

	if (m_hLastDropItem)
		if (GetParentItem(m_hLastDropItem) == NULL)
		{
			if (m_FoldersOnly)
				return DROPEFFECT_LINK;
			else
				return DROPEFFECT_NONE;
		}	
		else
		{
			m_LastDropFolder = GetItemText(m_hLastDropItem);
			SureBackSlash (m_LastDropFolder);
		}	

	if ( (m_bDragDataAcceptable == true) && (m_hLastDropItem != NULL))	
		if (IsExist(m_LastDropFolder))
			dwEffect = CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
		else dwEffect = DROPEFFECT_NONE;

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

void CFavTree::OnDragLeave()
{
	m_LastDropFolder.Empty ();
	m_DropFiles.clear ();

	if (m_hLastDropItem)
		SetItemBold (m_hLastDropItem, FALSE);

	m_hLastDropItem = NULL;
}

BOOL CFavTree::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	if (GetParentItem (m_hLastDropItem) == NULL)
	{
		if (m_FoldersOnly)
		{
			CString groupName;
			groupName = GetItemText(m_hLastDropItem);
			for (unsigned int i = 0; i < m_DropFiles.size(); i++)
				if (IsDirectory(m_DropFiles[i].m_Path))
					gGroupManager.AddToGroup(groupName, m_DropFiles[i].m_Path);

			Reload ();
			return TRUE;
		}		
		return FALSE;
	}
	return FALSE;
}

HWND CFavTree::GetSafeHwnd ()	
{
	return m_hWnd;
}


void CFavTree::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE( _T("CFavTree::Onclick(%p)\n"), pNMHDR );

	*pResult = NULL;

	UINT uFlags = 0;

	//Work out the position of where the context menu should be
	CPoint p(GetCurrentMessage()->pt);
	CPoint pt(p);

	ScreenToClient(&pt);

	HTREEITEM hItem = HitTest (pt, &uFlags);

	if ((hItem == NULL) ||  ( (TVHT_ONITEM & uFlags) == 0))
		return;

	HTREEITEM hParent = GetParentItem(hItem);

	if (!hParent)
		return;

	CString txt = GetItemText(hItem);
	::SetRoot (txt);	
}
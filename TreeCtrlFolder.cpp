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
#include "TreeCtrlFolder.h"
#include "resource.h"
#include "AddressBar\ComboBoxFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlFolder

CTreeCtrlFolder::CTreeCtrlFolder()
{
	m_selectedItem = NULL;
	m_comboBoxFolder = FALSE;
}

CTreeCtrlFolder::~CTreeCtrlFolder()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlFolder, CDriveTree)
	//{{AFX_MSG_MAP(CTreeCtrlFolder)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)	
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlFolder message handlers

void CTreeCtrlFolder::OnMouseMove(UINT nFlags, CPoint point) 
{
	UINT flags = 0;
	HTREEITEM item = HitTest(point, &flags);
	if (item != NULL)
		CTreeCtrl::SelectItem(item);
		
	CDriveTree::OnMouseMove(nFlags, point);
}

void CTreeCtrlFolder::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	return;
/*
	HTREEITEM hti = pNMTreeView->itemNew.hItem;
	int imageIndex = 0;
	int imageIndexExpanded = 1;
	CFolder* folder = (CFolder*) GetItemData(hti);
	if (folder != NULL)
	{	imageIndex = folder->m_imageIndex;
		imageIndexExpanded = folder->m_imageIndexExpanded;
	}
	if (pNMTreeView->action == TVE_EXPAND)
		SetItemImage(hti, imageIndexExpanded, imageIndexExpanded);
	else
		SetItemImage(hti, imageIndex, imageIndex);
*/	
//	
}


void CTreeCtrlFolder::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	UINT flags = 0;
	HTREEITEM item = HitTest(point, &flags);
	if (flags!=TVHT_ONITEMBUTTON)
	{	m_selectedItem = item;
		if (item == NULL)
			m_selectedItem = CTreeCtrl::GetSelectedItem();
		HideAndInform();
		return;
	}

	CDriveTree::OnLButtonDown(nFlags, point);
}

void CTreeCtrlFolder::OnKillFocus(CWnd* pNewWnd) 
{
	CPoint point;
	GetCursorPos(&point);
	CWnd* wnd = WindowFromPoint(point);
	if (wnd->GetSafeHwnd() != m_comboBoxFolder->GetSafeHwnd())
		ShowWindow(SW_HIDE);

	CDriveTree::OnKillFocus(pNewWnd);
}

BOOL CTreeCtrlFolder::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE &&
		pMsg->hwnd == m_hWnd)
	{	ShowWindow(SW_HIDE);
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN &&
		pMsg->hwnd == m_hWnd)
	{	m_selectedItem = CTreeCtrl::GetSelectedItem();
		HideAndInform();
		return TRUE;
	}

	return CDriveTree::PreTranslateMessage(pMsg);
}

void CTreeCtrlFolder::Display(CRect rc)
{	
	SetWindowPos(&wndNoTopMost, rc.left, rc.top, rc.Width (), rc.Height (), SWP_SHOWWINDOW);

	CWnd* pTopParent = GetParent()->GetParentOwner();
    if (pTopParent != NULL)
	{  pTopParent->SendMessage( WM_NCACTIVATE, TRUE );
	   pTopParent->SetRedraw( TRUE );
	}
}

void CTreeCtrlFolder::OnDestroy() 
{
	CDriveTree::OnDestroy();
}



void CTreeCtrlFolder::HideAndInform()
{
	ShowWindow(SW_HIDE);
	Inform();

	// collapse the child when selected the parent?
	// Expand(m_selectedItem, TVE_COLLAPSE);
}

void CTreeCtrlFolder::Inform()
{
	CWnd* parent = GetParent();
	if (parent != NULL)
	{	/*CFolder* folder = (CFolder*) GetItemData(m_selectedItem);
		parent->SendMessage(WM_SELECTITEM_CHANGE, (WPARAM)folder);*/
		SetRoot (m_selectedItem);
		m_comboBoxFolder->SetFocus();
	}
}

HTREEITEM CTreeCtrlFolder::GetSelectedItem()
{	return m_selectedItem;
}

int CTreeCtrlFolder::SelectItem(HTREEITEM item)
{
	m_selectedItem = item;
	return CDriveTree::SelectItem(item);
}


HTREEITEM CTreeCtrlFolder::Search(HTREEITEM hItem, CString path)
{
	if(!hItem)
		return NULL;

	/*CFolder* folder = (CFolder*)GetItemData(hItem);
	if (folder!=NULL && folder->m_path.CollateNoCase(path)==0) 
		return hItem;
	
	HTREEITEM hRet = NULL;
	if (ItemHasChildren(hItem))
		hRet = Search(GetChildItem(hItem), path);

	if(hRet == NULL)
		hRet = Search(GetNextSiblingItem(hItem), path);

	return hRet;*/

	return NULL;
}

HTREEITEM CTreeCtrlFolder::SearchChildOneLevel(HTREEITEM item, CString path)
{
/*	CFolder* folder;
	while (item)
	{	
		folder = (CFolder*)GetItemData(item);
		if (folder!=NULL && folder->m_path.CollateNoCase(path)==0) 
			return item;

		item = GetNextSiblingItem(item);		
	}*/
	return NULL;
}

CString CTreeCtrlFolder::GetSelectedPath()
{
	CString path = "";
/*	CFolder* folder = (CFolder*) GetItemData(m_selectedItem);
	if (folder != NULL)
		path = folder->m_path;*/
	return path;
}

void CTreeCtrlFolder::Init(CComboBoxFolder* comboBoxFolder)
{
	m_comboBoxFolder = comboBoxFolder;
	Fill ();
}


void CTreeCtrlFolder::SelectNextItem(BOOL selectNext)
{
	HTREEITEM item = GetSelectedItem();
	if (selectNext)
		item = GetNextVisibleItem(item);
	else
		item = GetPrevVisibleItem(item);
	if (item != NULL)
	{	SelectItem(item);
		Inform();
	}
}


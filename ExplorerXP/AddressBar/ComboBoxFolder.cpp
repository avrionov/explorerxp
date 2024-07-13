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
#include "ComboBoxFolder.h"
#include "globals.h"
#include "SplitPath.h"

#include "debug_new.h"


CComboBoxFolder::CComboBoxFolder()
{
	m_iconWidth = 16;
	m_droppedHeight = 100;
	m_droppedWidth = 0;
	m_nIcon = gnMyComputer;
}

CComboBoxFolder::~CComboBoxFolder()
{
}


BEGIN_MESSAGE_MAP(CComboBoxFolder, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxFolder)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(ENAC_UPDATE,OnUpdateEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxFolder message handlers
/*
void CComboBoxFolder::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	CDC dc;

	if(!dc.Attach(pDIStruct -> hDC ))
		return;

	CRect rc(pDIStruct->rcItem);
	CRect rc2;
	GetClientRect(rc2);
	CPoint p;
	p.x = 4;
	p.y = (rc2.bottom - m_iconWidth)/2 ;

	//HTREEITEM item = m_treeCtrl.GetSelectedItem();

	//if (item != NULL)
	{
		GetSysImageList()->Draw(&dc, m_nIcon, p, ILD_NORMAL);
		rc.top += 1;
		rc.bottom -= 1;
		rc.left += m_iconWidth + 2;
		CSize size = dc.GetTextExtent(m_Folder);

		rc.right = rc.left + size.cx + 4;

		if(pDIStruct -> itemState & ODS_SELECTED)
		{	dc.FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT) );
			dc.DrawFocusRect(rc);
			dc.SetTextColor((0x00FFFFFF & ~(GetSysColor(COLOR_WINDOWTEXT))));
		}
		else
			dc.SetTextColor( GetSysColor(COLOR_WINDOWTEXT) );
		rc.left += 2;
		dc.DrawText(m_Folder, rc, DT_SINGLELINE | DT_VCENTER);
		
	}

	dc.Detach();
}


// No need the MeasureItem to do anything. Whatever the system says, it stays

void CComboBoxFolder::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
}
*/
void CComboBoxFolder::Init(int w, int h)
{
	CRect rc(0, 0, 10, 10);
	UINT style =  WS_POPUP | WS_BORDER | TVS_DISABLEDRAGDROP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_FULLROWSELECT;
	CWnd* pWnd = &m_treeCtrl;
	;
    pWnd->CreateEx (0, WC_TREEVIEW, NULL, style, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, GetParent()->GetSafeHwnd(), 0, NULL);
	
	//m_treeCtrl.CreateEx (0, style, rc, GetParent(), 0);
	m_treeCtrl.Init(this);
	
	GetClientRect(rc);
	if (w <= 0)
		w = rc.right;
	if (h <= 0)
		h = 100;
	SetDroppedWidth(w);
	SetDroppedHeight(h);
}

BOOL CComboBoxFolder::GetDroppedState( )
{	return m_treeCtrl.IsWindowVisible();
}

void CComboBoxFolder::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_treeCtrl.IsWindowVisible())
		m_treeCtrl.ShowWindow(SW_HIDE);
	else
	{
		CRect rc;
		GetClientRect (rc);
		int scrollWidth = ::GetSystemMetrics (SM_CXHSCROLL);
		if (point.x >  (rc.right - scrollWidth))
			DisplayTree ();
		else
			CComboBox::OnLButtonDown (nFlags, point);
	}
		

//	CComboBox::OnLButtonDown(nFlags, point);
}


void CComboBoxFolder::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);

//	CComboBox::OnLButtonDblClk(nFlags, point);
}

CString CComboBoxFolder::GetSelectedPath()
{	return m_treeCtrl.GetSelectedPath();
}

void CComboBoxFolder::DisplayTree()
{
	CRect rc;
	GetWindowRect(rc);
	rc.top = rc.bottom + 1;
	//rc.right = GetDroppedWidth();
	rc.bottom = rc.top + GetDroppedHeight();
	m_treeCtrl.Display(rc);
	m_treeCtrl.Find (m_Folder);
}

int CComboBoxFolder::GetDroppedHeight()
{	return m_droppedHeight;
}

void CComboBoxFolder::SetDroppedHeight(int height)
{	m_droppedHeight = height;
}

int CComboBoxFolder::GetDroppedWidth()
{	return m_droppedWidth;
}

void CComboBoxFolder::SetDroppedWidth(int width)
{	m_droppedWidth = width;
}

BOOL CComboBoxFolder::PreTranslateMessage(MSG* pMsg) 
{
	/*if (pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_DOWN || pMsg->wParam==VK_RIGHT))
	{	m_treeCtrl.SelectNextItem(TRUE);
		Invalidate();
		return TRUE;
	}
	else if (pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_UP || pMsg->wParam==VK_LEFT))
	{	m_treeCtrl.SelectNextItem(FALSE);
		Invalidate();
		return TRUE;
	}
	else if (pMsg->message==WM_SYSKEYDOWN && pMsg->wParam==VK_DOWN)
	{	DisplayTree();
		return TRUE;
	}*/

	return CComboBox::PreTranslateMessage(pMsg);
}


void CComboBoxFolder::SetFolder (const TCHAR *folder)
{
	if (GetFocus ()->GetSafeHwnd () == m_PathEdit.m_hWnd)
		return;

	if (folder == NULL || folder[0] == 0)
	{
		m_Folder = csMyComputer;
		m_nIcon = gnMyComputer;
	}
	else
	{
		m_Folder = folder;
		m_nIcon = GetFolderIconEx (folder);
	}

	m_PathEdit.SetWindowText (m_Folder);
}


//#define SHACF_FILESYSTEM 1


void CComboBoxFolder::PreSubclassWindow() 
{	
	m_PathEdit.SubclassDlgItem (1001, this);

	CRect rc;
	m_PathEdit.GetWindowRect (rc);

	ScreenToClient  (rc);
	
	rc.left += m_iconWidth + 8;

	rc.OffsetRect (0,2);
	/*
	m_PathEdit.MoveWindow (rc, false);

	m_PathEdit.ModifyStyle (0, ES_WANTRETURN);
	IAutoComplete *pac;
    CoCreateInstance(CLSID_AutoComplete, NULL, CLSCTX_INPROC_SERVER, IID_IAutoComplete, (LPVOID*)&pac);
	
	IUnknown *punkSource;
    CoCreateInstance(CLSID_ACListISF, NULL, CLSCTX_INPROC_SERVER, IID_IACList, (LPVOID*)&punkSource);

	IACList2 *pal2;
    if (SUCCEEDED(punkSource->QueryInterface(IID_IACList2, (LPVOID*)&pal2)))
    {
        pal2->SetOptions(ACLO_FILESYSONLY);
        pal2->Release();
    }

	IAutoComplete2 *pac2;
    if (SUCCEEDED(pac->QueryInterface(IID_IAutoComplete2, (LPVOID*)&pac2)))
    {
        pac2->SetOptions(ACO_AUTOSUGGEST | ACO_USETAB );
        pac2->Release();
    }


	
	pac->Init(m_PathEdit.m_hWnd, punkSource, NULL, NULL);


	pac->Release();
    punkSource->Release();
*/
					 
	m_PathEdit.SetMode (_MODE_FS_ONLY_DIR_);
//	SHAutoComplete (m_PathEdit.m_hWnd, SHACF_FILESYSTEM);
	
	CComboBox::PreSubclassWindow();
}


void CComboBoxFolder::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
		
/*	CComboBox::OnPaint(); 

	CRect rc2;
	
	CClientDC dc (this);
	
	GetClientRect (rc2);
	
	CPoint p;
	p.x = 4;
	p.y = (rc2.bottom - m_iconWidth)/2 ;
	
	GetSysImageList()->Draw(&dc, m_nIcon, p, ILD_NORMAL);*/

	CPaintDC dc(this);

	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );

	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();

	// First let the control do its default drawing.
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);

	CPoint p;
	p.x = 4;
	p.y = (rcClient.bottom - m_iconWidth)/2 ;

	GetSysImageList()->Draw(&memDC, m_nIcon, p, ILD_NORMAL);

	dc.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, rcClip.left, rcClip.top, SRCCOPY);

	memDC.DeleteDC();
}

void CComboBoxFolder::OnSize(UINT nType, int cx, int cy) 
{
	m_PathEdit.ShowWindow (SW_HIDE);

	CComboBox::OnSize (nType, cx, cy);
	CRect rc;
	m_PathEdit.GetWindowRect (rc);

	ScreenToClient  (rc);
	
	rc.left += m_iconWidth + 8;

	rc.OffsetRect (0,1);
	
	m_PathEdit.MoveWindow (rc, FALSE);
	
	m_PathEdit.ShowWindow (SW_SHOW);
}

BOOL CComboBoxFolder::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}


 LRESULT CComboBoxFolder::OnUpdateEdit(WPARAM wParam, LPARAM lParam)
{
	if(lParam == 1001)
	{
		CString text;
		
		m_PathEdit.GetWindowText (text);
		
		SureBackSlash (text);
		TCHAR buf[_SPLIT_MAX_PATH];
		if (GetLongPathName( text, buf, _SPLIT_MAX_PATH-1))
		{
			text = buf;		
			SureBackSlash (text);
			if (IsDirectory(text))
			{
				SetRoot (text);
				m_PathEdit.SetWindowText(text);
			}			
		}		

		TRACE(_T("AFTER EDIT %s\n"), (LPCTSTR)text );
	}
		
	return 0;
}
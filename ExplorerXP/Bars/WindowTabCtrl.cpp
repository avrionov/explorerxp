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
#include "resource.h"       // main symbols
#include "WindowTabCtrl.h"
#include "globals.h"
#include "ExplorerXPView.h"
#include "ExplorerXPDoc.h"
#include "SplitPath.h"
#include "FileShellAttributes.h"

#include "debug_new.h"

#define CX_BORDER  1
#define CY_BORDER  1

// TODO: Uncomment the ff. (__NO_RIGHTCLICK_SEL) if you do not wish to have the 
//       tab selected, when right-clicked.
#define  __NO_RIGHTCLICK_SEL


/////////////////////////////////////////////////////////////////////////////
// CWindowTabCtrl

CWindowTabCtrl::CWindowTabCtrl() : m_bDisplayIcons(TRUE)
{
	m_crSelColor    = RGB(  0,  0, 255);
	m_crUnselColor  = RGB( 50, 50,  50);
	m_crDocModified = RGB(255,  0,   0);
	
	// Get the current font
	LOGFONT lFont;
	memset(&lFont, 0, sizeof(LOGFONT));
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	lFont = ncm.lfMessageFont; 
	
	// Create the "Selected" font
	m_SelFont.CreateFontIndirect(&lFont);
	// Create the "UnSelected" font
	m_UnselFont.CreateFontIndirect(&lFont);
}

CWindowTabCtrl::~CWindowTabCtrl()
{
	m_SelFont.DeleteObject();
	m_UnselFont.DeleteObject();
}


BEGIN_MESSAGE_MAP(CWindowTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CWindowTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	//ON_NOTIFY_REFLECT(TCN_GETOBJECT, OnGetObject)	
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowTabCtrl message handlers

void CWindowTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (pNMHDR->hwndFrom == NULL)
		return;                    // just to avoid level 4 compilation warning

	int idx = GetCurSel();

	
	CString szLabel;
	TCITEM tci;	
	tci.mask = TCIF_PARAM ;
	GetItem(idx, &tci);

	CMDIChildWnd* pChild = reinterpret_cast<CMDIChildWnd*>(tci.lParam);

	CMDIFrameWnd *pMDIFrame =  static_cast<CMDIFrameWnd*>(AfxGetMainWnd());

	BOOL bMaximized;

	CMDIChildWnd *pActiveChild = pMDIFrame->MDIGetActive (&bMaximized);

	pChild->GetWindowText (szLabel);
	/*
	if (bMaximized)
	{
		DisableSync();
		if (pChild)
			while (pActiveChild != pChild)
			{
				pMDIFrame->MDINext ();
				pActiveChild = pMDIFrame->MDIGetActive (&bMaximized);
			}
			EnableSync();
			SyncUI (szLabel);
	}
   else
	if (pChild)
*/
		pMDIFrame->MDIActivate(pChild);	
	
	*pResult = 0;
}
/*
void CWindowTabCtrl::OnGetObject(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}*/

void CWindowTabCtrl::SetIconDisplay(BOOL bDisplayIt)
{
	m_bDisplayIcons = bDisplayIt;
	Invalidate();
}

int CWindowTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
	{
		TRACE(_T("OnCreate function call failed!"));
		return -1;
	}
	
	//ModifyStyle(0, TCS_OWNERDRAWFIXED | TCS_FIXEDWIDTH);
	
	HFONT hFont  = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	CFont* pFont = CFont::FromHandle(hFont);
	SetFont(pFont);
	
	m_DropTarget.Register (this);
	
	return 0;
}

int GetFolderIconEx (const TCHAR *folder)
{
	if (_tcsicmp (folder, CONST_MYCOMPUTER) == 0)
		return gnMyComputer;

	if (_tcsicmp (folder, CONST_RECYCLEBIN) == 0)
	{
		ITEMIDLIST*   pidl_mycomputer;	
		SHGetSpecialFolderLocation (NULL, CSIDL_BITBUCKET, &pidl_mycomputer);	
		SHFILEINFO     sfi = {};
		
		SHGetFileInfo((LPCTSTR)pidl_mycomputer, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME| SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY);	
		return sfi.iIcon;
	}
	return CFileShellAttributes::GetFileIcon (folder);
}

void CWindowTabCtrl::GetTabText (int index, CString &text)
{
	TCHAR szLabel[_SPLIT_MAX_PATH];
	TCITEM tci;

	text.Empty();

	tci.mask       = TCIF_TEXT ;
	tci.pszText    = szLabel;     
	tci.cchTextMax = _SPLIT_MAX_PATH - 1;    	
	if (!GetItem(index, &tci)) 
		return;

	text = szLabel;
}

#ifdef _XOXO
void CWindowTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(IsWindow(m_hWnd));

	CRect rcTab   =  CRect(&(lpDrawItemStruct->rcItem));
	int nTabIndex = lpDrawItemStruct->itemID;
	if (nTabIndex < 0 || nTabIndex >= GetItemCount())
		return;
	
	BOOL bSelected = (nTabIndex == GetCurSel());
	
	TCHAR szLabel[_SPLIT_MAX_PATH];
	TCITEM tci;
	tci.mask       = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
	tci.pszText    = szLabel;     
	tci.cchTextMax = _SPLIT_MAX_PATH - 1;    	
	if (!GetItem(nTabIndex, &tci)) 
		return;
	
	CDC dc;
	if (lpDrawItemStruct->hDC == NULL)
		return;

	if (!dc.Attach(lpDrawItemStruct->hDC)) 
		return;
	
	DWORD dwStyle = GetStyle();
    rcTab.NormalizeRect();
	if ((dwStyle & TCS_BOTTOM) == 0)
	{
		rcTab.DeflateRect(CX_BORDER, CY_BORDER);
		//rcTab.top += ::GetSystemMetrics(SM_CYEDGE);
	}
	
	dc.SetBkMode(TRANSPARENT);
	dc.FillSolidRect(rcTab, ::GetSysColor(COLOR_BTNFACE));
	
	CMDIChildWnd* pChild = reinterpret_cast<CMDIChildWnd*>(tci.lParam);
	if (pChild == NULL)
		return;
	// Get the active view attached to the active MDI child window.
	CView* pView = reinterpret_cast<CView*>(pChild->GetActiveView());
	if (pView == NULL)
		return;

	CDocument* pDoc = pView->GetDocument();
	if (pDoc == NULL)
		return;

	// Draw image
	if (m_bDisplayIcons)
	{
		int iImage = GetFolderIconEx (szLabel);
		
		CImageList*   pImageList = GetSysImageList();

		rcTab.left += 1;//TODO--dc.GetTextExtent(_T(" ")).cx;		// Set a little margin
		
		// Get the height of image 
		IMAGEINFO info;
		if (pImageList->GetImageInfo(iImage, &info))
		{
			CRect ImageRect(info.rcImage);
			int nYpos = rcTab.top + (rcTab.Height() - ImageRect.Height()) / 2;
		
			pImageList->Draw(&dc, iImage, CPoint(rcTab.left, nYpos), ILD_TRANSPARENT);
			rcTab.left += ImageRect.Width() + 2;
		}
	}

	if (bSelected) 
	{
		if (pDoc->IsModified())
			dc.SetTextColor(m_crDocModified);
		else
			dc.SetTextColor(m_crSelColor);
		dc.SelectObject(&m_SelFont);
		//rcTab.top -= ::GetSystemMetrics(SM_CYEDGE);
	} 
	else 
	{
		if (pDoc->IsModified())
			dc.SetTextColor(m_crDocModified);
		else
			dc.SetTextColor(m_crUnselColor);
		dc.SelectObject(&m_UnselFont);
	}
	
	int len = _tcslen (szLabel);

	if (len)
	{
		if (szLabel[len-1] == '\\')
			szLabel[len-1] = 0;
		CSplitPath path(szLabel);

		dc.DrawText(szLabel, rcTab, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_PATH_ELLIPSIS /*| DT_END_ELLIPSIS*/);
		//dc.DrawText(path.GetFName(), rcTab, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS);
	}
	
	dc.Detach();
}


/////////////////////////////////////////////////////////////////////////////
// CWindowTabCtrl operations

void CWindowTabCtrl::SetColors(COLORREF crSelColor, COLORREF crUnselColor, 
							   COLORREF crDocModified)
{
	if ((crSelColor != CLR_NONE) || (crSelColor != CLR_DEFAULT))
		m_crSelColor    = crSelColor;
	if ((crUnselColor != CLR_NONE) || (crUnselColor != CLR_DEFAULT))
		m_crUnselColor  = crUnselColor;
	if ((crDocModified != CLR_NONE) || (crDocModified != CLR_DEFAULT))
		m_crDocModified = crDocModified;

	Invalidate();
}

void CWindowTabCtrl::SetFonts(CFont* pSelFont, CFont* pUnselFont)
{
	ASSERT(pSelFont && pUnselFont);

	LOGFONT lFont;
	memset(&lFont, 0, sizeof(LOGFONT));
	int nSelHeight = 0, nUnselHeight = 0;

	m_SelFont.DeleteObject();
	m_UnselFont.DeleteObject();

	pSelFont->GetLogFont(&lFont);
	m_SelFont.CreateFontIndirect(&lFont);
	nSelHeight = lFont.lfHeight;

	pUnselFont->GetLogFont(&lFont);
	m_UnselFont.CreateFontIndirect(&lFont);
	nUnselHeight = lFont.lfHeight;

	SetFont((nSelHeight > nUnselHeight) ? &m_SelFont : &m_UnselFont);
}

void CWindowTabCtrl::SetFonts(CFont* pFont, BOOL bSelFont)
{
	ASSERT(pFont);

	LOGFONT lFont;
	memset(&lFont, 0, sizeof(LOGFONT));

	if (bSelFont)
	{
		m_SelFont.DeleteObject();
		pFont->GetLogFont(&lFont);
		m_SelFont.CreateFontIndirect(&lFont);
		SetFont(&m_SelFont);
	}
	else
	{
		m_UnselFont.DeleteObject();
		pFont->GetLogFont(&lFont);
		m_UnselFont.CreateFontIndirect(&lFont);
		SetFont(&m_UnselFont);
	}
}

void CWindowTabCtrl::SetFonts(int nSelWeight,   BOOL bSelItalic,   BOOL bSelUnderline,
						  int nUnselWeight, BOOL bUnselItalic, BOOL bUnselUnderline)
{
	// Free any memory currently used by the fonts.
	m_SelFont.DeleteObject();
	m_UnselFont.DeleteObject();

	// Get the current font
	LOGFONT lFont;
	memset(&lFont, 0, sizeof(LOGFONT));
	CFont* pFont = GetFont();
	if (pFont)
		pFont->GetLogFont(&lFont);
	else 
	{
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 
			sizeof(NONCLIENTMETRICS), &ncm, 0));
		lFont = ncm.lfMessageFont; 
	}

	// Create the "Selected" font
	lFont.lfWeight    = static_cast<LONG>(nSelWeight);
	lFont.lfItalic    = static_cast<BYTE>(bSelItalic);
	lFont.lfUnderline = static_cast<BYTE>(bSelUnderline);
	m_SelFont.CreateFontIndirect(&lFont);

	// Create the "Unselected" font
	lFont.lfWeight    = static_cast<LONG>(nUnselWeight);
	lFont.lfItalic    = static_cast<BYTE>(bUnselItalic);
	lFont.lfUnderline = static_cast<BYTE>(bUnselUnderline);
	m_UnselFont.CreateFontIndirect(&lFont);

	SetFont((nSelWeight > nUnselWeight) ? &m_SelFont : &m_UnselFont);
}

#endif

void CWindowTabCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
#ifndef __NO_RIGHTCLICK_SEL
	// Select the tab which is clicked and activate the attached view
    TCHITTESTINFO   hti;
    
    hti.pt = CPoint(GetMessagePos());
    ScreenToClient(&hti.pt);
    int iTab = HitTest(&hti);
    if (iTab < 0)
        return;
	SetCurSel(iTab);                    // Select it

	TCITEM tci;
	tci.mask = TCIF_PARAM;
	GetItem(iTab, &tci);

	CMDIChildWnd* pChild = reinterpret_cast<CMDIChildWnd*>(tci.lParam);
	static_cast<CMDIFrameWnd*>(AfxGetMainWnd())->MDIActivate(pChild);
#endif //__NO_RIGHTCLICK_SEL

	// Now, process the popup menu for the tab control
	CExplorerXPView *pView = NULL;
	TCHITTESTINFO hi;	
	hi.pt = point;
	int item = HitTest (&hi);

	if (hi.flags != TCHT_NOWHERE)		
		pView = GetView (item);

	POINT  ptScreen = point;	
	ClientToScreen(&ptScreen);

	CMenu menu;
	menu.LoadMenu (IDR_TABMENU);
	CMenu *pPopup = menu.GetSubMenu (0);

	CWnd *pWnd = AfxGetMainWnd ();

	DWORD result = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, ptScreen.x, ptScreen.y, pWnd);
	
	switch (result)
	{
		case ID_FILE_CLOSE:
			if (pView)
			{	
				CExplorerXPDoc *pDoc = pView->GetDocument();
				pDoc->OnCloseDocument();			
				return;
			}
		  break;		
		case ID_XX_CLOSEOTHERTABS:
			if (pView)
			{
				CExplorerXPDoc *pDoc = pView->GetDocument();				
				pDoc->CanClose (FALSE);
				pWnd->SendMessage (WM_COMMAND, ID_WINDOWS_CLOSE_ALL);		
				pDoc->CanClose (TRUE);
			}
			break;
		default:
			AfxGetMainWnd()->SendMessage(WM_COMMAND, result);
			break;
	}	
	
	CTabCtrl::OnRButtonDown(nFlags, point);
}

void CWindowTabCtrl::SetTabStyle(BOOL bButtons, BOOL bFlatButtons)
{
	DWORD dwAdd          = 0;
	DWORD dwCurrentStyle = GetStyle();

	if (!bButtons && (dwCurrentStyle & TCS_BUTTONS))
	{
		ModifyStyle(TCS_BUTTONS | TCS_FLATBUTTONS, 0, 0);
		return;
	}
	else if (!bButtons)
		return;
		
	if (bButtons && (dwCurrentStyle & TCS_BUTTONS) && 
		(dwCurrentStyle & TCS_FLATBUTTONS))
		return;

	if (bButtons)
		dwAdd |= TCS_BUTTONS;
	if (bFlatButtons)
		dwAdd |= TCS_FLATBUTTONS;
	ModifyStyle(0, dwAdd, 0);
}


DROPEFFECT CWindowTabCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

	TCHITTESTINFO hi;
	
	hi.pt = point;
	int item = HitTest (&hi);

	if (hi.flags == TCHT_NOWHERE)
		return dwEffect;

	CExplorerXPView *pView = GetView (item);
	if (!pView)
		return dwEffect;
	
		
	m_DropFiles.clear ();
	m_DropFolder.Empty ();

	m_LastDropFolder = pView->GetViewer()->GetTitle ();

	if (ReadHDropData (pDataObject, m_DropFiles, m_DropFolder))
	{
			dwEffect = pView->GetViewer()->CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
	}

    return dwEffect;
}

DROPEFFECT CWindowTabCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TCHITTESTINFO hi;
	
	hi.pt = point;
	int item = HitTest (&hi);

	if (hi.flags == TCHT_NOWHERE)
		return DROPEFFECT_NONE;
	
	CExplorerXPView *pView = GetView (item);
	if (!pView)
		DROPEFFECT_NONE;

	m_LastDropFolder = pView->GetViewer()->GetTitle ();

	return pView->GetViewer()->CalcDropEffect (m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);;
	
}

void CWindowTabCtrl::OnDragLeave()
{
	m_DropFolder.Empty ();
}

BOOL CWindowTabCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	TCHITTESTINFO hi;
	
	hi.pt = point;
	int item = HitTest (&hi);

	if (hi.flags == TCHT_NOWHERE)
		return DROPEFFECT_NONE;
	
	CExplorerXPView *pView = GetView (item);
	if (!pView)
		return DROPEFFECT_NONE;

	m_LastDropFolder = pView->GetViewer()->GetTitle ();

	pView->GetViewer()->OnDrop (pDataObject, dropEffect, m_LastDropFolder);

	return TRUE;
}

CExplorerXPView *CWindowTabCtrl::GetView(int iID)
{
	TCITEM tci;
    tci.mask = TCIF_PARAM;
    tci.pszText    = NULL;
    tci.cchTextMax = NULL;
    GetItem(iID, &tci);

    CMDIChildWnd* pView = reinterpret_cast<CMDIChildWnd*>(tci.lParam);
    ASSERT(pView != NULL);
    ASSERT_KINDOF(CMDIChildWnd, pView);
    
	return (CExplorerXPView*)pView->GetActiveView ();
}

CMDIChildWnd *CWindowTabCtrl::GetMDIChild(int iID)
{
	TCITEM tci;
	tci.mask = TCIF_PARAM;
	tci.pszText    = NULL;
	tci.cchTextMax = NULL;
	GetItem(iID, &tci);

	CMDIChildWnd* pView = reinterpret_cast<CMDIChildWnd*>(tci.lParam);
	ASSERT(pView != NULL);
	ASSERT_KINDOF(CMDIChildWnd, pView);

	return pView;
}

void CWindowTabCtrl::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{

}
/*
BOOL CWindowTabCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_MBUTTONDOWN)
	{
		TCHITTESTINFO hi;
		hi.pt = pMsg->pt;
		ScreenToClient(&hi.pt);
		int item = HitTest (&hi);
			
		CExplorerXPView *pView = NULL;

		if (hi.flags != TCHT_NOWHERE)		
			pView = GetView (item);
		
		if (pView)
		{
			CExplorerXPDoc *pDoc = pView->GetDocument();
			pDoc->OnCloseDocument();						
		}

	}
	return CTabCtrl ::PreTranslateMessage(pMsg);
}*/
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
#include "resource.h"
#include "docselect.h"
#include "menubutton.h"
#include "Globals.h"
#include "Themes.h"
#include "SplitPath.h"
#include "Options.h"
#include "shlwapi.h"
#include "ExplorerXPDoc.h"

#include "debug_new.h"

#define ID_TABHEIGHT_OLD 44
//#define DS_HEIGHT				ID_TABHEIGHT+4		// Default
//is 22.
//#define DS_WIDTH				110		// Default is
//110.
#define DS_LEFT_MARGIN 2 // Default is 2.
//#define DS_TIMER_ID				1		// Default is 1.
#define ID_VIEWTAB 1005
#define ID_CLOSEBTN 1006

CDocSelector::CDocSelector() {
  m_iNextButtonStart = DS_LEFT_MARGIN;
  m_iSelectedButton = 0;
  
  m_TabHeight = GetSystemMetrics(SM_CYMENU) + 4;

  m_nLMargin = m_TabHeight - 4;
}

CDocSelector::~CDocSelector() {}

BEGIN_MESSAGE_MAP(CDocSelector, CControlBar)
//{{AFX_MSG_MAP(CDocSelector)
ON_WM_SIZE()
ON_WM_CREATE()
ON_WM_ERASEBKGND()
ON_WM_PAINT()
ON_WM_TIMER()
ON_WM_DESTROY()
ON_WM_LBUTTONDBLCLK()
//}}AFX_MSG_MAP
//	ON_MESSAGE(ID_DS_POPUP_BUTTON, OnDSPopup)
//  ON_NOTIFY(TTN_GETDISPINFO, ID_VIEWTAB, OnToolTip)
ON_NOTIFY(TTN_NEEDTEXT, 0, OnToolTip)
ON_COMMAND(ID_CLOSEBTN, OnCloseTab)
END_MESSAGE_MAP()

void CDocSelector::OnDestroy() {
  RemoveAll();
  CControlBar::OnDestroy();
}

void CDocSelector::RemoveAll() {
  m_iNextButtonStart = 0;
  m_iSelectedButton = -1;
}

void CDocSelector::OnSize(UINT nType, int cx, int cy) {
  CControlBar::OnSize(nType, cx, cy);

  if (!m_ViewTabCtrl.GetSafeHwnd())
    return;

  CRect rect;

  DWORD dwStyle = m_ViewTabCtrl.GetStyle();
  GetClientRect(rect);

  m_HoverClose.SetWindowPos(&wndTop, rect.Width() - m_TabHeight + 2,
                            //rect.top + 2, 0, 0, SWP_NOSIZE);
                            rect.top + 2, m_nLMargin, m_nLMargin, 0);

  if (dwStyle & TCS_BOTTOM)
    m_ViewTabCtrl.SetWindowPos(&wndTop, 2, rect.top,
                               rect.Width() - m_TabHeight -4 , m_TabHeight,
                               SWP_DEFERERASE);
  else
    m_ViewTabCtrl.SetWindowPos(&wndTop, 2, 0, rect.Width() - m_TabHeight - 4,
        m_TabHeight, SWP_DEFERERASE);
}

int CDocSelector::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CControlBar::OnCreate(lpCreateStruct) == -1)
    return -1;

  //    m_ViewTabImages.Create(16, 16, ILC_MASK, 5, 5);
  DWORD dwStyle = TCS_TOOLTIPS | TCS_SINGLELINE | TCS_FOCUSNEVER | 
                  TCS_FORCELABELLEFT /*| TCS_MULTILINE*/;

  if (!ThemeLibLoaded)
    dwStyle |= TCS_BUTTONS | TCS_FLATBUTTONS | TCS_HOTTRACK;
  else
    dwStyle |= TCS_TABS;

  m_ViewTabCtrl.Create(WS_CHILD | WS_VISIBLE | dwStyle, CRect(0, 0, 0, 0), this,
                       ID_VIEWTAB);

  CToolTipCtrl *pToolTip = m_ViewTabCtrl.GetToolTips();

  if (pToolTip)
    pToolTip->ModifyStyle(0, TTS_NOPREFIX);

  m_HoverClose.Create(EMPTYSTR,
                      WS_VISIBLE | WS_CHILD | BS_OWNERDRAW | BS_PUSHBUTTON,
                      CRect(0, 0, 0, 0), this, ID_CLOSEBTN);
  m_HoverClose.LoadBitmap(IDB_HOOVERBMP);
  m_HoverClose.SetToolTipText(IDS_CLOSETAB);
  m_HoverClose.EnableWindow(TRUE);

  TabCtrl_SetExtendedStyle(m_ViewTabCtrl.m_hWnd,
                           TCS_EX_FLATSEPARATORS | TCS_EX_REGISTERDROP);

  //TabCtrl_SetPadding(m_ViewTabCtrl.m_hWnd, 6, 2);
    
  ModifyStyle(0, WS_CLIPCHILDREN);

  // m_ViewTabCtrl.SetTabStyle (TRUE, TRUE);

  return 0;
}

CSize CDocSelector::CalcFixedLayout(BOOL bStretch, BOOL bHorz) {
  // Get border size (values will be negative)
  CRect rBorder;
  rBorder.SetRectEmpty();
  CalcInsideRect(rBorder, bHorz);

  // The Y value is the sum of the calculated height from DrawText,
  // plus the top and bottom border.
  CSize size;
  size.cx = 32767;
  size.cy = m_TabHeight;
  size.cy += (-rBorder.Height());

  return size;
}

void CDocSelector::OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler) {
  CMDIFrameWnd *pFrame = static_cast<CMDIFrameWnd *>(AfxGetApp()->m_pMainWnd);

  if (pFrame == NULL)
    return;

  CMDIChildWnd *pChild = static_cast<CMDIChildWnd *>(pFrame->GetActiveFrame());
  CView *pActiveView = reinterpret_cast<CView *>(pChild->GetActiveView());

  if (pActiveView == NULL) {
    m_ViewTabCtrl.ShowWindow(SW_HIDE);
    return;
  } else {
    if (!m_ViewTabCtrl.IsWindowVisible())
      m_ViewTabCtrl.ShowWindow(SW_SHOW);
  }

  int iSel = -1;
  if (pChild->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))) {
    CString strWin;
    for (int nTabs = 0; nTabs < m_ViewTabCtrl.GetItemCount(); ++nTabs) {
      CString szLabel;

      CMDIChildWnd *pViewAt = GetTabInfo(nTabs, szLabel);
      ASSERT_KINDOF(CMDIChildWnd, pViewAt);

      pViewAt->GetWindowText(strWin);

      CString tabText;
      m_ViewTabCtrl.GetTabText(nTabs, tabText);

      CString newTabText = MakeTabName(strWin);

      if (newTabText != tabText)
        SetViewName(strWin, pViewAt);

      if (pViewAt == pChild)
        iSel = nTabs;
    }
  }

  m_ViewTabCtrl.SetCurSel(iSel);

  //	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

BOOL CDocSelector::OnEraseBkgnd(CDC *pDC) { return TRUE; }

void CDocSelector::OnPaint() {
  CPaintDC dc(this); // device context for painting
  CRect r;
  GetClientRect(r);
  // Draw Seperator line from others controls.
  dc.FillSolidRect(r, ::GetSysColor(COLOR_3DFACE));
  // dc.Draw3dRect( r.left, r.top, r.right, r.top + 2, GetSysColor(
  // COLOR_3DSHADOW ), GetSysColor( COLOR_3DHILIGHT ) );
}

CString CDocSelector::MakeTabName(const TCHAR *name) {
  CString cs;
  TCHAR path[_SPLIT_MAX_PATH];
  wcsncpy(path, name, _SPLIT_MAX_PATH);

  //	'path' is Output from 'wcsncpy', which may not zero-terminate it
  //223
  path[_SPLIT_MAX_PATH - 1] = 0;

  if (gOptions.m_bStripPath)
    PathStripPath(path);

  PathRemoveBackslash(path);

  cs = path;

  if (gOptions.m_bLimitChars)
    if (cs.GetLength() > static_cast<int>(gOptions.m_CharLimit)) {
      cs = cs.Right(gOptions.m_CharLimit);
      cs.Insert(0, _T("..."));
    }
  cs.Replace(_T("&"), _T("&&"));
  return cs;
}

CMDIChildWnd *CDocSelector::GetTabInfo(int nTab, CString &szLabel) {
  CMDIChildWnd *pView = m_ViewTabCtrl.GetMDIChild(nTab);
  if (pView)
    pView->GetWindowText(szLabel);
  return pView;
}

int GetFolderIconEx(const TCHAR *folder);

void CDocSelector::AddView(const TCHAR *csName, CMDIChildWnd *pView) {
  if (!m_ViewTabCtrl.GetSafeHwnd())
    return;

  CString cs = MakeTabName(csName);

  if (m_ViewTabCtrl.GetSafeHwnd()) {
    TCITEM tci;
    tci.mask = TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
    tci.pszText = cs.LockBuffer();
    tci.lParam = reinterpret_cast<LPARAM>(pView);
    tci.iImage = GetFolderIconEx(csName);
    m_ViewTabCtrl.InsertItem(m_ViewTabCtrl.GetItemCount(), &tci);
    cs.UnlockBuffer();
  }
}

void CDocSelector::RemoveView(CMDIChildWnd *pView) {
  ASSERT(pView->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

  if (!m_ViewTabCtrl.GetSafeHwnd())
    return;

  for (int nTabs = 0; nTabs < m_ViewTabCtrl.GetItemCount(); nTabs++) {
    TCITEM tci;
    tci.mask = TCIF_PARAM;
    m_ViewTabCtrl.GetItem(nTabs, &tci);
    if (tci.lParam == reinterpret_cast<LPARAM>(pView)) {
      m_ViewTabCtrl.DeleteItem(nTabs);
      break;
    }
  }
}

void CDocSelector::UpdateTabs() {
  for (int nTabs = 0; nTabs < m_ViewTabCtrl.GetItemCount(); nTabs++) {
    CString szLabel;
    GetTabInfo(nTabs, szLabel);
    CString csName = MakeTabName(szLabel);
    TCITEM tci;
    tci.mask = TCIF_TEXT;
    tci.pszText = csName.LockBuffer();
    m_ViewTabCtrl.SetItem(nTabs, &tci);
    csName.UnlockBuffer();
  }
  m_ViewTabCtrl.Invalidate();
}

void CDocSelector::SetViewName(const TCHAR *cs, CMDIChildWnd *pView) {
  ASSERT(pView->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

  if (!m_ViewTabCtrl.GetSafeHwnd())
    return;

  CString csName = MakeTabName(cs);
  for (int nTabs = 0; nTabs < m_ViewTabCtrl.GetItemCount(); nTabs++) {
    TCITEM tci;
    tci.mask = TCIF_PARAM;
    m_ViewTabCtrl.GetItem(nTabs, &tci);
    if (tci.lParam == reinterpret_cast<LPARAM>(pView)) {
      tci.mask = TCIF_TEXT;
      tci.pszText = csName.LockBuffer();
      m_ViewTabCtrl.SetItem(nTabs, &tci);
      csName.UnlockBuffer();
      m_ViewTabCtrl.Invalidate();
      break;
    }
  }
}

int CDocSelector::GetWindowNum() { return m_ViewTabCtrl.GetItemCount(); }

bool CDocSelector::SelectDocument(const TCHAR *document_name) {
  CString strWin;

  if (m_ViewTabCtrl.m_hWnd == NULL)
    return false;

  for (int nTabs = 0; nTabs < m_ViewTabCtrl.GetItemCount(); ++nTabs) {
    CString szLabel;

    CMDIChildWnd *pViewAt = GetTabInfo(nTabs, szLabel);
    ASSERT_KINDOF(CMDIChildWnd, pViewAt);

    if (_tcsicmp(szLabel, document_name) == 0) {
    //  BOOL bMaximized;
      CMDIFrameWnd *pMDIFrame = static_cast<CMDIFrameWnd *>(AfxGetMainWnd());
      //CMDIChildWnd *pActiveChild = pMDIFrame->MDIGetActive(&bMaximized);

      /*if (bMaximized) {
        DisableSync();
        if (pViewAt)
          while (pActiveChild != pViewAt) {
            pMDIFrame->MDINext();
            pActiveChild = pMDIFrame->MDIGetActive(&bMaximized);
          }
        EnableSync();
        SyncUI(document_name);
      } else */
          if (pViewAt)
                pMDIFrame->MDIActivate(pViewAt);
      return true;
    }
  }
  return false;
}

void CDocSelector::GetTabs(CStringAr &tabs) {
  tabs.clear();

  CString strWin;
  if (m_ViewTabCtrl.m_hWnd == NULL)
    return;

  for (int nTabs = 0; nTabs < m_ViewTabCtrl.GetItemCount(); ++nTabs) {
    CString szLabel;
    GetTabInfo(nTabs, szLabel);
    tabs.push_back(szLabel);
  }
}

void CDocSelector::reDraw() { m_ViewTabCtrl.Invalidate(); }

void CDocSelector::OnToolTip(NMHDR *pNMHDR, LRESULT *pResult) {
  *pResult = 0;
  TCHITTESTINFO hti;
  TOOLTIPTEXT *pTTT = reinterpret_cast<TOOLTIPTEXT *>(pNMHDR);
  //	UINT nID = pNMHDR->idFrom;

  hti.pt = CPoint(GetMessagePos());
  m_ViewTabCtrl.ScreenToClient(&hti.pt);
  int nTab = m_ViewTabCtrl.HitTest(&hti);

  if (nTab >= 0) {
    CString szLabel;
    GetTabInfo(nTab, szLabel);
    static TCHAR pszText[_SPLIT_MAX_PATH];
    wcsncpy(pszText, (LPCTSTR)szLabel, _SPLIT_MAX_PATH - 1);
    pTTT->lpszText = pszText;
  }
}

void CDocSelector::OnLButtonDblClk(UINT nFlags, CPoint pt) {
  // OpenFolder(NULL);
  AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CDocSelector::OnCloseTab() {
  int iTab = m_ViewTabCtrl.GetCurSel();

  if (iTab == -1)
    return;

  CExplorerXPView *pView = m_ViewTabCtrl.GetView(iTab);

  if (pView) {
    CExplorerXPDoc *pDoc = pView->GetDocument();
    pDoc->OnCloseDocument();
    return;
  }
}
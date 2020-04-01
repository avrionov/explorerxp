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
#include "explorerxp.h"
#include "MainBarDropTarget.h"
#include "MainToolBar.h"

#include "debug_new.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainBarDropTarget::CMainBarDropTarget() {
  m_pToolbar = NULL;
  m_bRegistered = FALSE;
}

CMainBarDropTarget::~CMainBarDropTarget() {}

// Overloaded Register() function performs the normal COleDropTarget::Register
// but also serves to connect this COleDropTarget with the parent grid control,
// where all drop messages will ultimately be forwarded.
BOOL CMainBarDropTarget::Register(CMainToolBar *pBar) {
  if (m_bRegistered)
    return FALSE;

  // Stop re-entry problems
  static BOOL bInProcedure = FALSE;
  if (bInProcedure)
    return FALSE;
  bInProcedure = TRUE;

  //    ASSERT(pTree->IsKindOf(RUNTIME_CLASS(CMainFolder)));
  //  ASSERT(pTree);

  if (!pBar /*|| !pTree->IsKindOf(RUNTIME_CLASS(CMainFolder))*/) {
    bInProcedure = FALSE;
    return FALSE;
  }

  m_pToolbar = pBar;

  m_bRegistered = COleDropTarget::Register(m_pToolbar);

  bInProcedure = FALSE;
  return m_bRegistered;
}

void CMainBarDropTarget::Revoke() {
  m_bRegistered = FALSE;
  COleDropTarget::Revoke();
}

BEGIN_MESSAGE_MAP(CMainBarDropTarget, COleDropTarget)
//{{AFX_MSG_MAP(CMainTreeDropTarget)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainTreeDropTarget message handlers

DROPEFFECT CMainBarDropTarget::OnDragScroll(CWnd *pWnd, DWORD dwKeyState,
                                            CPoint /*point*/) {
  //    TRACE("In CMainTreeDropTarget::OnDragScroll\n");
  if (pWnd->GetSafeHwnd() == m_pToolbar->GetSafeHwnd()) {
    if (dwKeyState & MK_CONTROL)
      return DROPEFFECT_COPY;
    else
      return DROPEFFECT_MOVE;
  } else
    return DROPEFFECT_NONE;
}

DROPEFFECT CMainBarDropTarget::OnDragEnter(CWnd *pWnd,
                                           COleDataObject *pDataObject,
                                           DWORD dwKeyState, CPoint point) {
  TRACE(_T("In CMainBarDropTarget::OnDragEnter\n"));
  ASSERT(m_pToolbar);

  if (pWnd->GetSafeHwnd() == m_pToolbar->GetSafeHwnd())
    return m_pToolbar->OnDragEnter(pDataObject, dwKeyState, point);
  else
    return DROPEFFECT_NONE;
}

void CMainBarDropTarget::OnDragLeave(CWnd *pWnd) {
  TRACE(_T("In CMainBarDropTarget::OnDragLeave\n"));
  ASSERT(m_pToolbar);

  if (pWnd->GetSafeHwnd() == m_pToolbar->GetSafeHwnd())
    m_pToolbar->OnDragLeave();
}

DROPEFFECT CMainBarDropTarget::OnDragOver(CWnd *pWnd,
                                          COleDataObject *pDataObject,
                                          DWORD dwKeyState, CPoint point) {
  //    TRACE("In CMainTreeDropTarget::OnDragOver\n");
  ASSERT(m_pToolbar);

  if (pWnd->GetSafeHwnd() == m_pToolbar->GetSafeHwnd())
    return m_pToolbar->OnDragOver(pDataObject, dwKeyState, point);
  else
    return DROPEFFECT_NONE;
}

BOOL CMainBarDropTarget::OnDrop(CWnd *pWnd, COleDataObject *pDataObject,
                                DROPEFFECT dropEffect, CPoint point) {
  TRACE(_T("In CMainBarDropTarget::OnDrop\n"));
  ASSERT(m_pToolbar);

  if (pWnd->GetSafeHwnd() == m_pToolbar->GetSafeHwnd())
    return m_pToolbar->OnDrop(pDataObject, dropEffect, point);
  else
    return FALSE;
}

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
#include "explorerxp.h"
#include "TabDropTarget.h"
#include "WindowTabCtrl.h"

#include "debug_new.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTabDropTarget::CTabDropTarget() {
  m_pTab = NULL;
  m_bRegistered = FALSE;
}

CTabDropTarget::~CTabDropTarget() {}

// Overloaded Register() function performs the normal COleDropTarget::Register
// but also serves to connect this COleDropTarget with the parent grid control,
// where all drop messages will ultimately be forwarded.
BOOL CTabDropTarget::Register(CWindowTabCtrl *pTab) {
  if (m_bRegistered)
    return FALSE;

  // Stop re-entry problems
  static BOOL bInProcedure = FALSE;
  if (bInProcedure)
    return FALSE;
  bInProcedure = TRUE;

  //    ASSERT(pTree->IsKindOf(RUNTIME_CLASS(CMainFolder)));
  //  ASSERT(pTree);

  if (!pTab /*|| !pTree->IsKindOf(RUNTIME_CLASS(CMainFolder))*/) {
    bInProcedure = FALSE;
    return FALSE;
  }

  m_pTab = pTab;

  m_bRegistered = COleDropTarget::Register(m_pTab);

  bInProcedure = FALSE;
  return m_bRegistered;
}

void CTabDropTarget::Revoke() {
  m_bRegistered = FALSE;
  COleDropTarget::Revoke();
}

BEGIN_MESSAGE_MAP(CTabDropTarget, COleDropTarget)
//{{AFX_MSG_MAP(CTabDropTarget)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabDropTarget message handlers

DROPEFFECT CTabDropTarget::OnDragScroll(CWnd *pWnd, DWORD dwKeyState,
                                        CPoint /*point*/) {
  //    TRACE("In CTabDropTarget::OnDragScroll\n");
  if (pWnd->GetSafeHwnd() == m_pTab->GetSafeHwnd()) {
    if (dwKeyState & MK_CONTROL)
      return DROPEFFECT_COPY;
    else
      return DROPEFFECT_MOVE;
  } else
    return DROPEFFECT_NONE;
}

DROPEFFECT CTabDropTarget::OnDragEnter(CWnd *pWnd, COleDataObject *pDataObject,
                                       DWORD dwKeyState, CPoint point) {
  TRACE(_T("In CTabDropTarget::OnDragEnter\n"));
  ASSERT(m_pTab);

  if (pWnd->GetSafeHwnd() == m_pTab->GetSafeHwnd())
    return m_pTab->OnDragEnter(pDataObject, dwKeyState, point);
  else
    return DROPEFFECT_NONE;
}

void CTabDropTarget::OnDragLeave(CWnd *pWnd) {
  TRACE(_T("In CTabDropTarget::OnDragLeave\n"));
  ASSERT(m_pTab);

  if (pWnd->GetSafeHwnd() == m_pTab->GetSafeHwnd())
    m_pTab->OnDragLeave();
}

DROPEFFECT CTabDropTarget::OnDragOver(CWnd *pWnd, COleDataObject *pDataObject,
                                      DWORD dwKeyState, CPoint point) {
  //    TRACE("In CTabDropTarget::OnDragOver\n");
  ASSERT(m_pTab);

  if (pWnd->GetSafeHwnd() == m_pTab->GetSafeHwnd())
    return m_pTab->OnDragOver(pDataObject, dwKeyState, point);
  else
    return DROPEFFECT_NONE;
}

BOOL CTabDropTarget::OnDrop(CWnd *pWnd, COleDataObject *pDataObject,
                            DROPEFFECT dropEffect, CPoint point) {
  TRACE(_T("In CTabDropTarget::OnDrop\n"));
  ASSERT(m_pTab);

  if (pWnd->GetSafeHwnd() == m_pTab->GetSafeHwnd())
    return m_pTab->OnDrop(pDataObject, dropEffect, point);
  else
    return FALSE;
}

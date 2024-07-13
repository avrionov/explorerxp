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
#include "MainTreeDropTarget.h"
#include "MainFolder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainTreeDropTarget::CMainTreeDropTarget() {
  m_pTree = NULL;
  m_bRegistered = FALSE;
}

CMainTreeDropTarget::~CMainTreeDropTarget() {}

// Overloaded Register() function performs the normal COleDropTarget::Register
// but also serves to connect this COleDropTarget with the parent grid control,
// where all drop messages will ultimately be forwarded.
BOOL CMainTreeDropTarget::Register(CMainFolder *pTree) {
  if (m_bRegistered)
    return FALSE;

  // Stop re-entry problems
  static BOOL bInProcedure = FALSE;
  if (bInProcedure)
    return FALSE;
  bInProcedure = TRUE;

  //    ASSERT(pTree->IsKindOf(RUNTIME_CLASS(CMainFolder)));
  //  ASSERT(pTree);

  if (!pTree /*|| !pTree->IsKindOf(RUNTIME_CLASS(CMainFolder))*/) {
    bInProcedure = FALSE;
    return FALSE;
  }

  m_pTree = pTree;

  m_bRegistered = COleDropTarget::Register(m_pTree);

  bInProcedure = FALSE;
  return m_bRegistered;
}

void CMainTreeDropTarget::Revoke() {
  m_bRegistered = FALSE;
  COleDropTarget::Revoke();
}

BEGIN_MESSAGE_MAP(CMainTreeDropTarget, COleDropTarget)
//{{AFX_MSG_MAP(CMainTreeDropTarget)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainTreeDropTarget message handlers

DROPEFFECT CMainTreeDropTarget::OnDragScroll(CWnd *pWnd, DWORD dwKeyState,
                                             CPoint /*point*/) {
  //    TRACE("In CMainTreeDropTarget::OnDragScroll\n");
  if (pWnd->GetSafeHwnd() == m_pTree->GetSafeHwnd()) {
    if (dwKeyState & MK_CONTROL)
      return DROPEFFECT_COPY;
    else
      return DROPEFFECT_MOVE;
  } else
    return DROPEFFECT_NONE;
}

DROPEFFECT CMainTreeDropTarget::OnDragEnter(CWnd *pWnd,
                                            COleDataObject *pDataObject,
                                            DWORD dwKeyState, CPoint point) {
  TRACE(_T("In CMainTreeDropTarget::OnDragEnter\n"));
  ASSERT(m_pTree);

  if (pWnd->GetSafeHwnd() == m_pTree->GetSafeHwnd())
    return m_pTree->OnDragEnter(pDataObject, dwKeyState, point);
  else
    return DROPEFFECT_NONE;
}

void CMainTreeDropTarget::OnDragLeave(CWnd *pWnd) {
  TRACE(_T("In CMainTreeDropTarget::OnDragLeave\n"));
  ASSERT(m_pTree);

  if (pWnd->GetSafeHwnd() == m_pTree->GetSafeHwnd())
    m_pTree->OnDragLeave();
}

DROPEFFECT CMainTreeDropTarget::OnDragOver(CWnd *pWnd,
                                           COleDataObject *pDataObject,
                                           DWORD dwKeyState, CPoint point) {
  //    TRACE("In CMainTreeDropTarget::OnDragOver\n");
  ASSERT(m_pTree);

  if (pWnd->GetSafeHwnd() == m_pTree->GetSafeHwnd())
    return m_pTree->OnDragOver(pDataObject, dwKeyState, point);
  else
    return DROPEFFECT_NONE;
}

BOOL CMainTreeDropTarget::OnDrop(CWnd *pWnd, COleDataObject *pDataObject,
                                 DROPEFFECT dropEffect, CPoint point) {
  TRACE(_T("In CMainTreeDropTarget::OnDrop\n"));
  ASSERT(m_pTree);

  if (pWnd->GetSafeHwnd() == m_pTree->GetSafeHwnd())
    return m_pTree->OnDrop(pDataObject, dropEffect, point);
  else
    return FALSE;
}
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
#include "GenericDropTarget.h"
#include "Bars\MainToolBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGenericDropTarget::CGenericDropTarget()
{
	m_pClass = NULL;
    m_bRegistered = FALSE;
	

}

CGenericDropTarget::~CGenericDropTarget()
{

}


// Overloaded Register() function performs the normal COleDropTarget::Register
// but also serves to connect this COleDropTarget with the parent grid control,
// where all drop messages will ultimately be forwarded.
BOOL CGenericDropTarget::Register(CGenericDropClass *pClass)
{
    if (m_bRegistered)
        return FALSE;

    // Stop re-entry problems
    static BOOL bInProcedure = FALSE;
    if (bInProcedure)
        return FALSE;
    bInProcedure = TRUE;

//    ASSERT(pTree->IsKindOf(RUNTIME_CLASS(CMainFolder)));
  //  ASSERT(pTree);

    if (!pClass /*|| !pTree->IsKindOf(RUNTIME_CLASS(CMainFolder))*/)
    {
        bInProcedure = FALSE;
        return FALSE;
    }

    m_pClass = pClass;

	m_bRegistered = COleDropTarget::Register(CWnd::FromHandle (m_pClass->GetSafeHwnd()));

    bInProcedure = FALSE;
    return m_bRegistered;
}


void CGenericDropTarget::Revoke()
{
    m_bRegistered = FALSE;
    COleDropTarget::Revoke();
}

BEGIN_MESSAGE_MAP(CGenericDropTarget, COleDropTarget)
    //{{AFX_MSG_MAP(CMainTreeDropTarget)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainTreeDropTarget message handlers

DROPEFFECT CGenericDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint /*point*/)
{
//    TRACE("In CMainTreeDropTarget::OnDragScroll\n");
    if (pWnd->GetSafeHwnd() == m_pClass->GetSafeHwnd())
    {
        if (dwKeyState & MK_CONTROL)
            return DROPEFFECT_COPY;
        else
            return DROPEFFECT_MOVE;
    } else
        return DROPEFFECT_NONE;
}

DROPEFFECT CGenericDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    TRACE(_T("In CGenericDropTarget::OnDragEnter\n"));
    ASSERT(m_pClass);

    if (pWnd->GetSafeHwnd() == m_pClass->GetSafeHwnd())
        return m_pClass->OnDragEnter(pDataObject, dwKeyState, point);
    else
        return DROPEFFECT_NONE;
}

void CGenericDropTarget::OnDragLeave(CWnd* pWnd)
{
    TRACE(_T("In CGenericDropTarget::OnDragLeave\n"));
    ASSERT(m_pClass);

    if (pWnd->GetSafeHwnd() == m_pClass->GetSafeHwnd())
        m_pClass->OnDragLeave();
}

DROPEFFECT CGenericDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
//    TRACE("In CMainTreeDropTarget::OnDragOver\n");
    ASSERT(m_pClass);

    if (pWnd->GetSafeHwnd() == m_pClass->GetSafeHwnd())
        return m_pClass->OnDragOver(pDataObject, dwKeyState, point);
    else
        return DROPEFFECT_NONE;
}

BOOL CGenericDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
                             DROPEFFECT dropEffect, CPoint point)
{
    TRACE(_T("In CGenericDropTarget::OnDrop\n"));
    ASSERT(m_pClass);

    if (pWnd->GetSafeHwnd() == m_pClass->GetSafeHwnd())
        return m_pClass->OnDrop(pDataObject, dropEffect, point);
    else
        return FALSE;
}



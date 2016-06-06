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
#include "PathEdit.h"
#include "globals.h"

#include "debug_new.h"

CPathEdit::CPathEdit() {}

CPathEdit::~CPathEdit() {}

BEGIN_MESSAGE_MAP(CPathEdit, CEdit)
//{{AFX_MSG_MAP(CPathEdit)
ON_WM_KILLFOCUS()
ON_WM_SETFOCUS()
ON_WM_KEYDOWN()
ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPathEdit message handlers

void CPathEdit::OnKillFocus(CWnd *pNewWnd) {
  CEdit::OnKillFocus(pNewWnd);

  // EnableAcc ();
}

void CPathEdit::OnSetFocus(CWnd *pOldWnd) { CEdit::OnSetFocus(pOldWnd); }

void CPathEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  /*if (nChar == VK_RETURN)
  {
          CString csText;
          GetWindowText (csText);
          SetRoot (csText);
  }*/
  CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CPathEdit::PreTranslateMessage(MSG *pMsg) {
  if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN)) {
    CString csText;
    GetWindowText(csText);

    SureBackSlash(csText);
    SetRoot(csText);
    SyncUI(csText);
    return TRUE;
  }

  //	if (pMsg->message == )
  if (pMsg->message == WM_SETTEXT) {
    TRACE(_T("Set Text %s\n"), pMsg->lParam);
  }
  return CEdit::PreTranslateMessage(pMsg);
}

void CPathEdit::OnChange() {
  CString text;

  GetWindowText(text);

  TRACE(_T("%s"), (LPCTSTR)text);
}

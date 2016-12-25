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
#include "ACEdit.h"
#include "globals.h"
#include "Options.h"

#include "debug_new.h"

#define _EDIT_ 1
#define _COMBOBOX_ 2



CACEdit::CACEdit() {
  m_iMode = _MODE_STANDARD_;
  m_iType = -1;
  m_pEdit = NULL;
  m_CursorMode = false;
  m_PrefixChar = 0;
}

/*********************************************************************/

CACEdit::~CACEdit() { DestroyWindow(); }

/*********************************************************************/

BEGIN_MESSAGE_MAP(CACEdit, CWnd)
//{{AFX_MSG_MAP(CACEdit)
ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
ON_WM_KEYDOWN()
ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnChange)
ON_CONTROL_REFLECT(CBN_DROPDOWN, OnCloseList)
//}}AFX_MSG_MAP
ON_MESSAGE(ENAC_UPDATE, OnUpdateFromList)
END_MESSAGE_MAP()

/*********************************************************************/

void CACEdit::SetMode(int iMode) {
  if (m_iType == -1)
    Init();

  m_iMode = iMode;
  if (iMode & _MODE_FILESYSTEM_)
    m_SeparationStr = _T("\\");
}

/*********************************************************************/

void CACEdit::Init() {
  CString szClassName =
      AfxRegisterWndClass(CS_CLASSDC | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW, 0,
                          (HBRUSH)(COLOR_WINDOW), 0);
  CRect rcWnd, rcWnd1;
  GetWindowRect(rcWnd);

  VERIFY(m_Liste.CreateEx(
      WS_EX_TOOLWINDOW, szClassName, NULL,
      WS_THICKFRAME | WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS | WS_OVERLAPPED,
      // CRect(0,0,200,200),
      CRect(rcWnd.left, rcWnd.top + 20, rcWnd.left + 200, rcWnd.top + 200),
      GetDesktopWindow(), 0x3E8, NULL));

  CString m_ClassName;
  ::GetClassName(GetSafeHwnd(), m_ClassName.GetBuffer(32), 32);
  m_ClassName.ReleaseBuffer();

  if (m_ClassName.Compare(_T("Edit")) == 0) {
    m_iType = _EDIT_;
  } else if (m_ClassName.Compare(_T("ComboBox")) == 0) {
    m_iType = _COMBOBOX_;

    m_pEdit = (CEdit *)GetWindow(GW_CHILD);
    VERIFY(m_pEdit);
    ::GetClassName(m_pEdit->GetSafeHwnd(), m_ClassName.GetBuffer(32), 32);
    m_ClassName.ReleaseBuffer();
    VERIFY(m_ClassName.Compare(_T("Edit")) == 0);
  }

  if (m_iType == -1) {
    ASSERT(0);
    return;
  }

  m_Liste.Init(this);
}

/*********************************************************************/

void CACEdit::AddSearchStrings(LPCTSTR Strings[]) {
  int i = 0;
  LPCTSTR str;
  if (m_iType == -1) {
    ASSERT(0);
    return;
  }

  m_Liste.RemoveAll();

  do {
    str = Strings[i];
    if (str) {
      m_Liste.AddSearchString(str);
    }

    i++;
  } while (str);

  m_Liste.SortSearchList();
}

/*********************************************************************/

void CACEdit::AddSearchString(LPCTSTR lpszString) {
  if (m_iType == -1) {
    ASSERT(0);
    return;
  }

  m_Liste.AddSearchString(lpszString);
}

/*********************************************************************/

void CACEdit::RemoveSearchAll() {
  if (m_iType == -1) {
    ASSERT(0);
    return;
  }

  m_Liste.RemoveAll();
}

/*********************************************************************/

void CACEdit::OnKillfocus() { m_Liste.ShowWindow(SW_HIDE); }

/*********************************************************************/

void CACEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if (!HandleKey(nChar, false))
    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*********************************************************************/

bool CACEdit::HandleKey(UINT nChar, bool m_bFromChild) {
  if (nChar == VK_TAB) {
    if (IsSHIFTpressed())
      TabFrom(ADDRESS_BAR, BACKWARD);
    else
      TabFrom(ADDRESS_BAR, FORWARD);
    return TRUE;
  }

  if (nChar == VK_ESCAPE) {
    m_Liste.ShowWindow(SW_HIDE);
    return true;
  }

  if (nChar == VK_RETURN) {
    if (m_Liste.IsWindowVisible()) {
      m_EditText = m_Liste.GetString();
      m_Liste.ShowWindow(SW_HIDE);
      SetWindowText(m_EditText);
    }
    GetParent()->SendMessage(ENAC_UPDATE, WM_KEYDOWN, GetDlgCtrlID());
    return true;
  }

  if (nChar == VK_DOWN || nChar == VK_UP || nChar == VK_PRIOR ||
      nChar == VK_NEXT || nChar == VK_HOME || nChar == VK_END) {
    if (m_Liste.IsWindowVisible()) {
      
      if (m_iMode & _MODE_STANDARD_ || m_iMode & _MODE_FILESYSTEM_ ||
          m_iMode & _MODE_FS_START_DIR_) {
        m_CursorMode = true;

        if (!m_bFromChild)
          m_EditText = m_Liste.GetNextString(nChar);
        else
          m_EditText = m_Liste.GetString();

        if (m_iMode & _MODE_FILESYSTEM_) {
          if (m_EditText.Right(1) == SLASH)
            m_EditText = m_EditText.Mid(0, m_EditText.GetLength() - 1);
        }

        m_Liste.SelectItem(-1);
        SetWindowText(m_EditText);
        int pos = m_EditText.GetLength();

        if (m_iType == _COMBOBOX_) {
          m_pEdit->SetSel(pos, pos, TRUE);
          m_pEdit->SetModify(TRUE);
        }

        if (m_iType == _EDIT_) {
          ((CEdit *)this)->SetSel(pos, pos, TRUE);
          ((CEdit *)this)->SetModify(TRUE);
        }
        // GetParent()->SendMessage(ENAC_UPDATE, WM_KEYDOWN, GetDlgCtrlID());
        m_CursorMode = false;
        return true;
      }

      if (m_iMode & _MODE_SEPARATION_) {
        CString m_Text, m_Left, m_Right;
        int left, right, pos = 0, len;

        m_CursorMode = true;

        GetWindowText(m_EditText);

        if (m_iType == _EDIT_)
          pos = LOWORD(((CEdit *)this)->CharFromPos(GetCaretPos()));

        if (m_iType == _COMBOBOX_)
          pos = m_pEdit->CharFromPos(m_pEdit->GetCaretPos());

        left = FindSepLeftPos(pos - 1, true);
        right = FindSepRightPos(pos);

        m_Text = m_EditText.Left(left);

        if (!m_bFromChild)
          m_Text += m_Liste.GetNextString(nChar);
        else
          m_Text += m_Liste.GetString();

        m_Liste.SelectItem(-1);
        m_Text += m_EditText.Mid(right);
        len = m_Liste.GetString().GetLength();

        SetWindowText(m_Text);
        // GetParent()->SendMessage(ENAC_UPDATE, WM_KEYDOWN, GetDlgCtrlID());

        right = FindSepLeftPos2(pos - 1);
        left -= right;
        len += right;

        if (m_iType == _EDIT_) {
          ((CEdit *)this)->SetModify(TRUE);
          ((CEdit *)this)->SetSel(left, left + len, FALSE);
        }
        if (m_iType == _COMBOBOX_) {
          m_pEdit->SetModify(TRUE);
          m_pEdit->SetSel(left, left + len, TRUE);
        }
        m_CursorMode = false;
        return true;
      }
    }
  }
  return false;
}

/*********************************************************************/

void CACEdit::OnChange() {
  CString m_Text;
  int pos = 0, len;

  if (m_iType == -1) {
    ASSERT(0);
    return;
  }

  GetWindowText(m_EditText);
  len = m_EditText.GetLength();
  //----------------------------------------------
  if (m_iMode & _MODE_FILESYSTEM_ || m_iMode & _MODE_FS_START_DIR_) {
    if (!m_CursorMode) {
      if (m_iType == _EDIT_)
        pos = LOWORD(((CEdit *)this)->CharFromPos(GetCaretPos()));

      if (m_iType == _COMBOBOX_)
        pos = m_pEdit->CharFromPos(m_pEdit->GetCaretPos());

      if (m_iMode & _MODE_FS_START_DIR_) {
        if (len)
          m_Liste.FindString(-1, m_EditText);
        else
          m_Liste.ShowWindow(SW_HIDE);
      } else {
        if (len > 2 && pos == len) {
          if (_taccess(m_EditText, 0) == 0) {
            ReadDirectory(m_EditText);
          }
          m_Liste.FindString(-1, m_EditText);
        } else
          m_Liste.ShowWindow(SW_HIDE);
      }
    } // m_CursorMode
  }
  //----------------------------------------------
  if (m_iMode & _MODE_SEPARATION_) {
    if (!m_CursorMode) {
      if (m_iType == _EDIT_)
        pos = LOWORD(((CEdit *)this)->CharFromPos(GetCaretPos()));

      if (m_iType == _COMBOBOX_)
        pos = m_pEdit->CharFromPos(m_pEdit->GetCaretPos());

      int left, right;
      left = FindSepLeftPos(pos - 1);
      right = FindSepRightPos(pos);
      m_Text = m_EditText.Mid(left, right - left);
      m_Liste.FindString(-1, m_Text);
    }
  }
  //----------------------------------------------
  if (m_iMode & _MODE_STANDARD_) {
    if (!m_CursorMode)
      m_Liste.FindString(-1, m_EditText);
  }
  //----------------------------------------------
  // GetParent()->SendMessage(ENAC_UPDATE, EN_UPDATE, GetDlgCtrlID());
}

/*********************************************************************/

int CACEdit::FindSepLeftPos(int pos, bool m_bIncludePrefix) {
  int len = m_EditText.GetLength();
  

  if (pos >= len && len != 1)
    pos = len - 1;

  int i;
  for (i = pos; i >= 0; i--) {
    TCHAR ch = m_EditText.GetAt(i);
    if (m_PrefixChar == ch)
      return i + (m_bIncludePrefix ? 1 : 0);
    if (m_SeparationStr.Find(ch) != -1)
      break;
  }

  return i + 1;
}

/*********************************************************************/

int CACEdit::FindSepLeftPos2(int pos) {
  int len = m_EditText.GetLength();

  if (pos >= len && len != 1)
    pos = len - 1;

  if (len == 1)
    return 0;

  for (int i = pos; i >= 0; i--) {
    TCHAR ch = m_EditText.GetAt(i);
    if (m_PrefixChar == ch)
      return 1;
  }

  return 0;
}

/*********************************************************************/

int CACEdit::FindSepRightPos(int pos) {
  int len = m_EditText.GetLength();

  for (int i = pos; i < len; i++) {
    TCHAR ch = m_EditText.GetAt(i);
    if (m_SeparationStr.Find(ch) != -1)
      return i;
  }

  return len;
}

/*********************************************************************/

LONG CACEdit::OnUpdateFromList(UINT lParam, LONG /*wParam*/) {
  UpdateData(TRUE);

  if (lParam == WM_KEYDOWN) {
    HandleKey(VK_DOWN, true);
  }

  GetParent()->SendMessage(ENAC_UPDATE, WM_KEYDOWN, GetDlgCtrlID());
  return 0;
}

/*********************************************************************/

void CACEdit::OnCloseList() { m_Liste.ShowWindow(SW_HIDE); }

/*********************************************************************/

BOOL CACEdit::PreTranslateMessage(MSG *pMsg) {
  if (pMsg->message == WM_KEYDOWN) {
    if (pMsg->wParam == VK_TAB) {
      if (IsSHIFTpressed())
        TabFrom(ADDRESS_BAR, BACKWARD);
      else
        TabFrom(ADDRESS_BAR, FORWARD);
      return TRUE;
    }

    if (m_Liste.IsWindowVisible()) {
      if (m_iType == _COMBOBOX_) {
        if (pMsg->wParam == VK_DOWN || pMsg->wParam == VK_UP)
          if (HandleKey(pMsg->wParam, false))
            return TRUE;
      }

      if (pMsg->wParam == VK_ESCAPE)
        if (HandleKey(pMsg->wParam, false))
          return TRUE;

      if (pMsg->wParam == VK_RETURN)
        if (HandleKey(pMsg->wParam, false))
          return TRUE;
    }
  }

  if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
    // dipatch keyboard messages directly (skip main accelerators)
    ::TranslateMessage(pMsg);
    ::DispatchMessage(pMsg);
    return TRUE;
  }

  return CWnd::PreTranslateMessage(pMsg);
}

/*********************************************************************/

void CACEdit::ReadDirectory(CString m_Dir) {
  CFileFind FoundFiles;
  TCHAR ch;
  CWaitCursor hg;

  if (m_Dir.Right(1) != SLASH) {
    _tsplitpath(m_Dir, m_szDrive, m_szDir, m_szFname, m_szExt);
    m_Dir.Format(_T("%s%s"), m_szDrive, m_szDir);
  }

  // ist hübscher
  ch = (TCHAR)towupper(m_Dir.GetAt(0));
  m_Dir.SetAt(0, ch);

  CString m_Name, m_File, m_Dir1 = m_Dir;
  if (m_Dir.Right(1) != SLASH)
    m_Dir += _T("\\");

  if (m_LastDirectory.CompareNoCase(m_Dir) == 0 && m_Liste.m_SearchList.size())
    return;

  m_LastDirectory = m_Dir;
  m_Dir += _T("*.*");

  BOOL bContinue = FoundFiles.FindFile(m_Dir);

  if (bContinue)
    RemoveSearchAll();

  AddSearchString(m_LastDirectory);

  while (bContinue == TRUE) {
    bContinue = FoundFiles.FindNextFile();
    m_File = FoundFiles.GetFileName();

    if (!ifNotJunctionPoint(FoundFiles))
      continue;

    if (FoundFiles.IsHidden()) {
      if (!gOptions.m_bShowHidden)
        continue;
    }

    if (!ifNotJunctionPoint(FoundFiles)) {
      continue;
    }

    if (FoundFiles.IsDirectory()) {
      // if(m_iMode == _MODE_FS_ONLY_FILE_ || m_iMode == _MODE_SD_ONLY_FILE_)
      if (m_iMode & _MODE_ONLY_FILES)
        continue;
      if (FoundFiles.IsDots())
        continue;

      if (m_File.Right(1) != SLASH)
        m_File += _T("\\");
    }

    if (!FoundFiles.IsDirectory())
      // if(m_iMode == _MODE_FS_ONLY_DIR_ || m_iMode == _MODE_SD_ONLY_DIR_)
      if (m_iMode & _MODE_ONLY_DIRS)
        continue;

    if (m_iMode & _MODE_FS_START_DIR_) {
      m_Name = m_File;
    } else {
      m_Name = m_Dir1;
      if (m_Name.Right(1) != SLASH)
        m_Name += _T("\\");

      m_Name += m_File;
    }
    AddSearchString(m_Name);
  }
  FoundFiles.Close();
  return;
}

void CACEdit::SetStartDirectory(LPCTSTR lpszString) {
  if (m_iType == -1) {
    ASSERT(0);
    return;
  }

  if (m_iMode & _MODE_FS_START_DIR_)
    ReadDirectory(lpszString);
}


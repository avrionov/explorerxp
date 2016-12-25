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

#pragma once

#define _MODE_ONLY_FILES (1L << 16)
#define _MODE_ONLY_DIRS (1L << 17)

#define _MODE_STANDARD_ (1L << 0)
#define _MODE_SEPARATION_ (1L << 1)
#define _MODE_FILESYSTEM_ (1L << 2)
#define _MODE_FS_START_DIR_ (1L << 3)
#define _MODE_FS_ONLY_FILE_ (_MODE_FILESYSTEM_ | _MODE_ONLY_FILES)
#define _MODE_FS_ONLY_DIR_ (_MODE_FILESYSTEM_ | _MODE_ONLY_DIRS)
#define _MODE_SD_ONLY_FILE_ (_MODE_FS_START_DIR_ | _MODE_ONLY_FILES)
#define _MODE_SD_ONLY_DIR_ (_MODE_FS_START_DIR_ | _MODE_ONLY_FILES)

#include "ACListWnd.h"

class CACEdit : public CWnd {

public:
  CACEdit();
  void SetMode(int iMode = _MODE_STANDARD_);
  void SetSeparator(LPCTSTR lpszString, TCHAR lpszPrefixChar = 0) {
    m_SeparationStr = lpszString;
    m_Liste.m_PrefixChar = m_PrefixChar = lpszPrefixChar;
    SetMode(_MODE_SEPARATION_);
  }

public:
  void Init();
  void AddSearchString(LPCTSTR lpszString);
  void AddSearchStrings(LPCTSTR Strings[]);
  void RemoveSearchAll();
  void SetStartDirectory(LPCTSTR lpszString);
  BOOL IsDropWindowVisible() { return m_Liste.IsWindowVisible(); }
  
public:
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
  //{{AFX_VIRTUAL(CACEdit)
public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  //}}AFX_VIRTUAL

  // Implementierung
public:
  virtual ~CACEdit();
  CACListWnd m_Liste;
  // Generierte Nachrichtenzuordnungsfunktionen
protected:
  CString m_EditText, m_SeparationStr, m_LastDirectory;
  TCHAR m_PrefixChar;
  int m_iMode;
  //{{AFX_MSG(CACEdit)
  afx_msg void OnKillfocus();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnChange();
  afx_msg void OnCloseList();
  //}}AFX_MSG
  afx_msg LONG OnUpdateFromList(UINT lParam, LONG wParam);
  DECLARE_MESSAGE_MAP()

  void ReadDirectory(CString m_Dir);
  int FindSepLeftPos(int pos, bool FindSepLeftPos = false);
  int FindSepLeftPos2(int pos);
  int FindSepRightPos(int pos);
  bool HandleKey(UINT nChar, bool m_bFromChild);

  bool m_CursorMode;
  int m_iType;
  CEdit *m_pEdit;

  TCHAR m_szDrive[_MAX_DRIVE], m_szDir[_MAX_DIR], m_szFname[_MAX_FNAME],
      m_szExt[_MAX_EXT];
};

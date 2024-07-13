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
#include "MultiRenameDlg.h"
#include "TextUtil.h"
#include "globals.h"
#include "SplitPath.h"
#include "FileShellAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiRenameDlg dialog

CMultiRenameDlg::CMultiRenameDlg(CSelRowArray &ar, CWnd *pParent /*=NULL*/)
    : CResizableDialog(CMultiRenameDlg::IDD, pParent), m_Ar(ar) {
  m_Mode = 0;
  m_pCurrent = NULL;
}

void CMultiRenameDlg::DoDataExchange(CDataExchange *pDX) {
  CResizableDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMultiRenameDlg)
  DDX_Control(pDX, IDC_FILES, m_List);
  DDX_Control(pDX, IDC_CONVERT, m_Convert);
  DDX_Control(pDX, IDC_CASE, m_Case);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_STATIC_CHILD, m_Holder);
  DDX_Control(pDX, IDC_LIST1, m_ModeList);
}

BEGIN_MESSAGE_MAP(CMultiRenameDlg, CResizableDialog)
//{{AFX_MSG_MAP(CMultiRenameDlg)
ON_BN_DOUBLECLICKED(IDC_CASE, OnDoubleclickedCase)
ON_BN_DOUBLECLICKED(IDC_CONVERT, OnDoubleclickedConvert)
ON_EN_CHANGE(IDC_FIND, OnChangeFind)
ON_EN_CHANGE(IDC_REPLACE, OnChangeReplace)
//}}AFX_MSG_MAP
ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList)
ON_MESSAGE(WM_REN_VALUECNAGED, OnChildUpdate)
ON_BN_CLICKED(IDC_BTN_FILEUP, OnBnClickedBtnFileup)
ON_BN_CLICKED(IDC_BTN_FILEDOWN, OnBnClickedBtnFiledown)
ON_BN_CLICKED(IDC_BTN_FILEDEL, OnBnClickedBtnFiledel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiRenameDlg message handlers

BOOL CMultiRenameDlg::OnInitDialog() {
  // CResizableDialog::OnInitDialog();
  CDialog::OnInitDialog();

  // AddAnchor (IDC_FIND, TOP_LEFT, TOP_RIGHT);
  // AddAnchor (IDC_REPLACE, TOP_LEFT, TOP_RIGHT);

  // AddAnchor (IDC_STATIC_REPLACE_SEP, TOP_LEFT, TOP_RIGHT);
  // AddAnchor (IDC_STATIC_OPTIONS_SEP, TOP_LEFT, TOP_RIGHT);
  // AddAnchor (IDC_STATIC_FILES_SEP, TOP_LEFT, TOP_RIGHT);
  AddAnchor(IDC_STATIC_LAST_SEP, BOTTOM_LEFT, BOTTOM_RIGHT);

  AddAnchor(IDC_FILES, TOP_LEFT, BOTTOM_RIGHT);
  AddAnchor(IDC_STATIC_CHILD, TOP_LEFT, TOP_RIGHT);

  AddAnchor(IDOK, BOTTOM_LEFT);
  AddAnchor(IDCANCEL, BOTTOM_LEFT);

  AddAnchor(IDC_CASE, BOTTOM_LEFT);
  AddAnchor(IDC_CONVERT, BOTTOM_LEFT);

  AddAnchor(IDC_BTN_FILEDEL, TOP_RIGHT);
  AddAnchor(IDC_BTN_FILEDOWN, TOP_RIGHT);
  AddAnchor(IDC_BTN_FILEUP, TOP_RIGHT);

  m_ModeList.AddString(_T("Replace"));
  m_ModeList.AddString(_T("Re-Number"));
  m_ModeList.AddString(_T("Trim"));
  m_ModeList.AddString(_T("Insert"));

  m_ModeList.SetCurSel(0);
  CRect rc;

  CRect rcHolder;
  m_Holder.GetWindowRect(rcHolder);
  ScreenToClient(rcHolder);
  rcHolder.DeflateRect(5, 5);

  m_ReNumberDlg.Create(IDD_REN_COUNT, this);
  m_ReNumberDlg.ShowWindow(SW_HIDE);
  m_ReNumberDlg.MoveWindow(rcHolder);

  m_ReplaceDlg.Create(IDD_REN_REPLACE, this);
  m_ReplaceDlg.ShowWindow(SW_HIDE);
  m_ReplaceDlg.MoveWindow(rcHolder);

  /*	m_CaseDlg.Create(IDD_REN_CASE, this);
          m_CaseDlg.ShowWindow(SW_HIDE);
          m_CaseDlg.MoveWindow(rcHolder);*/

  m_TrimDlg.Create(IDD_REN_TRIM, this);
  m_TrimDlg.ShowWindow(SW_HIDE);
  m_TrimDlg.MoveWindow(rcHolder);

  m_RenameInsert.Create(IDD_REN_INSERT, this);
  m_RenameInsert.ShowWindow(SW_HIDE);
  m_RenameInsert.MoveWindow(rcHolder);

  m_Dialogs.push_back(&m_ReplaceDlg);
  m_Dialogs.push_back(&m_ReNumberDlg);
  m_Dialogs.push_back(&m_TrimDlg);
  m_Dialogs.push_back(&m_RenameInsert);
  // m_Dialogs.push_back(&m_CaseDlg);

  SwitchTo(0);
  m_List.GetClientRect(rc);
  // m_ReplaceDlg.ShowWindow(SW_SHOW);
  // m_ReplaceDlg.MoveWindow(rcHolder);
  // AddAnchor(m_ReplaceDlg.m_hWnd, TOP_LEFT, TOP_RIGHT);
  EnableSaveRestore(_T("MultiRenameDlg"));

  m_List.InsertColumn(0, _T("Name"), LVCFMT_LEFT, rc.Width() / 2);
  m_List.InsertColumn(1, _T("Renamed"), LVCFMT_LEFT, rc.Width() / 2);
  m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
                          LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER);

  LVITEM item;
  m_List.SetImageList(GetSysImageList(), LVSIL_SMALL);
  std::sort(m_Ar.begin(), m_Ar.end());

  m_ArNew.assign(m_Ar.begin(), m_Ar.end());

  for (unsigned int i = 0; i < m_Ar.size(); i++) {
    item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
    item.iItem = i;
    item.iSubItem = 0;
    item.lParam = i;
    item.iImage = CFileShellAttributes::GetFileIcon(m_Ar[i].m_Path);

    item.pszText = (LPTSTR)(LPCTSTR)m_Ar[i].m_Name;
    m_List.InsertItem(&item);
  }

  SetNewNames();
  m_Case.SetMenu(IDR_CASE);
  m_Convert.SetMenu(IDR_CONVERT);

  return FALSE; // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CMultiRenameDlg::SetNewNames() {
  for (unsigned int i = 0; i < m_Ar.size(); i++) {
    // m_List.SetItemText (i, 1, m_ArNew[i].m_Name);
    int iImage = CFileShellAttributes::GetFileIcon(m_Ar[i].m_Path);
    m_List.SetItem(i, 1, LVIF_TEXT | LVIF_IMAGE, m_ArNew[i].m_Name, iImage, 0,
                   0, 0);
    m_List.SetItem(i, 0, LVIF_TEXT | LVIF_IMAGE, m_Ar[i].m_Name, iImage, 0, 0,
                   0);
    TRACE(_T("%d %s\n"), i, (LPCTSTR)m_ArNew[i].m_Name);
  }
}

void CMultiRenameDlg::OnDoubleclickedCase() {
  DoReplace();
  for (unsigned int i = 0; i < m_Ar.size(); i++) {
    // m_ArNew[i].m_Name = m_Ar[i].m_Name;
    ChangeCase(m_Case.GetCommand(), m_ArNew[i].m_Name);
  }
  SetNewNames();
}

void CMultiRenameDlg::OnDoubleclickedConvert() {
  DoReplace();
  for (unsigned int i = 0; i < m_Ar.size(); i++) {
    // m_ArNew[i].m_Name = m_Ar[i].m_Name;
    ConvertSpaces(m_Convert.GetCommand(), m_ArNew[i].m_Name, true);
  }
  SetNewNames();
}

void CMultiRenameDlg::DoReplace() {
  UpdateData();

  for (unsigned int i = 0; i < m_Ar.size(); i++) {
    m_ArNew[i].m_Name = m_Ar[i].m_Name;
    if (!m_Find.IsEmpty())
      m_ArNew[i].m_Name.Replace(m_Find, m_Replace);
  }
}

void CMultiRenameDlg::OnChangeFind() {
  DoReplace();
  SetNewNames();
}

void CMultiRenameDlg::OnChangeReplace() {
  DoReplace();
  SetNewNames();
}
void CMultiRenameDlg::SwitchTo(int i) {
  if (i >= static_cast<int>(m_Dialogs.size()))
    return;

  if (m_pCurrent != NULL) {
    m_pCurrent->UpdateData(FALSE);
    m_pCurrent->ShowWindow(SW_HIDE);
  }

  m_pCurrent = m_Dialogs[i];
  m_pCurrent->ShowWindow(SW_SHOW);
  m_pCurrent->SetFocus();
  m_pCurrent->UpdateData();
}

void CMultiRenameDlg::OnLbnSelchangeList() {
  int curSel = m_ModeList.GetCurSel();
  SwitchTo(curSel);
  OnChildUpdate(0, 0);
}

CString ReplaceNoCase(LPCTSTR instr, LPCTSTR oldstr, LPCTSTR newstr) {
  CString output(instr);

  // lowercase-versions to search in.
  CString input_lower(instr);
  CString oldone_lower(oldstr);
  input_lower.MakeLower();
  oldone_lower.MakeLower();

  // search in the lowercase versions,
  // replace in the original-case version.
  int pos = 0;
  while ((pos = input_lower.Find(oldone_lower, pos)) != -1) {

    // need for empty "newstr" cases.
    input_lower.Delete(pos, lstrlen(oldstr));
    input_lower.Insert(pos, newstr);

    // actually replace.
    output.Delete(pos, lstrlen(oldstr));
    output.Insert(pos, newstr);
  }
  return output;
}

CString ReplaceAllCase(CString &instr, CString &oldstr, CString &newstr) {
  CString output(instr);

  CString input_lower(instr);
  CString oldone_lower(oldstr);

  // search in the lowercase versions,
  // replace in the original-case version.
  int pos = 0;
  int len_oldstr = oldstr.GetLength();
  int len_newstr = newstr.GetLength();

  while ((pos = input_lower.Find(oldone_lower, pos)) != -1) {

    // need for empty "newstr" cases.
    input_lower.Delete(pos, len_oldstr);
    input_lower.Insert(pos, newstr);

    // actually replace.
    output.Delete(pos, len_oldstr);
    output.Insert(pos, newstr);

    pos += len_newstr;
  }
  return output;
}

CString ReplaceAllNoCase(CString &instr, CString &oldstr, CString &newstr,
                         BOOL once) {
  CString output(instr);

  // lowercase-versions to search in.
  CString input_lower(instr);
  CString oldone_lower(oldstr);
  input_lower.MakeLower();
  oldone_lower.MakeLower();

  // search in the lowercase versions,
  // replace in the original-case version.
  int pos = 0;

  int len_oldstr = oldstr.GetLength();
  int len_newstr = newstr.GetLength();

  while ((pos = input_lower.Find(oldone_lower, pos)) != -1) {

    // need for empty "newstr" cases.
    input_lower.Delete(pos, len_oldstr);
    input_lower.Insert(pos, newstr);

    // actually replace.
    output.Delete(pos, len_oldstr);
    output.Insert(pos, newstr);

    if (once)
      break;

    pos += len_newstr;
  }
  return output;
}

void Replace(CSelRowArray &src, CSelRowArray &dst, CString &find,
             CString &replace, BOOL bCaseSensentive, BOOL bReplaceAll) {
  for (unsigned int i = 0; i < src.size(); i++) {
    dst[i].m_Name = src[i].m_Name;
    if (!find.IsEmpty()) {
      if (bCaseSensentive) {
        if (bReplaceAll)
          dst[i].m_Name = ReplaceAllCase(src[i].m_Name, find, replace);
        else
          dst[i].m_Name.Replace(find, replace);

      } else
        dst[i].m_Name =
            ReplaceAllNoCase(src[i].m_Name, find, replace, !bReplaceAll);
    }
  }
}

void Trim(CSelRowArray &src, CSelRowArray &dst, int pos, int count,
          BOOL bLeft) {
  for (unsigned int i = 0; i < src.size(); i++) {
    dst[i].m_Name = src[i].m_Name;

    if (count)
      if (bLeft)
        dst[i].m_Name.Delete(pos, count);
      else {
        int len = dst[i].m_Name.GetLength();
        dst[i].m_Name.Delete(len - pos - count, count);
      }
  }
}

void ReNumber(CSelRowArray &src, CSelRowArray &dst, int start, int step,
              int digits, CString pattern) {
  for (unsigned int i = 0; i < src.size(); i++) {
    CString num;
    int save_digit = digits;
    save_digit = max(0, save_digit);
    save_digit = min(save_digit, 10);
    num.Format(_T("%0*d"), save_digit, start);
    CSplitPath path(src[i].m_Name);
    dst[i].m_Name = pattern;
    dst[i].m_Name.Replace(_T("<NUM>"), num);
    dst[i].m_Name.Replace(_T("<FILENAME>"), path.GetFName());
    dst[i].m_Name.Replace(_T("<EXT>"), path.GetExt());
    start += step;
  }
}

void Insert(CSelRowArray &src, CSelRowArray &dst, CString &txt, int pos,
            bool bRight) {
  for (unsigned int i = 0; i < src.size(); i++) {
    int len = src[i].m_Name.GetLength();
    if (pos < len) {
      dst[i].m_Name = src[i].m_Name;

      if (bRight)
        dst[i].m_Name.Insert(len - pos, txt);
      else
        dst[i].m_Name.Insert(pos, txt);
    }
  }
}

LPARAM CMultiRenameDlg::OnChildUpdate(WPARAM wp, LPARAM lp) {
  if (!m_hWnd)
    return 0;

  int curSel = m_ModeList.GetCurSel();
  switch (curSel) {
  case 0:
    Replace(m_Ar, m_ArNew, m_ReplaceDlg.m_Find, m_ReplaceDlg.m_Replace,
            m_ReplaceDlg.m_CaseSensentive, m_ReplaceDlg.m_ReplaceAll);
    break;

  case 2:
    Trim(m_Ar, m_ArNew, m_TrimDlg.m_Position, m_TrimDlg.m_Count,
         !m_TrimDlg.m_bFromRight);
    break;

  case 1:
    ReNumber(m_Ar, m_ArNew, m_ReNumberDlg.m_Start, m_ReNumberDlg.m_Step,
             m_ReNumberDlg.m_Digits, m_ReNumberDlg.m_Pattern);
    break;
  case 3:
    Insert(m_Ar, m_ArNew, m_RenameInsert.m_Text, m_RenameInsert.m_Pos,
           m_RenameInsert.m_bRight == 1);
    break;
  }

  SetNewNames();
  return 0;
}

void Swap(CSelRowArray &ar, int i, int j) {
  CSelectRow row = ar[i];
  ar[i] = ar[j];
  ar[j] = row;
}

void CMultiRenameDlg::OnBnClickedBtnFileup() {
  POSITION pos = m_List.GetFirstSelectedItemPosition();

  if (!pos)
    return;

  int index = m_List.GetNextSelectedItem(pos);

  if (index != 0) {
    Swap(m_Ar, index, index - 1);
    Swap(m_ArNew, index, index - 1);
    // SetNewNames();
    OnChildUpdate(0, 0);
    m_List.SetItemState(index - 1, LVIS_SELECTED, LVIS_SELECTED);
  }
}

void CMultiRenameDlg::OnBnClickedBtnFiledown() {
  POSITION pos = m_List.GetFirstSelectedItemPosition();

  if (!pos)
    return;

  int index = m_List.GetNextSelectedItem(pos);

  if (index < (static_cast<int>(m_Ar.size()) - 1)) {
    Swap(m_Ar, index, index + 1);
    Swap(m_ArNew, index, index + 1);
    // SetNewNames();
    OnChildUpdate(0, 0);
    m_List.SetItemState(index + 1, LVIS_SELECTED, LVIS_SELECTED);
  }
}

void CMultiRenameDlg::OnBnClickedBtnFiledel() {

  POSITION pos = m_List.GetFirstSelectedItemPosition();

  if (!pos)
    return;

  int index = m_List.GetNextSelectedItem(pos);

  if (index < static_cast<int>(m_Ar.size())) {
    m_Ar.erase(m_Ar.begin() + index);
    m_ArNew.erase(m_ArNew.begin() + index);
    m_List.DeleteItem(index);
    // SetNewNames();
  }
}

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
#include "FileViewer.h"
#include "globals.h"
#include "ClipboardFiles.h"
#include "Options.h"
#include "FileShellAttributes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static const TCHAR *Headers[] = { _T("Name"),          _T("Size"),
                                  _T("Size on Disk"),  _T("Type"),
                                  _T("Date Modified"), _T("Attributes"),
                                  EMPTYSTR };

struct SortByName {

  SortByName(bool bAscending) : m_bAscending(bAscending) {}
  bool operator()(CFileInfo &f1, CFileInfo &f2) {
    if (m_bAscending) {
      if (f1.IsDir() == f2.IsDir())
        return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;

      if (f1.IsDir())
        return true;

      return false;
    } else {
      if (f1.IsDir() == f2.IsDir())
        return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) >= 0;

      if (f2.IsDir())
        return true;

      return false;
    }
  }

  bool m_bAscending;
};

bool sort_by_size_a(const CFileInfo &f1, const CFileInfo &f2) {
  if (f1.IsNA() && f2.IsNA())
    return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;

  if (f1.IsNA())
    return true;

  if (f2.IsNA())
    return false;

  if (f1.GetSize() < f2.GetSize())
    return true;

  if (f1.GetSize() > f2.GetSize())
    return false;

  return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;
}

bool sort_by_size_d(const CFileInfo &f1, const CFileInfo &f2) {
  if (f1.IsNA() && f2.IsNA())
    return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;

  if (f1.IsNA())
    return false;

  if (f2.IsNA())
    return true;

  if (f1.GetSize() < f2.GetSize())
    return false;

  if (f1.GetSize() > f2.GetSize())
    return true;

  return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;
}

bool sort_by_size_on_disk_a(const CFileInfo &f1, const CFileInfo &f2) {
  if (f1.IsNA() && f2.IsNA())
    return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;

  if (f1.IsNA())
    return true;

  if (f2.IsNA())
    return false;

  if (f1.GetSizeOnDisk() < f2.GetSizeOnDisk())
    return true;

  if (f1.GetSizeOnDisk() > f2.GetSizeOnDisk())
    return false;

  return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;
}

bool sort_by_size_on_disk_d(const CFileInfo &f1, const CFileInfo &f2) {
  if (f1.IsNA() && f2.IsNA())
    return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;

  if (f1.IsNA())
    return false;

  if (f2.IsNA())
    return true;

  if (f1.GetSizeOnDisk() < f2.GetSizeOnDisk())
    return false;

  if (f1.GetSizeOnDisk() > f2.GetSizeOnDisk())
    return true;

  return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <= 0;
}

struct SortByType {
  SortByType(bool bAscending) : m_bAscending(bAscending) {}

  bool operator()(const CFileInfo &f1, const CFileInfo &f2) {
    if (m_bAscending) {
      if (f1.IsDir() == f2.IsDir()) {
        int ret = _tcsicmp(f1.m_Type, f2.m_Type);

        if (ret == 0)
          return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) <=
                 0;

        return ret <= 0;
      }

      if (f1.IsDir())
        return true;

      return false;
    } else {
      if (f1.IsDir() == f2.IsDir()) {
        int ret = _tcsicmp(f1.m_Type, f2.m_Type);

        if (ret == 0)
          return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) >=
                 0;

        return ret >= 0;
      }

      if (f2.IsDir())
        return true;

      return false;
    }
  }
  bool m_bAscending;
};

struct SortByTime {
  SortByTime(bool bAscending) : m_bAscending(bAscending) {}

  bool operator()(const CFileInfo &f1, const CFileInfo &f2) {

    if (m_bAscending) {
      return CompareFileTime(&f1.m_FileInfo.ftLastWriteTime,
                             &f2.m_FileInfo.ftLastWriteTime) < 0;
	}

    return CompareFileTime(&f2.m_FileInfo.ftLastWriteTime,
                           &f1.m_FileInfo.ftLastWriteTime) < 0;
  }

  bool m_bAscending;
};

struct SortByAttrib {
  SortByAttrib(bool bAscending) : m_bAscending(bAscending) {}

  bool operator()(const CFileInfo &f1, const CFileInfo &f2) {
    if (m_bAscending) {
      // return CompareFileTime (&f1.m_FileInfo.ftLastWriteTime ,
      // &f2.m_FileInfo.ftLastWriteTime) >= 0;

      if (f1.m_FileInfo.dwFileAttributes == f2.m_FileInfo.dwFileAttributes)
        return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) < 0;

      return f1.m_FileInfo.dwFileAttributes > f2.m_FileInfo.dwFileAttributes;
    }

    if (f1.m_FileInfo.dwFileAttributes == f2.m_FileInfo.dwFileAttributes)
      return _tcsicmp(f1.m_FileInfo.cFileName, f2.m_FileInfo.cFileName) > 0;

    return f2.m_FileInfo.dwFileAttributes > f1.m_FileInfo.dwFileAttributes;
  }

  bool m_bAscending;
};

CFileViewer::CFileViewer() : m_bCheckForCutFiles(false) {}

CFileViewer::~CFileViewer() {}

void CFileViewer::GetFolder(fast_array<CFileInfo> &array, const TCHAR *path) {
  InOut log(_T("GetFolder %s"), path);

  TCHAR path_buffer[MAX_PATH * 2];

  int len = _tcslen(path);
  _tcscpy(path_buffer, path);

  CString mask = path;
  mask += "*.*";

  HANDLE hFind;

  array.clear();

  hFind = FindFirstFile(mask, &array[0].m_FileInfo);

  if (hFind == INVALID_HANDLE_VALUE)
    return;

  BOOL bNext = TRUE;
  while (bNext) {
    // calc full_name
    WIN32_FIND_DATA &FindFileData = array.last().m_FileInfo;

    if (!ifNotJunctionPoint(FindFileData.dwFileAttributes))
      goto next_file;

    if (!gOptions.m_bShowHidden)
      if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ==
          FILE_ATTRIBUTE_HIDDEN)
        goto next_file;

    bool bDir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
                FILE_ATTRIBUTE_DIRECTORY;

    if (!(bDir && IsDots(FindFileData.cFileName))) {

      _tcscpy(path_buffer + len, FindFileData.cFileName);

      // array.push_back ( CFileInfo (path_buffer, FindFileData));
      array.last().Init(path_buffer);
      array.add();

      if (bDir && CalculateSizes(path)) {
        ULONGLONG size, sizeondisk;
        WIN32_FIND_DATA &FindFileData = array.back().m_FileInfo;
        if (GetDirSize(path, FindFileData.cFileName, size, sizeondisk,
                       FindFileData.ftLastWriteTime))
          array.back().SetSize(size, sizeondisk);
      }
    }
  next_file:
    bNext = FindNextFile(hFind, &array.last().m_FileInfo);
  }
  FindClose(hFind);
}

void CFileViewer::Fill(const TCHAR *root) {
  m_pGridCtrl->SetRedraw(FALSE);
  CString tmpRoot = root;
  SureBackSlash(tmpRoot);
  if (root && (m_Root != tmpRoot) && (!m_Root.IsEmpty()))
    SaveState(m_Root);

  if (m_Root != tmpRoot)
    gFolderStateMan.LoadState(tmpRoot, m_State);

  m_Root = tmpRoot;
  SureBackSlash(m_Root);
  GetFolder(m_Array, m_Root);
  Sort();
  m_pGridCtrl->SetRowCount(m_Array.size() + 1);
  SetupGrid();
  m_pGridCtrl->SetRedraw(TRUE);
  m_pGridCtrl->ResetScrollBars();
}

CString CFileViewer::GetText(int row, int col) {
  return m_Array[row].as_text(col);
}

void CFileViewer::GridCallBack(GV_DISPINFO *pDispInfo) {
  pDispInfo->item.nState |= GVIS_READONLY;

  if (pDispInfo->item.row == 0) {
    pDispInfo->item.strText = Headers[pDispInfo->item.col];
    pDispInfo->item.lfFont.lfWeight = FW_BOLD;
    return;
  }

  int row = pDispInfo->item.row - 1;

  if (static_cast<int>(m_Array.size()) > row) {
    switch (pDispInfo->item.col) {
    case 0: {
      CString cs = m_Root;
      cs += m_Array[row].as_text(pDispInfo->item.col);

      if (m_Array[row].m_nIcon == -1)
        m_Array[row].m_nIcon = CFileShellAttributes::GetFileIcon(cs, m_Array[row].IsDir());

      pDispInfo->item.iImage = m_Array[row].m_nIcon;
      pDispInfo->item.strText = m_Array[row].as_text(pDispInfo->item.col);

      if (gClipboard.IsFileIn(cs))
        pDispInfo->item.lfFont.lfItalic = TRUE;
    } break;

    case 3:
      if (m_Array[row].m_Type[0] == 0) {
        CString cs = m_Root;
        cs += m_Array[row].m_FileInfo.cFileName;
        wcsncpy(m_Array[row].m_Type, CFileShellAttributes::GetFileType(cs), 80);
      }
      pDispInfo->item.strText = m_Array[row].m_Type;
      break;

    case 1:
    case 2:
      pDispInfo->item.nFormat |= DT_RIGHT;
      pDispInfo->item.strText = m_Array[row].as_text(pDispInfo->item.col);
      pDispInfo->item.strText += _T(" ");
      break;

    default:
      pDispInfo->item.strText = m_Array[row].as_text(pDispInfo->item.col);
      break;
    }

    if (m_Array[row].m_FileInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
      pDispInfo->item.crFgClr = RGB(0, 0, 255);
    else
      pDispInfo->item.crFgClr = GetSysColor(COLOR_BTNTEXT);
  }

  if (pDispInfo->item.col == m_State.m_nSortColumn)
    pDispInfo->item.crBkClr =
        HLS_TRANSFORM(GetSysColor(COLOR_WINDOW), 0, -5); // RGB(247,247,247);

  if (pDispInfo->item.nState & GVIS_SELECTED)
    pDispInfo->item.crBkClr = GetSelectedColor();
}

void CFileViewer::Sort() {
  switch (m_State.m_nSortColumn) {
  case 0:
    std::sort(m_Array.begin(), m_Array.end(), SortByName(m_State.m_bAscending));
    break;

  case 1:
    if (m_State.m_bAscending)
      std::stable_sort(m_Array.begin(), m_Array.end(), sort_by_size_a);
    else
      std::stable_sort(m_Array.begin(), m_Array.end(), sort_by_size_d);
    break;

  case 2:
    if (m_State.m_bAscending)
      std::stable_sort(m_Array.begin(), m_Array.end(), sort_by_size_on_disk_a);
    else
      std::stable_sort(m_Array.begin(), m_Array.end(), sort_by_size_on_disk_d);
    break;

  case 3:
    for (int row = 0; row < static_cast<int>(m_Array.size()); row++) {
      if (m_Array[row].m_Type[0] == 0) {
        CString cs = m_Root;
        cs += m_Array[row].m_FileInfo.cFileName;
        wcsncpy(m_Array[row].m_Type, CFileShellAttributes::GetFileType(cs), 80);
      }
    }
    std::stable_sort(m_Array.begin(), m_Array.end(),
                     SortByType(m_State.m_bAscending));
    break;

  case 4:
    std::stable_sort(m_Array.begin(), m_Array.end(),
                     SortByTime(m_State.m_bAscending));
    break;

  case 5:
    std::sort(m_Array.begin(), m_Array.end(),
              SortByAttrib(m_State.m_bAscending));
    break;
  }
}

bool CFileViewer::Sync(const TCHAR *folder, const TCHAR *name) {
  if (m_Root != folder)
    return false;

  bool bSet = false;

  for (unsigned int i = 0; i < m_Array.size(); i++)
    if (m_Array[i].IsDir()) {
      if (_tcsicmp(m_Array[i].m_FileInfo.cFileName, name) == 0) {
        ULONGLONG size, sizeondisk;

        if (GetDirSizeLight(m_Root, m_Array[i].m_FileInfo.cFileName, size,
                            sizeondisk,
                            m_Array[i].m_FileInfo.ftLastWriteTime)) {
          m_Array[i].SetSize(size, sizeondisk);
          bSet = true;
          break;
        }
      }
    }
  return bSet;
}

const TCHAR *CFileViewer::GetTitle() { return m_Root; }

bool CFileViewer::CanChangeTo(int iRow) {
  if ((iRow < 0) || (iRow > static_cast<int>(m_Array.size())))
    return false;

  return m_Array[iRow].IsDir();
}

CString CFileViewer::GetPath(int iRow) {
  CString cs = m_Root;
  cs += m_Array[iRow].m_FileInfo.cFileName;
  return cs;
}

CString  CFileViewer::GetPath(int iRow, bool& bFolder) {
  bFolder = m_Array[iRow].IsDir();
  CString cs = m_Root;
  cs += m_Array[iRow].m_FileInfo.cFileName;
  return cs;
}

const TCHAR *CFileViewer::GetName(int iRow) {
  return m_Array[iRow].m_FileInfo.cFileName;
}

inline bool CFileViewer::GetObjectSize(int iRow, ULONGLONG &size,
                                       ULONGLONG &sizeOnDisk) {
  if (m_Array[iRow].IsNA())
    return false;

  size = m_Array[iRow].GetSize();
  sizeOnDisk = m_Array[iRow].GetSizeOnDisk();

  return true;
}

void CFileViewer::Rename(CString &oldName, CString &newName) {
  for (unsigned int i = 0; i < m_Array.size(); i++) {
    if (oldName == m_Array[i].m_FileInfo.cFileName) {
      _tcsncpy(m_Array[i].m_FileInfo.cFileName, newName, MAX_PATH);
      if (m_Array[i].IsDir()) {
        ULONGLONG size, sizeondisk;
        if (GetDirSize(m_Root, m_Array[i].m_FileInfo.cFileName, size,
                       sizeondisk, m_Array[i].m_FileInfo.ftLastWriteTime))
          m_Array[i].SetSize(size, sizeondisk);
      }
      return;
    }
  }
}

void CFileViewer::FillHeader() {
  for (int i = 0; i < 6; i++) {
    m_Headers.push_back(Headers[i]);
    // m_State.m_Visible |= 2 << i;
  }

  m_State.m_Visible = 0x1B;
  m_State.m_Widths[0] = 200;
  m_State.m_Widths[1] = 120;
  m_State.m_Widths[2] = 120;
  m_State.m_Widths[3] = 140;
  m_State.m_Widths[4] = 120;
  m_State.m_Widths[5] = 70;
}

void CFileViewer::OnClose() { SaveState(m_Root); }

boolean CFileViewer::CalculateSizes(const TCHAR *path) {
  if (gOptions.m_FolderSizeMode == FOLDERSIZE_MANUAL)
    return false;

  return true;
}
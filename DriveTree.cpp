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
#include "DriveTree.h"
#include "DirSize.h"
#include "options.h"
#include "FileShellAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// IMPLEMENT_DYNCREATE(CDriveTree, baseTree)

BEGIN_MESSAGE_MAP(CDriveTree, baseTree)
ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
END_MESSAGE_MAP()

CDriveTree::CDriveTree() : m_event(FALSE, TRUE) {
  m_nThreadCount = 0;
  m_bNeedUpdate = false;
}

void CDriveTree::OnSelectionChanged(CString &strPathName) {}

void CDriveTree::OnItemExpanding(NMHDR *pnmh, LRESULT *pResult) {
  NM_TREEVIEW *pnmtv = (NM_TREEVIEW *)pnmh;
  HTREEITEM hItem = pnmtv->itemNew.hItem;

  if (hItem == m_hParent)
    return;

  CString strPathName = GetPathFromItem(hItem);
  *pResult = FALSE;

  // Reset the drive node if the drive is empty or the media changed.
  if (!IsMediaValid(strPathName)) {
    HTREEITEM hRoot = GetDriveNode(hItem);
    Expand(hRoot, TVE_COLLAPSE | TVE_COLLAPSERESET);
    DeleteChildren(hRoot);
    AddDummyNode(hRoot);
    Select(m_hParent, TVGN_CARET);
    *pResult = TRUE;
    return;
  }

  // Delete the item if strPathName no longer specifies a valid path.
  if (!IsPathValid(strPathName)) {
    DeleteItem(hItem);
    *pResult = TRUE;
    return;
  }

  // If the item is expanding, delete the dummy item attached to it
  // and add folder items. If the item is collapsing instead, delete
  // its folder items and add a dummy item if appropriate.
  if (pnmtv->action == TVE_EXPAND) {
    DeleteChildren(hItem);
    if (!AddDirectoryNodes(hItem, strPathName))
      *pResult = TRUE;
  } else {
    Expand(hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
    DeleteChildren(hItem);
    if (IsDriveNode(hItem))
      AddDummyNode(hItem);
    else
      SetButtonState(hItem, strPathName);
  }
}

void CDriveTree::OnSelChanged(NMHDR *pnmh, LRESULT *pResult) {
  HTREEITEM hItem = ((NM_TREEVIEW *)pnmh)->itemNew.hItem;

  if (hItem == m_hParent) {
    CString cs = CONST_MYCOMPUTER;

    if (!m_bNotSelect)
      OnSelectionChanged(cs);

    return;
  }
  CString strPathName = GetPathFromItem(hItem);

  // Reset the drive node if the drive is empty or the media changed.
  if (!IsMediaValid(strPathName)) {
    HTREEITEM hRoot = GetDriveNode(hItem);
    Expand(hRoot, TVE_COLLAPSE | TVE_COLLAPSERESET);
    DeleteChildren(hRoot);
    AddDummyNode(hRoot);
    return;
  }

  // Delete the item if strPathName no longer specifies a valid path.
  if (!IsPathValid(strPathName)) {
    DeleteItem(hItem);
    return;
  }

  // Update the item's button state if the item is not expanded.
  if (!(GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) ||
      !ItemHasChildren(hItem))
    UpdateButtonState(hItem, strPathName);

  // Call the view's virtual OnSelectionChanged function.*/
  if (!m_bNotSelect)
    OnSelectionChanged(strPathName);
}

/////////////////////////////////////////////////////////////////////////
// Public member functions

void CDriveTree::RefreshDrive(UINT nDrive) {
  CString strDrive = "?:\\";
  strDrive.SetAt(0, static_cast<TCHAR>(0x41 + nDrive));

  CString drive_text;

  for (unsigned int i = 0; i < m_Array.size(); i++)
    if (m_Array[i].m_Path == strDrive) {
      drive_text = m_Array[i].m_Name;
      break;
    }

  HTREEITEM hItem = FindItem(GetNextItem(m_hParent, TVGN_CHILD), drive_text);

  if (hItem == NULL)
    return;

  // Reset the drive node if the drive is empty or the media changed.
  if (!IsMediaValid(strDrive)) {
    Expand(hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
    DeleteChildren(hItem);
    AddDummyNode(hItem);
    return;
  }

  // Refresh the drive node and all displayed subfolders.
  if (hItem != NULL) {
    SetRedraw(FALSE);
    RefreshDirectory(strDrive, hItem);
    SetRedraw(TRUE);
  }

  // Return to the original drive and directory.
  //  ::SetCurrentDirectory (szHome);
}

CString CDriveTree::GetPathFromItem(HTREEITEM hItem) {
  CString strPathName;

  while (hItem != NULL && hItem != m_hParent) {
    CString string;

    HTREEITEM hParent = GetParentItem(hItem);

    if (hParent == m_hParent) {
      DWORD data = GetItemData(hItem);
      string = m_Array[data].m_Path;
    } else
      string = GetItemStr(hItem);

    if ((string.Right(1) != "\\") && !strPathName.IsEmpty())
      string += "\\";
    strPathName = string + strPathName;
    hItem = GetParentItem(hItem);
  }
  return strPathName;
}

BOOL CDriveTree::ExpandPath(LPCTSTR pszPath, BOOL bSelectItem) {
  if (::lstrlen(pszPath) < 3)
    return FALSE;

  // Begin by finding the corresponding drive node.
  CString strPathName = pszPath;
  CString strDrive = strPathName.Left(3);

  HTREEITEM hItem = FindItem(GetNextItem(NULL, TVGN_ROOT), strDrive);

  if (hItem == NULL)
    return FALSE; // Invalid drive specification

  strPathName = strPathName.Right(strPathName.GetLength() - 3);

  // Now bore down through the directory structure searching for the
  // item that corresponds to the final directory name in pszPath.
  while (strPathName.GetLength() > 0) {
    Expand(hItem, TVE_EXPAND);
    hItem = GetChildItem(hItem);
    if (hItem == NULL)
      return FALSE;

    int nIndex = strPathName.Find('\\');
    CString findPath = nIndex == -1 ? strPathName : strPathName.Left(nIndex);

    hItem = FindItem(hItem, findPath);

    if (hItem == NULL)
      return FALSE; // Invalid path name

    if (nIndex == -1)
      strPathName.Empty();
    else
      strPathName = strPathName.Right(strPathName.GetLength() - nIndex - 1);
  }

  Expand(hItem, TVE_EXPAND);
  // if (bSelectItem)
  // Select (hItem, TVGN_CARET);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// Protected helper functions

UINT CDriveTree::InitTree() {
  for (unsigned int i = 0; i < m_Array.size(); i++)
    AddDriveNode(i);

  return 0;

  /*int nPos = 0;
  UINT nCount = 0;
  CString strDrive = "?:\\";

  DWORD dwDriveList = ::GetLogicalDrives ();

  while (dwDriveList) {
      if (dwDriveList & 1) {
          strDrive.SetAt (0, 0x41 + nPos);
          if (AddDriveNode (strDrive))
              nCount++;
      }
      dwDriveList >>= 1;
      nPos++;
  }
  return nCount;*/
}

BOOL CDriveTree::AddDriveNode(int nDrive, HTREEITEM hInsertAfter) {
  HTREEITEM hItem;

  hItem = InsertItem(m_Array[nDrive].m_Name, m_Array[nDrive].m_nImage,
                     m_Array[nDrive].m_nImage, m_hParent, hInsertAfter);

  SetItemData(hItem, nDrive);

  switch (m_Array[nDrive].m_nType) {

  case DRIVE_REMOVABLE:
    AddDummyNode(hItem);
    m_Array[nDrive].m_dwMediaID = 0xFFFFFFFF;
    break;

  case DRIVE_FIXED:
    SetButtonState(hItem, m_Array[nDrive].m_Path);
    break;

  case DRIVE_REMOTE:
    SetButtonState(hItem, m_Array[nDrive].m_Path);
    break;

  case DRIVE_CDROM:
    AddDummyNode(hItem);
    m_Array[nDrive].m_dwMediaID = 0xFFFFFFFF;
    break;

  case DRIVE_RAMDISK:
    SetButtonState(hItem, m_Array[nDrive].m_Path);
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

UINT CDriveTree::AddDirectoryNodes(HTREEITEM hItem, CString &strPathName) {
  HANDLE hFind;
  WIN32_FIND_DATA fd;

  UINT nCount = 0;

  SureBackSlash(strPathName);
  CString strFileSpec = strPathName;

  strFileSpec += "*.*";
  hFind = FindFirstFileEx(strFileSpec, FindExInfoStandard, &fd,
                          FindExSearchLimitToDirectories, NULL, 0);
  // hFind = ::FindFirstFile ((LPCTSTR) strFileSpec, &fd);

  if (hFind == INVALID_HANDLE_VALUE) {
    if (IsDriveNode(hItem))
      AddDummyNode(hItem);
    return 0;
  }

  do {

    if (ifNotJunctionPoint(fd.dwFileAttributes)) {
      if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if (!gOptions.m_bShowHidden)
          if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ==
              FILE_ATTRIBUTE_HIDDEN)
            continue;

        CString strFileName = (LPCTSTR) & fd.cFileName;
        if (!IsDots(strFileName)) {
          int nIcon = CFileShellAttributes::GetFileIcon(strPathName + strFileName);
          HTREEITEM hChild = InsertItem((LPCTSTR) & fd.cFileName, nIcon, nIcon,
                                        hItem, TVI_SORT);

          CString strNewPathName = strPathName;
          if (strNewPathName.Right(1) != "\\")
            strNewPathName += "\\";

          strNewPathName += (LPCTSTR) & fd.cFileName;
          SetButtonState(hChild, strNewPathName);
          nCount++;
        }
      }
    }
  } while (::FindNextFile(hFind, &fd));

  ::FindClose(hFind);
  return nCount;
}

void CDriveTree::SetButtonState(HTREEITEM hItem, CString &strPathName) {
  if (HasSubdirectory(strPathName))
    AddDummyNode(hItem);
}

void CDriveTree::UpdateButtonState(HTREEITEM hItem, CString &strPathName) {
  if (HasSubdirectory(strPathName)) {
    if (!ItemHasChildren(hItem)) {
      AddDummyNode(hItem);
      Invalidate();
    }
  } else {
    if (ItemHasChildren(hItem))
      DeleteChildren(hItem);
  }
}

BOOL CDriveTree::HasSubdirectory(CString &strPathName) {
  // return (PathIsDirectoryEmpty (strPathName) == FALSE);
  return TRUE;

  /*  HANDLE hFind;
    WIN32_FIND_DATA fd;
    BOOL bResult = FALSE;

    CString strFileSpec = strPathName;

        SureBackSlash (strFileSpec);

    strFileSpec += "*.*";

        //if ((hFind = FindFirstFileEx (strFileSpec, FindExInfoStandard , &fd,
    FindExSearchLimitToDirectories, NULL, 0)) != INVALID_HANDLE_VALUE)
    if ((hFind = ::FindFirstFile ((LPCTSTR) strFileSpec, &fd)) !=
    INVALID_HANDLE_VALUE)
        {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                        {
                if (!IsDots (fd.cFileName))
                    bResult = TRUE;
            }
        } while (!bResult && ::FindNextFile (hFind, &fd) );
        ::FindClose (hFind);
    }

    return bResult;*/
}

BOOL CDriveTree::IsDriveNode(HTREEITEM hItem) {
  return (GetParentItem(hItem) == m_hParent) ? TRUE : FALSE;
}

void CDriveTree::AddDummyNode(HTREEITEM hItem) {
  InsertItem(EMPTYSTR, 0, 0, hItem);
}

HTREEITEM CDriveTree::FindItem(HTREEITEM hItem, CString &strTarget) {
  while (hItem != NULL) {
    if (GetItemStr(hItem) == strTarget)
      break;
    hItem = GetNextSiblingItem(hItem);
  }
  return hItem;
}

UINT CDriveTree::DeleteChildren(HTREEITEM hItem) {
  UINT nCount = 0;
  HTREEITEM hChild = GetChildItem(hItem);

  while (hChild != NULL) {
    HTREEITEM hNextItem = GetNextSiblingItem(hChild);
    DeleteItem(hChild);
    hChild = hNextItem;
    nCount++;
  }
  return nCount;
}

HTREEITEM CDriveTree::GetDriveNode(HTREEITEM hItem) {
  HTREEITEM hParent;

  do {
    hParent = GetParentItem(hItem);
    if (hParent != NULL && hParent != m_hParent)
      hItem = hParent;
  } while (hParent != NULL && hParent != m_hParent);
  return hItem;
}

DWORD CDriveTree::GetSerialNumber(CString &strDrive) {
  DWORD dwSerialNumber;
  if (!::GetVolumeInformation((LPCTSTR)strDrive, NULL, 0, &dwSerialNumber, NULL,
                              NULL, NULL, 0))
    dwSerialNumber = 0xFFFFFFFF;
  return dwSerialNumber;
}

BOOL CDriveTree::IsMediaValid(CString &strPathName) {
  //	return TRUE;

  // Return TRUE if the drive doesn't support removable media.
  UINT nDriveType = GetDriveType((LPCTSTR)strPathName);
  if ((nDriveType != DRIVE_REMOVABLE) && (nDriveType != DRIVE_CDROM))
    return TRUE;

  // Return FALSE if the drive is empty (::GetVolumeInformation fails).
  DWORD dwSerialNumber;
  CString strDrive = strPathName.Left(3);
  INT nDrive = -1;

  TCHAR szPath[MAX_PATH];
  DWORD dwLeng = GetLogicalDriveStrings(MAX_PATH, szPath);
  TCHAR *pszTemp = szPath;
  // Try all drives one by one
  for (DWORD dw = 0; dw < dwLeng; dw += 4) {
    if (strDrive.CompareNoCase(pszTemp) == 0) {
      nDrive = dw / 4;
      break;
    }
    pszTemp += 4;
  }

  if (nDrive == -1)
    return FALSE;

  if (!::GetVolumeInformation((LPCTSTR)strDrive, NULL, 0, &dwSerialNumber, NULL,
                              NULL, NULL, 0)) {
    m_Array[nDrive].m_dwMediaID = 0xFFFFFFFF;
    return FALSE;
  }

  // Also return FALSE if the disk's serial number has changed.
  if ((m_Array[nDrive].m_dwMediaID != dwSerialNumber) &&
      (m_Array[nDrive].m_dwMediaID != 0xFFFFFFFF)) {
    m_Array[nDrive].m_dwMediaID = dwSerialNumber;
    return FALSE;
  }

  // Update our record of the serial number and return TRUE.
  m_Array[nDrive].m_dwMediaID = dwSerialNumber;
  return TRUE;
}

BOOL CDriveTree::IsPathValid(CString &strPathName) {
  DWORD fAttrib = GetFileAttributes(strPathName);

  if (fAttrib == 0xFFFFFFFF)
    return FALSE;

  //	bool bExist = IsExist (strPathName);

  if (fAttrib & FILE_ATTRIBUTE_DIRECTORY) {
    SureBackSlash(strPathName);
    return TRUE;
  }

  return FALSE;
}

void CDriveTree::RefreshDirectory(CString path, HTREEITEM hItem) {
  // If the item is not expanded, update its button state and return.
  CString strPathName = GetPathFromItem(hItem);

  if (!(GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) ||
      !ItemHasChildren(hItem)) {
    if (!IsDriveNode(hItem)) {

      UpdateButtonState(hItem, strPathName);
      Expand(hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
    }
    return;
  }

  // Delete items corresponding to subdirectories that no longer exist
  // and build a CStringList containing the names of all the items that
  // are children of hItem.
  CStringList list;
  WIN32_FIND_DATA fd;
  HANDLE hFind;

  HTREEITEM hChild = GetChildItem(hItem);

  SureBackSlash(strPathName);

  while (hChild != NULL) {
    HTREEITEM hNextItem = GetNextSiblingItem(hChild);

    CString strDirPathName = strPathName;
    CString strDirName = GetItemStr(hChild);
    strDirPathName += strDirName;

    if ((hFind = ::FindFirstFile((LPCTSTR)strDirPathName, &fd)) !=
        INVALID_HANDLE_VALUE) {
      if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if (gOptions.m_bShowHidden)
          list.AddTail(strDirName);
        else if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ==
                 FILE_ATTRIBUTE_HIDDEN)
          DeleteItem(hChild);
        else
          list.AddTail(strDirName);
      } else
        DeleteItem(hChild);

      ::FindClose(hFind);
    } else
      DeleteItem(hChild);

    hChild = hNextItem;
  }

  // Add items for newly created subdirectories.

  CString new_path = strPathName;

  new_path += "*.*";

  //	COLORREF colorWnd = GetSysColor(COLOR_WINDOWTEXT);

  if ((hFind = ::FindFirstFile(new_path, &fd)) != INVALID_HANDLE_VALUE) {
    do {
      if (ifNotJunctionPoint(fd.dwFileAttributes)) {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

          if (!gOptions.m_bShowHidden)
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ==
                FILE_ATTRIBUTE_HIDDEN)
              continue;

          CString strFileName = (LPCTSTR) & fd.cFileName;
          if (!IsDots(fd.cFileName)) {
            if (list.Find(strFileName) == NULL) {
              int iImage = CFileShellAttributes::GetFileIcon(strPathName + strFileName);

              hChild = InsertItem(strFileName, iImage, iImage, hItem, TVI_SORT);
              if (fd.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
                SetItemColor(hChild, RGB(0, 0, 255));
              /*else
                      SetItemColor(hChild, colorWnd);		*/

              CString strPathName = GetPathFromItem(hChild);
              SetButtonState(hChild, strPathName);
              list.AddTail(strFileName);
            }
          }
        }
      }
    } while (::FindNextFile(hFind, &fd));
    ::FindClose(hFind);
  }

  // Remove all items from the CStringList.
  list.RemoveAll();

  // Now repeat this procedure for hItem's children.
  hChild = GetChildItem(hItem);

  while (hChild != NULL) {
    CString string = GetItemStr(hChild);
    RefreshDirectory(string, hChild); // Recursion!
    hChild = GetNextSiblingItem(hChild);
  }
}

void CDriveTree::Fill() {
  m_bNotSelect = true;
  ModifyStyle(0, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS);

  SetImageList(GetSysImageList(), TVSIL_NORMAL);
  GetDrives(m_Array);
  ITEMIDLIST *pidl_mycomputer;
  SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidl_mycomputer);
  SHFILEINFO sfi;
  SHGetFileInfo((LPCTSTR)pidl_mycomputer, 0, &sfi, sizeof(sfi),
                SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX |
                    SHGFI_SMALLICON | SHGFI_LINKOVERLAY);
  m_hParent = InsertItem(sfi.szDisplayName, sfi.iIcon, sfi.iIcon);

  csMyComputer = sfi.szDisplayName;
  gnMyComputer = sfi.iIcon;
  InitTree();

  Expand(m_hParent, TVE_EXPAND);
  SelectItem(m_hParent);
  m_bNotSelect = false;
}

void CDriveTree::SetRoot(HTREEITEM hItem) {
  CString sPath;

  sPath = GetPathFromItem(hItem);

  ::SetRoot(sPath);
}

bool GetFirstFolder(CString &path, CString &name) {
  int pos = path.Find('\\');
  if (pos) {
    name = path.Left(pos + 1);
    path.Delete(0, pos + 1);
    return true;
  }
  return false;
}

void CDriveTree::Find(const TCHAR *folder, bool bExpand) {
  return;
  if (_tcsicmp(folder, CONST_MYCOMPUTER) == 0) {
    SelectItem(m_hParent);
    return;
  }

  CString path = folder;
  SureBackSlash(path);
  CString name;
  path.MakeLower();

  GetFirstFolder(path, name);

  // find the root folder first;
  HTREEITEM hItem = GetChildItem(m_hParent);
  while (hItem != NULL) {
    DWORD data = GetItemData(hItem);
    CString csTemp = m_Array[data].m_Path;
    csTemp.MakeLower();
    if (csTemp == name)
      break;

    hItem = GetNextItem(hItem, TVGN_NEXT);
  }

  while (!path.IsEmpty() && hItem != NULL) {
    Expand(hItem, TVE_EXPAND);
    GetFirstFolder(path, name);
    name.Delete(name.GetLength() - 1);
    HTREEITEM hChild = GetChildItem(hItem);
    while (hChild != NULL) {
      if (name.CompareNoCase(GetItemStr(hChild)) == 0) {
        hItem = hChild;
        break;
      }

      hChild = GetNextItem(hChild, TVGN_NEXT);
    }
  }

  m_bNotSelect = true;
  if (hItem != NULL) {
    SelectItem(hItem);
    if (bExpand)
      Expand(hItem, TVE_EXPAND);
  }
  m_bNotSelect = false;
}

BOOL CDriveTree::PopulateItem(HTREEITEM hParent) { return TRUE; }

CString CDriveTree::GetItemStr(HTREEITEM hItem) { return GetItemText(hItem); }

int FindDrive(CDriveArray &ar, CString &path) {
  for (unsigned int i = 0; i < ar.size(); i++)
    if (ar[i].m_Path == path)
      return i;

  return -1;
}

void CDriveTree::MediaChanged() {
  CDriveArray ar = m_Array;
  GetDrives(m_Array);

  for (unsigned int i = 0; i < m_Array.size(); i++) {
    HTREEITEM hItem =
        FindItem(GetNextItem(m_hParent, TVGN_CHILD), m_Array[i].m_Name);
    if (hItem == NULL) {
      HTREEITEM hInsertLast = TVI_FIRST;
      HTREEITEM hNext = GetNextItem(m_hParent, TVGN_CHILD);
      for (unsigned int j = 0; j < i - 1; j++)
        hNext = GetNextItem(hNext, TVGN_NEXT);

      if (hNext)
        hInsertLast = hNext;
      else
        hInsertLast = TVI_LAST;

      AddDriveNode(i, hInsertLast);
      TRACE("Delete Drive %s\n", (LPCTSTR)m_Array[i].m_Name);

      hNext = GetNextItem(m_hParent, TVGN_CHILD);
      for (unsigned j = 0; j < m_Array.size(); j++) {
        if (hNext)
          SetItemData(hNext, j);
        hNext = GetNextItem(hNext, TVGN_NEXT);
      }
    }
  }

  for (unsigned int i = 0; i < ar.size(); i++) {
    HTREEITEM hItem =
        FindItem(GetNextItem(m_hParent, TVGN_CHILD), ar[i].m_Name);
    if (!hItem)
      continue;

    int pos = FindDrive(m_Array, ar[i].m_Path);
    if (pos == -1) {
      DeleteItem(hItem);
      TRACE("Delete Drive %s\n", (LPCTSTR)ar[i].m_Path);
      HTREEITEM hNext = GetNextItem(m_hParent, TVGN_CHILD);
      for (unsigned j = 0; j < m_Array.size(); j++) {
        if (hNext)
          SetItemData(hNext, j);
        hNext = GetNextItem(hNext, TVGN_NEXT);
      }
    } else
      m_Array[pos].m_dwMediaID = ar[i].m_dwMediaID;
  }
}

void CDriveTree::CheckUpdate() {
  if (m_bNeedUpdate) {
    TRACE(_T("Check Update Folder Tree\n"));
    MediaChanged();
    m_bNeedUpdate = false;
  }
}
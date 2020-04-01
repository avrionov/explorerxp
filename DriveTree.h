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


#ifndef __DRIVETREE_H__
#define __DRIVETREE_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDriveTree window

#include "globals.h"
#include "TreeCtrlEx.h"

#define baseTree CTreeCtrlEx

class CDriveTree : public baseTree {

  // Operations
public:
  CDriveTree();
  void Find(const TCHAR *path, bool bExpand = false);
  bool m_bNotSelect;
  void SetRoot(HTREEITEM hItem);
  void Fill();
  CString GetPathFromItem(HTREEITEM hItem);
  void MediaChanged();
  void MarkForUpdate() { m_bNeedUpdate = true; }
  void CheckUpdate();

protected:
  //	CImageList m_imglDrives;
  CEvent m_event;
  UINT m_nThreadCount;
  // DWORD m_dwMediaID[MAX_THREAD];
  virtual void OnSelectionChanged(CString &strPathName);

  // Generated message map functions
protected:
  //{{AFX_MSG(CDemoView)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG

  afx_msg void OnItemExpanding(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
  LPARAM OnDriveContentsChanged(WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()

  UINT InitTree();

  BOOL IsMediaValid(CString &strPathName);
  DWORD GetSerialNumber(CString &strDrive);
  HTREEITEM GetDriveNode(HTREEITEM hItem);
  BOOL HasSubdirectory(CString &strPathName);
  UINT DeleteChildren(HTREEITEM hItem);
  void AddDummyNode(HTREEITEM hItem);
  BOOL IsDriveNode(HTREEITEM hItem);
  void UpdateButtonState(HTREEITEM hItem, CString &strPathName);
  void SetButtonState(HTREEITEM hItem, CString &strPathName);
  void RefreshDrive(UINT nDrive);
  BOOL ExpandPath(LPCTSTR pszPath, BOOL bSelectItem);
  BOOL AddDriveNode(int i, HTREEITEM hInsertAfter = TVI_LAST);

  BOOL IsPathValid(CString &strPathName);
  void RefreshDirectory(CString path, HTREEITEM hItem);
  HTREEITEM FindItem(HTREEITEM hItem, CString &strTarget);

  CDriveArray m_Array;
  HTREEITEM m_hParent;

  // virtual
  virtual UINT AddDirectoryNodes(HTREEITEM hItem, CString &strPathName);
  virtual CString GetItemStr(HTREEITEM hItem);

protected:
  virtual BOOL PopulateItem(HTREEITEM hParent);

  bool m_bNeedUpdate;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // __DRIVETREE_H_

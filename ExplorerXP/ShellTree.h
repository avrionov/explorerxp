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


#pragma once

#include "StdAfx.h"

// this class is almost a copy of CMFCShellTreeCtrl
// The original class is impossible to extend or customize

class CShellTree : public CTreeCtrl
  {
    friend class CMFCShellListCtrl;

  DECLARE_DYNAMIC(CShellTree)

  // Construction
  public:
    CShellTree();

    // Attributes
  public:
    BOOL GetItemPath(CString& strPath, HTREEITEM htreeItem = NULL /* NULL - selected */) const;
    CMFCShellListCtrl* GetRelatedList() const;

    // Flags are same as in IShellFolder::EnumObjects
    DWORD GetFlags() const { return m_dwFlags; }
    void SetFlags(DWORD dwFlags, BOOL bRefresh = TRUE);

    // Operations
  public:
    void Refresh();
    BOOL SelectPath(LPCTSTR lpszPath, BOOL fExpandToShowChildren = TRUE);
    BOOL SelectPath(LPCITEMIDLIST lpidl, BOOL fExpandToShowChildren = TRUE);

    void EnableShellContextMenu(BOOL bEnable = TRUE);

    void SetRelatedList(CMFCShellListCtrl* pShellList);

    // Overrides
  public:
    virtual CString OnGetItemText(LPAFX_SHELLITEMINFO pItem);
    virtual int OnGetItemIcon(LPAFX_SHELLITEMINFO pItem, BOOL bSelected);

  public:
    virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

  protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void PreSubclassWindow();

    // Implementation
  public:
    virtual ~CShellTree();

  protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnDestroy();
    afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

    BOOL GetRootItems();
    BOOL GetChildItems(HTREEITEM hParentItem);
    virtual HRESULT EnumObjects(HTREEITEM hParentItem, LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent);
    void OnShowContextMenu(CPoint point);
    void InitTree();

    static int CALLBACK CompareProc(LPARAM, LPARAM, LPARAM);

    AFX_IMPORT_DATA static IContextMenu2* m_pContextMenu2;

    BOOL  m_bContextMenu;
    BOOL  m_bNoNotify;
    HWND  m_hwndRelatedList;
    DWORD m_dwFlags;  // Flags for IShellFolder::EnumObjects
};

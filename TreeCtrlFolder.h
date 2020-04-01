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

#pragma once

#define WM_SELECTITEM_CHANGE	(WM_USER + 102)

class CComboBoxFolder;

#include "DriveTree.h"

class CTreeCtrlFolder : public CDriveTree
{
// Construction
public:
	CTreeCtrlFolder();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlFolder)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectNextItem(BOOL selectNext=TRUE);
	void Inform();
	void Init(CComboBoxFolder* comboBoxFolder);
	CString GetSelectedPath();
	HTREEITEM SearchChildOneLevel(HTREEITEM item, CString path);
	HTREEITEM Search(HTREEITEM item, CString path);
	
	int SelectItem(HTREEITEM item);
	HTREEITEM GetSelectedItem();
	void HideAndInform();
	HTREEITEM FreeMemory(HTREEITEM item);
	void FreeMemory();
	void Display(CRect rc);
	virtual ~CTreeCtrlFolder();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlFolder)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	HTREEITEM m_selectedItem;
	CComboBoxFolder* m_comboBoxFolder;
};
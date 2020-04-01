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

#include "TreeCtrlFolder.h"
#include "ACEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CComboBoxFolder window

class CComboBoxFolder : public CComboBox
{
// Construction
public:
	CComboBoxFolder();

// Attributes
public:
	
// Operations
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxFolder)
	protected:
     //virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
     //virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	 afx_msg void OnSize(UINT nType, int cx, int cy);
	 protected:

	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetDroppedState();
	void SetDroppedHeight(int height);
	void SetDroppedWidth(int width);
	int GetDroppedWidth();
	int GetDroppedHeight();
	void DisplayTree();
	CString GetSelectedPath();
	void AddPath(CString path);
	void Init(int w=0, int h=0);
	virtual ~CComboBoxFolder();
	void SetFolder (const TCHAR *folder);

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxFolder)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LONG OnUpdateEdit(UINT lParam, LONG wParam);
	DECLARE_MESSAGE_MAP()


private:
	int m_iconWidth;
	int m_droppedHeight;
	int m_droppedWidth;
	int m_nIcon;
	CString m_Folder;
	CTreeCtrlFolder	m_treeCtrl;
	CACEdit m_PathEdit;
};



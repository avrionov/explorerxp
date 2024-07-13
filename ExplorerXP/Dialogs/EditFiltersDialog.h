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

#ifndef __EDIT_FILTERS_DIALOG_H__
#define __EDIT_FILTERS_DIALOG_H__

#pragma once

#include "ResizableLib\ResizableDialog.h"
#include "FileFilter.h"

class CEditFiltersDialog : public CResizableDialog
{
// Construction
public:
	CEditFiltersDialog (CFilterMan &filterMan, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditFiltersDialog)
	enum { IDD = IDD_EDITFILTERS };
	CButton	m_Edit;
	CButton	m_Delete;
	CButton	m_Add;
	CListCtrl	m_FilterList;
	//}}AFX_DATA

	CFilterMan & GetFilterMan () { return m_FilterMan;};
	virtual BOOL OnInitDialog();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditFiltersDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableControls ();
	void Fill ();
	CFilterMan m_FilterMan;
	// Generated message map functions
	//{{AFX_MSG(CEditFiltersDialog)

	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITFILTERSDIALOG_H__3DF50331_4269_4C27_B221_6AF65A7D66E9__INCLUDED_)

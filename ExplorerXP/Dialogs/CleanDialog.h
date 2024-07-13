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

#ifndef __CLEANDIALOG_H__
#define __CLEANDIALOG_H__


#pragma once

#include "ResizableLib\ResizableDialog.h"
#include "FileFilter.h"
#include "globals.h"

/////////////////////////////////////////////////////////////////////////////
// CCleanDialog dialog

class CCleanDialog : public CResizableDialog
{
// Construction
public:
	CCleanDialog (CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCleanDialog)
	enum { IDD = IDD_CLEAN };
	CButton	m_EditFilters;
	CButton	m_Clean;
	CButton	m_Edit;
	CButton	m_Delete;
	CListCtrl	m_FilterList;	
	//}}AFX_DATA

	CFilterMan m_FilterMan;	
	int m_nSel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCleanDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void EnableControls ();
// Implementation
protected:
	void Fill ();
	
	
	// Generated message map functions
	//{{AFX_MSG(CCleanDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdit();
	afx_msg void OnEditfilters();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CLEANDIALOG_H__

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

#ifndef __EDITFILTER_DIALOG_H__
#define __EDITFILTER_DIALOG_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CEditFilterDialog dialog

#include "ResizableLib\ResizableDialog.h"
#include "FileFilter.h"

class CEditFilterDialog : public CResizableDialog
{
// Construction
public:
	CEditFilterDialog(CFilterMan &filterman, int ef, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditFilterDialog)
	enum { IDD = IDD_EDITFILTER };
	CButton	m_Edit;
	CButton	m_Delete;
	CListCtrl m_FilterList;
	CString	m_Name;
	//}}AFX_DATA
	CFilter & GetFilter() { return m_Filter;};

	virtual BOOL OnInitDialog();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditFilterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Fill ();
	void EnableControls ();
	CFilter m_Filter;
	int m_EF;
	// Generated message map functions
	//{{AFX_MSG(CEditFilterDialog)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EDITFILTER_DIALOG_H__

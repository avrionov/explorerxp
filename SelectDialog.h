/* Copyright 2002-2016 Nikolay Avrionov. All Rights Reserved.
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*  http://www.apache.org/licenses/LICENSE-2.0
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/


#ifndef __SELECT_DIALOG_H__
#define __SELECT_DIALOG_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CSelectDialog dialog
#include "ResizableLib\ResizableDialog.h"

class CSelectDialog : public CResizableDialog
{
// Construction
public:
	CSelectDialog(const TCHAR *title, const TCHAR *file_name, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectDialog)
	enum { IDD = IDD_SELECT };	
	CString	m_Wildcards;
	//}}AFX_DATA

	virtual BOOL OnInitDialog();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDialog)
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CString m_Title;		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SELECT_DIALOG_H__

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

#ifndef __NEWFOLDERDIALOG_H__
#define __NEWFOLDERDIALOG_H__

#pragma once

#include "ResizableLib\ResizableDialog.h"
#include "MenuButton.h"

class CNewFolderDialog : public CResizableDialog
{
// Construction
public:
	CNewFolderDialog(const TCHAR *name, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewFolderDialog)
	enum { IDD = IDD_NEWFOLDER };
	CString	m_FolderName;
	CMenuButton	m_Convert;
	CMenuButton	m_Case;
	//}}AFX_DATA

	virtual BOOL OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewFolderDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewFolderDialog)
	
	afx_msg void OnDoubleclickedCase();
	afx_msg void OnDoubleclickedConvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __NEWFOLDERDIALOG_H__

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

#ifndef __RENAME_DGL_H__
#define __RENAME_DGL_H__

#pragma once


#include "ResizableLib\ResizableDialog.h"
#include "MenuButton.h"

class CRenameDlg : public CResizableDialog
{
// Construction
public:
	CRenameDlg(const TCHAR *name, CWnd* pParent = NULL);   // standard constructor
	void HideExt () {m_bHideExt = true;}
// Dialog Data
	//{{AFX_DATA(CRenameDlg)
	enum { IDD = IDD_RENAME };
	CMenuButton	m_Convert;
	CMenuButton	m_Case;
	CString	m_NewName;
	CString	m_OrgName;
	bool m_bHideExt;
	//}}AFX_DATA

	virtual BOOL OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenameDlg)

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDoubleclickedCase();
	afx_msg void OnDoubleclickedConvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:	
	CString m_NewExt;
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __RENAME_DGL_H__

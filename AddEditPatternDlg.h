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

#ifndef __ADDEDITPATTERN_H__
#define __ADDEDITPATTERN_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CAddEditPatternDlg dialog

class CAddEditPatternDlg : public CDialog
{
// Construction
public:
	CAddEditPatternDlg(BOOL bAddDlg, CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddEditPatternDlg)
	enum { IDD = IDD_ADD_EDIT_PATTERN };
	CString	m_sPattern;
	//}}AFX_DATA
	virtual BOOL OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddEditPatternDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    BOOL m_bAddDlg;

	// Generated message map functions
	//{{AFX_MSG(CAddEditPatternDlg)
	
	//}}AFX_MSG
    afx_msg void OnKickIdle();
    afx_msg void OnUpdateOK(CCmdUI*);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __ADDEDITPATTERN_H__

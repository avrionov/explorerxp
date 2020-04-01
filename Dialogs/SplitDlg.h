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

#ifndef __SPLITDLG_H__
#define __SPLITDLG_H__

#pragma once

#include "ResizableLib\ResizableDialog.h"
#include "FileSplit.h"

class CSplitDlg : public CResizableDialog
{
// Construction
public:
	CSplitDlg(const TCHAR *file, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSplitDlg)
	enum { IDD = IDD_SPLIT };
	CProgressCtrl	m_Progress;
	CComboBox	m_Multi;
	CString	m_InputFile;
	CString	m_OutputDir;
	int		m_nFileSize;
	//}}AFX_DATA

	virtual BOOL OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSplitDlg)
	
	afx_msg void OnBtnSelinputfile();
	afx_msg void OnBtnSeloutputdir();
	afx_msg void OnBtnStartsplit();	
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateStatus(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
	

	void EnableAllControls(BOOL bEnable);

	bool m_bIsWorking;	
	UINT	m_nFiles;	
	int		m_nOutputtype;
	CFileSplit *m_pFS;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SPLITDLG_H__

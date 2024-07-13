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

#ifndef __MERGE_DLG_H__
#define __MERGE_DLG_H__

#pragma once

#include "ResizableLib\ResizableDialog.h"
#include "FileMerge.h"


#define CButtonST CButton

class CMergeDlg : public CResizableDialog
{
// Construction
public:
	CMergeDlg(const TCHAR *initial_name, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMergeDlg)
	enum { IDD = IDD_MERGE };
	CButtonST	m_DirSel;
	CButtonST	m_FileAdd;
	CButtonST m_FileDel;
	CButtonST	m_FileDown;
	CButtonST	m_FileUp;
	CProgressCtrl	m_Progress;
	CButtonST	m_SelFiles;
	CListCtrl	m_FileList;
	CString	m_OutFile;
	CString m_InitialName;
	//}}AFX_DATA


	CString m_OutDirName, m_OutFileName;
	CFileMerge *m_pFM;
	void FillWith (const TCHAR *name);
 	virtual BOOL OnInitDialog();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bIsWorking;
	void EnableAllControls(bool bEnable);
	// Generated message map functions
	//{{AFX_MSG(CMergeDlg)
	afx_msg void OnBtnSelfiles();
	afx_msg void OnBtnStartmerge();
	afx_msg void OnBtnFileup();

	afx_msg void OnBtnFiledown();
	afx_msg void OnBtnFiledel();
	afx_msg void OnBtnFileadd();
	afx_msg void OnBtnSeldir();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateStatus(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __MERGE_DLG_H__

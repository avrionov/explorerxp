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

#ifndef __COPYMOVEDLG_H__
#define __COPYMOVEDLG_H__


#pragma once

#include "ResizableLib\ResizableDialog.h"
#include "DriveTree.h"

/////////////////////////////////////////////////////////////////////////////
// CCopyMoveDlg dialog

class CCopyMoveDlg : public CResizableDialog
{
// Construction
public:
	CCopyMoveDlg(const TCHAR *Title, const TCHAR *current_path, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCopyMoveDlg)
	enum { IDD = IDD_COPYMOVE };
	CListCtrl	m_List;
	CDriveTree	m_Folders;
	//}}AFX_DATA
	void LoadPaths();
	void SavePaths ();
	CString GetPath() { return m_NewPath;}
	void RemoveDups ();
	virtual BOOL OnInitDialog();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCopyMoveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCopyMoveDlg)

	virtual void OnOK();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CString m_Title;
	CString m_Path;	
	CString m_NewPath;
	CStringAr m_Paths;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __COPYMOVEDLG_H__

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

#ifndef _MULTIRENAMEDLG_H__
#define _MULTIRENAMEDLG_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMultiRenameDlg dialog

#include "ResizableLib\ResizableDialog.h"
#include "MenuButton.h"
#include "RenaneReplace.h"
#include "ReNumberDlg.h"
#include "RenTrimDlg.h"
#include "RenameInsert.h"

#include "globals.h"

class CMultiRenameDlg : public CResizableDialog
{
// Construction
public:
	void SetNewNames();
	CMultiRenameDlg (CSelRowArray &ar, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMultiRenameDlg)
	enum { IDD = IDD_MULTIRENAME };
	CListCtrl	m_List;
	CMenuButton	m_Convert;
	CMenuButton	m_Case;
	CString	m_Replace;
	CString	m_Find;
	CSelRowArray m_ArNew;
	//}}AFX_DATA

	CRenaneReplace m_ReplaceDlg;
	CReNumberDlg m_ReNumberDlg;
	CRenameInsert m_RenameInsert;

	CDialog m_CaseDlg;
	CRenTrimDlg m_TrimDlg;
	int m_Mode;
	CDialog *m_pCurrent;
	std::vector<CDialog *> m_Dialogs;
	void SwitchTo (int i);
	
	virtual BOOL OnInitDialog();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiRenameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMultiRenameDlg)	
	afx_msg void OnDoubleclickedCase();
	afx_msg void OnDoubleclickedConvert();
	afx_msg void OnChangeFind();
	afx_msg void OnChangeReplace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CSelRowArray &m_Ar;
	void DoReplace ();
public:
	CStatic m_Holder;
	CListBox m_ModeList;
	afx_msg void OnLbnSelchangeList();
	LPARAM OnChildUpdate (WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedBtnFileup();
	afx_msg void OnBnClickedBtnFiledown();
	afx_msg void OnBnClickedBtnFiledel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #if _MSC_VER > 1000

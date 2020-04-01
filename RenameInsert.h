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


#pragma once
#include "afxcmn.h"


// CRenameInsert dialog

class CRenameInsert : public CDialog
{
	DECLARE_DYNAMIC(CRenameInsert)

public:
	CRenameInsert(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRenameInsert();

// Dialog Data
	enum { IDD = IDD_REN_INSERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bRight;
	CString m_Text;
	CSpinButtonCtrl m_Spin;
	boolean m_bInit;
	afx_msg void OnEnChangeWhat();
	afx_msg void OnEnChangeEdit1();
	virtual BOOL OnInitDialog();
	int m_Pos;
	afx_msg void OnBnClickedFromrigh();
};

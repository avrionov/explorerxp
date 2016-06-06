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

#pragma once
#include "afxcmn.h"

class CRenTrimDlg : public CDialog
{
	DECLARE_DYNAMIC(CRenTrimDlg)

public:
	CRenTrimDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRenTrimDlg();

// Dialog Data
	enum { IDD = IDD_REN_TRIM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	int m_Position;
	int m_Count;
	int m_Left;
	bool m_bInit;
	afx_msg void OnEnChangePos();
	afx_msg void OnEnChangeCount();
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spinPos;
	CSpinButtonCtrl m_spinCount;
	BOOL m_bFromRight;
	afx_msg void OnBnClickedFromrigh();
};

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

class CReNumberDlg : public CDialog
{
	DECLARE_DYNAMIC(CReNumberDlg)

public:
	CReNumberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReNumberDlg();
	bool m_bInit;

// Dialog Data
	enum { IDD = IDD_REN_COUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int m_Digits;
	int m_Start;
	int m_Step;
	CString m_Pattern;
	afx_msg void OnEnChangeStart();
	CSpinButtonCtrl m_spinStart;
	CSpinButtonCtrl m_spinStep;
	CSpinButtonCtrl m_spinDigit;
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	CComboBox m_Patterns;
	afx_msg void OnCbnSelchangePatterns();
	afx_msg void OnCbnEditchangePatterns();
	afx_msg void OnCbnEditupdatePatterns();
	afx_msg void OnCbnSelendokPatterns();
};

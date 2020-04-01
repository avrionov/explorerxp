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
#include "afxwin.h"


// CTabOptionsDlg dialog

class CTabOptionsDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CTabOptionsDlg)

public:
	CTabOptionsDlg();
	virtual ~CTabOptionsDlg();

// Dialog Data
	enum { IDD = IDD_OPTIONS_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bStripPath;
	BOOL m_bLimitChars;
	DWORD m_CharLimit;

	virtual BOOL OnApply();
	virtual void OnOK();
	void GetFromDlg();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLimit();
	afx_msg void OnEnChangeLimitchar();
	afx_msg void OnBnClickedStrip();
};

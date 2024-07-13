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

#pragma once

#include "ResizableLib\ResizableDialog.h"

class CRenaneReplace : public CResizableDialog
{
//	DECLARE_DYNAMIC(CRenaneReplace)

public:
	CRenaneReplace(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRenaneReplace();
	
// Dialog Data
	enum { IDD = IDD_REN_REPLACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_Find;
	CString m_Replace;
	BOOL m_CaseSensentive;
	afx_msg void OnChange();
	afx_msg void OnBnClickedCaseSensetive();	

protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	BOOL m_ReplaceAll;
	afx_msg void OnBnClickedReplaceall();
};

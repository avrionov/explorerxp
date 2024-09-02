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

#include <afxext.h>

#include "resource.h"

class CPatternMatchBar :public CDialogBar
{
	// Construction
public:
	CPatternMatchBar(CWnd* pParent = NULL);   // standard constructor
	//void SetFolder(const TCHAR* folder);
	//void FocusOnAddress();
	//void ShowDropDown();
	// Dialog Data
		//{{AFX_DATA(CAddressBar)
	enum { IDD = IDR_PATTERNMATCH };
	//CComboBoxFolder	m_Address;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressBar)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressBar)
	afx_msg LRESULT OnInitDialog(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
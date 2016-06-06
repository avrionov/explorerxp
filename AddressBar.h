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

#ifndef __ADDRESS_BAR_H__
#define __ADDRESS_BAR_H__


#pragma once

#include "ComboBoxFolder.h"

class CAddressBar : public CDialogBar
{
// Construction
public:
	CAddressBar(CWnd* pParent = NULL);   // standard constructor
	void SetFolder (const TCHAR *folder);
	void FocusOnAddress();
	void ShowDropDown ();
// Dialog Data
	//{{AFX_DATA(CAddressBar)
	enum { IDD = IDR_MAINFRAME };
	CComboBoxFolder	m_Address;
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
	afx_msg LONG OnInitDialog( UINT, LONG ); 
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDRESSBAR_H__21B0E1EF_BAC0_4A92_A162_9C2450C3B5F3__INCLUDED_)

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

class CHotKeyCtrlEx : public CHotKeyCtrl
{
// Construction
public:
	CHotKeyCtrlEx();

// Attributes
protected:
	WORD m_wInvalidComb;
	WORD m_wModifiers;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHotKeyCtrlEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHotKeyCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHotKeyCtrlEx)
	//}}AFX_MSG
	afx_msg LRESULT OnSetRules(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
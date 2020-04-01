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


// CKeyboardPage dialog

#include "TreeCtrlEx.h"
#include "HotKeyCtrlEx.h"

class CKeyboardPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CKeyboardPage)

public:
	CKeyboardPage();
	virtual ~CKeyboardPage();

// Dialog Data
	enum { IDD = IDD_KEYBOARD };

	//CHotKeyCtrlEx	m_hkCur;	
	//CHotKeyCtrlEx	m_hkNew;

	CHotKeyCtrlEx	m_hkCur;	
	CHotKeyCtrlEx	m_hkNew;

	CString	m_sOtherCmdID;
	CMap<UINT, UINT, DWORD, DWORD&> m_mapID2Shortcut;
	CMap<DWORD, DWORD, HTREEITEM, HTREEITEM&> m_mapShortcut2HTI;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSelchangedShortcuts(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnAssignshortcut();	
	afx_msg void OnChangeShortcut();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrlEx m_tcCommands;
	virtual BOOL OnInitDialog();
	HTREEITEM AddMenuItem(HTREEITEM htiParent, const CMenu* pMenu, int nPos);
	virtual void OnOK();
	virtual void OnCancel();
};

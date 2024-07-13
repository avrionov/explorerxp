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

#include "Subclass.h"
#include <afxtempl.h>

// some more 'subtle' invalid modifiers
enum 
{
	HKCOMB_EXFKEYS		= 0x0100, // lets function keys thru
	HKCOMB_EDITCTRLS	= 0x0200 // prevents clashing shortcuts working when in an edit control
};

class CShortcutManager : protected CSubclassWnd  
{
public:
	CShortcutManager(BOOL bAutoSendCmds = TRUE);
	virtual ~CShortcutManager();

	// hooks AfxGetMainWnd() and only sends commands there
	BOOL Initialize(WORD wInvalidComb = HKCOMB_EDITCTRLS , WORD wFallbackModifiers = 0);
	UINT ProcessMessage(MSG* pMsg) const; // call this in PreTranslateMessage. returns the cmd ID or 0

	// AddShortcut fails if the shortcut is already being used
	BOOL AddShortcut(UINT nCmdID, WORD wVirtKeyCode, WORD wModifiers = HOTKEYF_CONTROL); 
	BOOL AddShortcut(UINT nCmdID, DWORD dwShortcut); 

	// SetShortcut never fails and will overwrite any existing shortcuts
	void SetShortcut(UINT nCmdID, WORD wVirtKeyCode, WORD wModifiers = HOTKEYF_CONTROL); 
	void SetShortcut(UINT nCmdID, DWORD dwShortcut); 

	DWORD GetShortcut(UINT nCmdID) const;
	WORD ValidateModifiers(WORD wModifiers, WORD wVirtKeyCode) const;

	static CString GetShortcutText(DWORD dwShortcut);
	CString GetShortcutTextByCmd(UINT nCmdID);

	int IsEmpty() { return (m_mapID2Shortcut.GetCount() == 0); }
	WORD GetInvalidComb() { return m_wInvalidComb; }
	BOOL LoadAccel (UINT idAccel);

protected:
	CMap<DWORD, DWORD, UINT, UINT&> m_mapShortcut2ID; // for use in ProcessMsg
	CMap<UINT, UINT, DWORD, DWORD&> m_mapID2Shortcut; // for use in PrepareMenuItems
	WORD m_wInvalidComb, m_wFallbackModifiers;
	BOOL m_bAutoSendCmds;

protected:
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	virtual void PreDetachWindow();

	void PrepareMenuItems(CMenu* pMenu) const;
	DWORD GetShortcut(WORD wVirtKeyCode, BOOL bExtended) const;
	void LoadSettings();
	void SaveSettings();

	//static CString GetKeyName(WORD wVirtKeyCode, BOOL bExtended = FALSE); 
	static CString GetKeyName(UINT wVirtKeyCode, BOOL bExtended = FALSE); 
};

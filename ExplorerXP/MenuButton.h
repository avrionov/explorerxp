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


#ifndef __MENU_BUTTON_H__
#define __MENU_BUTTON_H__

#pragma once

#define BCMenu CNewMenu

class CMenuButton : public CButton
{
// Construction
public:
	CMenuButton();
	int m_nIndex;
	int m_nCommand;
	DWORD m_MenuID;
	CMenu m_Menu;
	CMenu *m_pPopupMenu;
	bool m_bMouseOverButton;

// Attributes
public:

// Operations
public:    
	void SetIndex (int nIndex);
	void SetMenu (DWORD nID);
	int GetIndex () { return m_nIndex;}
	int GetCommand () { return m_nCommand;};
	bool	InRect();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMenuButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMenuButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMenuButton)
	afx_msg BOOL OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	void SetTextFromMenu ();
	DWORD DoSingleClick ();
	int PosFromID (DWORD nID);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //__MENU_BUTTON_H__
//
///EOF

void DDX_MenuButton (CDataExchange *pDX, int nIDC, CMenuButton &rCFEC);

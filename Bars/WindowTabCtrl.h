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

#ifndef __WINDOW_TAB_CONTROL_H__
#define __WINDOW_TAB_CONTROL_H__

#pragma once

class CViewManager;

#include "ExplorerXPView.h"
#include "TabDropTarget.h"

class CWindowTabCtrl : public CTabCtrl
{
private:
	BOOL m_bDisplayIcons;
// Construction
public:
	CWindowTabCtrl();

// Attributes
public:

// Operations
public:
	void SetFonts(CFont* pSelFont, CFont* pUnselFont);
	void SetFonts(CFont* pFont, BOOL bSelFont);
	void SetFonts(int nSelWeight = FW_SEMIBOLD, BOOL bSelItalic = FALSE,   
		BOOL bSelUnderline = FALSE, int nUnselWeight = FW_MEDIUM, 
		BOOL bUnselItalic = FALSE, BOOL bUnselUnderline = FALSE);

	void GetTabText (int index, CString &text);

	CFont* GetTabFont(BOOL bSelFont);


	void SetColors(COLORREF bSelColor, COLORREF bUnselColor, COLORREF rcDocModified);
	COLORREF GetSelColor() const;
	COLORREF GetUnSelColor() const;
	COLORREF GetDocModifiedColor() const;

	void SetIconDisplay(BOOL bDisplayIt = TRUE);
	BOOL GetIconDisplay() const;

	void SetTabStyle(BOOL bButtons, BOOL bFlatButtons);
//	DWORD GetTabStyle;
	
	 //virtual void OnBeginDrag();
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	CExplorerXPView *GetView(int iID);
	CMDIChildWnd *GetMDIChild(int iID);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowTabCtrl)
	public:
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
//	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWindowTabCtrl();

// Implementation
protected:
	COLORREF m_crSelColor, m_crUnselColor, m_crDocModified;
	CFont    m_SelFont,	   m_UnselFont;

	CViewManager*  m_pViewManager;      // ptr back to the ViewManager
	CTabDropTarget m_DropTarget;	
	CString	m_LastDropFolder;
	CString m_DropFolder;
	CSelRowArray m_DropFiles;

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowTabCtrl)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnGetObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWindowTabCtrl inlines

inline CFont* CWindowTabCtrl::GetTabFont(BOOL bSelFont)
{
	if (bSelFont)
		return &m_SelFont;
	else
		return &m_UnselFont;
}

inline COLORREF CWindowTabCtrl::GetSelColor() const
{
	return m_crSelColor;
}

inline COLORREF CWindowTabCtrl::GetUnSelColor() const
{
	return m_crUnselColor;
}

inline COLORREF CWindowTabCtrl::GetDocModifiedColor() const
{
	return m_crDocModified;
}

inline BOOL CWindowTabCtrl::GetIconDisplay() const
{
	return m_bDisplayIcons;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __WINDOW_TAB_CONTROL_H__

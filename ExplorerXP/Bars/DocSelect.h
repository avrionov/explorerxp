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

#ifndef __DOC_SELECT_H__
#define __DOC_SELECT_H__

#pragma once

#include "menubutton.h"
#include "WindowTabCtrl.h"
#include "globals.h"
#include "HoverButton.h"


/////////////////////////////////////////////////////////////////////////////
// CDocSelector window
class CDocSelector : public CControlBar
{
// Construction
public:
	CDocSelector();

// Attributes
public:
	

// Operations
public:
	
	
	void				ResizeButtons(int NewSize);
	void				RemoveAll();
	
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);   
    
	void OnActivateView(const BOOL bActivate, CMDIChildWnd* pView);
	int  GetWindowNum();
	void SetViewName(const TCHAR* cs, CMDIChildWnd* pView);
//	void RemoveAll();
	void RemoveView(CMDIChildWnd* pView);
	void AddView(const TCHAR* csName, CMDIChildWnd* pView);

	CMDIChildWnd* GetTabInfo(int nTab, CString &name);
    bool SelectDocument (const TCHAR *document_name);
	void GetTabs (CStringAr &tabs);
	void reDraw ();
	CString MakeTabName (const TCHAR * name);
	void UpdateTabs ();
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocSelector)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocSelector();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocSelector)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
		afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk (UINT nFlags, CPoint pt);
	afx_msg void OnCloseTab();
	//}}AFX_MSG
	//afx_msg LRESULT OnDSPopup( WPARAM, LPARAM );	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnToolTip (NMHDR *pNotifyStruct, LRESULT* pResult); 

	int					m_iNextButtonStart;
	int					m_iButtonWidth;
	int					m_iSelectedButton;
    CWindowTabCtrl      m_ViewTabCtrl;
	CHoverButton		m_HoverClose;
    int                 m_nLMargin;
	int					m_TabHeight;
    
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCSELECTOR_H__C789E26C_DA4B_11D2_BF44_006008085F93__INCLUDED_)

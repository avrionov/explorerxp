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

#ifndef __ACLIST_WND_H__
#define __ACLIST_WND_H__

#pragma once

#define ENAC_UPDATE        (WM_USER + 1200)

#define IDTimerInstall 10

#include "globals.h"

class CACListWnd : public CWnd
{
public:
	CACListWnd();
	void Init(CWnd *pWnd);
	bool EnsureVisible(int item,bool m_bWait);
	bool SelectItem(int item);
	int FindString(int nStartAfter, LPCTSTR lpszString );
	int FindStringExact( int nIndexStart, LPCTSTR lpszFind );
	int SelectString(LPCTSTR lpszString );
	bool GetText(int item, CString& m_Text);
	void AddSearchString(LPCTSTR lpszString);
	void RemoveAll(){m_SearchList.clear(); m_DisplayList.clear();}
	CString GetString();
	CString GetNextString(int m_iChar);

	
	void SortSearchList(){SortList(m_SearchList);}
// Attribute
public:
	CListCtrl m_List;
	CString m_DisplayStr;
	TCHAR m_PrefixChar;

public:
	CStringAr m_SearchList;

	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CACListWnd)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CACListWnd();
	void DrawItem(CDC* pDC,long m_lItem,long width);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CACListWnd)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	#if _MSC_VER >= 1400
		afx_msg LRESULT OnNcHitTest(CPoint point);
	#else
		afx_msg UINT OnNcHitTest(CPoint point);
	#endif 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CStringAr m_DisplayList;
	CScrollBar m_VertBar, m_HoriBar;
	CRect m_LastSize, m_ParentRect;
	CFont *pFontDC;
	CFont fontDC, boldFontDC;
	CEdit *m_pEditParent;
	LOGFONT logfont;

	int m_nIDTimer;
	long m_lTopIndex,m_lCount,m_ItemHeight,m_VisibleItems,m_lSelItem;

	int HitTest(CPoint point);
	void SetScroller();
	void SetProp();
	long ScrollBarWidth();
	void InvalidateAndScroll();
	void SortList(CStringAr& m_List);	
};

//{{AFX_INSERT_LOCATION}}
#endif //

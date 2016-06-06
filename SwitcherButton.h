#if !defined(AFX_SWITCHERBUTTON_H__91B9D502_D8DB_11D2_9FDF_D79776C64A53__INCLUDED_)
#define AFX_SWITCHERBUTTON_H__91B9D502_D8DB_11D2_9FDF_D79776C64A53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SwitcherButton.h
//
// Programmed by: JIMMY BRUSH (Kathy007@email.msn.com)
// 
// Legal:
//
// THIS CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
// You may use and distribute this code provided that you do not
// remove this title header and that you do not charge money for
// it. If you want to update the code, feel free to do so, as long
// as you *mark your changes* in code AND in the revision log below
// (and send it to me ;)

#define SWITCHBUTTON_UP			0
#define SWITCHBUTTON_DOWN		1
#define SWITCHBUTTON_SELECTED	2

/////////////////////////////////////////////////////////////////////////////
// CSwitcherButton window

class CSwitcherButton : public CWnd
{
// Construction
public:
	CSwitcherButton();
	virtual ~CSwitcherButton();

// Attributes
public:
	// Yogesh Jagota
	BOOL			m_bLBtnDown;
	BOOL			m_bPainted;
	CWnd*			m_AttachedView;
	CFrameWnd*		m_FrameWnd;
	CToolTipCtrl	m_ToolTip;
	// End Yogesh Jagota

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwitcherButton)
    virtual BOOL PreTranslateMessage (MSG * pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	CFont	m_fNormal;
//	CFont	m_fBold; // Yogesh Jagota
	CWnd*	m_wndParent;
	HICON	m_iIcon;
	UINT	m_nState;

	int		m_iID;
	void	Unselect();
	void	Select();
	void	Refresh();
	void	ReplaceIcon(HICON icon);
	void	SetText(const char * text);
	bool	HasCapture;
	BOOL	DoCreate(CWnd *parent, int x, int y, int cx, int cy, 
		CString sFileName, CString sPath ); // Yogesh Jagota

	// Generated message map functions
protected:
	//{{AFX_MSG(CSwitcherButton)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWITCHERBUTTON_H__91B9D502_D8DB_11D2_9FDF_D79776C64A53__INCLUDED_)

#ifndef _MEMDC_H_
#define _MEMDC_H_

//////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// This class implements a memory Device Context

class CMemDC2 : public CDC {
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;
public:
	CMemDC2(CDC* pDC, const CRect& rcBounds) : CDC()
	{
		CreateCompatibleDC(pDC);
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.Width(), rcBounds.Height());
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rcBounds = rcBounds;
	}
	~CMemDC2() 
	{
		m_pDC->BitBlt(m_rcBounds.left, m_rcBounds.top, m_rcBounds.Width(), m_rcBounds.Height(), 
					this, m_rcBounds.left, m_rcBounds.top, SRCCOPY);
		SelectObject(m_oldBitmap);
		if (m_bitmap != NULL) delete m_bitmap;
	}
	CMemDC2* operator->() {
		return this;
	}
};

#endif
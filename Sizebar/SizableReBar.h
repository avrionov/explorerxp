/////////////////////////////////////////////////////////////////////////////
// SizableReBar.h: interface for the CSizableReBar class.
//
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 by Nikolay Denisov. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Please email bug reports, bug fixes, enhancements, requests and
// comments to: nick@actor.ru
/////////////////////////////////////////////////////////////////////////////

#ifndef __SIZABLEREBAR_H__
#define __SIZABLEREBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "GlobalData.h"

/////////////////////////////////////////////////////////////////////////////
// CSizableReBar

class CSizableReBar : public CControlBar
{
    
// Construction
public:
    CSizableReBar();
    ~CSizableReBar();

// Operations
public:
    bool Create( CWnd* pParentWnd, UINT nID, DWORD dwStyle );
    bool AddBar( CWnd* pBar, LPCTSTR pszText = 0, CBitmap* pbmp = 0,
        DWORD dwStyle = RBBS_GRIPPERALWAYS | RBBS_FIXEDBMP,
        LPCTSTR lpszTitle = 0, bool bAlwaysVisible = true );
    bool AddBar( CWnd* pBar, COLORREF clrFore, COLORREF clrBack, LPCTSTR pszText = 0,
        DWORD dwStyle = RBBS_GRIPPERALWAYS,
        LPCTSTR lpszTitle = 0, bool bAlwaysVisible = true );
    bool SetBkImage( CBitmap* pbmp );

    void EnableContextMenu( bool bCommonPart = true, bool bCustomPart = true );

    void LoadState( LPCTSTR lpszProfileName );
    void SaveState( LPCTSTR lpszProfileName );

    CReBarCtrl& GetReBarCtrl() const;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSizableReBar)
    public:
    virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
    protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL

    virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHandler );

// Implementation
protected:
    bool PostAddBar( CWnd* pBar, LPCTSTR lpszTitle, bool bAlwaysVisible );
    UINT GetSizingEdgeHitCode() const;
    bool GetSizingEdgeRect( const CRect& rcBar, CRect& rcEdge, UINT nHitTest ) const;
    void StartTracking( UINT nHitTest, CPoint pt );
    void StopTracking();
    void OnTrackUpdateSize( CPoint pt );

// Implementation data
protected:
    CReBar          m_wndReBar;
    UINT            m_htEdge;
    CSize           m_szCurrent;
    CSize           m_szOld;
    CPoint          m_ptOld;
    bool            m_bTracking;
    int             m_cxEdge;
    int             m_cyEdge;
    HBITMAP         m_hbmBack;
    bool            m_bCommonPart;
    bool            m_bCustomPart;

    struct BARINFO
    {
        UINT    nID;
        TCHAR   szTitle[ MAX_PATH ];
        bool    bAlwaysVisible;
    };

    CArray< BARINFO, BARINFO& > m_aBars;

    static LPCTSTR m_lpszStateInfoFormat;
    static LPCTSTR m_lpszStateInfoEntry;

// Generated message map functions
protected:
    //{{AFX_MSG(CSizableReBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnNcPaint();
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
    #if _MSC_VER >= 1400
		afx_msg LRESULT OnNcHitTest(CPoint point);
	#else
		afx_msg UINT OnNcHitTest(CPoint point);
	#endif 
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP();
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__SIZABLEREBAR_H__

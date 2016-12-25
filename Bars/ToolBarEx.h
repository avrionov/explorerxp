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

#ifndef __TOOLBAREX_H__
#define __TOOLBAREX_H__


#pragma once

#include "tgabitmap.h"

//#include "CustomizeDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CToolBarEx

struct TBBUTTONEX
{
    TBBUTTON tbinfo;
    bool     bInitiallyVisible;
};

//const CSize szImageSmall( 16, 16 );
//const CSize szImageLarge( 32, 32 );

const CSize szImageSmall( 16, 16 );
const CSize szImageLarge( 24, 25 );

enum ETextOptions
{
    toTextLabels    =  0,
    toTextOnRight   =  1,
    toNoTextLabels  =  2,
    toNone          = -1
};

enum EIconOptions
{
    ioSmallIcons    =  0,
    ioLargeIcons    =  1,
    ioNone          = -1
};

class CToolBarEx : public CToolBar
{
    friend class CToolBarPopup;
    friend class CCustomizeDialog;

// Construction
public:
    CToolBarEx();
    ~CToolBarEx();

    BOOL Create( CWnd* pParentWnd, DWORD dwStyle, UINT nID = AFX_IDW_TOOLBAR );

// Attributes
public:
    void SetTextOptions( ETextOptions eTextOptions, bool bUpdate = true );
    ETextOptions GetTextOptions() const;
    void SetIconOptions( EIconOptions eIconOptions, bool bUpdate = true );
    EIconOptions GetIconOptions() const;

// Operations
public:
    void SetBitmaps( UINT nIDSmallCold, UINT nIDSmallHot, UINT nIDSmallDisabled,
                     UINT nIDLargeCold, UINT nIDLargeHot, UINT nIDLargeDisabled,
                     EIconOptions eIconOptionsDefault,
                     COLORREF clrMask = RGB( 255, 0, 255 ) );
    void SetButtons( int nNumButtons, TBBUTTONEX* lpButtons,
                     ETextOptions eTextOptionsDefault );

    void LoadState( LPCTSTR lpszProfileName );
    void SaveState( LPCTSTR lpszProfileName );
	void ChangeButtonText (int nID, const TCHAR *text);
	void ChangeButtonImage (int nID, int nImage);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CToolBarEx)
    protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL

   // virtual LRESULT DoCustomDraw( NMHDR* pNMHDR, CWnd* pToolBar );

    virtual bool HasButtonText( UINT nID );
    virtual bool HasButtonTip( UINT nID );
    virtual void GetButtonText( UINT nID, CString& strText );
    virtual void GetButtonTip( UINT nID, CString& strTip );
	virtual bool PassThroughTip (UINT nID);

public:
    virtual void Init();
    virtual bool IsTextOptionAvailable( ETextOptions eTextOptions ) const;
    virtual bool IsIconOptionAvailable( EIconOptions eIconOptions ) const;

	void Customize ();

// Implementation
protected:
    CReBarCtrl& GetParentReBarCtrl() const;
    int GetParentBandIndex() const;
    void ReloadButtons();
    void UpdateParentBandInfo();
    bool GetButtonInfo( UINT nID, TBBUTTON& tbinfo );

    static LRESULT CALLBACK CBTProc( int nCode, WPARAM wParam, LPARAM lParam );

// Implementation data
protected:
    CArray< TBBUTTONEX, TBBUTTONEX& > m_aButtons;

    ETextOptions    m_eTextOptions;
    EIconOptions    m_eIconOptions;
    ETextOptions    m_eTextOptionsDefault;
    EIconOptions    m_eIconOptionsDefault;
    UINT            m_nIDSmallCold;
    UINT            m_nIDSmallHot;
    UINT            m_nIDLargeCold;
    UINT            m_nIDLargeHot;
    UINT            m_nIDSmallDisabled;
    UINT            m_nIDLargeDisabled;
    CImageList      m_imageListCold;
    CImageList      m_imageListHot;
    CImageList      m_imageListDisabled;
    COLORREF        m_clrMask;

    static LPCTSTR              m_lpszStateInfoEntry;
    static CToolBarEx*          m_pToolBar;
    static CCustomizeDialog*    m_pCustomizeDlg;
    static HHOOK                m_hCBTHook;

	TGABitmap					m_tgaCold;

// Generated message map functions
protected:
    //{{AFX_MSG(CToolBarEx)
    afx_msg void OnCustomize();
    //}}AFX_MSG

    afx_msg LRESULT OnReBarContextMenu( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnReBarChevronPushed( WPARAM wParam, LPARAM lParam );

    afx_msg void OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg BOOL OnGetInfoTip( NMHDR* pNMHDR, LRESULT* pResult );

    // Toolbar customization
    afx_msg void OnBeginAdjust( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnEndAdjust( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnQueryInsert( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnQueryDelete( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnInitCustomize( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnGetButtonInfo( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnReset( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnToolBarChange( NMHDR* pNMHDR, LRESULT* pResult );

    // Saving and restoring toolbar
    afx_msg void OnSave( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnRestore( NMHDR* pNMHDR, LRESULT* pResult );
    DECLARE_MESSAGE_MAP();
};

/////////////////////////////////////////////////////////////////////////////
// CToolBarPopup

class CToolBarPopup : public CWnd
{
   // Construction
public:
    CToolBarPopup( CToolBarEx* pToolBar );
    ~CToolBarPopup();

// Operations
public:
    bool ShowPopup( UINT nFlags, CPoint pt, CRect& rcExclude );

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CToolBarPopup)
    protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// Implementation
protected:
    void SetPosition( UINT nFlags, CPoint pt, CRect& rcExclude );
    void Show( UINT nFlags, const CRect& rc );
    void OnKeyDown( UINT nChar );

    static LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam );

// Implementation data
protected:
    CToolBarEx*     m_pToolBar;
    CToolBarCtrl    m_tbCtrl;
    bool            m_bOverTbCtrl;
    bool            m_bTextLabels;

    static HHOOK            m_hKeyboardHook;
    static CToolBarPopup*   m_pPopup;

// Generated message map functions
protected:
    //{{AFX_MSG(CToolBarPopup)
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    //}}AFX_MSG
    afx_msg void OnEndModalLoop();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__MENUBAR_H__

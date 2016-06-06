/////////////////////////////////////////////////////////////////////////////
// SizableReBar.cpp: implementation of the CSizableReBar class.
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

#include "stdafx.h"
#include "SizeBarInclude.h"
#include "SizableReBar.h"
//#include "ToolBarEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSizableReBar

LPCTSTR CSizableReBar::m_lpszStateInfoFormat = _T("wID=%04X,cx=%d,fStyle=%08X");
LPCTSTR CSizableReBar::m_lpszStateInfoEntry  = _T("RebarStateInfo (v1.01)");

//IMPLEMENT_DYNAMIC( CSizableReBar, CControlBar )

CSizableReBar::CSizableReBar()
{
    m_szCurrent   = CSize( 0, 0 );
    m_bTracking   = false;
    m_cxEdge      = 3;
    m_cyEdge      = 3;
    m_hbmBack     = 0;
    m_bCommonPart = true;
    m_bCustomPart = true;
}

CSizableReBar::~CSizableReBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// Operations

bool CSizableReBar::Create( CWnd* pParentWnd, UINT nID, DWORD dwStyle )
{
    ASSERT_VALID( pParentWnd );     // must have a parent
    ASSERT( !( ( dwStyle & CBRS_SIZE_FIXED ) && ( dwStyle & CBRS_SIZE_DYNAMIC ) ) );

    m_dwStyle = ( dwStyle & CBRS_ALL );    // save the control bar styles

    // Register and create the window - skip CControlBar::Create()
    CString strWndClass = ::AfxRegisterWndClass( CS_DBLCLKS,
        ::LoadCursor( 0, IDC_ARROW ), ::GetSysColorBrush( COLOR_BTNFACE ), 0 );

    dwStyle &= ~CBRS_ALL;       // keep only the generic window styles
    dwStyle |= WS_CLIPCHILDREN; // prevents flashing

    if ( !CWnd::Create( strWndClass, 0, dwStyle, CRect( 0, 0, 0, 0 ), pParentWnd, nID ) )
    {
        return false;
    }

    return true;
}

bool CSizableReBar::AddBar( CWnd* pBar, LPCTSTR pszText, CBitmap* pbmp,
                            DWORD dwStyle, LPCTSTR lpszTitle, bool bAlwaysVisible )
{
    if ( !m_wndReBar.AddBar( pBar, pszText, pbmp, dwStyle ) )
    {
        return false;
    }

    return PostAddBar( pBar, lpszTitle, bAlwaysVisible );
}

bool CSizableReBar::AddBar( CWnd* pBar, COLORREF clrFore, COLORREF clrBack, LPCTSTR pszText,
                            DWORD dwStyle, LPCTSTR lpszTitle, bool bAlwaysVisible )
{
    if ( !m_wndReBar.AddBar( pBar, clrFore, clrBack, pszText, dwStyle ) )
    {
        return false;
    }

    return PostAddBar( pBar, lpszTitle, bAlwaysVisible );
}

bool CSizableReBar::SetBkImage( CBitmap* pbmp )
{
    m_hbmBack = *pbmp;

    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof( rbbi );
    rbbi.fMask  = RBBIM_BACKGROUND;

    CReBarCtrl& rbCtrl = m_wndReBar.GetReBarCtrl();
    for ( UINT nBand = 0; nBand < rbCtrl.GetBandCount(); nBand++ )
    {
        rbbi.hbmBack = m_hbmBack;
        VERIFY( rbCtrl.SetBandInfo( nBand, &rbbi ) );
    }

    return true;
}

void CSizableReBar::EnableContextMenu( bool bCommonPart /*=true*/, bool bCustomPart /*=true*/ )
{
    m_bCommonPart = bCommonPart;
    m_bCustomPart = bCustomPart;
}

void CSizableReBar::LoadState( LPCTSTR lpszProfileName )
{
    // This function restores index, width and style from the registry for
    // each band in the rebar.

    CString strValue = AfxGetApp()->GetProfileString( lpszProfileName,
        m_lpszStateInfoEntry );
    if ( !strValue.IsEmpty() )
    {
        REBARBANDINFO rbbi;
        rbbi.cbSize = sizeof( rbbi );
        rbbi.fMask  = RBBIM_STYLE | RBBIM_SIZE | RBBIM_ID;

        CReBarCtrl& rbCtrl = m_wndReBar.GetReBarCtrl();
        for ( UINT nBand = 0; nBand < rbCtrl.GetBandCount(); nBand++ )
        {
            CString strBandState;
            VERIFY( AfxExtractSubString( strBandState, strValue, nBand, _T('\n') ) );

            UINT nID, cx, nStyle;
            int nResult = _stscanf( strBandState, m_lpszStateInfoFormat, &nID, &cx, &nStyle );
            ASSERT( nResult == 3 );

            rbCtrl.MoveBand( rbCtrl.IDToIndex( nID ), nBand );
            VERIFY( rbCtrl.GetBandInfo( nBand, &rbbi ) );
            rbbi.cx     = cx;
            rbbi.fStyle = ( rbbi.fStyle & ~( RBBS_HIDDEN | RBBS_BREAK ) ) | nStyle;
            VERIFY( rbCtrl.SetBandInfo( nBand, &rbbi ) );
        }
    }
}

void CSizableReBar::SaveState( LPCTSTR lpszProfileName )
{
    // This function saves index, width and style in the registry for each
    // band in the rebar, so that it could be possible to restore all these
    // settings when the user runs the program next time.

    CString strValue;

    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof( rbbi );
    rbbi.fMask  = RBBIM_STYLE | RBBIM_SIZE | RBBIM_ID;

    CReBarCtrl& rbCtrl = m_wndReBar.GetReBarCtrl();
    for ( UINT nBand = 0; nBand < rbCtrl.GetBandCount(); nBand++ )
    {
        VERIFY( rbCtrl.GetBandInfo( nBand, &rbbi ) );

        CString strBandState;
        strBandState.Format( m_lpszStateInfoFormat, rbbi.wID, rbbi.cx, rbbi.fStyle );
        strValue += ( strValue.IsEmpty() ? EMPTYSTR : _T("\n") ) + strBandState;
    }

    VERIFY( AfxGetApp()->WriteProfileString( lpszProfileName,
        m_lpszStateInfoEntry, strValue ) );
}

CReBarCtrl& CSizableReBar::GetReBarCtrl() const
{
    return m_wndReBar.GetReBarCtrl();
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

bool CSizableReBar::PostAddBar( CWnd* pBar, LPCTSTR lpszTitle, bool bAlwaysVisible )
{
    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof( rbbi );
    rbbi.fMask  = RBBIM_CHILD | RBBIM_STYLE;

    CReBarCtrl& rbCtrl = m_wndReBar.GetReBarCtrl();
    for ( UINT nBand = 0; nBand < rbCtrl.GetBandCount(); nBand++ )
    {
        VERIFY( rbCtrl.GetBandInfo( nBand, &rbbi ) );
        if ( rbbi.hwndChild == pBar->GetSafeHwnd() )
        {
            UINT nID = ( UINT )pBar->GetDlgCtrlID();
            ASSERT( nID != 0 );
            ASSERT( rbCtrl.IDToIndex( nID ) == -1 );    // ID must be unique!

            // Save bar info for context menu
            BARINFO barinfo;
            barinfo.nID            = nID;
            barinfo.bAlwaysVisible = bAlwaysVisible;
            _tcsncpy( barinfo.szTitle, lpszTitle ? lpszTitle : EMPTYSTR, MAX_PATH );
            m_aBars.Add( barinfo );

            // To allow easy finding of parent band index,
            // we set band's ID to dialog control ID of child window.
            rbbi.fMask   = RBBIM_ID | RBBIM_BACKGROUND;
            rbbi.wID     = nID;
            rbbi.hbmBack = m_hbmBack;

            //if (/* pBar->IsKindOf( RUNTIME_CLASS( CMenuBar ) ) ||*/
            //     pBar->IsKindOf( RUNTIME_CLASS( CToolBarEx ) ) )
            //{
            //    rbbi.fMask  |= RBBIM_STYLE;
            //    rbbi.fStyle |= RBBS_USECHEVRON;
            //}

            VERIFY( rbCtrl.SetBandInfo( nBand, &rbbi ) );
            return true;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
// Overrides

CSize CSizableReBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
/*    ASSERT( bStretch ); // the bar is stretched (is not the child of a dockbar)*/

    m_szCurrent = m_wndReBar.CalcFixedLayout( bStretch, bHorz );

    switch ( GetSizingEdgeHitCode() )
    {
        case HTLEFT:
        case HTRIGHT:
            if ( m_szCurrent.cx > 0 )
            {
                m_szCurrent.cx += m_cxEdge;
            }
            break;
        case HTTOP:
        case HTBOTTOM:
            if ( m_szCurrent.cy > 0 )
            {
                m_szCurrent.cy += m_cyEdge;
            }
            break;
        default:
            ASSERT( false );
            break;
    }

    return m_szCurrent;
}

CSize CSizableReBar::CalcDynamicLayout( int /*nLength*/, DWORD dwMode )
{
    return CalcFixedLayout( dwMode & LM_STRETCH, dwMode & LM_HORZ );
}

BOOL CSizableReBar::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
    if ( wParam == AFX_IDW_REBAR )
    {
        REBARBANDINFO rbbi;
        rbbi.cbSize = sizeof( rbbi );
        rbbi.fMask  = RBBIM_CHILD;

        // Let child windows handle certain messages
        NMHDR* pNMHDR = ( NMHDR* )lParam;
        switch ( pNMHDR->code )
        {
            case RBN_CHILDSIZE:
            {
                NMREBARCHILDSIZE* lprbcs = ( NMREBARCHILDSIZE* )pNMHDR;
                VERIFY( m_wndReBar.GetReBarCtrl().GetBandInfo( lprbcs->uBand, &rbbi ) );

                ::SendMessage( rbbi.hwndChild, WM_REBAR_CHILDSIZE,
                    ( WPARAM )&lprbcs->rcBand,
                    ( LPARAM )&lprbcs->rcChild );
                break;
            }
            case RBN_CHEVRONPUSHED:
            {
                NMREBARCHEVRON* lpnm = ( NMREBARCHEVRON* )pNMHDR;
                VERIFY( m_wndReBar.GetReBarCtrl().GetBandInfo( lpnm->uBand, &rbbi ) );

                CRect rcChevron( lpnm->rc );
                m_wndReBar.ClientToScreen( rcChevron );
                ::SendMessage( rbbi.hwndChild, WM_REBAR_CHEVRONPUSHED,
                    ( WPARAM )( LPCRECT )rcChevron, lpnm->lParamNM );
                break;
            }
            default:
                break;
        }

        // Forward notifications from the re-bar control to owner/parent
        *pResult = GetParent()->SendMessage( WM_NOTIFY, wParam, lParam );
        return TRUE;
    }

    return FALSE;
}

void CSizableReBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHandler )
{
    UpdateDialogControls( pTarget, bDisableIfNoHandler );
}

/////////////////////////////////////////////////////////////////////////
// CSizableReBar message handlers

BEGIN_MESSAGE_MAP( CSizableReBar, CControlBar )
    //{{AFX_MSG_MAP(CSizableReBar)
    ON_WM_CREATE()
    ON_WM_NCPAINT()
    ON_WM_NCCALCSIZE()
    ON_WM_NCHITTEST()
    ON_WM_CAPTURECHANGED()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_NCLBUTTONDOWN()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_WM_INITMENUPOPUP()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
// CSizableReBar message handlers

int CSizableReBar::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( CControlBar::OnCreate( lpCreateStruct ) == -1 )
    {
        return -1;
    }

    if ( !m_wndReBar.Create( this,
            RBS_BANDBORDERS | RBS_VARHEIGHT | RBS_DBLCLKTOGGLE,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
            m_dwStyle | CBRS_BORDER_ANY | CBRS_BORDER_3D ) )
    {
        return -1;
    }

    m_wndReBar.SetOwner( GetOwner() );
    return 0;
}

void CSizableReBar::OnNcLButtonDown( UINT nHitTest, CPoint point )
{
    if ( !m_bTracking )
    {
        if ( ( nHitTest >= HTSIZEFIRST ) && ( nHitTest <= HTSIZELAST ) )
        {
            StartTracking( nHitTest, point );
        }
    }
}

void CSizableReBar::OnMouseMove( UINT nFlags, CPoint point )
{
    if ( m_bTracking )
    {
        OnTrackUpdateSize( point );
    }

    CControlBar::OnMouseMove( nFlags, point );
}

void CSizableReBar::OnLButtonUp( UINT nFlags, CPoint point )
{
    if ( m_bTracking )
    {
        StopTracking();
    }

    CControlBar::OnLButtonUp( nFlags, point );
}

void CSizableReBar::OnCaptureChanged( CWnd *pWnd )
{
    if ( m_bTracking && ( pWnd != this ) )
    {
        StopTracking();
    }

    CControlBar::OnCaptureChanged( pWnd );
}

void CSizableReBar::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp )
{
    CControlBar::OnNcCalcSize( bCalcValidRects, lpncsp );

    CRect rcWindow( lpncsp->rgrc[ 0 ] );
    CRect rcClient( rcWindow );
    CRect rcEdge;
    if ( GetSizingEdgeRect( rcWindow, rcEdge, GetSizingEdgeHitCode() ) )
    {
        if ( rcClient.SubtractRect( rcWindow, rcEdge ) )
        {
            lpncsp->rgrc[ 0 ] = rcClient;
        }
    }
}

void CSizableReBar::OnNcPaint()
{
    EraseNonClient();
}

#if _MSC_VER >= 1400
LRESULT CSizableReBar::OnNcHitTest(CPoint point)
#else
UINT CSizableReBar::OnNcHitTest(CPoint point)
#endif 
{
    CRect rcWindow;
    GetWindowRect( rcWindow );

    CRect rcEdge;
    UINT nHitTest = GetSizingEdgeHitCode();
    if ( GetSizingEdgeRect( rcWindow, rcEdge, nHitTest ) && rcEdge.PtInRect( point ) )
    {
        return nHitTest;
    }

    return HTCLIENT;
}

void CSizableReBar::OnSize( UINT nType, int cx, int cy )
{
    CControlBar::OnSize( nType, cx, cy );

    CRect rcClient;
    GetClientRect( rcClient );
    m_wndReBar.MoveWindow( rcClient );
}

void CSizableReBar::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{
    if ( !m_bCommonPart && !m_bCustomPart )
    {
        return;
    }

    CMenu menu;
    VERIFY( menu.CreatePopupMenu() );

    CReBarCtrl& rbCtrl = m_wndReBar.GetReBarCtrl();
    int nBand;

    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof( rbbi );

    // Populate menu by adding titles of the bars that can be shown/hidden
    if ( m_bCommonPart )
    {
        for ( int nIndex = 0; nIndex <= m_aBars.GetUpperBound(); nIndex++ )
        {
            if ( _tcslen( m_aBars[ nIndex ].szTitle ) > 0 )
            {
                nBand = rbCtrl.IDToIndex( m_aBars[ nIndex ].nID );
                if ( nBand != -1 )
                {
                    rbbi.fMask = RBBIM_STYLE | RBBIM_CHILD;
                    VERIFY( rbCtrl.GetBandInfo( nBand, &rbbi ) );

                    UINT nFlags = 0;
                    if ( m_aBars[ nIndex ].bAlwaysVisible )
                    {
                        nFlags |= MF_GRAYED;
                    }
                    if ( !( rbbi.fStyle & RBBS_HIDDEN ) )
                    {
                        nFlags |= MF_CHECKED;
                    }

                    VERIFY( menu.AppendMenu( MF_BYCOMMAND | nFlags,
                        m_aBars[ nIndex ].nID, m_aBars[ nIndex ].szTitle ) );
                }
            }
        }
    }

    // Populate menu by adding items specific to the bar that was clicked
    HWND hwndBar = 0;
    if ( m_bCustomPart )
    {
        RBHITTESTINFO rbhti;
        rbhti.pt = point;
        rbCtrl.ScreenToClient( &rbhti.pt );

        nBand = rbCtrl.HitTest( &rbhti );
        if ( nBand != -1 )
        {
            rbbi.fMask = RBBIM_CHILD;
            VERIFY( rbCtrl.GetBandInfo( nBand, &rbbi ) );
            hwndBar = rbbi.hwndChild;

            UINT nBars = menu.GetMenuItemCount();
            ::SendMessage( hwndBar, WM_REBAR_CONTEXTMENU, ( WPARAM )&menu, 0 );
            if ( 0 < nBars && nBars < menu.GetMenuItemCount() )
            {
                VERIFY( menu.InsertMenu( nBars, MF_BYPOSITION | MF_SEPARATOR ) );
            }
        }
    }

    // Track menu
/*    CWinAppEx::GetInstance()->SetMenuIcons( &menu, true );
    UINT nID = menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
        point.x, point.y, GetParentFrame() );
    nBand = rbCtrl.IDToIndex( nID );
    if ( nBand != -1 )  // was it "Show/Hide Bar" command?
    {
        rbbi.fMask = RBBIM_STYLE;
        VERIFY( rbCtrl.GetBandInfo( nBand, &rbbi ) );
        VERIFY( rbCtrl.ShowBand( nBand, ( rbbi.fStyle & RBBS_HIDDEN ) != 0 ) );
    }
    else if ( hwndBar != 0 )
    {
        // It was bar-specific command, so forward it to the bar
        ::SendMessage( hwndBar, WM_COMMAND, MAKEWPARAM( nID, 0 ), 0 );
    }

    VERIFY( menu.DestroyMenu() );*/
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

UINT CSizableReBar::GetSizingEdgeHitCode() const
{
    if ( m_dwStyle & CBRS_ALIGN_LEFT )
    {
        return HTRIGHT;
    }
    if ( m_dwStyle & CBRS_ALIGN_TOP )
    {
        return HTBOTTOM;
    }
    if ( m_dwStyle & CBRS_ALIGN_RIGHT )
    {
        return HTLEFT;
    }
    if ( m_dwStyle & CBRS_ALIGN_BOTTOM )
    {
        return HTTOP;
    }

    return HTNOWHERE;
}

bool CSizableReBar::GetSizingEdgeRect( const CRect& rcBar, CRect& rcEdge, UINT nHitTest ) const
{
    rcEdge = rcBar;

    switch ( nHitTest )
    {
        case HTLEFT:
            rcEdge.left = rcEdge.right - m_cxEdge;
            break;
        case HTBOTTOM:
            rcEdge.top = rcEdge.bottom - m_cyEdge;
            break;
        case HTRIGHT:
            rcEdge.right = rcEdge.left + m_cxEdge;
            break;
        case HTTOP:
            rcEdge.bottom = rcEdge.top + m_cyEdge;
            break;
        default:
            return false;
    }

    return true;
}

void CSizableReBar::StartTracking( UINT nHitTest, CPoint pt )
{
    SetCapture();

    // Make sure no updates are pending
    VERIFY( RedrawWindow( 0, 0, RDW_ALLCHILDREN | RDW_UPDATENOW ) );

    m_htEdge    = nHitTest;
    m_szOld     = m_szCurrent - CSize( m_cxEdge, m_cyEdge );
    m_ptOld     = pt;
    m_bTracking = true;
}

void CSizableReBar::StopTracking()
{
    m_bTracking = false;
    ReleaseCapture();
    GetParentFrame()->DelayRecalcLayout();
}

void CSizableReBar::OnTrackUpdateSize( CPoint pt )
{
    ClientToScreen( &pt );

    CSize szDelta = pt - m_ptOld;
    CSize sizeNew = m_szOld;

    switch ( m_htEdge )
    {
        case HTLEFT:
            sizeNew.cx -= szDelta.cx;
            break;
        case HTTOP:
            sizeNew.cy -= szDelta.cy;
            break;
        case HTRIGHT:
            sizeNew.cx += szDelta.cx;
            break;
        case HTBOTTOM:
            sizeNew.cy += szDelta.cy;
            break;
    }

    if ( sizeNew != m_szOld )
    {
        m_szOld = sizeNew;
        m_ptOld = pt;

        // Try to re-size the re-bar control
        CRect rc( CPoint( 0, 0 ), sizeNew );
        if ( m_wndReBar.GetReBarCtrl().SizeToRect( rc ) )
        {
            GetParentFrame()->DelayRecalcLayout();
        }
    }
}

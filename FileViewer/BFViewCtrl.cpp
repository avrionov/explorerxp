/*

Name : CBFViewCtrl ( BigFile View Controller )
File : BFViewerCtrl.h

Author    : Mathias Svensson  ( ms@result42.com )

Copyright : Mathias Svensson , 2004

Version   : v1.1

Rights of Use : You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) when your product is released in binary form. 
                You are allowed to modify the source code in any way you want except you cannot modify the copyright details at the top of each module. 
                If you want to distribute source code with your application, then you are only allowed to distribute versions released by the author.

Disclaimer : There is no warranty. So use this code at your own risk. 

Restriction : Since this controller is almost an application in it self, 
              so you are not allowed to use this controller to create a 
              program where this controller is the main purpose of the 
              application (eg a text file view program). 
              A modified and highly improved version of it can be used 
              in freeware application as long as the author is mention in 
              the credits in the program (About dialog) and is informed about it. 
              All other kind of software is allowed to use it as they like.




BUGS / Limits
----
*  None Unicode Build is not able to view Unicode files correctly
*  Horizontal scrolling where there are tabs in the text bugs out sometimes.


History
---------
v1.0    8-Sep-2004     Public Release.

v1.1    9-Sep-2004     * PrepareReverse(...) could in some situation go into a unlimited loop and the program would crash.
                       * When viewing binary files as text the temporary string buffer could be overflowed so limit text string to 2048 bytes.
					   * reading illegal memory bug. Sometimes reading past memorymap buffer apperad and the program would crash. this is fixed as GetBufferEnd is not long return pointer to past end.. 
					   * Bug when parsing Unicode revers fixed.





*/



#include "stdafx.h"
#include "BFViewCtrl.h"
#include "Memdc.h"
#include "AFXPRIV.H" // USES_CONVERSION
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CBFVIEWCTRL_CLASSNAME    _T("CBFViewCtrl")  // Window class name

UINT ZGC_GetMouseScrollLines()
{
    int nScrollLines = 3;            // reasonable default

#ifndef _WIN32_WCE
    // Do things the hard way in win95
    OSVERSIONINFO VersionInfo;
    VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&VersionInfo) || 
        (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && VersionInfo.dwMinorVersion == 0))
    {
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Desktop"),
            0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
        {
            TCHAR szData[128];
            DWORD dwKeyDataType;
            DWORD dwDataBufSize = sizeof(szData);
            
            if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
                (LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
            {
                nScrollLines = _tcstoul(szData, NULL, 10);
            }
            RegCloseKey(hKey);
        }
    }
    // win98 or greater
    else
           SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
#endif

    return nScrollLines;
}

BOOL CDataBuffert::OpenFile( LPCTSTR strFilename )
{
	if( m_Memfile.IsOpen() )
	{
		m_Memfile.Close();
		m_Memfile.UnMap();
	}

	if( m_Memfile.MapFile( strFilename , FILE_SHARE_READ|FILE_SHARE_WRITE , 0 , m_dwMapSize ) )
	{
		m_strFilename = strFilename;
		m_pBuffert= (LPCBYTE)m_Memfile.Open( 10000 ); // 10s timeout..
		if( m_pBuffert == NULL )
		{
			m_Memfile.Close();
			m_Memfile.UnMap();
			m_dwBuffertSize = 0;
			m_nTotalSize = 0;
			m_nBuffertOffset = 0;
			return FALSE;
		}
		else
		{
			m_dwBuffertSize = m_Memfile.GetFileMapLength();
			m_nTotalSize = m_Memfile.GetTotalFileSize();		
			m_nBuffertOffset = m_Memfile.GetOffset();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDataBuffert::Reload( INT64 nOffset )
{
	if( m_Memfile.Reload( nOffset ) )
	{
		m_pBuffert = (LPCBYTE)m_Memfile.GetData();
		m_dwBuffertSize = m_Memfile.GetFileMapLength();
		m_nTotalSize = m_Memfile.GetTotalFileSize();		
		m_nBuffertOffset = m_Memfile.GetOffset();
		return TRUE;
	}
	ASSERT(0);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CZTextViewer

CBFViewerCtrl::CBFViewerCtrl()
{			
	m_bMemDC = TRUE;
	m_pFileData = 0;
	m_nFontSize = 10;
	RegisterWindowClass();
	m_nRowsPerWheelNotch = ZGC_GetMouseScrollLines();
	m_bOnlyWheelScrollIfMouseInsideWindow = FALSE;
	m_nViewMode = TEXTTYPE_ASCII;
	m_pDataHandler = NULL;

	m_bTrackSelection = FALSE;

	m_bAutoReload = FALSE;

	// is not a static text since I need to be able to change language at runtime.
	m_strTextSaveClip = _T("Selected text is to big for the clipboard!\rDo you want to save it to a file instead ?");
}

CBFViewerCtrl::~CBFViewerCtrl()
{
	if( m_pDataHandler )
		delete m_pDataHandler;
}


BEGIN_MESSAGE_MAP(CBFViewerCtrl, CWnd)
	//{{AFX_MSG_MAP(CBFViewerCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SIZING()
	ON_WM_DROPFILES()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CBFViewerCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, CBFVIEWCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = CBFVIEWCTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}


void  CBFViewerCtrl::SetReloadChkTimer( DWORD ms	)
{
	if( ms == 0 )
	{
		KillTimer( 1000 );
		m_bAutoReload = FALSE;
	}
	else
	{
		if( m_bAutoReload )
			KillTimer( 1000);
		SetTimer( 1000 , ms , NULL );
		m_bAutoReload = TRUE;
	}
}

BOOL CBFViewerCtrl::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CBFViewerCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_bMemDC)
    {
        CMemDC MemDC(&dc);
		CRect rect;
		GetClientRect(rect);
		if( m_pDataHandler )
			MemDC.FillSolidRect(rect, m_pDataHandler->GetColor(2) );
		else
			MemDC.FillSolidRect(rect, RGB(255,255,255) );
        Draw(&MemDC);
    }
    else
	{
		// Clear BG
		CRect rect;
		GetClientRect(rect);
		if( m_pDataHandler )
			dc.FillSolidRect(rect, m_pDataHandler->GetColor(2) );
		else
			dc.FillSolidRect(rect, RGB(255,255,255) );

        Draw(&dc);
	}
}

void CBFViewerCtrl::Draw(CDC* pDC)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	DWORD nCol = 0;
	if( GetScrollInfo(SB_HORZ, &si) )
		nCol = si.nPos;

	pDC->SelectObject( m_Font );
	if( m_pDataHandler )
		m_pDataHandler->Draw( pDC , nCol );
}

BOOL CBFViewerCtrl::OpenFile( CString &filename , short nMode , BOOL bRefresh )
{
	BOOL bAutoReload = m_bAutoReload;
	if( m_bAutoReload )
		m_bAutoReload = FALSE; // no timer will do nothing if it is called during the opening of a new file

	if( !m_Buffert.OpenFile( filename ) )
		return FALSE;

	m_strFilename = filename;
	
	if( nMode == TEXTTYPE_AUTO )
		nMode = IdentifyFileType();
	else
	{
		m_nViewMode = nMode;
//		m_Buffert.SetPrefixSize( 0 );
	}

	if( CreateDataHandler( nMode ) == NULL )
		return FALSE;

	m_pDataHandler->Set_LineHeight( m_lRowHeight );
	m_pDataHandler->Prepare( NULL , 0 , 0 );
	m_pDataHandler->SetScrollRange();
	m_pDataHandler->UpdateScrollInfo();

	if( bRefresh )
		InvalidateRect( NULL );	

	SendNotifyToParent( BFVN_OPEN );

	m_bAutoReload = bAutoReload;
	return TRUE;
}

CString CBFViewerCtrl::GetViewModeAsString()
{
	CString str;
	str = _T("Unknown");
	if( m_pDataHandler )
		str = m_pDataHandler->GetName();

	return str;
}

BOOL CBFViewerCtrl::ChangeViewMode( short nNewMode )
{
	if( m_Buffert.GetBufferSize() == 0 ) // no file open
		return FALSE;

	// Get TopLine Absolute position.
	__int64 nPos = m_pDataHandler->GetCurrentTopPos();
	if( nPos == -1 )
		return FALSE;

	// Close
	m_Buffert.Close();

	// Reopen in new Mode
    if( !OpenFile( m_strFilename , nNewMode , FALSE ) )
		return FALSE;
	
	// goto Position
	if( !m_pDataHandler->GoToPos( nPos ) )
		return FALSE;

    m_pDataHandler->SetScrollRange();
	m_pDataHandler->UpdateScrollInfo();
	InvalidateRect( NULL );	

	return TRUE;
}
BOOL  CBFViewerCtrl::SetSelection( __int64 nStart , __int64 nEnd )
{
	if( !m_pDataHandler )
		return FALSE;

	BOOL bRet = m_pDataHandler->SetSelection( nStart , nEnd );
	InvalidateRect( NULL );	
	return bRet;
}

BOOL CBFViewerCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= ( WS_HSCROLL | WS_VSCROLL | WS_BORDER ); // H_SCROLL is not yet supported
	return CWnd::PreCreateWindow(cs);
}

void CBFViewerCtrl::PreSubclassWindow()
{
	SetFont( _T("Courier New") , 10 );
	CWnd::PreSubclassWindow();
}

void CBFViewerCtrl::UpdateScollbars()
{
	if( m_pDataHandler == NULL )
		return;
	m_pDataHandler->UpdateScrollInfo();
}

void CBFViewerCtrl::OnHScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/) 
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	VERIFY(GetScrollInfo(SB_HORZ, &si));
	switch (nSBCode)
	{
	case SB_LEFT:
		si.nPos = si.nMin;
		break;
	case SB_RIGHT:
		si.nPos = si.nMax;
		break;
	case SB_LINELEFT:
		if( si.nPos > si.nMin  )
			si.nPos--;
		break;
	case SB_LINERIGHT:
		if( si.nPos < si.nMax  )
			si.nPos++;
		break;
	case SB_PAGELEFT:
		if( si.nPos >= 15 )
			si.nPos -= 15;
		else
			si.nPos = 0;
		break;
	case SB_PAGERIGHT:
		if( si.nPos + 15 < si.nMax )
			si.nPos += 15;
		else
			si.nPos = si.nMax;
		
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	default:
		return;
	}
	SetScrollInfo( SB_HORZ , &si );
	InvalidateRect( NULL );
	
//	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CBFViewerCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if( !GetScrollInfo(SB_VERT, &si) )
		return CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	switch (nSBCode)
	{
	case SB_TOP:
		Top();
		break;
	case SB_BOTTOM:
		Bottom();
		break;
	case SB_LINEUP:
		LineUp();
		break;
	case SB_LINEDOWN:
		LineDown();
		break;
	case SB_PAGEUP:
		PageUp();
		break;
	case SB_PAGEDOWN:
		PageDown();
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			if( si.nTrackPos >= 0 && m_pDataHandler )
			{	
				if( m_pDataHandler->MoveToScrollPos( si.nTrackPos ) )
					InvalidateRect( NULL );
			}
			break;
		}
	default:
		return;
	}
	UpdateScollbars();
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CBFViewerCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO
	//
	//  Select the complete word that user is double clicking on
	//
	//
	CWnd::OnLButtonDblClk(nFlags, point);
}
void CBFViewerCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();	
	
	if( m_pDataHandler )
	{
		SetCapture();
		m_bTrackSelection = TRUE;
		m_pDataHandler->StartTracking( point );
	}
		
	CWnd::OnLButtonDown(nFlags, point);
}

void CBFViewerCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	if( m_bTrackSelection )
	{
		if( m_pDataHandler )
			m_pDataHandler->StopTracking( point );
		InvalidateRect(NULL);
	}
	m_bTrackSelection = FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}

void CBFViewerCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bTrackSelection )
	{
		if( m_pDataHandler )
		{
			m_pDataHandler->Track( point );
			InvalidateRect(NULL);
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}


void CBFViewerCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	SetFocus();	
	
	if( nChar == VK_DOWN )
		LineDown();
	if( nChar == VK_UP )
		LineUp();
	if( nChar == VK_PRIOR )
		PageUp();
	if( nChar == VK_NEXT )
		PageDown();
	if( nChar == VK_HOME )
		Top();
	if( nChar == VK_END )
		Bottom();

	if( nChar == 16)
		return;

	int ctrl = GetKeyState(VK_CONTROL);
	if(ctrl & 0xf0 ) //control key down
	{
		switch( nChar )
		{
			
			case 65 : // ctrl + A
				SetSelection( 0 , -1 );
				break;

			case 67 : // ctrl + C and Ctrl + INSERT
			case VK_INSERT:
				DoCopy();
				return;
				break;	

			default: break;
		}

	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CBFViewerCtrl::DoCopy( BOOL bToFile )
{
	if( m_pDataHandler == NULL )
		return FALSE;

	LPCBYTE pStart = NULL;
	DWORD len = 0;

    __int64 nSize = m_pDataHandler->GetSelectionSize();
	if( nSize == 0 )
		return TRUE;
	
	len = m_pDataHandler->GetSelection( &pStart  );
	if( len == -1 || bToFile )
	{
        if( bToFile || AfxMessageBox( m_strTextSaveClip , MB_YESNO ) == IDYES )
		{
			// open SaveAs Dialog
			CFileDialog FileDlg( FALSE , _T("txt") , _T("Unknown.txt") );
			if( FileDlg.DoModal() == IDOK )
			{
				m_pDataHandler->GetSelection( FileDlg.GetPathName() );
			}
		}
		else
			return FALSE;
	}

	if( len > 0 )
	{
		if( m_nViewMode == TEXTTYPE_UNICODE )
			PutIntoClipboardW( (WCHAR*)pStart ,  len );
		else
			PutIntoClipboardA( pStart ,  len );
	}

	m_pDataHandler->TempCleanup();
	
	return TRUE;
}

BOOL CBFViewerCtrl::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg);
}

UINT CBFViewerCtrl::OnGetDlgCode() 
{
	return DLGC_WANTARROWS;   
}

void CBFViewerCtrl::LineDown()
{
	if( m_pDataHandler )
		if( m_pDataHandler->MoveLineDown() )
		{
			m_pDataHandler->UpdateScrollInfo();
			InvalidateRect(NULL);
		}

}

void CBFViewerCtrl::LineUp()
{
	if( m_pDataHandler )
		if( m_pDataHandler->MoveLineUp() )
		{
			m_pDataHandler->UpdateScrollInfo();
			InvalidateRect(NULL);
		}

}

void CBFViewerCtrl::PageDown()
{
	if( m_pDataHandler )
		if( m_pDataHandler->MovePageDown() )
		{
			m_pDataHandler->UpdateScrollInfo();
			InvalidateRect(NULL);
		}
}

void CBFViewerCtrl::PageUp()
{
	if( m_pDataHandler )
		if( m_pDataHandler->MovePageUp() )
		{
			m_pDataHandler->UpdateScrollInfo();
			InvalidateRect(NULL);
		}
}

void CBFViewerCtrl::Top()
{
	if( m_pDataHandler )
		if( m_pDataHandler->MoveToTop() )
		{
			m_pDataHandler->UpdateScrollInfo();
			InvalidateRect(NULL);
		}

}

void CBFViewerCtrl::Bottom()
{
	if( m_pDataHandler )
		if( m_pDataHandler->MoveToBottom() )
		{
			m_pDataHandler->UpdateScrollInfo();
			InvalidateRect(NULL);
		}
}

void CBFViewerCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if( m_pDataHandler )
		m_pDataHandler->OnSize(); 
	
}



int CBFViewerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DragAcceptFiles( TRUE );
	SetFont( _T("Courier New") , 10 );
	return 0;
}

void CBFViewerCtrl::CalcLineHeight()
{
	CClientDC dc(this);

	CFont * oldfont = dc.SelectObject(&m_Font);
	// Get the Average Char Width:
	TEXTMETRIC tm;
	dc.GetOutputTextMetrics (&tm) ;
//	int cxChar = tm.tmAveCharWidth ;
//	int cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
	m_lRowHeight = tm.tmHeight + tm.tmExternalLeading ;
	dc.SelectObject(oldfont);
}

void CBFViewerCtrl::SetFont( CFont* pFont , BOOL bRedraw )
{
	if( pFont )
	{
		LOGFONT lf;
		pFont->GetLogFont( &lf );
		m_Font.SetLogFont( lf );

		CalcLineHeight();
		if( m_pDataHandler )
			m_pDataHandler->Set_LineHeight( m_lRowHeight );

		if( bRedraw )
			InvalidateRect( NULL );
	}

}
void CBFViewerCtrl::SetFont( LPCTSTR  strFontName , int nFontSize )
{

	CClientDC dc(this);

	m_Font.SetFaceName( strFontName );
	m_nFontSize = nFontSize;

	long height = -MulDiv( m_nFontSize , GetDeviceCaps(dc , LOGPIXELSY ) , 72 );
	m_Font.SetHeight( height );


	CalcLineHeight();
	if( m_pDataHandler )
		m_pDataHandler->Set_LineHeight( m_lRowHeight );

	//CWnd::SetFont( &m_Font );
}

void CBFViewerCtrl::ShowFontDlg()
{
	DWORD dwColor=0;
	m_Font.GetFontFromDialog( NULL , &dwColor , NULL ,this );
	CalcLineHeight();
	if( m_pDataHandler )
		m_pDataHandler->Set_LineHeight( m_lRowHeight );
	
	InvalidateRect( NULL );

}


BOOL CBFViewerCtrl::Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle )
{
	return CWnd::Create( CBFVIEWCTRL_CLASSNAME , _T("") , dwStyle , rect , pParentWnd , nID );
}

void CBFViewerCtrl::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CWnd::OnSizing(fwSide, pRect);
	
}

BOOL CBFViewerCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( nHitTest == HTCLIENT )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CBFViewerCtrl::OnDropFiles(HDROP hDropInfo) 
{
	if ( hDropInfo ) 
	{
		UINT nFiles = DragQueryFile (hDropInfo, 0xFFFFFFFF, NULL, 0);
		if(nFiles >= 0 ) 
		{
			CString strFilename;
			UINT nLen = DragQueryFile (hDropInfo, 0 , NULL, 0);
			_TCHAR *pzsFN = strFilename.GetBufferSetLength (nLen+(2*sizeof(_TCHAR) ) );
			DragQueryFile (hDropInfo, 0, pzsFN, nLen+(2*sizeof(_TCHAR) ));
			strFilename.ReleaseBuffer();
			m_strFilename = strFilename;
		}
		DragFinish(hDropInfo);
	}


	OpenFile( m_strFilename );
}

BOOL CBFViewerCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( m_bOnlyWheelScrollIfMouseInsideWindow ) // i Know.. Stupid long name.. 
	{
		// Bugs Out in MultiMonitor
		CRect rcClient;
		ScreenToClient(&pt);
		GetClientRect( &rcClient );
		
		if( !rcClient.PtInRect(pt ) )
			return FALSE;
	}
	
    if (m_nRowsPerWheelNotch == -1)
    {
        int nPagesScrolled = zDelta / 120;

        if (nPagesScrolled > 0)
            for (int i = 0; i < nPagesScrolled; i++)
                PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
        else
            for (int i = 0; i > nPagesScrolled; i--)
                PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
    }
    else
    {
        int nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;

        if (nRowsScrolled > 0)
            for (int i = 0; i < nRowsScrolled; i++)
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
        else
            for (int i = 0; i > nRowsScrolled; i--)
                PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
    }
	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CBFViewerCtrl::IsSpace( int ch )
{
	if( (ch >= 0x09 && ch <= 0x0D) || ch == 0x20 )
		return TRUE;

	return FALSE;

}
BOOL CBFViewerCtrl::IsPrint( int ch )
{
	if( ch >= 0x20 && ch <= 0x7E )
		return TRUE;

	return FALSE;
}

BOOL CBFViewerCtrl::IsDataBinary( LPCBYTE pData , int iLength )
{
	int nCount_AscII = 0;
	int nCount_NoneAscII = 0;
	CHAR* pStart = (CHAR*)pData;
	for( int i = 0; i < iLength; i++ ,pStart++)
	{
		if( IsPrint( *pStart ) )
			nCount_AscII++;
		else
		{
			if( IsSpace( *pStart ) )
				nCount_AscII++;
			else
				nCount_NoneAscII++;
		}
			
	}
	// if more the 50% is NoneAscii then its binary
	return (nCount_NoneAscII > (int)(iLength * 0.50) ? TRUE : FALSE );
}

BOOL CBFViewerCtrl::IsDataUTF8( LPCBYTE /*pData*/ , int /*iLength*/ )
{
	// not supported yet
	return FALSE;
}

BOOL CBFViewerCtrl::PutIntoClipboardW( const WCHAR* strText , DWORD len )
{
#ifdef UNICODE
	if (strText == NULL || len == 0)
		return FALSE;

	CWaitCursor wc;
	BOOL bOK = FALSE;
	if (OpenClipboard())
	{
		EmptyClipboard();
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len + 2);
		if (hData != NULL)
		{
			WCHAR* pszData = (WCHAR*) ::GlobalLock(hData);
			wcsncpy(pszData, (const WCHAR*)strText,len/sizeof(WCHAR));
			GlobalUnlock(hData);
			bOK = SetClipboardData(CF_UNICODETEXT, hData) != NULL;
		}
		CloseClipboard();
	}
	return bOK;
#else
	return FALSE;
#endif

}

BOOL CBFViewerCtrl::PutIntoClipboardA( const unsigned char* strText , DWORD len )
{

	if (strText == NULL || len == 0)
		return FALSE;

	CWaitCursor wc;
	BOOL bOK = FALSE;
	if (OpenClipboard())
	{
		EmptyClipboard();
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len + 1);
		if (hData != NULL)
		{
			LPSTR pszData = (LPSTR) ::GlobalLock(hData);
			strncpy(pszData, (const char*)strText,len);
			GlobalUnlock(hData);
			bOK = SetClipboardData(CF_TEXT, hData) != NULL;
		}
		CloseClipboard();
	}
	return bOK;

}
short CBFViewerCtrl::IdentifyFileType()
{
	int iLength = 0;

	if( m_Buffert.GetBufferSize() > 500 )
		iLength = 500;
	else
		iLength = m_Buffert.GetBufferSize();

	LPCBYTE pData = m_Buffert.GetBuffer();
	if( pData == NULL )
		return -1;

	m_Buffert.SetPrefixSize( 0 );

	int iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE ;
	if( IsTextUnicode( pData , iLength , &iUniTest  ) )
	{
		if( iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE )
			m_nViewMode = TEXTTYPE_UNICODEBE;
		else
			m_nViewMode = TEXTTYPE_UNICODE;

		m_Buffert.SetPrefixSize( 2 );
		return m_nViewMode;
	}

	// is text UTF8 .. ( NOT SUPPORTED YET )
	if( IsDataUTF8( pData , iLength ) )
	{
		m_nViewMode = TEXTTYPE_UTF8;
		return m_nViewMode;
	}

	// check if Binary
	if( IsDataBinary( pData , iLength ) )
	{
		m_nViewMode = TEXTTYPE_BINARY;
		return m_nViewMode;
	}
	m_nViewMode = TEXTTYPE_ASCII;
	return m_nViewMode;
}
LRESULT CBFViewerCtrl::SendNotifyToParent( int nMsg )
{
	NMHDR hdr;
	hdr.code = nMsg;
	hdr.hwndFrom = m_hWnd;
	hdr.idFrom   = GetDlgCtrlID();
	
		
	CWnd* pOwner = GetOwner();
	if( pOwner && IsWindow( pOwner->m_hWnd) )
	{
		return pOwner->SendMessage( WM_NOTIFY ,hdr.idFrom , (LPARAM)&hdr );
	}
	return 0;
}

CDataHandler* CBFViewerCtrl::CreateDataHandler( int nMode )
{
	if( m_pDataHandler )
	{
		delete m_pDataHandler;
		m_pDataHandler = NULL;
	}

	switch( nMode )
	{
		case TEXTTYPE_ASCII   : m_pDataHandler = new CAsciiHandler( this , &m_Buffert  ); break;
		case TEXTTYPE_UNICODE : m_pDataHandler = new CUnicodeHandler( this , &m_Buffert); break;
		case TEXTTYPE_BINARY  : m_pDataHandler = new CBinaryHandler( this , &m_Buffert ); break;
	}
	if( m_pDataHandler )
		m_pDataHandler->SetFont( &m_Font );

	return m_pDataHandler;
}


void CBFViewerCtrl::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == 1000 && m_bAutoReload )
	{
		if( m_Buffert.IsFileChanged() )
		{
            if( m_pDataHandler->Reload() )
			{
				m_pDataHandler->SetScrollRange();
				UpdateScollbars();
				InvalidateRect( NULL );
				SendNotifyToParent( BFVN_OPEN );
			}
		}
	}

	CWnd::OnTimer(nIDEvent);
}
void CBFViewerCtrl::Print( CPrintDialog* /*pPrntDlg*/ )
{
	AfxMessageBox( _T("Not Implemented yet!") );
}

void CBFViewerCtrl::SetColor( DWORD crTextColor , DWORD crBGColor , DWORD crSelTextColor , DWORD crSelBGColor , BOOL bRedraw )
{
	CDataHandler* pData = GetDataHandler();
	if( pData )
	{
		pData->SetColor( crTextColor , crBGColor , crSelTextColor , crSelBGColor );
		if( bRedraw )
			InvalidateRect( NULL );
	}
}



////=======================================================================================================================================

CDataHandler::CDataHandler( CWnd* pWnd , CDataBuffert* pBuffert ) : m_pWnd( pWnd ) , m_pBuffert( pBuffert )
{
	m_nTopLine			= 0;
	m_nLinesAPage		= 0;
	m_nEndIndex			= 0;
	m_nStartIndex		= 0;

	m_nLeftMargin		= 5;
	m_nSnap				= 5;

	m_nSelectionStart   = 0;
	m_nSelectionEnd     = 0;

	m_pTempBuffert		= NULL;
	
	m_dwVertScrollRes = 100;

	m_crTextColor          = RGB(0,0,0);	   // Black
	m_crBGColor            = RGB(255,255,255); // White

	m_crSelectionTextColor = RGB(255,255,255); // White
	m_crSelectionBGColor   = RGB(0,0,0);       // Black

	ZeroMemory( &m_LineCache , sizeof(LineInfo)*MAXLINES );

}
void CDataHandler::SetColor( DWORD crTextColor , DWORD crBGColor , DWORD crSelTextColor , DWORD crSelBGColor )
{
	if( crTextColor != -1 )
		m_crTextColor = crTextColor;

	if( crBGColor != -1 )
		m_crBGColor = crBGColor;

	if( crSelTextColor != -1 )
		m_crSelectionTextColor = crSelTextColor;

	if( crSelBGColor != -1 )
		m_crSelectionBGColor = crSelBGColor;
}

DWORD CDataHandler::GetColor( int nColor )
{
	switch( nColor )
	{
		case 1 : return m_crTextColor; break;
		case 2 : return m_crBGColor; break;

		case 3 : return m_crSelectionTextColor; break;
		case 4 : return m_crSelectionBGColor;   break;
	}
	return (DWORD)-1;
}

int CDataHandler::FindLine( LPVOID pData )
{
	for( DWORD x = m_nStartIndex ; x <= m_nEndIndex ; x++ )
	{
		if( m_LineCache[x].pStart == pData )
			return x;
	}
	return -1;
}

BOOL CDataHandler::SetScrollRange()
{
	if( m_pWnd == NULL )
		return FALSE;

	m_pWnd->SetScrollRange( SB_HORZ , 0 , 150 );


	if( GetLinesInIndex() <= GetLinesAPage() )
	{
		if( m_LineCache[ m_nEndIndex ].sFlags & LIF_LASTLINE && m_LineCache[ m_nTopLine ].sFlags & LIF_FIRSTLINE )
			m_dwVertScrollRes = 0;
		else
		{
			if( m_pBuffert->GetDataLength() > 10*(1024*1024) ) // more then 10MB
				m_dwVertScrollRes = 10000;
			else 
				m_dwVertScrollRes = 1000;
		}
	}
	else
	{
		if( m_pBuffert->GetDataLength() > 10*(1024*1024) ) // more then 10MB
			m_dwVertScrollRes = 10000;
		else 
			m_dwVertScrollRes = 1000;
	}

	m_pWnd->SetScrollRange( SB_VERT , 0 , m_dwVertScrollRes  ); 

	return TRUE;
}

BOOL CDataHandler::UpdateScrollInfo()
{
	if( m_pWnd == NULL )
		return FALSE;

	if( m_nStartIndex == m_nEndIndex )
		return FALSE;

	int nPos = 0;

	DWORD nLastShowedLine = m_nTopLine + (m_nLinesAPage-1);
	if( nLastShowedLine >= m_nEndIndex )
	{
		nLastShowedLine = m_nEndIndex; 
	}

	double dPos = 0;
	__int64 nDataPos = 0;

	if( m_LineCache[ m_nTopLine ].sFlags & LIF_FIRSTLINE )
		nPos = 0;
	else  if(m_nTopLine + (m_nLinesAPage - 1)  >= m_nEndIndex && m_LineCache[ m_nEndIndex ].sFlags & LIF_LASTLINE )
		nPos = m_dwVertScrollRes; 
	else
	{

		LPCBYTE pLine = m_LineCache[ nLastShowedLine ].pStart;
		nDataPos = m_pBuffert->GetAbsolutePos( pLine );

		// check if nPos is First or Last half of the total length.. 
		if( nDataPos < (m_pBuffert->GetDataLength() / 2) )
		{
			nDataPos = m_pBuffert->GetAbsolutePos( m_LineCache[ m_nTopLine ].pStart );
		}
		else
			nDataPos += m_LineCache[ nLastShowedLine ].dwLength; 

		if( nDataPos == 0 ) // Can't Divied with 0,
			nDataPos = 1;

		if( nDataPos > 0xFFFFFFFFFFFFF ) 
		{
			// don't know if this is correct.. but can't use the alternative below since double can't handle that big values..
			// and double is needed. or the first ( Length / nDataPos ) might return 1 instead of 1.0023244 that is need for the other stuff to do its work correctly
			dPos = (double)m_dwVertScrollRes / ( (double)m_pBuffert->GetDataLength()/10000) / (double)(nDataPos/10000);
		}
		else
			dPos = (double)m_dwVertScrollRes / ((double)m_pBuffert->GetDataLength() / (double)nDataPos);
		dPos += 0.5;
		nPos = (int)(dPos);
	}

	if( (DWORD)nPos > m_dwVertScrollRes )
		nPos = m_dwVertScrollRes;

	SCROLLINFO si;
	si.cbSize = sizeof( SCROLLINFO );
	si.fMask = SIF_POS;
	si.nPos = nPos; //(char*)pStart - (char*)m_pFileData;
	m_pWnd->SetScrollInfo(SB_VERT, &si);

	return TRUE;

}


void   CDataHandler::StartTracking( CPoint& pt )
{
	LPCBYTE pPos = GetDataPostion( pt );
	if( pPos )
	{
		m_nSelectionStart = m_nSelectionEnd = m_pBuffert->GetAbsolutePos( pPos );
	}

//	m_pSelectionEnd = m_pSelectionStart = NULL;
}
void   CDataHandler::StopTracking( CPoint& pt )
{
	LPCBYTE pPos = GetDataPostion( pt );
	if( pPos == NULL )
		return;

	__int64 nPos = m_pBuffert->GetAbsolutePos( pPos );
	
	if( (nPos == m_nSelectionStart && m_nSelectionStart == m_nSelectionEnd) || m_nSelectionEnd == 0 )
	{
		m_nSelectionStart = 0;
		m_nSelectionEnd = 0;
	}
}

void CDataHandler::Track( CPoint& pt )
{
	LPCBYTE pPos = GetDataPostion( pt );
	if( pPos == NULL )
		return;

	__int64 nPos = m_pBuffert->GetAbsolutePos( pPos );

	if( nPos > m_nSelectionStart )
	{
		m_nSelectionEnd = nPos;
	}
	else if( nPos < m_nSelectionStart )
	{
		
		if( m_nSelectionEnd == NULL )
			m_nSelectionEnd = m_nSelectionStart;
		m_nSelectionStart = nPos;
		
	}

	if( m_nSelectionEnd == m_nSelectionStart )
		m_nSelectionEnd = m_nSelectionEnd = NULL;

}

LPCBYTE CDataHandler::GetDataPostion( CPoint& pt )
{
	if( m_pWnd == NULL )
		return NULL;
	if( pt.y < 0)
		pt.y = 0;

	// Get Row
	int RowIdx = (pt.y / m_dwLineHeight); 
	RowIdx +=m_nTopLine;
	m_pWnd->GetDC();

	// Get Horizont Position of row
	LPCBYTE pStart = GetHDataPosition( RowIdx , pt.x );
	return pStart;
}



LPCBYTE CUnicodeHandler::GetHDataPosition( int rowidx , int x )
{
	USES_CONVERSION;
	
	CClientDC dc( m_pWnd );
	dc.SelectObject( m_pFont );
	

	DWORD lStringLen = m_LineCache[rowidx].dwLength/sizeof(WCHAR);
	CSize sz;

	int LastCharPos = 0;
	for( DWORD n = 1; n <= lStringLen ; n++ )
	{
		if( m_LineCache[rowidx].pStart != NULL )
		{
			sz = dc.GetTabbedTextExtent( CW2CT( (WCHAR*)m_LineCache[rowidx].pStart ) , n , 0 , NULL );
			sz.cx += m_nLeftMargin;
			if( sz.cx > x+m_nSnap )
			{
				return (LPCBYTE) ((WCHAR*)m_LineCache[rowidx].pStart + LastCharPos);
			}
			LastCharPos=n;
		}
	}
	return (LPCBYTE)((WCHAR*)m_LineCache[rowidx].pStart + lStringLen);
}
BOOL CDataHandler::hasSelection()
{
	if(  m_nSelectionEnd != 0 && m_nSelectionStart >= 0 )
		return TRUE;

	return FALSE;
}
BOOL CDataHandler::lineHasSelection( LPCBYTE pLineStart , LPCBYTE pLineEnd )
{
	__int64 nStartPos = m_pBuffert->GetAbsolutePos( pLineStart );
	__int64 nEndPos = m_pBuffert->GetAbsolutePos( pLineEnd );

	if( m_nSelectionEnd > nStartPos && m_nSelectionStart <= nEndPos) 
	{
		return TRUE;
	}
	return FALSE;
}

BOOL  CDataHandler::SetSelection( __int64 nStart , __int64 nEnd )
{
	__int64 nMaxLen = m_pBuffert->GetDataLength();
	if( nStart >= 0 && nStart < nMaxLen )
		m_nSelectionStart = nStart;
	else
	{
		m_nSelectionEnd = 0;
		m_nSelectionStart = 0;
		return FALSE;
	}


	if( nEnd == -1 )
		m_nSelectionEnd = nMaxLen;
	else if( nEnd <= nMaxLen )
		m_nSelectionEnd = nEnd;
	else
	{
		m_nSelectionEnd = 0;
		m_nSelectionStart = 0;
		return FALSE;
	}

	return TRUE;
}

DWORD CDataHandler::GetSelection( LPCBYTE* pStart )
{

	if( m_nSelectionEnd == 0 )
		return 0;

	BOOL bInRange = IsSelectionInBuffer();
	if( bInRange )
	{
        *pStart = m_pBuffert->GetPointerToPos( m_nSelectionStart );
		return (DWORD)GetSelectionSize(); // cast to DWORD becouse a selection that is in range can't be that big so it require a int64
	}
	else
	{
		__int64 nSize = GetSelectionSize();
		if( nSize > MAX_CLIPBOARD_SIZE )
			return (DWORD)-1; // TO BIG..  ask to Save it to a file instead.

		if( m_pTempBuffert )
			delete[] m_pTempBuffert;

		m_pTempBuffert = new BYTE[ (DWORD)nSize ];
		if( m_pTempBuffert == NULL )
			return 0; // Out of memory ..


        if( CopyToBuffert( m_pTempBuffert , m_pBuffert->GetFileName() , 64 , m_nSelectionStart , nSize ) )
		{
			*pStart = m_pTempBuffert;
			return (DWORD)nSize;
		}

		return 0;
		

	}


	return 0;
}
BOOL CDataHandler::GetSelection( LPCTSTR strFilename )
{
	if( m_pTempBuffert )
		delete[] m_pTempBuffert;

	__int64 nSize = GetSelectionSize();
	if( nSize == 0 )
		return FALSE;

	if( CopyToFile( strFilename , m_pBuffert->GetFileName() , 64 , m_nSelectionStart , nSize ) )
		return TRUE;

	return FALSE;
}
BOOL CDataHandler::RearrangeCache_Back( int nCount )
{
	ASSERT( nCount > 0 );
	ASSERT( m_nStartIndex < MAXLINES );
	// free up nCount solts in linecache in the front.. so move everything back..
	if( m_nStartIndex >= m_nEndIndex )
	{
		// no Rows in Cache..
		return TRUE;
	}

	// +1 for Index to Count Conversion
	DWORD nToMove = (m_nEndIndex - m_nStartIndex)+1;
	if( (nToMove + 1 + nCount) > MAXLINES )
		nToMove = MAXLINES - nCount;

	MoveMemory( &m_LineCache[nCount] , &m_LineCache[0] , sizeof(LineInfo)*(nToMove) );
	ZeroMemory( &m_LineCache[0] , sizeof(LineInfo)*nCount );
	m_nTopLine += nCount;
	ASSERT( (m_nStartIndex  + nCount) < MAXLINES );
	m_nStartIndex += nCount;

	m_nEndIndex += nCount;
	if( m_nEndIndex >= MAXLINES )
		m_nEndIndex = MAXLINES-1;

	return TRUE;
}

BOOL CDataHandler::RearrangeCache_Front( )
{
	DWORD nCount = (m_nEndIndex - m_nTopLine)+1;
	MoveMemory( &m_LineCache[0] , &m_LineCache[m_nTopLine] , sizeof(LineInfo)*nCount );
	ZeroMemory( &m_LineCache[nCount] , sizeof(LineInfo)*(MAXLINES-nCount) );
	m_nTopLine = 0;
	m_nStartIndex = 0;
	m_nEndIndex = nCount - 1;
	return TRUE;
}
void CDataHandler::ClearCache()
{
	ZeroMemory( &m_LineCache , sizeof(LineInfo)*MAXLINES );
	m_nTopLine			= 0;
	m_nEndIndex			= 0;
	m_nStartIndex		= 0;
}

//  Start at pData and scan backward.  and DO not Scan past m_pBuffer->GetBuffer()
//
//
BOOL CDataHandler::ParseDataReverse( LPCBYTE pData , DWORD dwLines )
{
	if( pData == NULL )
		return FALSE;

	LPCBYTE pBegin = m_pBuffert->GetBuffer();
	LPCBYTE pEnd = m_pBuffert->GetBufferEnd( ByteSize() );
	LPCBYTE pLastLine = pData;
	LPCBYTE pStartLine = NULL;
//	LPCBYTE pEndLine = NULL;

	if( !RearrangeCache_Back( dwLines ) )
	{
		ASSERT(0);
		return FALSE;
	}

	if( m_nStartIndex == 0 && m_nEndIndex == 0 )
	{
		// empty cache..
		m_nStartIndex = dwLines;
		m_nEndIndex = dwLines-1;
	}
	
	short nByteSize = ByteSize();
	DWORD dwLength=0;
	for( int n = dwLines ; n > 0 ; n-- )
	{
		if( pData < pEnd ) // if equal to pEnd. then don't do it
			pData -= nByteSize; // since it is pointing to the beging of a line. we need to point it someplace else.
		if( pData > pBegin )
		{
			pStartLine = ParseLineReverse( pData ,  dwLength );
			if( pStartLine == (LPCBYTE)-1 )
			{
				return -1;
			}

			if( pStartLine )
			{
				m_nStartIndex--;
				ASSERT( m_nStartIndex >= 0 && m_nStartIndex < MAXLINES );

				if( dwLength == 0 && pStartLine > pData )
					pStartLine--;

				m_LineCache[ m_nStartIndex ].pStart = pStartLine;
				m_LineCache[ m_nStartIndex ].pEnd = pLastLine;
				m_LineCache[ m_nStartIndex ].dwLength = dwLength;

				if( pStartLine == pBegin )
				{
					m_LineCache[ m_nStartIndex ].sFlags = LIF_FIRSTLINE_CHUNK;
					if( m_pBuffert->GetDataOffset() == 0 )
						m_LineCache[ m_nStartIndex ].sFlags |= LIF_FIRSTLINE;
				}
				else if( pEnd == pData )
				{
					m_LineCache[ m_nStartIndex ].sFlags = LIF_LASTLINE_CHUNK;
					if( m_pBuffert->GetDataOffset() + m_pBuffert->GetBufferSize() == m_pBuffert->GetDataLength() )
						m_LineCache[ m_nStartIndex ].sFlags |= LIF_LASTLINE;
					m_LineCache[ m_nStartIndex ].pEnd = NULL;
				}

				pLastLine = pStartLine;
				pData = pStartLine;
				ASSERT( pData );
					
			}
		}
		else
		{
			// pData is before beging and offset is not 0. so we need a remap
			if( m_pBuffert->GetDataOffset() > 0 )
				return -1;
		}
	}

	return TRUE;
}

int CDataHandler::ParseData( LPCBYTE pData , DWORD nStartIndex , DWORD nLines )
{
	LPCBYTE pEnd = m_pBuffert->GetBufferEnd( ByteSize() );
	LPCBYTE pStart = m_pBuffert->GetBuffer();

	// check if nStartIndex + nLines fits in Cache.. if not invalite cache
	int nStopIndex = nStartIndex + nLines;
	if( (DWORD)(nStartIndex + nLines + 1) > GetMaxCacheCount() )
	{
		// All the lines will not fit in The Cache. so rearrange it.. cut first 1/2 of it and move 
		RearrangeCache_Front();
		nStartIndex = 0;
		nStopIndex = nStartIndex + nLines;

	}
	
	LPCBYTE pLine = NULL;
	LPCBYTE pLineEnd = NULL;
	DWORD dwLength=0;

	for( int nIndex = nStartIndex ; nIndex <= nStopIndex && nIndex < MAXLINES ; nIndex++ )
	{
		if( m_LineCache[ nIndex ].pStart == NULL )
		{
			if( nIndex == 0 )
			{
				// Start with first Index and it is NULL. we need to start from the begining of the buffert
				pLine = pData;
			}
			else
			{
				pLine = m_LineCache[ nIndex-1 ].pEnd;
			}
			if( pLine == NULL )
				return -1;

			if( pLine >= pEnd )
				return nStartIndex;

			pLineEnd = ParseLine( pLine , dwLength );
			if( pLineEnd == ((LPCBYTE)-1 ) )
			{
				// reached end of buffer.  But not eof. remap and start again..
				return -1;
			}

			m_LineCache[ nIndex ].pStart = pLine;
			m_LineCache[ nIndex ].pEnd = pLineEnd;
			m_LineCache[ nIndex ].dwLength = dwLength;

			// Check pLine if same a m_pData. then this is the first line
			if( pLine == pStart )
			{
				m_LineCache[ nIndex ].sFlags = LIF_FIRSTLINE_CHUNK;
				if( m_pBuffert->GetDataOffset() == 0 )
					m_LineCache[ nIndex ].sFlags |= LIF_FIRSTLINE;
			}
			else if( pLineEnd >= pEnd )
			{
				if( pLineEnd > pEnd )
					pLineEnd = pEnd;

				m_LineCache[ nIndex ].sFlags = LIF_LASTLINE_CHUNK;
				if( m_pBuffert->GetDataOffset() + m_pBuffert->GetBufferSize() == m_pBuffert->GetDataLength() )
					m_LineCache[ nIndex ].sFlags |= LIF_LASTLINE;
				m_LineCache[ nIndex ].pEnd = NULL;
			}


			pLine = pLineEnd;
			m_nEndIndex = nIndex; // +1

			if( m_nStartIndex > nStartIndex )
				m_nStartIndex = nStartIndex;

			if( pEnd == pLineEnd )
				return nStartIndex;

		}
		
			

	}
	return nStartIndex;
}

int CDataHandler::PrepareReverse( LPCBYTE pLineStart , int nStartIndex , int nLines , BOOL bBreak )
{
	ASSERT( nLines );

	if( nStartIndex >= MAXLINES )// Just to make sure
		nStartIndex = MAXLINES-1;

	LPCBYTE pLine = NULL;
	if( pLineStart == NULL )
		pLine = m_LineCache[ nStartIndex ].pStart;
	else
		pLine = pLineStart;

	if( pLine )
	{
		if( ParseDataReverse( pLine , nLines ) == -1 )
		{
			if( pLine == NULL )
			{
				pLine = ReMapPrev( m_LineCache[ m_nTopLine + (nLines -1) ].pStart );
			}
			else
				pLine = ReMapPrev( pLine );

			if( pLine && bBreak == FALSE )
				PrepareReverse( pLine , 0 , nLines , TRUE ); // Ahh hope it does not go into unlimited loop here
			else
				return -1;
		}
	}
	return nStartIndex;

}
int CDataHandler::Prepare( LPCBYTE pLineStart , int nStartIndex , int nLines )
{
	if( nLines == 0 )
		nLines = m_nLinesAPage;
	ASSERT( nLines );

	LPCBYTE pLine = NULL;
	if( pLineStart )
		pLine = pLineStart;
	else
	{
		if( m_nEndIndex == 0 )
			pLine = m_pBuffert->GetBuffer();
		else 
			pLine = NULL;
        
	}
	// check if there is cached data in nStartIndex + nLines.
	if( (DWORD)(nStartIndex + (nLines -1 )) > m_nEndIndex )
	{
		if( ParseData( pLine , nStartIndex , nLines ) == -1 )
		{
			if( pLine == NULL )
				pLine = m_LineCache[m_nStartIndex ].pStart;

			LPCBYTE pTopLine = ReMapNext( pLine , nStartIndex );
			if( pTopLine == NULL )
			{
				ClearCache();
				if( m_pBuffert->ReMap( m_pBuffert->GetDataOffset() ) )
				{
					if( ParseData( m_pBuffert->GetBuffer() , 0 , m_nLinesAPage ) == -1 )
					{
						// close
						ClearCache();
						ASSERT(0);
						return -1;
					}
					return 0;
				}
				return -1;
			}
			else
			{
				if( ParseData( pTopLine , 0 , nLines ) == - 1 )
				{
//                    ASSERT(0);
					return -1;
				}
			}

		}
	}

	return nStartIndex; // no Need everything is allready cached
}

BOOL CDataHandler::Reload()
{
	if( m_LineCache[ m_nTopLine ].pStart == NULL )
		return FALSE;

	__int64 nTopPos = m_pBuffert->GetAbsolutePos( m_LineCache[ m_nTopLine ].pStart );

	ASSERT( nTopPos != -1 );
	// check if last line is visible then it should be visible after reload to..

	BOOL bMoveToBottom = FALSE;
	if( m_nEndIndex <= m_nTopLine + m_nLinesAPage )
	{
		if( m_LineCache[ m_nEndIndex ].sFlags & LIF_LASTLINE )
		{
			bMoveToBottom  = TRUE;
		}
	}

	ClearCache();
	if( m_pBuffert->Reload( nTopPos ) )
	{
		if( bMoveToBottom )
			return MoveToBottom();

		return GoToPos( nTopPos );
//		if( Prepare( 0 , m_nLinesAPage*2 ) == -1 )
	//		return FALSE;

	}

	ASSERT(0);
	return FALSE;	
}

LPCBYTE CDataHandler::ReMapNext( LPCBYTE pNewRemapPos , DWORD nRetIndex )
{
	if( !(nRetIndex >= m_nStartIndex && nRetIndex <= m_nEndIndex) )
		return NULL;

	__int64 nStartPos = m_pBuffert->GetAbsolutePos( pNewRemapPos );
	__int64 nLinePos = m_pBuffert->GetAbsolutePos( m_LineCache[ nRetIndex ].pStart );

	if( m_pBuffert->ReMap( nStartPos ) )
	{
		LPCBYTE pStart = m_pBuffert->GetPointerToPos( nLinePos );
		ClearCache();
		return pStart;
	}
	return NULL;
}

LPCBYTE CDataHandler::ReMapPrev( LPCBYTE pBottomPos )
{
	__int64 nEndPos = m_pBuffert->GetAbsolutePos( pBottomPos );
	
	DWORD nChunkSize = m_pBuffert->GetMappingChunkSize();
	__int64 nOffSet = m_pBuffert->GetDataOffset();
	if( nOffSet < nChunkSize )
		nOffSet = 0;
	else
	{
		nOffSet = nEndPos - (nChunkSize/2);
	}

	if( m_pBuffert->ReMap(nOffSet) )
	{
		LPCBYTE pBottom = m_pBuffert->GetPointerToPos( nEndPos );
		ClearCache();
		return pBottom;
	}

	
	return NULL;
}

// Data View Position
BOOL CDataHandler::MoveToTop()
{
	if( m_LineCache[ m_nStartIndex ].sFlags & LIF_FIRSTLINE_CHUNK )
	{
		if( m_nTopLine >= m_nStartIndex )
		{
			m_nTopLine = m_nStartIndex;
			return TRUE;
		}
		else
		{
			ASSERT(0);
			// should not go here. m_nTopLine should never be less then m_nStartIndex
		}
	}
	else
	{
		if( m_pBuffert->GetDataOffset() == 0 )
		{
			// No Need to remap. just reparse..
			ClearCache();
			if( ParseData( m_pBuffert->GetBuffer() , 0 , m_nLinesAPage ) == -1 )
				ASSERT(0);
			return TRUE;
		}
		else
		{
			m_pBuffert->ReMap( 0 );
			ClearCache();
			if( ParseData( m_pBuffert->GetBuffer() , 0 , m_nLinesAPage ) == -1 )
				ASSERT(0);

			return TRUE;
		}
	}

	return TRUE;

}

BOOL CDataHandler::MoveToBottom()
{
	LPCBYTE pEnd = m_pBuffert->GetBufferEnd( ByteSize() );
	__int64 nPos = m_pBuffert->GetAbsolutePos( pEnd );

	// if nPos is same as DataLength ( filesize ) then we allready have the buffert chunk that has the end of the file..
	if( nPos == m_pBuffert->GetDataLength() )
	{
		DWORD nLastLine = 0;
		if( m_nTopLine + (m_nLinesAPage - 1) > m_nEndIndex )
			nLastLine = m_nEndIndex;
		else
			nLastLine = m_nTopLine + (m_nLinesAPage - 1);

		// check if last row is shows..
		if( m_LineCache[ nLastLine ].sFlags & LIF_LASTLINE )
			return FALSE;

		if( m_LineCache[ m_nEndIndex].sFlags & LIF_LASTLINE )
		{
			m_nTopLine = m_nEndIndex - (m_nLinesAPage-1);
			if( m_nTopLine + 1 < m_nEndIndex )
				m_nTopLine += 1;  // when showing end of file.  go 1 extra lines down. looks better and we don't risk of getting half a line
			return TRUE;
		}

		// else last row is not cache.. reparse is needed
		ClearCache();
		LPCBYTE pEnd = m_pBuffert->GetBufferEnd( ByteSize() );
		
		if( ParseDataReverse( pEnd , m_nLinesAPage*2 ) )
		{
			m_nTopLine = m_nEndIndex - (m_nLinesAPage - 1 );
			if( m_nTopLine + 1 < m_nEndIndex )
				m_nTopLine += 1;  // when showing end of file.  go 1 extra lines down. looks better and we don't risk of getting half a line
			if( m_nTopLine < m_nStartIndex )
				m_nTopLine = m_nStartIndex;

		}
		return TRUE;

	}
	else
	{
		// remap to the end..
		if( m_pBuffert->ReMapLast() )
		{
			ClearCache();
			LPCBYTE pEnd = m_pBuffert->GetBufferEnd( ByteSize() );
			if( ParseDataReverse( pEnd , m_nLinesAPage*2 ) )
			{
				m_nTopLine = m_nEndIndex - (m_nLinesAPage - 1 );
				if( m_nTopLine + 1 < m_nEndIndex )
					m_nTopLine += 1; // when showing end of file.  go 1 extra lines down. looks better and we don't risk of getting half a line
				if( m_nTopLine < m_nStartIndex )
					m_nTopLine = m_nStartIndex;

			}
			return TRUE;
		}
		return FALSE;

	}
}

BOOL CDataHandler::MovePageUp()
{
	BOOL bMoved=FALSE;
	for( DWORD nLines = m_nLinesAPage-1; nLines > 0 ; nLines-- )
	{
		if( MoveLineUp() )
			bMoved = TRUE;

	}
	return bMoved;
}

BOOL CDataHandler::MovePageDown()
{
	BOOL bMoved=FALSE;
	for( DWORD nLines = m_nLinesAPage-1; nLines > 0 ; nLines-- )
	{
		if( MoveLineDown() )
			bMoved = TRUE;
	}
	return bMoved;
}

BOOL CDataHandler::MoveLineUp()
{
	if( m_nTopLine == 0 || m_nStartIndex == m_nTopLine )
	{
		if( m_LineCache[ m_nTopLine ].sFlags & LIF_FIRSTLINE	) // First line in file.. can't to up
			return FALSE;

		if( PrepareReverse( NULL , m_nTopLine ,  m_nLinesAPage*2 ) != -1 )
		{
			if( m_nStartIndex < m_nTopLine )
				m_nTopLine--;
			return TRUE;
		}
		return FALSE;
	}

	if( m_nStartIndex < m_nTopLine )
	{
		m_nTopLine--; 
	}

	return TRUE;
}

BOOL CDataHandler::MoveLineDown()
{
//	DWORD nLastLine = m_nTopLine + m_nLinesAPage;

	if( m_nEndIndex == 0 ) // No Lines Cached.
		return FALSE;

	if( m_nTopLine < m_nEndIndex )
	{
		if( m_nEndIndex - m_nTopLine <= (DWORD)(m_nLinesAPage - 1) ) // Less or equal to LinesAPage. if this is true. we don't have more lines in the cache to show more.
		{
			if( m_LineCache[m_nEndIndex].sFlags & LIF_LASTLINE ) // this is the lastline in the file.. can't go down more.
			{
				if( m_nEndIndex - m_nTopLine == (DWORD)(m_nLinesAPage - 1) ) // Let it go 1 extra line down. so we get 1 empty line..
				{
					m_nTopLine++;
					return TRUE;
				}
				return FALSE;
			}

			BOOL bRet = Prepare( NULL ,  m_nTopLine+1 , m_nLinesAPage+(m_nLinesAPage/2) );
			if( !bRet )
			{
				ASSERT(0);
				return FALSE;
			}
			m_nTopLine++;

		}
		else
			m_nTopLine++;

		return TRUE;
	}
	else
	{
		// remap me
		ASSERT(0); // should not happend
	}

	return FALSE;

}

BOOL CDataHandler::MoveToScrollPos( DWORD pos )
{

	__int64 nPos =  0;
	if( m_pBuffert->GetDataLength() > 0xFFFFFFFFFFFFF ) 
	{
		nPos = pos * ( m_pBuffert->GetDataLength() / m_dwVertScrollRes );
	}
	else
		nPos = pos * ( (double)m_pBuffert->GetDataLength() /(double)m_dwVertScrollRes );

	if( nPos == 0 )
		return MoveToTop();

//	LPCBYTE pStart = m_pBuffert->GetBuffer();

	if( m_pBuffert->IsInRange( nPos ) )
	{
		if( pos < (m_dwVertScrollRes / 2 )) 
		{
			ClearCache();
			LPCBYTE pLine = m_pBuffert->GetPointerToPos( nPos );
			if( pLine == NULL )
				return FALSE;

			pLine = FindLineStart( pLine );
			if( Prepare( pLine , 0 , m_nLinesAPage*2 ) == - 1 )
			{
				return FALSE;
			}
			return TRUE;
		}
		else
		{
			ClearCache();
			LPCBYTE pLine = m_pBuffert->GetPointerToPos( nPos );
			if( pLine == NULL )
				return FALSE;

			pLine = FindLineStart( pLine );
			if( nPos == m_pBuffert->GetDataLength() )
				pLine = m_pBuffert->GetBufferEnd( ByteSize() );

			if( PrepareReverse( pLine , 0 , m_nLinesAPage ) == -1 )
			{
				ASSERT(0);
				return FALSE;
			}
			return TRUE;

		}

	}
	else
	{
		/// neeed to remap
		int x=0;
		x++;
		DWORD dwChunkSize = m_pBuffert->GetMappingChunkSize();

		if( pos < (m_dwVertScrollRes / 2 )) 
		{
			// parse from beging
			__int64 nNewPos = nPos - (dwChunkSize/4); // Need to remap area a bit smaller, since we need to scan backward to find the begining of the line
			if( nNewPos < 0 )
				nNewPos = 0;

			if( m_pBuffert->ReMap( nNewPos ) )
			{
				ClearCache();
				LPCBYTE pLine = m_pBuffert->GetPointerToPos( nPos );
				if( pLine == NULL )
					return FALSE;

				pLine = FindLineStart( pLine );
				if( ParseData( pLine , 0 , m_nLinesAPage ) == -1  )
				{
					return FALSE;
				}
				return TRUE;

			}

		}
		else
		{
			// parse from end
			__int64 nNewPos = nPos;// + (dwChunkSize/4); // Need to remap to and area a litte less since we need to scan backward to find the begining of the line
			if( nNewPos > m_pBuffert->GetDataLength() )
				nNewPos = m_pBuffert->GetDataLength();

			if( m_pBuffert->ReMap( nNewPos ) )
			{
				ClearCache();
				LPCBYTE pLine = m_pBuffert->GetPointerToPos( nPos );
				if( pLine == NULL )
					return FALSE;

				pLine = FindLineStart( pLine );

				if( nPos == m_pBuffert->GetDataLength() )
					pLine = m_pBuffert->GetBufferEnd(ByteSize() );

				if( PrepareReverse( pLine , 0 , m_nLinesAPage ) == -1 )
				//if( ParseDataReverse( pLine , m_nLinesAPage ) == -1  )
				{
					ASSERT(0);
					return FALSE;
				}
				
				return TRUE;
			}

		}

	}
	return FALSE;
}

BOOL CDataHandler::GoToPos( __int64 nPos )
{
	LPCBYTE pData = m_pBuffert->GetPointerToPos( nPos );
	ClearCache();
	if( pData == NULL )
	{
		if( m_pBuffert->ReMap( nPos ) )
		{
			pData = m_pBuffert->GetPointerToPos( nPos );
			if( pData == NULL )
				pData = m_pBuffert->GetBuffer();
		}
		else
			return FALSE;
	}

	if( pData )
	{

		pData = FindLineStart( pData );
		if( ParseData( pData , m_nTopLine , m_nLinesAPage*2 ) == - 1 )
		{
			LPCBYTE pTopLine = ReMapNext( m_LineCache[ m_nTopLine ].pStart , m_nTopLine );
			if( ParseData( pTopLine , 0 , m_nLinesAPage*2 ) != -1 )
				return TRUE;

			return FALSE;
		}
		else
			return TRUE;
	}

	return FALSE;
}

BOOL    CDataHandler::CopyToBuffert( LPCBYTE pBuffert , LPCTSTR strFilename , DWORD nRWChunkSize , __int64 nOffset , __int64 nSize )
{
	HANDLE hFile = CreateFile( strFilename , GENERIC_READ , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE , NULL ,OPEN_EXISTING ,FILE_FLAG_SEQUENTIAL_SCAN , 0 );
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	ULARGE_INTEGER ul;
	ul.QuadPart = nOffset;
	ULARGE_INTEGER ulSize;
	ulSize.QuadPart = nSize;

	LONG dwHighPart = ul.HighPart;
	DWORD dwPtrLow = SetFilePointer( hFile , ul.LowPart , &dwHighPart, FILE_BEGIN );
	if (dwPtrLow == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
	{
		CloseHandle( hFile );
		return FALSE;
	}

	DWORD nBytesRead=0;
	DWORD nChunkSize = nRWChunkSize * 1024;
	while( nSize > 0 )
	{
		if( nSize < nChunkSize )
			nChunkSize = (DWORD)nSize;

		if( ReadFile( hFile , (LPVOID)pBuffert , nChunkSize , &nBytesRead , 0 ) )
		{
			pBuffert += nBytesRead;
			nSize -= nBytesRead;
		}
	}

	CloseHandle( hFile );

	if( nSize == 0 )
		return TRUE;

	return FALSE;
}
BOOL    CDataHandler::CopyToFile( LPCTSTR strToFilename ,LPCTSTR strFromFilename , DWORD nRWChunkSize  , __int64 nOffset , __int64 nSize )
{
	HANDLE hTo   = CreateFile( strToFilename , GENERIC_WRITE , FILE_SHARE_READ , NULL , CREATE_ALWAYS , FILE_FLAG_SEQUENTIAL_SCAN , 0 );
	if( hTo == INVALID_HANDLE_VALUE )
		return FALSE;

	HANDLE hFrom = CreateFile( strFromFilename , GENERIC_READ , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE , NULL ,OPEN_EXISTING ,FILE_FLAG_SEQUENTIAL_SCAN , 0 );
	if( hFrom == INVALID_HANDLE_VALUE )
	{
		CloseHandle(hTo);
		return FALSE;
	}

	ULARGE_INTEGER ul;
	ul.QuadPart = nOffset;
	ULARGE_INTEGER ulSize;
	ulSize.QuadPart = nSize;

	LONG dwHighPart = ul.HighPart;
	DWORD dwPtrLow = SetFilePointer( hFrom , ul.LowPart , &dwHighPart, FILE_BEGIN );
	if (dwPtrLow == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
	{
		CloseHandle( hTo );
		CloseHandle( hFrom );
		return FALSE;
	}

	
	DWORD nBytesRead=0;
	DWORD nBytesWrite=0;
	DWORD nChunkSize = nRWChunkSize * 1024;
	
	LPCBYTE pBuffert = new BYTE[nChunkSize];

	while( nSize > 0 )
	{
		if( nSize < nChunkSize )
			nChunkSize = (DWORD)nSize;

		if( ReadFile( hFrom , (LPVOID)pBuffert , nChunkSize , &nBytesRead , 0 ) )
		{
			nSize -= nBytesRead;
			if( !WriteFile( hTo , pBuffert , nBytesRead , &nBytesWrite , 0 ) )
			{
				// Failed
                return FALSE;
			}
		}
	}

	delete[] pBuffert;

	CloseHandle( hTo );
	CloseHandle( hFrom );

	if( nSize == 0 )
		return TRUE;

	return FALSE;
}


//
//   CAsciiHandler
//
LPCBYTE CAsciiHandler::ParseLine( LPCBYTE pLineBegin , DWORD& dwLength )
{
	UCHAR* pData = (UCHAR*)pLineBegin;
	UCHAR* pLineEnd = NULL;
	UCHAR* pNextLine;

	LPCBYTE pDataEnd = m_pBuffert->GetBufferEnd(ByteSize() );
	if( pData >= pDataEnd )
	{
		pLineEnd = pData;
		return NULL;
	}

	// if viewing binary data as ASCII or Unicode. and it does not find a cr/lr the complete file can be on ONE line.. so have a max line with.
	// and use MAX_TMPSTRING_SIZE as limit.
	while( pData < pDataEnd && *pData != 0x0a && *pData != 0x0d && (pData - pLineBegin ) < MAX_TMPSTRING_SIZE )
	{
		pData++;
	} 

	// if pData == pDataEnd. then we reach the end of the buffert. 
	//  find out if we at the EOF. if we are then it is okey. else we need to remap the memorymap thing and rescan some rows.
	if( pData == pDataEnd && m_pBuffert->IsEOF( pData ) == FALSE )
	{
		if( m_pBuffert->IsEOB( pData ) )
			return (UCHAR*)-1; // Will make it remap and rescan
	}

	pLineEnd = pData;

	if( pData < (UCHAR*)pDataEnd && pData < ((UCHAR*)pDataEnd+1) )
	{
		if( *pData == 0x0d && *(pData+1) == 0x0a ) 
			pData += 2;
		else if( *pData == 0x0a ) 
			pData++;
	}

	// pData is still pointing at same char. can't be right..
	if( pLineEnd == pData && pData < pDataEnd )
		pData++;

	pNextLine = pData;
	dwLength = pLineEnd - pLineBegin;
	return (LPCBYTE)pNextLine;
}

/*
pData is pointing to end of line... and in ASCII case.. it is proberbly pointing to CR/LF it is the last line in the file. it is not.

*/
LPCBYTE CAsciiHandler::ParseLineReverse( LPCBYTE pData , DWORD& dwLength ) //, LPCBYTE* pEnd )
{
	LPCBYTE pBegin = m_pBuffert->GetBuffer();
	LPCBYTE pEnd = pData;

	// End of line might have a LF / LF , CR first. so skip it.
	if(	pData > pBegin )
	{
		if( *pData == 0x0a && *(pData-1) == 0x0d ) // LF,CR Win (reverse order)
			pData -= 2;
		else if( *pData == 0x0a ) // LF , Unix
			pData--;
	}

	pEnd = pData;

	BOOL bParsed = FALSE;
	while( pData > pBegin && *pData != 0x0d && *pData != 0x0a && (pEnd - pData) < MAX_TMPSTRING_SIZE)
	{
		pData--;
		bParsed = TRUE;
	} 

	// Check if pData points at the begining of the buffer and if it does and offset is bigger then 0.
	// then we need to remap the buffert and do a rescan
	if( pData <= pBegin && m_pBuffert->GetDataOffset() > 0 )
	{
		return (UCHAR*)-1; // Will make it remap and rescan
	}

	// if we Scanned for lf,cr pData is now pointing to the CR or LF character.  and a line can't start with that.
	LPCBYTE pStart=NULL;
	if( bParsed && pData > pBegin ) 
		pStart = pData + 1;
	else if( pEnd == pData )
		pStart = pData + 1; // Line was not parsed. so this line is proberbly only cr or cr/lf. 
	else
		pStart = pData; // This row properbly was a CR or LF or CRLF only. no text. since the while loop was not run

	if( *pStart != 0x0d && *pStart != 0x0a )
	{
		dwLength = pEnd - pStart;
		dwLength += sizeof(UCHAR); // 
	}
	else
		dwLength = 0;
	
	return pStart;
}

LPCBYTE CAsciiHandler::GetHDataPosition( int rowidx , int x )
{
	USES_CONVERSION;

	CClientDC dc( m_pWnd );
	dc.SelectObject( m_pFont );

	DWORD lStringLen = m_LineCache[rowidx].dwLength;
	if( m_LineCache[rowidx].pStart == NULL )
		return FALSE;

	if( lStringLen > MAX_TMPSTRING_SIZE  )
		lStringLen = MAX_TMPSTRING_SIZE;

	MultiByteToWideChar(CP_ACP, 0, (char*)m_LineCache[rowidx].pStart,lStringLen, m_strTempBuffer, lStringLen) ;
	CSize sz;

	int LastCharPos = 0;
	for( DWORD n = 1; n <= lStringLen ; n++ )
	{
		sz = dc.GetTabbedTextExtent( W2CT(m_strTempBuffer) , n ,  0 , NULL );
		sz.cx += m_nLeftMargin;
	
		if( sz.cx > x+m_nSnap )
		{
			return m_LineCache[rowidx].pStart + LastCharPos;
		}
		LastCharPos=n;
	}
	return m_LineCache[rowidx].pStart + lStringLen;
}

void CAsciiHandler::Draw( CDC *pDC , DWORD nCol )
{
	USES_CONVERSION;

	long x=0;
	long y=0;
	DWORD dwStopLine = m_nTopLine + m_nLinesAPage;

	CString str;
	int lStringLen=0;
	UCHAR* pLine;
	UCHAR* pLineEnd;
	int SelPosStart=0;
	int SelPosEnd =0;
	BOOL bDrawSelection = FALSE;

	pDC->SetBkColor( m_crBGColor );
	pDC->SetTextColor( m_crTextColor );

//	TEXTMETRIC tm;
//	pDC->GetOutputTextMetrics (&tm) ;
//	int cxChar = tm.tmAveCharWidth;
	
	for( DWORD nLine = m_nTopLine  ;  nLine < dwStopLine  ; nLine++ )
	{

		bDrawSelection = FALSE;

		x=m_nLeftMargin;

//   Problem!! if text file has Tabs and we are going to draw from COL 1. and first char is a TAB it disapars.. 
//   So instead of going forward "nCol" numbers of char and draw,  we can move the draw position back X pixel.. 
//   BUT Tabs was not Draw correctly when x was less then 0. 
//   Another solution is to convert TAB into 8 space char before drawing. and then go "nCol" of step forward in the string and start draw
//
//		if( nCol > 0 )
//		{
//			x = 0;
//			x-= (cxChar * nCol);
//		}

		if( nLine <= m_nEndIndex )
		{
			lStringLen = m_LineCache[nLine].dwLength;
			if( (DWORD)lStringLen < nCol )
			{
				y += m_dwLineHeight;
				continue;
			}
			
			pLine = (UCHAR*)m_LineCache[nLine].pStart;
			pLineEnd = pLine + lStringLen;

			pLine += nCol;
			lStringLen -= nCol;

#ifdef _UNICODE
			if( lStringLen > MAX_TMPSTRING_SIZE ) // Max line size . so the MultiByteToWideChar is not overwriting m_strTempBuffer
				lStringLen = MAX_TMPSTRING_SIZE;

			// Why convert ASCII to Wide ( Unicode ).. on Unicode build. the API TabbedTextOut(..) expecting the string to be Unicode
			MultiByteToWideChar (CP_ACP, 0, (char*)pLine,lStringLen, m_strTempBuffer, lStringLen) ;
#endif

			if( hasSelection() && lineHasSelection( pLine , pLineEnd ) )
			{
				__int64 nLinePos = m_pBuffert->GetAbsolutePos( pLine );
				__int64 nLineEndPos = m_pBuffert->GetAbsolutePos( pLineEnd );

				if( m_nSelectionStart <= nLinePos && m_nSelectionEnd > nLinePos ) 
					SelPosStart = 0;  // Start of selection is before this this line start and end is after so SelPosStart should be 0 since the line start with selection
				else 
					SelPosStart = (int)(m_nSelectionStart - nLinePos);

				if( m_nSelectionEnd > nLineEndPos )
					SelPosEnd = (int)lStringLen-1;// The Selection is to the end of the line // (pLineEnd - (UCHAR*)m_pSelectionStart)-1;
				else
					SelPosEnd =  (int)(m_nSelectionEnd - nLinePos)-1;

				if( SelPosStart >= 0 && SelPosEnd >= 0 )
				{
#ifdef _UNICODE
					_TCHAR* pString = W2T(m_strTempBuffer);
#else
					char* pString = (char*)pLine;
#endif
					CSize sz;
					int pos = 0;
					int posend = 0; 
					int len =0;
					// Draw Pre Selection
					if( SelPosStart > 0 )
					{
						pos = 0;
						posend = SelPosStart - 1;
						len = (posend - pos) +1;
						sz = pDC->TabbedTextOut( x , y , pString , len , 0 , NULL , 0 );
						x += sz.cx;
						pString += len;
					}
					// Draw Selection
					pos = SelPosStart;
					len = (SelPosEnd-pos)+1;
					pDC->SetTextColor( m_crSelectionTextColor );
					pDC->SetBkColor(m_crSelectionBGColor );
					sz = pDC->TabbedTextOut( x , y , pString , len , 0 , NULL , 0 );
					x+= sz.cx;
					pString += len;
					pDC->SetBkColor( m_crBGColor );
					pDC->SetTextColor( m_crTextColor );
					// Draw Post Selection
					if( SelPosEnd < (lStringLen-1) )
					{
						len = (int)(nLineEndPos - m_nSelectionEnd);
						pDC->TabbedTextOut( x , y, pString , len , 0 , NULL , 0 );
					}
					
				}

			}
			else
			{
#ifdef _UNICODE
				_TCHAR* pString = W2T(m_strTempBuffer);
#else
				char* pString = (char*)pLine;
#endif
				pDC->TabbedTextOut( x , y , pString , lStringLen ,0 , NULL , nCol );
			}
		}
		y += m_dwLineHeight;
	}
}





LPCBYTE CAsciiHandler::FindLineStart( LPCBYTE pData )
{
	LPCBYTE pBegin = m_pBuffert->GetBuffer();
	LPCBYTE pEnd = m_pBuffert->GetBufferEnd(ByteSize());

	while( pData > pBegin && pData < pEnd  && *pData != 0x0d && *pData != 0x0a )
	{
		pData--;
	} 

	// pStart should NOT Stand on cr/lf character
	if( pData > pBegin && pData < (pEnd - 1) )
		pData++;

	return pData;
}

void CUnicodeHandler::Draw( CDC *pDC , DWORD nCol )
{
	//
	// Problem !!  In NONE Unicode build the TabbedTextOut can't print Unicode text.
	// How to show unicode characters in None Unicode build.??  convert string into normal CHAR string. but then
	// some unicode files will not be shown correctly.
	// BUT it only effects 98/ME since 2000/XP/+ have support for it. so is it worth to fix or not...

	USES_CONVERSION;
	long x=0;
	long y=0;
	DWORD dwStopLine = m_nTopLine  + m_nLinesAPage;

	CString str;
	int lStringLen=0;
	WCHAR* pLine;
	WCHAR* pLineEnd;
	int SelPosStart=0;
	int SelPosEnd =0;

	pDC->SetBkColor( m_crBGColor );
	pDC->SetTextColor( m_crTextColor );

	for( DWORD nLine = m_nTopLine  ;  nLine < dwStopLine  ; nLine++ )
	{
		x=m_nLeftMargin;

		if( nLine <= m_nEndIndex  )
		{
			lStringLen = m_LineCache[nLine].dwLength;
			lStringLen = lStringLen / sizeof(WCHAR);
			pLine = (WCHAR*)m_LineCache[nLine].pStart;
			pLineEnd = pLine + lStringLen;

			if( (DWORD)lStringLen < nCol )
			{
				y += m_dwLineHeight;
				continue;
			}
			pLine += nCol;
			lStringLen -= nCol;


#ifndef _UNICODE
			if( lStringLen > MAX_TMPSTRING_SIZE )
				lStringLen = MAX_TMPSTRING_SIZE;

			WideCharToMultiByte( CP_ACP , 0  , (WCHAR*)pLine , lStringLen , m_strTempBuffer , lStringLen , NULL , NULL );
#endif

			if( hasSelection() && lineHasSelection( (UCHAR*)pLine , (UCHAR*)pLineEnd ) )
			{
				__int64 nLinePos = m_pBuffert->GetAbsolutePos( (LPCBYTE)pLine );
				__int64 nLineEndPos = m_pBuffert->GetAbsolutePos( (LPCBYTE)pLineEnd );

				if( m_nSelectionStart <= nLinePos && m_nSelectionEnd > nLinePos ) 
					SelPosStart = 0;// Begining of selection is before this line start and end is after so SelPosStart should be 0
				else 
					SelPosStart = (int)(m_nSelectionStart - nLinePos) / sizeof(WCHAR);

				if( m_nSelectionEnd > nLineEndPos )
					SelPosEnd = lStringLen-1;// The Selection is to the end of the line 
				else
					SelPosEnd = (int)((m_nSelectionEnd - nLinePos) / sizeof(WCHAR)) - 1;// ((WCHAR*)m_nSelectionEnd - pLine)-1;

				if( SelPosStart >= 0 && SelPosEnd >= 0 )
				{
#ifdef _UNICODE
					WCHAR* pString = pLine;
#else
					char* pString = m_strTempBuffer;
#endif
					CSize sz;
					// eg Start = 1  , End = 9
					int pos = 0;
					int posend = 0; 
					int len =0;
					// Draw Pre Selection
					if( SelPosStart > 0 )
					{
						pos = 0;
						posend = SelPosStart - 1;
						len = (posend - pos)+1;
						sz = pDC->TabbedTextOut( x , y , pString , len , 0 , NULL , 0 ); 
						x += sz.cx;
						pString += len;
					}
					// Draw Selection
					pos = SelPosStart;
					len = (SelPosEnd-pos)+1;
					pDC->SetTextColor( m_crSelectionTextColor );
					pDC->SetBkColor( m_crSelectionBGColor );
					sz = pDC->TabbedTextOut( x , y , pString , len , 0 , NULL , 0 );
					x+= sz.cx;
					pString += len;
					pDC->SetBkColor( m_crBGColor );
					pDC->SetTextColor(m_crTextColor );
					// Draw Post Selection
					if( SelPosEnd < (lStringLen-1) )
					{
						len = (int)((nLineEndPos - m_nSelectionEnd)/sizeof(WCHAR)); 
						pDC->TabbedTextOut( x , y, pString , len , 0 , NULL , 0 );
					}
					
				}
			}
			else
			{
#ifdef _UNICODE
				WCHAR* pString = pLine;
#else
				char* pString = m_strTempBuffer;
#endif

				pDC->TabbedTextOut( x , y , pString , lStringLen ,0 , NULL , 0 );
			}
		}
		y += m_dwLineHeight;
	}
	
}

LPCBYTE CUnicodeHandler::ParseLine( LPCBYTE pLineBegin , DWORD& dwLength )
{
	WCHAR* pLineEnd = NULL;
	WCHAR* pNextLine = NULL;

	LPCBYTE pDataEnd = m_pBuffert->GetBufferEnd(ByteSize() );
	WCHAR *pWData =(WCHAR*)pLineBegin;
	if( pWData >= (WCHAR*)pDataEnd )
	{
		pLineEnd = pWData;
		return NULL;
	}

	while( pWData <= (WCHAR*)pDataEnd && *pWData != 0x000a && *pWData != 0x000d && ( (LPCBYTE)pWData - pLineBegin ) < MAX_TMPSTRING_SIZE )
	{
		pWData++;

	} 

	// if pWData == pDataEnd. then we reach the end of the buffert. 
	//  find out if we at the EOF. if we are then it is okey. else we need to remap the memorymap thing and rescan some rows.
	if( pWData >= (WCHAR*)pDataEnd && m_pBuffert->IsEOF( (LPCBYTE)pWData ) == FALSE )
	{
		if( m_pBuffert->IsEOB( (LPCBYTE)pWData ) )
			return (UCHAR*)-1; // Will make it remap and rescan
	}

	pLineEnd = pWData;

	if( pWData < (WCHAR*)pDataEnd && pWData < ((WCHAR*)pDataEnd) )
	{
		if( *pWData == 0x000d && *(pWData+1) == 0x000a ) // CR,LF Win
			pWData += 2;
		else if( *pWData == 0x000a ) // LF , Unix
			pWData++;
	}

	// pData is still pointing at same char. can't be right..
	if( pLineEnd == pWData && pWData < (WCHAR*)pDataEnd )
		pWData++;

	pNextLine = pWData;
	
	dwLength = (LPCBYTE)pLineEnd - pLineBegin;
	return (LPCBYTE)pNextLine;
}




LPCBYTE CUnicodeHandler::ParseLineReverse( LPCBYTE pData , DWORD& dwLength ) //LPCBYTE* pEnd )
{
	WCHAR* pBegin = (WCHAR*)m_pBuffert->GetBuffer();
//	WCHAR* pEOD = (WCHAR*)m_pBuffert->GetBufferEnd();
	WCHAR *pWData =(WCHAR*) pData;
	LPCBYTE pEnd = NULL;


	// End of line might have a LF / CR , CR first. so skip it.
	if(	pWData > pBegin )
	{
		if( *pWData == 0x000a && *(pWData-1) == 0x000d ) // LF,CR Win (reverse order)
			pWData -= 2;
		else if( *pWData == 0x0a ) // LF , Unix
			pWData--;
	}

	pEnd = (LPCBYTE)pWData;
	
	BOOL bParsed = FALSE;
	while( pWData > pBegin && *pWData != 0x000d && *pWData != 0x000a && ( (LPCBYTE)pEnd - (LPCBYTE)pWData) < MAX_TMPSTRING_SIZE )
	{
		pWData--;
		bParsed = TRUE;

	} 
	// Check if pData points at the begining of the buffer and if it is and offset is bigger then 0.
	// then we need to remap the buffert and do a rescan
	if( pWData <= pBegin && m_pBuffert->GetDataOffset() > 0 )
	{
		return (UCHAR*)-1; // Will make it remap and rescan
	}

	// if we Scanned for lf,cr pData is now pointing to the CR or LF character.  and a line can't start with that.
	WCHAR* pStart=NULL;
	if( bParsed && pWData > pBegin ) 
		pStart = pWData + 1;
	else if( pEnd == (LPCBYTE)pWData )
		pStart = pWData + 1; // Line was not parsed. so this line is proberbly only cr or cr/lf. 
	else
		pStart = pWData; // This row properbly was a CR or LF or CRLF only. no text. since the while loop was not run

	if( *pStart != 0x000d && *pStart != 0x000a )
	{
		dwLength = pEnd - (LPCBYTE)pStart;
		dwLength += sizeof(WCHAR); // 
	}
	else
		dwLength = 0;

	return (LPCBYTE)pStart;
}

LPCBYTE CUnicodeHandler::FindLineStart( LPCBYTE pData )
{
	LPCBYTE pBegin = m_pBuffert->GetBuffer();
	LPCBYTE pEnd = m_pBuffert->GetBufferEnd(ByteSize());

	while( pData > pBegin && pData <= pEnd  && *pData != 0x000d && *pData != 0x000a )
	{
		pData--;
	} 
	// pStart should NOT Stand on cr/lf character
	if( pData > pBegin && pData < (pEnd - 3 ) )
		pData += 2;

	return pData;
}


void CBinaryHandler::Draw( CDC *pDC , DWORD nCol )
{
	USES_CONVERSION;

	long x=0;
	long y=0;
	DWORD dwStopLine = m_nTopLine  + m_nLinesAPage;

	LPCBYTE pLine;
	LPCBYTE pLineEnd;

	CString str;
	int lStringLen=0;

	int SelPosStart=0;
	int SelPosEnd =0;

	pDC->SetBkColor( m_crBGColor );
	pDC->SetTextColor( m_crTextColor );

	for( DWORD nLine = m_nTopLine  ;  nLine < dwStopLine  ; nLine++ )
	{
		x=m_nLeftMargin;

		if( nLine <= m_nEndIndex  )
		{
			lStringLen = m_LineCache[nLine].dwLength;

			if( (DWORD)lStringLen < nCol )
			{
				y += m_dwLineHeight;
				continue;
			}

			if( lStringLen > 0 )
			{
				pLine = m_LineCache[nLine].pStart;
				pLineEnd = pLine + lStringLen;

				pLine += nCol;
				lStringLen -= nCol;

				if( lStringLen > MAX_TMPSTRING_SIZE )
					lStringLen = MAX_TMPSTRING_SIZE;

#ifdef _UNICODE
				MultiByteToWideChar (CP_ACP, 0, (char*)pLine,lStringLen, m_strTempBuffer, lStringLen) ;
#endif

				if( hasSelection() && lineHasSelection( pLine  ,pLineEnd ) )
				{
					__int64 nLinePos = m_pBuffert->GetAbsolutePos( pLine );
					__int64 nLineEndPos = m_pBuffert->GetAbsolutePos( pLineEnd );

					if( m_nSelectionStart <= nLinePos && m_nSelectionEnd > nLinePos ) 
						SelPosStart = 0;  // Start of selection is before this this line start and end is after so SelPosStart should be 0 since the line start with selection
					else 
						SelPosStart = (int)(m_nSelectionStart - nLinePos);

					if( m_nSelectionEnd > nLineEndPos )
						SelPosEnd = (int)lStringLen-1;// The Selection is to the end of the line // (pLineEnd - (UCHAR*)m_pSelectionStart)-1;
					else
						SelPosEnd =  (int)(m_nSelectionEnd - nLinePos)-1;

					if( SelPosStart >= 0 && SelPosEnd >= 0 )
					{
#ifdef _UNICODE
						const _TCHAR* pString = CW2CT(m_strTempBuffer);
#else
						const char* pString = (LPCSTR)pLine;                   
#endif
						CSize sz;
						int pos = 0;
						int posend = 0; 
						int len =0;
						// Draw Pre Selection
						if( SelPosStart > 0 )
						{
							pos = 0;
							posend = SelPosStart - 1;
							len = (posend - pos) +1;
							if( pDC->TextOut( x , y , pString , len) )
							 sz = pDC->GetTextExtent( pString , len );
							
							x += sz.cx;
							pString += len;
						}
						// Draw Selection
						pos = SelPosStart;
						len = (SelPosEnd-pos)+1;
						pDC->SetTextColor( m_crSelectionTextColor );
						pDC->SetBkColor(m_crSelectionBGColor );
						if( pDC->TextOut( x , y , pString , len ) )
							 sz = pDC->GetTextExtent( pString , len );
						x+= sz.cx;
						pString += len;
						pDC->SetBkColor( m_crBGColor );
						pDC->SetTextColor( m_crTextColor );
						// Draw Post Selection
						if( SelPosEnd < (lStringLen-1) )
						{
							len = (int)(nLineEndPos - m_nSelectionEnd);
							if( pDC->TextOut( x , y, pString , len ) )
								 sz = pDC->GetTextExtent( pString , len );
						}

					}

				}
				else
				{
				
#ifdef _UNICODE
					pDC->TextOut( x , y , CW2CT(m_strTempBuffer) , lStringLen );
#else
					pDC->TextOut( x , y , (LPCSTR)pLine , lStringLen );
#endif
				}
			}
		}
		y += m_dwLineHeight;
	}

}

LPCBYTE  CBinaryHandler::ParseLine( LPCBYTE pLineBegin , DWORD& dwLength )
{
	UCHAR* pData = (UCHAR*)pLineBegin;
	UCHAR* pLineEnd = NULL;
	UCHAR* pNextLine;

	LPCBYTE pDataEnd = m_pBuffert->GetBufferEnd(ByteSize());
	if( pData >= pDataEnd )
	{
		pLineEnd = pData;
		return NULL;
	}
//	int nCharCount = 0;

	if( pData + m_LineWidth < pDataEnd )
		pData += m_LineWidth;
	else
		pData = (UCHAR*)pDataEnd;

	// if pData == pDataEnd. then we reach the end of the buffert. 
	//  find out if we at the EOF. if we are then it is okey. else we need to remap the memorymap thing and rescan some rows.
	if( pData == pDataEnd && m_pBuffert->IsEOF( pData ) == FALSE )
	{
		if( m_pBuffert->IsEOB( pData ) )
			return (UCHAR*)-1; // Will make it remap and rescan
	}

	pLineEnd = pData;

	pNextLine = pData;

	dwLength = pLineEnd - pLineBegin;
	return (LPCBYTE)pNextLine;
}

LPCBYTE	 CBinaryHandler::ParseLineReverse( LPCBYTE pData , DWORD& dwLength ) //LPCBYTE* pEnd  )
{

	LPCBYTE pBegin = m_pBuffert->GetBuffer();
	LPCBYTE pEOB = m_pBuffert->GetBufferEnd(ByteSize()); // End of Buffert
	LPCBYTE pEnd = NULL;
	if( pData > pEOB )
	{
		return pData;
	}

	// pData is END OF LINE..
	// find Start of line
	
	__int64 nPos = m_pBuffert->GetAbsolutePos( pData );
	__int64 nLineStartPos = ( ((__int64)(nPos / m_LineWidth)) * m_LineWidth );
	if( nLineStartPos == nPos )
		nLineStartPos -= m_LineWidth;

	// the nLineStartPos inside current buffer.
	if( nLineStartPos < m_pBuffert->GetDataOffset() )
		return (LPCBYTE)-1;

	// get pointer to position
	LPCBYTE pLineBegin = m_pBuffert->GetPointerToPos( nLineStartPos );
	if( pLineBegin > pEOB )
		pLineBegin = pEOB;

	// Check if pData points at the begining of the buffer and if it does and offset is bigger then 0.
	// then we need to remap the buffert and do a rescan
	if( pLineBegin <= pBegin && m_pBuffert->GetDataOffset() > 0 )
	{
		return (UCHAR*)-1; // Will make it remap and rescan
	}

	
	if( pLineBegin + m_LineWidth > pEOB )
	{
//		DWORD dwLen = pEOB - pLineBegin;
		pEnd = pEOB;
	}
	else
		pEnd = (pLineBegin + m_LineWidth); 

	dwLength = pEnd - pLineBegin;

	return pLineBegin;
}

LPCBYTE CBinaryHandler::GetHDataPosition( int rowidx , int x )
{
	CClientDC dc( m_pWnd );
	dc.SelectObject( m_pFont );

	DWORD lStringLen = m_LineCache[rowidx].dwLength;
	if( m_LineCache[rowidx].pStart == NULL )
		return FALSE;



#ifdef _UNICODE

	if( lStringLen > MAX_TMPSTRING_SIZE  )
		lStringLen = MAX_TMPSTRING_SIZE;

	MultiByteToWideChar(CP_ACP, 0, (char*)m_LineCache[rowidx].pStart,lStringLen, m_strTempBuffer, lStringLen) ;
#endif


	CSize sz;

	int LastCharPos = 0;
	for( DWORD n = 1; n <= lStringLen ; n++ )
	{

#ifdef _UNICODE
		sz = dc.GetTextExtent( m_strTempBuffer , n );
#else
		sz = dc.GetTextExtent( (char*)m_LineCache[rowidx].pStart , n );
#endif
		sz.cx += m_nLeftMargin;

		if( sz.cx > x+m_nSnap )
		{
			return m_LineCache[rowidx].pStart + LastCharPos;
		}
		LastCharPos=n;
	}
	return m_LineCache[rowidx].pStart + lStringLen;
}

LPCBYTE	CBinaryHandler::FindLineStart( LPCBYTE pData )
{
	LPCBYTE pBegin = m_pBuffert->GetBuffer();
//	LPCBYTE pEnd = m_pBuffert->GetBufferEnd();

	DWORD  pos = (pData - (UCHAR*)pBegin);
	UCHAR* pStart = (UCHAR*)pBegin + ( ( (DWORD)pos / m_LineWidth ) * m_LineWidth);
	return pStart;
}


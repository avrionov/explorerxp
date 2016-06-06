// File:		FadeWnd.cpp
// Ver:			0.00 - Nov 2003
//
// Copyright:	(C) 2003 John A. Johnson
// Author:		John A. Johnson (Andy)
// E-mail:		andyj@clois.it
// Homepage:	http://www.clois.com
//				http://www.yrrwo.com
//
// Code link:	http://www.codeproject.com/dialog/WinMakeInactive.asp
//
// Implementation of a Fade effect from COLOR to BW helper function
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name and all copyright
// notices remains intact.
//
// An email letting me know how you are using it would be nice as well.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
////////////////////////////////////////////////////////////////////////////
//
// Credits for the CopyScreenToBitmap(...)
//
//   Main code implementation from: Barretto VN
//     Developer: http://www.codeproject.com/script/profile/whos_who.asp?id=43422
//     Found:     http://www.codeproject.com/gdi/barry_s_screen_capture.asp
//
// Credits for the FadeBitmap(...) (originally ChangeColor(...))
//
//   Main code implementation from: Dimitri Rochette drochette@ltezone.net
//     Developer: http://www.codeproject.com/script/profile/whos_who.asp?id=46712
//     Found:     http://www.codeproject.com/bitmap/rplcolor.asp
//
// Credits for the hiding of icon on the task bar
//
//   A cool new implementation made by Chris Maunder
//     Developer: http://www.codeproject.com/script/profile/whos_who.asp?id=1
//     Found:     http://www.codeproject.com/docview/notaskbaricon.asp
//
// Credits for the development of a more flexible color calculation
//
//   A cool new implementation made by Mikko Mononen
//     Developer: http://www.codeproject.com/script/profile/whos_who.asp?id=41615
//     Found:     http://www.codeproject.com/dialog/WinMakeInactive.asp?select=656456&df=100&forumid=25834#xx656456xx
//
////////////////////////////////////////////////////////////////////////////
//
// Suggestions & Bug fixes
//
//   Peter Mares (http://www.codeproject.com/script/profile/whos_who.asp?id=23552)
//     tip: Colorize using a different color scale
//
//   JOHN11 (http://www.codeproject.com/script/profile/whos_who.asp?id=219794)
//     bug: Disable parent window
//     bug: Hide task bar window
//
//   WREY (http://www.codeproject.com/script/profile/whos_who.asp?id=4650)
//     tip: Security options to allow the CFadeWnd to close only on aprticular conditions
//
////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "FadeWnd.h"


#pragma warning(disable: 4201)

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#pragma warning(default: 4201)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFadeWnd

CFadeWnd::CFadeWnd(
	CWnd*		pParentWnd	/* = NULL */,
	COLORREF	clrLight	/* = RGB(255,255,255) */,
	COLORREF	clrDark		/* = RGB(  0,  0,  0) */)
{
	// register the window
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, WC_FADEWND, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;	// no application icon
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = NULL;	// no background
        wndcls.lpszMenuName     = NULL;	// no manu application
        wndcls.lpszClassName    = WC_FADEWND;

        if (!AfxRegisterClass(&wndcls))
            AfxThrowResourceException();
    }

	// default attribut values
	m_bAutoDel		= false;
	m_hBitmap		= NULL;
	m_hNewBitmap	= NULL;
	m_pWndLock		= NULL;

    // if the parent window was valid, fade it!
	CreateGradient(clrLight, clrDark);
	if (pParentWnd  &&  IsWindow(pParentWnd->GetSafeHwnd()))
		CFadeWnd::Create(pParentWnd);
}

CFadeWnd::~CFadeWnd()
{
	// free used resources
	if (m_hBitmap)		{	DeleteObject(m_hBitmap);	m_hBitmap    = NULL;	}
	if (m_hNewBitmap)	{	DeleteObject(m_hNewBitmap);	m_hNewBitmap = NULL;	}

	if (m_pWndLock)
	{
		// unlock and give focus to the parent
		m_pWndLock->EnableWindow();
		m_pWndLock->SetFocus();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFadeWnd operations

void CFadeWnd::AutoFade(
	CWnd*		pParentWnd	/* = NULL */,
	COLORREF	clrLight	/* = RGB(255,255,255) */,
	COLORREF	clrDark		/* = RGB(  0,  0,  0) */)
{
	if(pParentWnd)
	{
		CFadeWnd* pFade = new CFadeWnd;

		if (pFade)
		{
			pFade->CreateGradient(clrLight, clrDark);
			pFade->Create(pParentWnd, true);
		}
	}
}

BOOL CFadeWnd::Create(CWnd* pParentWnd, bool bAutoDel, CRect rc) 
{
	if (!pParentWnd  ||  !IsWindow(pParentWnd->GetSafeHwnd()))
		return FALSE;

	// verify real color mode
	CClientDC dcParent(pParentWnd);
	if (dcParent.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
		return FALSE;

	// define a valid rectangle if none aviable
	if (rc == CRect(0,0,0,0))
		pParentWnd->GetWindowRect(rc);

	CString	szText;
	pParentWnd->GetWindowText(szText);

	if (!CWnd::CreateEx(
			0,
			WC_FADEWND,
			szText,
			WS_POPUP | WS_CHILD | WS_VISIBLE,
			rc,
			pParentWnd,
			NULL,
			NULL))
	{
		if (bAutoDel)
			delete this;
	
		return FALSE;
	}

	if (pParentWnd != GetDesktopWindow())
	{
		// disable the parent to prevent focusing actions
		m_pWndLock = pParentWnd;
		m_pWndLock->EnableWindow(FALSE);

		// get and use the parent region (for shaped windows)
		HRGN hRgn = CreateRectRgn(0,0,0,0);
		int regionType = pParentWnd->GetWindowRgn(hRgn);
		if (regionType != ERROR)
			SetWindowRgn(hRgn, FALSE);

		DeleteObject(hRgn);
	}

	// drawing loop (directly in the creation look);
	m_bAutoDel	= bAutoDel;
	m_hBitmap	= CopyScreenToBitmap(rc);

	CClientDC dc(this);

	int nStep = 0;
	for (int i=0; i<100; i+=nStep)
	{
		DWORD dwStartCount = timeGetTime();	// tick counter in milliseconds

		if(m_hNewBitmap)
		{
			DeleteObject(m_hNewBitmap);
			m_hNewBitmap = NULL;
		}

		m_hNewBitmap = FadeBitmap(
			m_hBitmap,
			(double)i);

		OnDraw(&dc);

		DWORD dwEndCount = timeGetTime();	// tick counter in milliseconds

		if (nStep == 0)
		{
			// timer frequency calculation
			if (dwEndCount > dwStartCount)
				nStep = 100 / (dwEndCount - dwStartCount);  // close in 1.5 sec
			else
				nStep = 20;

			// time to frequency
			nStep = 100 / nStep;

			nStep = max(nStep,  3);	// min allowed
			nStep = min(nStep, 49);	// max allowed

			TRACE("Loop frequency: %d\n", nStep);
		}
	}

	return TRUE;
}

void CFadeWnd::CreateGradient(
	COLORREF	clrLight	/* = RGB(255,255,255) */,
	COLORREF	clrDark		/* = RGB(  0,  0,  0) */)
{
	for(int i=0; i<256; i++)
	{
		m_clrGradient[i] = RGB(
			(GetRValue(clrLight) * i + GetRValue(clrDark) * (255 - i)) >> 8,
			(GetGValue(clrLight) * i + GetGValue(clrDark) * (255 - i)) >> 8,
			(GetBValue(clrLight) * i + GetBValue(clrDark) * (255 - i)) >> 8 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFadeWnd generated virtual function overrides

BOOL CFadeWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	CWnd::PreCreateWindow(cs);

	// Main code implementation made by Chris Maunder
	//   Coder: http://www.codeproject.com/script/profile/whos_who.asp?id=1
	//   Found: http://www.codeproject.com/docview/notaskbaricon.asp
	
	// Create invisible window
	if (!::IsWindow(m_wndInvisible.m_hWnd))
	{
		LPCTSTR pstrOwnerClass = AfxRegisterWndClass(0);
		if (!m_wndInvisible.CreateEx(
				0, pstrOwnerClass, _T(""), WS_POPUP,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				AfxGetMainWnd()->GetSafeHwnd(), 0))
			return FALSE;
	}

    cs.hwndParent	= m_wndInvisible.m_hWnd;

	cs.style		&= ~WS_MAXIMIZEBOX;
	cs.style		&= ~WS_MINIMIZEBOX;

	cs.dwExStyle	&= ~WS_EX_CLIENTEDGE;
	cs.lpszClass	= AfxRegisterWndClass(0);

	return TRUE;
}

void CFadeWnd::PostNcDestroy() 
{
	if (m_bAutoDel)
	    delete this;
	else
		CWnd::PostNcDestroy();
}


BEGIN_MESSAGE_MAP(CFadeWnd, CWnd)
	//{{AFX_MSG_MAP(CFadeWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFadeWnd message handlers

void CFadeWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

void CFadeWnd::OnDraw(CDC* pDC)
{
	CRect rc;	GetWindowRect(rc);

	// create a DC for the bitmap drawing
	HDC		hBmDc	= CreateCompatibleDC(pDC->GetSafeHdc());

	// select new bitmap into memory DC     
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hBmDc, (HGDIOBJ)m_hNewBitmap);      

	// bitblt screen DC to memory DC     
	BitBlt(pDC->GetSafeHdc(), 0,0, rc.Width(),rc.Height(), hBmDc, 0,0, SRCCOPY);     

	// select old bitmap back into memory DC and get handle to     
	// bitmap of the screen          
	::SelectObject(hBmDc, (HGDIOBJ)hOldBitmap);      

	DeleteDC(hBmDc);
}

// Main code implementation from: Barretto VN
//   Coder: http://www.codeproject.com/script/profile/whos_who.asp?id=43422
//   Found: http://www.codeproject.com/gdi/barry_s_screen_capture.asp

HBITMAP CFadeWnd::CopyScreenToBitmap(LPRECT lpRect)
{
	HDC	hScrDC, hMemDC;			// screen DC and memory DC     
	int	nX, nY, nX2, nY2;		// coordinates of rectangle to grab     
	int	nWidth, nHeight;		// DIB width and height     
	int	xScrn,  yScrn;			// screen resolution      

	HGDIOBJ	hOldBitmap, hBitmap;

	// check for an empty rectangle 
	if (IsRectEmpty(lpRect))       
		return NULL;      

	// create a DC for the screen and create
	// a memory DC compatible to screen DC
	hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);     
	hMemDC = CreateCompatibleDC(hScrDC);

	// get points of rectangle to grab  
	nX  = lpRect->left;     
	nY  = lpRect->top;     
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	// get screen resolution      
	xScrn = GetDeviceCaps(hScrDC, HORZRES);     
	yScrn = GetDeviceCaps(hScrDC, VERTRES);      
   
	// make sure bitmap rectangle is visible      
	if (nX  < 0)		nX = 0;     
	if (nY  < 0)		nY = 0;     
	if (nX2 > xScrn)	nX2 = xScrn;     
	if (nY2 > yScrn)	nY2 = yScrn;      

	nWidth	= nX2 - nX;     
	nHeight	= nY2 - nY;      
   
	// create a bitmap compatible with the screen DC     
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);      

	// select new bitmap into memory DC     
	hOldBitmap = SelectObject(hMemDC, hBitmap);      

	// bitblt screen DC to memory DC     
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);     

	// select old bitmap back into memory DC and get handle to     
	// bitmap of the screen          
	hBitmap = SelectObject(hMemDC, hOldBitmap);      
   
	// clean up      
	DeleteDC(hScrDC);     
	DeleteDC(hMemDC);      
   
	// return handle to the bitmap      
	return (HBITMAP)hBitmap; 
}


//	COLORREF	0x00bbggrr
//	DIBRGB		0x00rrggbb

#define COLORREF2DIBRGB(clr)	\
	((((clr) << 16) & 0x00ff0000)	|\
	 ( (clr)        & 0x0000ff00)	|\
	 (((clr) >> 16) & 0x000000ff))

#define GetDibR(drgb)	(((drgb) & 0x00ff0000) >> 16)
#define GetDibG(drgb)	(((drgb) & 0x0000ff00) >>  8)
#define GetDibB(drgb)	 ((drgb) & 0x000000ff)

#define DIBRGB2COLORREF(clr)	COLORREF2DIBRGB(clr)


// Main code implementation from: Dimitri Rochette drochette@ltezone.net
//   Coder: http://www.codeproject.com/script/profile/whos_who.asp?id=46712
//   Found: http://www.codeproject.com/bitmap/rplcolor.asp

HBITMAP CFadeWnd::FadeBitmap(
	HBITMAP		hBmp,
	double		dfTrans,
	HDC			hBmpDC)
{
	HBITMAP hRetBmp = NULL;

	if (hBmp)
	{
		// DC for Source Bitmap
		HDC hBufferDC = CreateCompatibleDC(NULL);
		if (hBufferDC)
		{
			HBITMAP hTmpBitmap = (HBITMAP) NULL;
			if (hBmpDC)
				if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
				{
					hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
					SelectObject(hBmpDC, hTmpBitmap);
				}

			// here hBufferDC contains the bitmap
			HGDIOBJ hPrevBufObject = SelectObject(hBufferDC, hBmp);
			
			HDC hDirectDC = CreateCompatibleDC(NULL);	// DC for working		
			if (hDirectDC)
			{
				// Get bitmap size
				BITMAP bm;
				GetObject(hBmp, sizeof(bm), &bm);
				
				// create a BITMAPINFO with minimal initilisation for the CreateDIBSection
				BITMAPINFO bmInfo; 
				ZeroMemory(&bmInfo,sizeof(bmInfo));
				bmInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
				bmInfo.bmiHeader.biWidth	= bm.bmWidth;
				bmInfo.bmiHeader.biHeight	= bm.bmHeight;
				bmInfo.bmiHeader.biPlanes	=  1;
				bmInfo.bmiHeader.biBitCount	= 32;

				UINT* ptPixels;	// pointer used for direct Bitmap pixels access

				HBITMAP hDirectBitmap = CreateDIBSection(hDirectDC, (BITMAPINFO*)&bmInfo, DIB_RGB_COLORS,(void**)&ptPixels, NULL, 0);
				if (hDirectBitmap)
				{
					// here hDirectBitmap!=NULL so ptPixels!=NULL no need to test
					HGDIOBJ hPrevBufDirObject = SelectObject(hDirectDC, hDirectBitmap);
					BitBlt(hDirectDC,0,0,bm.bmWidth,bm.bmHeight,hBufferDC,0,0,SRCCOPY);					

					// process loop

					// A cool new implementation made by Mikko Mononen
					//   Coder: http://www.codeproject.com/script/profile/whos_who.asp?id=41615
					//   Found: http://www.codeproject.com/dialog/WinMakeInactive.asp?select=656456&df=100&forumid=25834#xx656456xx

					int iAlpha = (int)(255.0 * dfTrans / 100.0);

					int nSize = bm.bmWidth * bm.bmHeight;
					for (int i=0; i<nSize; i++)
					{
						// Calcuate grey
						// See Color Space FAQ at: http://www.faqs.org/faqs/graphics/colorspace-faq/
						// 0.212671 * R + 0.715160 * G + 0.072169 * B
						int iSrcR = GetDibR(ptPixels[i]);
						int iSrcG = GetDibG(ptPixels[i]);
						int iSrcB = GetDibB(ptPixels[i]);
						int iGrey = (iSrcR * 54 + iSrcG * 182 + iSrcB * 19) >> 8;

						// Take color from the gradient
						COLORREF Col = m_clrGradient[iGrey]; // You may want to try claming here...

						ptPixels[i] = RGB(
							(GetBValue( Col ) * iAlpha + iSrcB * (255 - iAlpha)) >> 8,
							(GetGValue( Col ) * iAlpha + iSrcG * (255 - iAlpha)) >> 8,
							(GetRValue( Col ) * iAlpha + iSrcR * (255 - iAlpha)) >> 8 );
					}

					// little clean up
					// Don't delete the result of SelectObject because it's our modified bitmap (hDirectBitmap)
					SelectObject(hDirectDC,hPrevBufDirObject);
					
					// finish
					hRetBmp = hDirectBitmap;
				}

				// clean up
				DeleteDC(hDirectDC);
			}

			if (hTmpBitmap)
			{
				SelectObject(hBmpDC, hBmp);
				DeleteObject(hTmpBitmap);
			}

			SelectObject(hBufferDC, hPrevBufObject);

			// hBufferDC is now useless
			DeleteDC(hBufferDC);
		}
	}

	return hRetBmp;
}

void CFadeWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	if (m_bAutoDel  &&  OnLButtonCheck())
		DestroyWindow();
}

void CFadeWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonDown(nFlags, point);

	if (m_bAutoDel  &&  OnRButtonCheck())
		DestroyWindow();
}

void CFadeWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	SetFocus();
}

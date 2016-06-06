// SwitcherButton.cpp
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
//
//
//
// Version: 1.0 revision 1
//
// Revision Log:
//
// SUN MAR 14 1999 - JIMMY BRUSH -  Finished Writing version 1.0
// MON MAR 15 1999 - JIMMY BRUSH -  Fixed RemoveButton to correctly decrement selected button
//									Added CMemDC by Keith Rule
//									Fixed up Animation a bit
//
//
//
// In the next version \ Wish List:
//
// 1. Tool Tips in CSwitcherButton
// 2. Support for more buttons than can display (with the up/down button at the right)
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Modifications by Yogesh Jagota.
// All my modifications in the existing code...
//	 a) either start with 'Yogesh Jagota' and end with 'End Yogesh Jagota',
//	 b) or, are commented in the end of line with 'Yogesh Jagota',
//
//						/************************
// Everything after the  * All the following... * box is added by me.
//						 ************************/
//
//  Sn. Description											Date
// ==== =================================================== ===============
//   1. Removed bold fonts from pressed buttons (Reason :
//		because I did'nt liked them. One pressed button was 
//		destroying the looks of all the other buttons).		07th Mar, 2000
// ---- ---------------------------------------------------	---------------
//   2. Turned buttons to "Hot" flat buttons [active on 
//		mouse over]. (Reason : Everything is going the hot
//		way, isn'nt it....)									07th Mar, 2000
// ---- ---------------------------------------------------	---------------
//	 3. Added borders before buttons to look more cool.		07th Mar, 2000
// ---- ---------------------------------------------------	---------------
//   4. Added Tooltip support.								07th Mar, 2000
// ---- ---------------------------------------------------	---------------
//   5. Now, the button cannot be unselected.				08th Mar, 2000
// ---- ---------------------------------------------------	---------------
//   6. The default font is now 'Tahoma'. If it's not found
//		'MS Sans Serif' is used. (You know something, I 
//		love that font.)									09th Mar, 2000
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SwitcherButton.h"
#include "docselect.h"

#include <winuser.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSwitcherButton

CSwitcherButton::CSwitcherButton()
{
	m_nState = SWITCHBUTTON_UP;
	HasCapture = false;
	m_iIcon = NULL;
	m_iID = -1;

	// Yogesh Jagota
	m_bPainted		= FALSE; 
	m_bLBtnDown		= FALSE;
	m_AttachedView	= NULL;
	// End Yogesh Jagota

	// Win95/98 balks at creating & deleting the font in OnPaint
	// so put it here

	// Yogesh Jagota
	if ( !m_fNormal.CreateFont( 13, 0, 0, 0, FW_NORMAL, 0, 
		0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_QUALITY | FF_DONTCARE, _T("Tahoma") ) )
	{
		m_fNormal.CreateFont(10,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_QUALITY|FF_DONTCARE,_T("MS Sans Serif"));
	}
	// End Yogesh Jagota

//	Yogesh Jagota
//	m_fBold.CreateFont(10,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_QUALITY|FF_DONTCARE,"MS Sans Serif");
//	End Yogesh Jagota
}

CSwitcherButton::~CSwitcherButton()
{
	m_fNormal.DeleteObject();
//	m_fBold.DeleteObject(); // Yogesh Jagota
}


BEGIN_MESSAGE_MAP(CSwitcherButton, CWnd)
	//{{AFX_MSG_MAP(CSwitcherButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSwitcherButton message handlers

void CSwitcherButton::OnPaint() 
{
	CPaintDC paintdc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);

	CMemDC2 dc(&paintdc, &rect);
	
	
	CString text;
	GetWindowText(text);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));

	CBrush brush;

	// Yogesh Jagota
	// Don't want borders before buttons, remove this fraction
	// of code, and a single line (commented) in the OnTimer function.
	brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	dc.FillRect(&rect, &brush);
	brush.DeleteObject();

	CRect rct;
	rct.left = rect.left;
	rct.top = rect.top;
	rct.right  = rect.left + 2;
	rct.bottom = rect.bottom;

	dc.Draw3dRect(&rct, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DSHADOW));

	rect.left += 3;
	// End Yogesh Jagota

	if (m_nState == SWITCHBUTTON_UP)
	{
		brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		dc.FillRect(&rect, &brush);

//		Yogesh Jagota
//		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DDKSHADOW));
//		rect.DeflateRect(1, 1);
//		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DLIGHT), ::GetSysColor(COLOR_3DSHADOW));
//		End Yogesh Jagota
	}
	else if (m_nState == SWITCHBUTTON_DOWN)
	{
		brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		dc.FillRect(&rect, &brush);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
//		Yogesh Jagota
//		rect.DeflateRect(1,1);
//		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DLIGHT));
//		End Yogesh Jagota

		// make it look pressed
		rect.top += 1;

	}
	else if (m_nState == SWITCHBUTTON_SELECTED)
	{
		brush.CreateSolidBrush(::GetSysColor(COLOR_3DLIGHT));
		dc.FillRect(&rect, &brush);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
//		Yogesh Jagota
//		rect.DeflateRect(1,1);
//		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DLIGHT));
//		End Yogesh Jagota

		// make it look pressed
		rect.top+= 1;
	}

	// dont even bother if no text
	if (text.IsEmpty() == FALSE)
	{
		// dont want text near the border
		rect.DeflateRect(2,2);

		// MS BUG: DT_VCENTER dont work! (or im doing something wrong)
		// so must Vertical Center it ourself
		CSize size;
		GetTextExtentPoint32(dc.GetSafeHdc(), text, text.GetLength(), &size);

		rect.top += (rect.Height() / 2) - (size.cy / 2) + 1;
		rect.left += 4;

//		Yogesh Jagota
//		if (m_nState == SWITCHBUTTON_SELECTED)
//			dc.SelectObject(&m_fBold);
//		else
//		End Yogesh Jagota
		dc.SelectObject(&m_fNormal);

		dc.DrawText(text, &rect, DT_PATH_ELLIPSIS | DT_VCENTER);
	}

	//if (m_iIcon != NULL)
	//	::DrawIconEx(dc.GetSafeHdc(), 4, icontop, m_iIcon, 16, 16, 0, (HBRUSH)brush, DI_NORMAL);

	brush.DeleteObject();
}

void CSwitcherButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	if (m_nState != SWITCHBUTTON_SELECTED)
	{
		SetCapture();
		m_nState = SWITCHBUTTON_DOWN;
		HasCapture = true;
		Invalidate();
	}

	m_bLBtnDown = TRUE; // Yogesh Jagota
}

void CSwitcherButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);

	bool change = true;

	if (m_nState == SWITCHBUTTON_DOWN)
	{
		m_nState = SWITCHBUTTON_SELECTED;
		::SendMessage(m_wndParent->GetSafeHwnd(), SWM_SELCHANGE, (WPARAM)this, 0);
	}
//	Yogesh Jagota.
//	Can't deselect button now...
//	else
//	{
//		m_bLBtnDown = FALSE; // Yogesh Jagota
//
//		if (m_nState == SWITCHBUTTON_UP)
//			change = false;
//
//		if (m_nState == SWITCHBUTTON_SELECTED)
//			::SendMessage(m_wndParent->GetSafeHwnd(), SWM_UNSELECT, (WPARAM)this, 0);
//
//		m_nState = SWITCHBUTTON_UP;
//	}
//	End Yogesh Jagota

	ReleaseCapture();
	HasCapture = false;

	if (change)
		Invalidate();
}

void CSwitcherButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);

	SetTimer(1, 10, NULL);	// Yogesh Jagota
	OnTimer(1);				// Yogesh Jagota

	if (HasCapture)
	{
		RECT rect;
		GetClientRect(&rect);

		if (point.x > rect.right || point.x < rect.left || point.y < rect.top || point.y > rect.bottom)
		{
			if (m_nState != SWITCHBUTTON_UP)
			{
				m_nState = SWITCHBUTTON_UP;
				Invalidate();
			}
		}
		else
		{
			if (m_nState != SWITCHBUTTON_DOWN)
			{
				m_nState = SWITCHBUTTON_DOWN;
				Invalidate();
			}
		}
	}
}

// Yogesh Jagota : Added support for tooltips...
BOOL CSwitcherButton::DoCreate(CWnd *parent, int x, int y, int cx, int cy, CString sFileName, CString sPath )
{
	m_wndParent = parent;

	WNDCLASS myclass;
	myclass.style = CS_HREDRAW|CS_VREDRAW|CS_PARENTDC;
	myclass.lpfnWndProc = AfxWndProc;
	myclass.cbClsExtra = 0;
	myclass.cbWndExtra = 0;
	myclass.hInstance = AfxGetInstanceHandle();
	myclass.hIcon = NULL;
	myclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myclass.hbrBackground = NULL;
	myclass.lpszMenuName = NULL;
	myclass.lpszClassName = _T("SwitcherButtonClass");
	AfxRegisterClass(&myclass);

	CRect rct(x,y,cx+x,cy+y);
	BOOL ret = Create( _T("SwitcherButtonClass"), sFileName, WS_CHILD|WS_VISIBLE, rct, parent, 0);

	// Yogesh Jagota
	GetClientRect( rct );

	if ( !m_ToolTip.Create( this ) )
		AfxMessageBox( _T("Unable to add tooltip to the button.") );
	else
		m_ToolTip.AddTool( this, sPath, rct, 1 );
	// End Yogesh Jagota

	return ret;
}

void CSwitcherButton::SetText(const char * text)
{
	m_ToolTip.UpdateTipText (text, this, 1);
	SetWindowText(text);
	Invalidate();
	return;
}

void CSwitcherButton::ReplaceIcon(HICON icon)
{
	m_iIcon = icon;
	Invalidate();
}

void CSwitcherButton::Refresh()
{
	Invalidate();
}

void CSwitcherButton::Select()
{
	if (HasCapture)
	{
		ReleaseCapture();
		HasCapture = false;
	}

	m_nState = SWITCHBUTTON_SELECTED;
	m_bLBtnDown = TRUE; // Yogesh Jagota
	Invalidate();
}

void CSwitcherButton::Unselect()
{
	if (HasCapture)
	{
		ReleaseCapture();
		HasCapture = false;
	}

	m_bLBtnDown = FALSE; // Yogesh Jagota
	m_nState = SWITCHBUTTON_UP;
	Invalidate();
}

/************************************************************************
 ************************************************************************
 *																		*
 * All following code is added by Yogesh Jagota							*
 *																		*
 ************************************************************************
 ************************************************************************/
void CSwitcherButton::OnTimer(UINT nIDEvent) 
{
	CRect rcItem;
	GetWindowRect(rcItem);

	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	if(( m_bLBtnDown == TRUE ) || ( !rcItem.PtInRect( ptCursor )))
	{
		KillTimer(1);

		if (m_bPainted == TRUE) {
			InvalidateRect (NULL);
		}

		m_bPainted = FALSE;
		return;
	}

	// On mouse over, show raised button.
	else if ( !m_bPainted )
	{
		CDC* pDC = GetDC();
		GetClientRect(rcItem);
		rcItem.left += 3; // Don't want borders. Delete this line.
		pDC->Draw3dRect( rcItem, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DSHADOW));
		m_bPainted = TRUE;
		ReleaseDC(pDC);
	}
	
	CWnd::OnTimer(nIDEvent);
}

BOOL CSwitcherButton::PreTranslateMessage( MSG *pMsg )
{
	m_ToolTip.RelayEvent( pMsg );

	return CWnd::PreTranslateMessage( pMsg );
}

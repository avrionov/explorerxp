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


#include "stdafx.h"
#include "MenuButton.h"
#include "Themes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuButton

const UINT ButtonHoverTimer = 666;

CMenuButton::CMenuButton()
{
	m_MenuID = m_nIndex = -1;
	m_pPopupMenu = NULL;
	m_bMouseOverButton = false;
}

CMenuButton::~CMenuButton()
{
}


BEGIN_MESSAGE_MAP(CMenuButton, CButton)
	//{{AFX_MSG_MAP(CMenuButton)
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMenuButton message handlers

DWORD CMenuButton::DoSingleClick ()
{
    DWORD dwSelectionMade  =0;

	m_pPopupMenu = (CMenu *) m_Menu.GetSubMenu (0);

    ASSERT(m_pPopupMenu != NULL);
    POINT point;

    CRect rect;
    GetWindowRect(rect);
    point.x = rect.right;
    point.y = rect.top;
    
	m_pPopupMenu->CheckMenuItem (m_nIndex, MF_CHECKED | MF_BYCOMMAND);
	
    m_nCommand  = m_pPopupMenu->TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD),
                                                  point.x, point.y, this );
	if (m_nCommand )
		SetIndex (PosFromID ( dwSelectionMade));	
	
	   
    return dwSelectionMade;
}

void CMenuButton::DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lpDrawItemStruct;
	HTHEME hTheme = NULL;	
	BOOL Themed = FALSE;

	if (ThemeLibLoaded) 
	{
		hTheme = zOpenThemeData(m_hWnd, L"Button");
		if(hTheme)
			Themed = TRUE;
	}
	
//			if(lpDIS->CtlID != IDC_OWNERDRAW_BTN)
//				return (0);

	HDC dc = lpDIS->hDC;

	// button state
	BOOL bIsPressed =	(lpDIS->itemState & ODS_SELECTED);
	BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
	BOOL bIsDisabled = (lpDIS->itemState & ODS_DISABLED);
	BOOL bDrawFocusRect = !(lpDIS->itemState & ODS_NOFOCUSRECT);

	RECT itemRect = lpDIS->rcItem;

	SetBkMode(dc, TRANSPARENT);

	// Prepare draw... paint button background			
	if(Themed && (hTheme != NULL))
	{
		DWORD state = (bIsPressed)?PBS_PRESSED:PBS_NORMAL;
		
		if(state == PBS_NORMAL) {
				
				if(bIsFocused)
					state = PBS_DEFAULTED;

				if(m_bMouseOverButton)
					state = PBS_HOT;
			}
		zDrawThemeBackground(hTheme, dc, BP_PUSHBUTTON, state, &itemRect, NULL);
	} else {
		if (bIsFocused)
			{
				HBRUSH br = CreateSolidBrush(RGB(0,0,0));  
				FrameRect(dc, &itemRect, br);
				InflateRect(&itemRect, -1, -1);
				DeleteObject(br);
			} // if		

		COLORREF crColor = GetSysColor(COLOR_BTNFACE);

		HBRUSH	brBackground = CreateSolidBrush(crColor);

		FillRect(dc, &itemRect, brBackground);

		DeleteObject(brBackground);

		// Draw pressed button
		if (bIsPressed)
		{
			HBRUSH brBtnShadow = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
			FrameRect(dc, &itemRect, brBtnShadow);
			DeleteObject(brBtnShadow);
		}
		else // ...else draw non pressed button
		{
			UINT uState = DFCS_BUTTONPUSH |
                    ((m_bMouseOverButton) ? DFCS_HOT : 0) |
                    ((bIsPressed) ? DFCS_PUSHED : 0);

			DrawFrameControl(dc, &itemRect, DFC_BUTTON, uState);
		} // else
	}
	// Read the button's title
	TCHAR sTitle[100];
	GetWindowText(sTitle, 100);

	RECT captionRect = lpDIS->rcItem;

	// Draw the icon
	BOOL bHasTitle = (sTitle[0] != '\0');
	

	// Write the button title (if any)
	if (bHasTitle)
	{// Draw the button's title
		// If button is pressed then "press" title also
		if (bIsPressed && !Themed)
			OffsetRect(&captionRect, 1, 1);

		// Center text
		RECT centerRect = captionRect;
		DrawText(dc, sTitle, -1, &captionRect, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
		LONG captionRectWidth = captionRect.right - captionRect.left;
		LONG captionRectHeight = captionRect.bottom - captionRect.top;
		LONG centerRectWidth = centerRect.right - centerRect.left;
		LONG centerRectHeight = centerRect.bottom - centerRect.top;
		OffsetRect(&captionRect, (centerRectWidth - captionRectWidth)/2, (centerRectHeight - captionRectHeight)/2);

		if(Themed)
		{	
			zDrawThemeText(	hTheme, dc, BP_PUSHBUTTON, PBS_NORMAL,
								sTitle, _tcslen(sTitle),
								DT_CENTER | DT_VCENTER | DT_SINGLELINE,
								0, &captionRect);
			
			
		}
		else
		{
			SetBkMode(dc, TRANSPARENT);

			if (bIsDisabled)
				{
				OffsetRect(&captionRect, 1, 1);
				SetTextColor(dc, ::GetSysColor(COLOR_3DHILIGHT));
				DrawText(dc, sTitle, -1, &captionRect, DT_WORDBREAK | DT_CENTER);
				OffsetRect(&captionRect, -1, -1);
				SetTextColor(dc, ::GetSysColor(COLOR_3DSHADOW));
				DrawText(dc, sTitle, -1, &captionRect, DT_WORDBREAK | DT_CENTER);
				} // if
			else
				{
				SetTextColor(dc, ::GetSysColor(COLOR_BTNTEXT));
				SetBkColor(dc, ::GetSysColor(COLOR_BTNFACE));
				DrawText(dc, sTitle, -1, &captionRect, DT_WORDBREAK | DT_CENTER);
				} // if
			} // if
	}

	CSize sizeArrow(5,9); // was (4,7), now matches Microsoft Outlook 2000 - jjszucs, 14 July 1999
    CSize sizeOffsetTop(1,+1);          // size of the top stairsteps
    CSize sizeOffsetBottom(1,-1);       // size of the bottom stairsteps

	CPoint pointStart, pointStop;
	CRect rect(itemRect);
	if (!bHasTitle)
	{
		CPoint pointCenter( (rect.Width()/2), (rect.Height()/2) );
		pointStart.x=pointCenter.x-(sizeArrow.cx/2);
		pointStart.y=pointCenter.y-(sizeArrow.cy/2);
		pointStop.x=pointStart.x;
		pointStop.y=pointStart.y+sizeArrow.cy;		
	} 
	else
	{
		CPoint ptArrow(
			rect.right-6, // 2 pixels for border + 4 pixels margin
			rect.top + rect.Height()/2
		);

		pointStart.x=ptArrow.x-sizeArrow.cx;
		pointStart.y=ptArrow.y-(sizeArrow.cy/2);
		pointStop.x=pointStart.x;
		pointStop.y=pointStart.y+sizeArrow.cy;

	}

	CDC *pDC        = CDC::FromHandle(lpDrawItemStruct->hDC);
    for(int iInd=0; iInd<sizeArrow.cx; iInd++)
    {
        pDC->MoveTo(pointStart);
        pDC->LineTo(pointStop);

        pointStart.Offset(sizeOffsetTop);
        pointStop .Offset(sizeOffsetBottom);
    }

	pDC->SetBkMode (TRANSPARENT);

			// Draw the focus rect
			//if (bIsFocused )
	if (bIsFocused && bDrawFocusRect)
	{
		RECT focusRect = itemRect;
		InflateRect(&focusRect, -3, -3);
		DrawFocusRect(dc, &focusRect);
	} 
}

BOOL CMenuButton::OnClicked()
{
	//if (m_pPopupMenu)
	{
		DoSingleClick ();
		GetParent()->SendMessage (WM_COMMAND, MAKEWPARAM (GetDlgCtrlID (), BN_DBLCLK), (LPARAM)m_hWnd);
	}
		
	return TRUE;
}


void CMenuButton::SetIndex (int nIndex)
{
	m_nIndex = nIndex;
	//if (m_MenuID != -1)
	//	SetTextFromMenu ();
}

void CMenuButton::SetMenu (DWORD nID)
{
	m_MenuID = nID;
	m_Menu.DestroyMenu ();
	m_Menu.LoadMenu (m_MenuID);

	if (m_nIndex != -1)
		SetTextFromMenu ();
}

void CMenuButton::SetTextFromMenu ()
{
	m_pPopupMenu = (CMenu*) m_Menu.GetSubMenu (0);

	ASSERT (m_pPopupMenu != NULL);
	
	CString cs;
//	m_pPopupMenu->GetMenuText (m_nIndex, cs);


	MENUITEMINFO MInfo;
	TCHAR szBuffer[140];
	MInfo.cbSize     = sizeof(MENUITEMINFO);
	MInfo.fMask      = MIIM_DATA | MIIM_ID |MIIM_SUBMENU |MIIM_TYPE; 
	MInfo.fType      = MFT_STRING;
	MInfo.fState     = MFS_DEFAULT;
	MInfo.dwTypeData = szBuffer;
	MInfo.cch        = 40; //sizeof(Buffer);
	
	if (m_pPopupMenu->GetMenuItemInfo (m_nIndex, &MInfo, TRUE))
		SetWindowText (cs);
}

int CMenuButton::PosFromID (DWORD dID)
{
	m_pPopupMenu = (CMenu *) m_Menu.GetSubMenu (0);

	ASSERT (m_pPopupMenu != NULL);

	MENUITEMINFO MInfo;	
	MInfo.cbSize     = sizeof(MENUITEMINFO);
	MInfo.fMask      = MIIM_ID; 
	MInfo.fState     = MFS_DEFAULT;

	for (int i = 0; i < m_pPopupMenu->GetMenuItemCount (); i++)
	{
		MInfo.wID = (UINT)-1;
		if (m_pPopupMenu->GetMenuItemInfo (i, &MInfo, TRUE))
			if (MInfo.wID == dID)
				return i;
	}

	return -1;
}

void DDX_MenuButton (CDataExchange *pDX, int nIDC, CMenuButton &rCFEC)
{
	// Subclass the specified wcSliderButton class object to the edit control
	// with the ID nIDC. dwFlags is used to setup the control
	ASSERT (pDX->m_pDlgWnd->GetDlgItem(nIDC));
	if (rCFEC.m_hWnd == NULL)					// not yet subclassed
	{
		ASSERT (!pDX->m_bSaveAndValidate);
		// subclass the control to the edit control with the ID nIDC
		HWND hWnd = pDX->PrepareEditCtrl(nIDC);
		if (!rCFEC.SubclassWindow(hWnd))
		{										// failed to subclass the edit control
			ASSERT(FALSE);
			AfxThrowNotSupportedException();
		}
		
	}
}

void CMenuButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bMouseOverButton && InRect())
	{
		m_bMouseOverButton = true;
		InvalidateRect(NULL, FALSE);
		SetTimer(ButtonHoverTimer, 10, NULL);
	}	
	CButton::OnMouseMove(nFlags, point);
}

bool CMenuButton::InRect() 
{
	CPoint	p;
	GetCursorPos(&p);

	CRect	wr;
	GetWindowRect(wr);

	return wr.PtInRect(p) ? true:false;
}

void CMenuButton::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == ButtonHoverTimer)
	{
		if (m_bMouseOverButton && !InRect())
		{
			// Change the state

			m_bMouseOverButton = false;
			InvalidateRect(NULL, FALSE);			

			KillTimer(nIDEvent);
		}
	}


	CButton::OnTimer(nIDEvent);
}

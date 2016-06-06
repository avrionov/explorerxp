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


#include "stdafx.h"
#include "explorerxp.h"
#include "GridList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGridList, CGridCtrl)
//{{AFX_MSG_MAP(CGridCtrl)
//  ON_WM_SIZE()
    ON_WM_KEYDOWN()    
//}}AFX_MSG_MAP
 END_MESSAGE_MAP()

CGridList::CGridList()
{

}

CGridList::~CGridList()
{

}


void CGridList::OnBeginDrag()
{
	m_MouseMode = MOUSE_NOTHING;
	SendMessageToParent (-1,-1, GVN_BEGINRDRAG);
}


void CGridList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsAltpressed () || IsCTRLpressed ())
	{
		CGridCtrl::OnKeyDown (nChar, nRepCnt, nFlags);	
		m_LookStr = "";
		return;
	}

	if ( ((nChar >= 'A') && (nChar <= 'Z')) || (nChar >= '0' && nChar <= '9') )
	{
		CCellID next;
		if (!IsValid(m_idCurrentCell))
			next = CCellID (1, 0);
		else
			next = m_idCurrentCell;

		BOOL bChangeLine = FALSE;
		BOOL bHorzScrollAction = FALSE;
		BOOL bVertScrollAction = FALSE;
		BOOL bFoundVisible = FALSE;

		CGridCellBase	*cell = GetCell (next.row, 0);
		CString text = cell->GetText (); // never return NULL
		text.MakeLower();

		int iOrig = next.row;

		CString tmpLook = m_LookStr;
		tmpLook += (char) nChar;
		tmpLook.MakeLower();

		if (text.Find (tmpLook) == 0)			
        {
            bFoundVisible = TRUE;
			m_LookStr = tmpLook;
            
        }
		else
		{

			next.row++;

			if (next.row >= GetRowCount ())
				next.row = 1;

			while( next.row != iOrig)
			{
				CGridCellBase	*cell = GetCell (next.row, 0);
				CString text = cell->GetText (); // never return NULL
				text.MakeLower();

				if (text.Find (tmpLook) == 0)			
				{
					bFoundVisible = TRUE;
					m_LookStr = tmpLook;
					break;
				}
				next.row++;
				if (next.row >= GetRowCount ())
					next.row = 1;
			}
		}
        
		

		if( !bFoundVisible)
		{
			next.row = iOrig;
			next.row++;

			if (next.row >= GetRowCount ())
				next.row = 1;

			m_LookStr =  (char)nChar;
			m_LookStr.MakeLower();
			while( next.row != iOrig)
			{
				CGridCellBase	*cell = GetCell (next.row, 0);
				CString text = cell->GetText (); // never return NULL
				text.MakeLower();

				if (text.Find (m_LookStr) == 0)
				//if(text && (text[0] == nChar) || text[0] == tolower (nChar) )
				{
					bFoundVisible = TRUE;
					//m_LookStr = tmpLook;
					break;
				}
				next.row++;
				if (next.row >= GetRowCount ())
					next.row = 1;
			}

		}

        if( !bFoundVisible)
		{
			m_LookStr = "";
            next.row = iOrig;
		}

	if (next != m_idCurrentCell)
		{
			// While moving with the Cursorkeys the current ROW/CELL will get selected
			// OR Selection will get expanded when SHIFT is pressed
			// Cut n paste from OnLButtonDown - Franco Bez
			// Added check for NULL mouse mode - Chris Maunder.
			if (m_MouseMode == MOUSE_NOTHING)
			{
				m_PrevSelectedCellMap.RemoveAll();
				m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
				if (!IsSHIFTpressed() || nChar == VK_TAB)
					m_SelectionStartCell = next;

				// Notify parent that selection is changing - Arthur Westerman/Scot Brennecke 
				SendMessageToParent(next.row, next.col, GVN_SELCHANGING);
				OnSelecting(next);
				SendMessageToParent(next.row, next.col, GVN_SELCHANGED);

				m_MouseMode = MOUSE_NOTHING;
			}

			SetFocusCell(next);

			if (!IsCellVisible(next))
			{

				switch (nChar)
				{
				case VK_RIGHT:  
					SendMessage(WM_HSCROLL, SB_LINERIGHT, 0); 
					bHorzScrollAction = TRUE;
					break;
                
				case VK_LEFT:   
					SendMessage(WM_HSCROLL, SB_LINELEFT, 0);  
					bHorzScrollAction = TRUE;
					break;
                
				case VK_DOWN:   
					SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);  
					bVertScrollAction = TRUE;
					break;
                
				case VK_UP:     
					SendMessage(WM_VSCROLL, SB_LINEUP, 0);    
					bVertScrollAction = TRUE;
					break;                
                
				case VK_TAB:    
					if (IsSHIFTpressed())
					{
						if (bChangeLine) 
						{
							SendMessage(WM_VSCROLL, SB_LINEUP, 0);
							bVertScrollAction = TRUE;
							SetScrollPos32(SB_HORZ, m_nHScrollMax);
							break;
						}
						else 
						{
							SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
							bHorzScrollAction = TRUE;
						}
					}
					else
					{
						if (bChangeLine) 
						{
							SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
							bVertScrollAction = TRUE;
							SetScrollPos32(SB_HORZ, 0);
							break;
						}
						else 
						{
							SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
							bHorzScrollAction = TRUE;
						}
					}
					break;
				}
				EnsureVisible(next); // Make sure cell is visible
				Invalidate();
			}
			EnsureVisible(next); // Make sure cell is visible

			if (bHorzScrollAction)
				SendMessage(WM_HSCROLL, SB_ENDSCROLL, 0);
			if (bVertScrollAction)
				SendMessage(WM_VSCROLL, SB_ENDSCROLL, 0);
		}
	}
	
	CGridCtrl::OnKeyDown (nChar, nRepCnt, nFlags);	
}


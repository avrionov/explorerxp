/* Copyright 2002-2021 Nikolay Avrionov. All Rights Reserved.
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
#include "ExplorerXP.h"
#include "Viewer.h"
#include "Globals.h"
#include "ShellContextMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewer::CViewer()
{
	
}

CViewer::~CViewer()
{

}

DROPEFFECT CViewer::CalcDropEffect (CSelRowArray &ar, const TCHAR * root, const TCHAR * drop_source,  DWORD dwKeyState)
{
	if (!CanDropToView ())
		if (_tcsicmp (root , GetTitle ()) == 0)
			 return DROPEFFECT_NONE;
		
	return ::CalcDropEffect (ar, root, drop_source, dwKeyState);
}

bool CViewer::OnDrop (COleDataObject *object, DROPEFFECT dwEffect , const TCHAR *pTo)
{

	CSelRowArray ar;

	CString source_folder;
	
	CString dest_folder;


	dest_folder = pTo;
	
	if (!ReadHDropData (object, ar, source_folder))
		return false;

	UINT file_oper = FO_COPY;

	DROPEFFECT drop_effect = dwEffect;

	FILEOP_FLAGS copyOnCollison = 0;

	switch (drop_effect)
		{
			case DROPEFFECT_COPY:
				file_oper = FO_COPY;
				if (source_folder ==dest_folder)
					copyOnCollison = FOF_RENAMEONCOLLISION;
			break;

			case DROPEFFECT_MOVE:
				file_oper = FO_MOVE;
				break;
			case DROPEFFECT_LINK:
				return CreateShortcuts (ar, GetTitle ());
				
			default:
				TRACE(_T("Invalide file operation %d\n"), drop_effect);
				return false;
		}					
		
	return FileOperation (m_pGridCtrl, ar, file_oper, FOF_ALLOWUNDO | copyOnCollison , dest_folder) != 0;	
}


void CViewer::ContextMenu (CView* pView, CSelRowArray &ar, CPoint &pt) 
{
//	CMenu * pMenu = NULL;
	UINT idCommand;
	
	CShellContextMenu scm;
	
	scm.SetObjects (ar);

	idCommand = scm.ShowContextMenu (pView, pt);
	
	switch (idCommand)
	{
		case CONTEXT_EXPLORE:
		{
			for (unsigned int i = 0 ; i < ar.size (); i++)
				if (IsDirectory (ar[i].m_Path))
				{
					SureBackSlash (ar[i].m_Path);
					OpenFolder (ar[i].m_Path);
				}				
		} break;
		
		case CONTEXT_OPEN:
		{
			
			for (unsigned int i = 0 ; i < ar.size (); i++)
				if (IsDirectory (ar[i].m_Path))
				{
					::SetRoot (ar[i].m_Path);
					break;
				}				
			
			CSelRowArray ar2;

			for (size_t i = 0 ; i < ar.size (); i++)
				if (!IsDirectory (ar[i].m_Path))
				{
					ar2.push_back (ar[i]);			
				}				
				
			if (ar2.size () != 0)
			{
				CShellContextMenu scm2;
				scm2.SetObjects (ar2);
				scm2.InvokeCommand (_T("open"));
			}

		} break;

		case CONTEXT_RENAME:
		{	
			if (pView)
				pView->SendMessage (WM_COMMAND, ID_RENAME, 0);
		} break;
	}		
}

void CViewer::Properties (CSelRowArray &ar)
{	
	CShellContextMenu scm2;
	scm2.SetObjects (ar);
	scm2.InvokeCommand (_T("properties"));	
}

void CViewer::ContextMenuOnHeader(CView *pView, CPoint &pt, NM_GRIDVIEW* pItem) 
{
	CMenu menu;
	menu.CreatePopupMenu();
	
	DWORD flag = 0;
	if (GetCount() == 0)
		flag |= MF_GRAYED;

	CString str;

	str.LoadString (IDS_AUTOSIZETHIS);
	menu.AppendMenu (MF_STRING | flag, 101, str);		
	str.LoadString (IDS_AUTOSIZEALL);
	menu.AppendMenu (MF_STRING | flag, 102, str);	
	
	menu.AppendMenu (MF_SEPARATOR , 0, (LPCTSTR)NULL);		

	for (unsigned int i = 0; i < m_Headers.size (); i++) 
	{
		DWORD mask = 1 << i;
		DWORD itemFlag = ((mask & m_State.m_Visible) == mask) ? MF_CHECKED : 0;
		
		int width = m_pGridCtrl->GetColumnWidth (i);
		if (width <= 1) 
		{
			m_State.m_Visible &= ~(1 << i);
			itemFlag = 0;
		}
		else
		{
			m_State.m_Widths[i] = width;
		}

		if (i == 0)
			itemFlag |= MF_GRAYED;
		
		menu.AppendMenu (MF_STRING | itemFlag, i+1, m_Headers[i]);		
	}


	 int result = menu.TrackPopupMenu (TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, pView );
	 if (result == 101)
	 {
		 m_pGridCtrl->AutoSizeColumn (pItem->iColumn, GVS_DATA);
		 return;
	 }

	 if (result == 102)
	 {
		 m_pGridCtrl->AutoSizeColumns (GVS_DATA);
		 return;
	 }

	 if (result != 0) 
	 {
		 result --;
		 DWORD mask = 1 << result;	
		 DWORD state = m_State.m_Visible & mask;
		 state = ~state;
		 state = state & 0xFFFFFFFF;
		 //m_State.m_Visible &= state;
		 m_State.m_Visible = (( m_State.m_Visible & state) | state) | ( m_State.m_Visible & ~ mask);
		
		 int newWidth = 0;
		 
		 if  ((mask & m_State.m_Visible) == mask) 
		 {
			 if ( m_State.m_Widths[result] <= 1) m_State.m_Widths[result] = 70;
			 newWidth =  m_State.m_Widths[result];
		 }
					 
		 if (m_pGridCtrl)
			m_pGridCtrl->SetColumnWidth (result, newWidth);
	 }
}

void CViewer::SetupGrid ()
{
	if (!m_pGridCtrl)
		return;

	m_pGridCtrl->SetColumnCount(static_cast<int>(m_Headers.size ()));		
	m_pGridCtrl->SetSortColumn (m_State.m_nSortColumn);
	m_pGridCtrl->SetSortAscending (m_State.m_bAscending);			

	for (unsigned int i = 0; i < m_Headers.size (); i++) 
	{
		DWORD mask = 1 << i;
		if ((mask & m_State.m_Visible) == mask) 
		{
			if ( m_State.m_Widths[i] <= 1)
				m_State.m_Widths[i] = 70;

			m_pGridCtrl->SetColumnWidth (i, m_State.m_Widths[i]);
		}
		else
			m_pGridCtrl->SetColumnWidth (i, 0);
	}

	if (ExpandLastColumn())
		m_pGridCtrl->ExpandLastColumn ();

	m_pGridCtrl->ClearSelection ();
	m_pGridCtrl->Invalidate ();
}

void CViewer::OnClose ()
{

}

void CViewer::SaveState(const TCHAR *name)
{	
	CFolderState state;
	GetState (state);
	gFolderStateMan.SaveState (name, state);
}

void CViewer::GetState (CFolderState &state)
{
	if (!m_pGridCtrl)
		return;

	if (m_pGridCtrl->GetColumnCount() == 0) return;

	state.m_Visible = 0;
	
	size_t column_count = min(m_Headers.size(), m_pGridCtrl->GetColumnCount());

	for (unsigned int i = 0; i < column_count; i++) 
	{
		DWORD mask = 1 << i;					
		int width = m_pGridCtrl->GetColumnWidth (i);
		if (width <= 1) 			
			;//state.m_Visible &= ~(mask);						
		else
		{
			state.m_Widths[i] = width;
			state.m_Visible |= mask;
		}
				
	}

	state.m_bAscending = m_pGridCtrl->GetSortAscending();
	state.m_nSortColumn = m_pGridCtrl->GetSortColumn();
}

void CViewer::SetState (CFolderState &state)
{
	//m_State.LimitedEq (state);
	m_State = state;
	SetupGrid ();
}

COLORREF CViewer::GetSelectedColorBackground ()
{
	return HLS_TRANSFORM(::GetSysColor(COLOR_HIGHLIGHT), 25, 0);
}

COLORREF CViewer::GetSelectedColorText()
{
	return ::GetSysColor(COLOR_HIGHLIGHTTEXT);
}
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
#include "ShellContextMenu.h"
#include "Viewers\RecBinViewer.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IContextMenu3 * g_IContext3 = NULL;

struct _CommandEx
{
	TCHAR _Name[256];
	UINT _Cmd;
};

WNDPROC gOldWndProc = NULL;

_CommandEx Commands [] = { _T("Explore"), CONTEXT_EXPLORE,
							_T("Open"),	  CONTEXT_OPEN,
							_T("rename"), CONTEXT_RENAME,
						};

const int command_size = sizeof (Commands) / sizeof (_CommandEx);


bool IsSpecialCommand (const TCHAR *verb, UINT &idCommand)
{
	for (int i = 0; i  < command_size; i++ )
	{
		if (_tcsicmp (verb, Commands[i]._Name) == 0)
		{
			idCommand = Commands[i]._Cmd;
			return true;
		}
	}
	return false;
}

WNDPROC CShellContextMenu::m_pOldWndProc = NULL;

CShellContextMenu::CShellContextMenu()
{
	m_psfFolder = NULL;
	m_pidlArray = NULL;
	m_pMenu = NULL;
}

CShellContextMenu::~CShellContextMenu()
{	
	Clean();
	if (m_pMenu)
		delete m_pMenu;
}

void CShellContextMenu::Clean ()
{
	if (m_psfFolder && bDelete)
		m_psfFolder->Release ();

	m_psfFolder = NULL;
	FreePIDLArray (m_pidlArray);
	m_pidlArray = NULL;
}

BOOL CShellContextMenu::GetContextMenu (void ** ppContextMenu)
{
	if (!m_psfFolder)
		return FALSE;

	*ppContextMenu = NULL;
	LPCONTEXTMENU icm1 = NULL;

	m_psfFolder->GetUIObjectOf (NULL, nItems, (LPCITEMIDLIST *) m_pidlArray, IID_IContextMenu, NULL, (void**) &icm1);
		
	if (icm1->QueryInterface (IID_IContextMenu3, ppContextMenu) == NOERROR)
	{
		icm1->Release(); 
		return TRUE;
	}

	return FALSE;
}


LRESULT CALLBACK CShellContextMenu::HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_IContext3)
	{
		switch (message)
		{ 
			case WM_MENUCHAR:				
				{
					LRESULT lResult = 0;
					g_IContext3->HandleMenuMsg2 (message, wParam, lParam, &lResult);
					return (lResult);
				}		

			case WM_DRAWITEM:
			case WM_MEASUREITEM:
				if (wParam) 
					break; 

			case WM_INITMENUPOPUP:			
					g_IContext3->HandleMenuMsg (message, wParam, lParam);				
				
				return (message == WM_INITMENUPOPUP ? 0 : TRUE); 
				
			default:
				break;
		}	
	}
	
	// Windows Vista Fix
	
	return ::CallWindowProc (m_pOldWndProc , hWnd, message, wParam, lParam);
	//return ::CallWindowProc ((WNDPROC) GetProp ( hWnd, TEXT ("OldWndProc")), hWnd, message, wParam, lParam);
}

LRESULT CALLBACK CShellContextMenu::NewMenuHookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_IContext3)
	{
		switch (message)
		{ 
		case WM_MENUCHAR:				
			{
				LRESULT lResult = 0;
				g_IContext3->HandleMenuMsg2 (message, wParam, lParam, &lResult);
				return (lResult);
			}		

		case WM_DRAWITEM:
		case WM_MEASUREITEM:
			if (wParam) 
				break; 
			g_IContext3->HandleMenuMsg (message, wParam, lParam);				
			return (message == WM_INITMENUPOPUP ? 0 : TRUE); 

		default:
			break;
		}	
	}

	return ::CallWindowProc (gOldWndProc, hWnd, message, wParam, lParam);
}

UINT CShellContextMenu::ShowContextMenu(CWnd *pWnd, CPoint pt)
{	
	LPCONTEXTMENU pContextMenu = NULL;	
   
	if (!GetContextMenu ((void**) &pContextMenu))	
		return 0;

	m_pMenu = GetMenu ();

	HRESULT hResult = pContextMenu->QueryContextMenu (m_pMenu->m_hMenu, m_pMenu->GetMenuItemCount (), MIN_ID, MAX_ID, CMF_NORMAL | CMF_EXPLORE | CMF_CANRENAME );

	if (FAILED(hResult)) 
		return 0;
 			
	m_pOldWndProc = (WNDPROC) SetWindowLongPtr  (pWnd->m_hWnd, GWLP_WNDPROC, (LONG_PTR)&CShellContextMenu::HookWndProc);
	g_IContext3 = (LPCONTEXTMENU3) pContextMenu;
	
	UINT idCommand = m_pMenu->TrackPopupMenu (TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, pWnd);

	if (m_pOldWndProc) 
		SetWindowLongPtr  (pWnd->m_hWnd, GWLP_WNDPROC, (LONG_PTR) m_pOldWndProc);

	if (idCommand >= MIN_ID && idCommand <= MAX_ID)	// see if returned idCommand belongs to shell menu entries
	{
		TCHAR verbname[100] = {0};		
		pContextMenu->GetCommandString (idCommand - MIN_ID, GCS_VERB, NULL, ( LPSTR)verbname, 100);		
		
		if (!IsSpecialCommand (verbname, idCommand))		
		{
			InvokeCommand (pContextMenu, idCommand - MIN_ID);	// execute related command
			idCommand = 0;
		}
	}
	
	if (pContextMenu) 
	{
		pContextMenu->Release();
		g_IContext3 = NULL;
	}
	return (idCommand);
}


void CShellContextMenu::InvokeCommand (LPCONTEXTMENU pContextMenu, UINT idCommand)
{
	CMINVOKECOMMANDINFO cmi = {0};
	cmi.cbSize = sizeof (CMINVOKECOMMANDINFO);
	cmi.lpVerb = (LPSTR)MAKEINTRESOURCE (idCommand);
	cmi.nShow = SW_SHOWNORMAL;	
	pContextMenu->InvokeCommand (&cmi);
}

void CShellContextMenu::InvokeCommand (const TCHAR *command)
{	
	LPCONTEXTMENU pContextMenu;	
	
	if (!GetContextMenu ((void**) &pContextMenu))	
		return;	

	try {

		m_pMenu = GetMenu();
		
		pContextMenu->QueryContextMenu (m_pMenu->m_hMenu, m_pMenu->GetMenuItemCount (), MIN_ID, MAX_ID, CMF_NORMAL |  CMF_CANRENAME );
	 
		int idCommand = -1;
		bool bFound = false;
		for ( int i = 0; i < m_pMenu->GetMenuItemCount (); i++)
		{
			int id = m_pMenu->GetMenuItemID ( i);
			if (id > 0)
			{
				TCHAR verbname[100]= {0};						

				HRESULT hr = pContextMenu->GetCommandString (id - MIN_ID, GCS_VERB, NULL, (LPSTR)verbname, 100);
				if (SUCCEEDED (hr))
				{				
					if (_tcsicmp (verbname, command) == 0)
					{
						bFound = true;
						idCommand = id - MIN_ID;
					}
				}			
			}
		}
		
		if (bFound)
		{
			CMINVOKECOMMANDINFO cmi = {0};
			cmi.cbSize = sizeof (CMINVOKECOMMANDINFO);
			cmi.lpVerb = (LPSTR)MAKEINTRESOURCE(idCommand);
			cmi.nShow = SW_SHOWNORMAL;
		
			//HRESULT res = pContextMenu->InvokeCommand (&cmi);
		}	
	}
	catch (...)
	{
		// crash in the contect menu
		// fix!!!
	}
	pContextMenu->Release();
	g_IContext3 = NULL;
}

void CShellContextMenu::SetObjects(CStringArray &strArray)
{	
	Clean ();
	IShellFolder * psfDesktop = NULL;
	SHGetDesktopFolder (&psfDesktop);	// needed to obtain full qualified pidl	
	LPITEMIDLIST pidl = NULL;
	
	psfDesktop->ParseDisplayName (NULL, 0, strArray.ElementAt (0).GetBuffer (0), NULL, &pidl, NULL);

	if (!pidl) return;

	LPITEMIDLIST pidlItem = NULL;	// relative pidl

	SHBindToParent (pidl, IID_IShellFolder, (void **) &m_psfFolder, NULL);

	if (!m_psfFolder)
		return;

	CoTaskMemFree (pidl);
	
	IShellFolder * psfFolder = NULL;
	nItems = static_cast<int>(strArray.GetSize ());
	m_pidlArray = (LPITEMIDLIST *) realloc (m_pidlArray, (nItems) * sizeof (LPITEMIDLIST));
	for (int i = 0; i < nItems; i++)
	{
		psfDesktop->ParseDisplayName (NULL, 0, strArray.ElementAt(i).GetBuffer(0), NULL, &pidl, NULL);				
		SHBindToParent (pidl, IID_IShellFolder, (void **) &psfFolder, (LPCITEMIDLIST *) &pidlItem);
		m_pidlArray[i] = ILClone (pidlItem);	
		CoTaskMemFree (pidl);		
		psfFolder->Release ();
	}
	
	psfDesktop->Release ();

	bDelete = TRUE;	// indicates that m_psfFolder should be deleted by CShellContextMenu
}

void CShellContextMenu::SetObjects (CSelRowArray &ar)
{
	CStringArray strArray;

	for (unsigned int i = 0; i < ar.size () ; i++)
		strArray.Add (ar[i].m_Path);

	SetObjects (strArray);
}

void CShellContextMenu::SetObjects(IShellFolder * psfFolder, LPITEMIDLIST *pidlArray, int nItemCount)
{	
	Clean ();

	m_psfFolder = psfFolder;

	m_pidlArray = (LPITEMIDLIST *) malloc (nItemCount * sizeof (LPITEMIDLIST));

	for (int i = 0; i < nItemCount; i++)
		m_pidlArray[i] = ILClone (pidlArray[i]);

	nItems = nItemCount;
	bDelete = FALSE;	// indicates wheter m_psfFolder should be deleted by CShellContextMenu
}


void CShellContextMenu::FreePIDLArray(LPITEMIDLIST *pidlArray)
{
	if (!pidlArray)
		return;

	int iSize = nItems; 

	for (int i = 0; i < iSize; i++)
		ILFree (pidlArray[i]);
	free (pidlArray);
}

CMenu * CShellContextMenu::GetMenu()
{
	if(m_pMenu){
		delete m_pMenu;
		m_pMenu = NULL;
	}


	if (!m_pMenu)
	{
		m_pMenu = new CMenu;
		m_pMenu->CreatePopupMenu();	// create the popupmenu (its empty)
	}
	return (m_pMenu);
}

void CShellContextMenu::NewMenu (CString path, CPoint pt, CWnd *pWnd)
{
	IShellExtInit *newMenu;
	CoCreateInstance(CLSID_NewMenu, NULL, CLSCTX_INPROC_SERVER, IID_IShellExtInit, (LPVOID*)&newMenu);	
	IShellFolder * psfDesktop = NULL;
	SHGetDesktopFolder (&psfDesktop);	
	LPITEMIDLIST pidl = NULL;
	psfDesktop->ParseDisplayName (NULL, 0, path.GetBuffer (0), NULL, &pidl, NULL);
	HRESULT res= newMenu->Initialize(pidl, NULL, NULL);

	CMenu menu;
	menu.LoadMenu (IDR_EMPTY);
	CMenu *pPopup = menu.GetSubMenu (0);

	m_pMenu = GetMenu();
	
	LPCONTEXTMENU pContextMenu;	
	res = newMenu->QueryInterface(IID_IContextMenu3, (void**)&pContextMenu);		
	pContextMenu->QueryContextMenu (m_pMenu->m_hMenu, m_pMenu->GetMenuItemCount (), MIN_ID, MAX_ID, CMF_NORMAL);

	gOldWndProc = NULL;
	
	gOldWndProc = (WNDPROC) SetWindowLongPtr(pWnd->m_hWnd, GWLP_WNDPROC, (LONG_PTR)CShellContextMenu::NewMenuHookWndProc);
	g_IContext3 = (LPCONTEXTMENU3) pContextMenu;

	int count = pPopup->GetMenuItemCount();
	CMenu *newPopup = pPopup->GetSubMenu (10);
		
	g_IContext3->HandleMenuMsg (WM_INITMENUPOPUP, (WPARAM)m_pMenu->m_hMenu, 0);
	count = m_pMenu->GetMenuItemCount ();	
	TCHAR buf[100];
	TCHAR menu_text[100];

	for (int i = 0; i < count; i++)
	{
		MENUITEMINFO info;
		ZeroMemory (&info, sizeof (info));
		info.cbSize = sizeof (MENUITEMINFO); // must fill up this field
		
		info.dwItemData = (ULONG_PTR)buf;
		info.dwTypeData = menu_text;
		info.cch = 99;		
		info.fMask = MIIM_STATE | MIIM_STRING | MIIM_ID |MIIM_FTYPE | MIIM_BITMAP | MIIM_DATA ;
		m_pMenu->GetMenuItemInfo(i, &info, TRUE);
		//DWORD Error = GetLastError ();
		newPopup->InsertMenuItem (i+1, &info, TRUE);
	}

	newPopup->DeleteMenu (ID_NEW_XXX, MF_BYCOMMAND);
	int result = pPopup->TrackPopupMenu (TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, pWnd);

	if (gOldWndProc) 
		SetWindowLongPtr (pWnd->m_hWnd, GWLP_WNDPROC, (LONG_PTR) gOldWndProc);

	bool bInvoked = false;
	if ( (result >= MIN_ID) && (result <= MAX_ID))
	{
		InvokeCommand(pContextMenu, result - MIN_ID);
		bInvoked = true;
	}
	
	pContextMenu->Release();
	newMenu->Release();
	psfDesktop->Release();

	if (result != 0 && !bInvoked)
		pWnd->SendMessage(WM_COMMAND, result);
}
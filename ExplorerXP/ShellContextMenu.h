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


#ifndef __SHELLCONTEXTMENU_H__
#define __SHELLCONTEXTMENU_H__


#pragma once

#include "globals.h"


#define MIN_ID 20000
#define MAX_ID 25000

#define CONTEXT_EXPLORE (MAX_ID + 1)
#define CONTEXT_RENAME  (MAX_ID + 2)
#define CONTEXT_OPEN	(MAX_ID + 3)

/////////////////////////////////////////////////////////////////////
// class to show shell contextmenu of files/folders/shell objects
// developed by R. Engels 2003
/////////////////////////////////////////////////////////////////////

class CShellContextMenu  
{
public:
	CMenu* GetMenu ();
	void SetObjects (IShellFolder * psfFolder, LPITEMIDLIST * pidlArray, int nItemCount);
	void SetObjects (CStringArray &strArray);
	void SetObjects (CSelRowArray &ar);	

	UINT ShowContextMenu (CWnd* pWnd, CPoint pt);
	CShellContextMenu();
	virtual ~CShellContextMenu();
	void InvokeCommand (const TCHAR *command);
	void NewMenu (CString path, CPoint pt, CWnd *pWnd);
	void Clean ();

private:
	int nItems;
	BOOL bDelete;
	CMenu * m_pMenu;
	IShellFolder * m_psfFolder;
	LPITEMIDLIST * m_pidlArray;		
	static WNDPROC	m_pOldWndProc;

	void InvokeCommand (LPCONTEXTMENU pContextMenu, UINT idCommand);	
	BOOL GetContextMenu (void ** ppContextMenu);
	HRESULT SHBindToParentEx (LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast);
	static LRESULT CALLBACK HookWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK NewMenuHookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void FreePIDLArray (LPITEMIDLIST * pidlArray);	
};

#endif // __SHELLCONTEXTMENU_H__

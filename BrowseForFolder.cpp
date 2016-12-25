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
#include "BrowseForFolder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#ifndef BIF_NEWDIALOGSTYLE
	#define BIF_NEWDIALOGSTYLE     0x0040
#endif
#ifndef BIF_SHAREABLE
	#define BIF_SHAREABLE          0x8000
#endif

CBrowseForFolder::CBrowseForFolder()
{
	Image = 0;
	flags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_SHAREABLE;
	hWndOwner = NULL;
	SetNewStyle(true);
}

CBrowseForFolder::~CBrowseForFolder()
{
}

// browse for a folder
bool CBrowseForFolder::GetFolder(CString &returnPath)
{
	LPITEMIDLIST pidlRoot = ILCreateFromPathW (strDirRoot);;
		
	TCHAR strBuff[MAX_PATH + 1];
	
	returnPath.Empty();
	
	BROWSEINFO brInfo= 
	{
			hWndOwner, 
			pidlRoot, 
			strBuff, 
			strTitle,
			flags,
			BrowseCallbackProc,
			(LPARAM)&strStartupDir,
			Image
	};

	if (flags & 0x0040) OleInitialize(NULL);

	LPITEMIDLIST pidl = SHBrowseForFolder(&brInfo);

	if (flags & 0x0040) OleUninitialize();

	if (pidl)
	{
		SHGetPathFromIDList(pidl, strBuff);
		CoTaskMemFree(pidl);
		returnPath = strBuff;
	}

	if (pidlRoot)  CoTaskMemFree(pidlRoot);		
	
	
	if (!returnPath.IsEmpty())	return true;
	else						return false;
}

void CBrowseForFolder::SetEditBox(bool val)
{
	if (val) 
		flags |= BIF_EDITBOX;
	else 
		flags &= ~BIF_EDITBOX;
}


void CBrowseForFolder::SetStatusBar(bool val)
{
	if (val) 
		flags |= BIF_STATUSTEXT;
	else 
		flags &= ~BIF_STATUSTEXT;
}

int CALLBACK CBrowseForFolder::BrowseCallbackProc(HWND hwnd, UINT uMsg, 
												  LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED: 
			{
				// set the initial directory
				CString* pString = (CString*)lpData;
				if (!pString) 
					return 0;
				LPTSTR szDir = pString->GetBuffer(pString->GetLength());
				// WParam is TRUE since you are passing a path.
				// It would be FALSE if you were passing a pidl.
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);             
				break;       
			}
		case BFFM_SELCHANGED: 
			{
				// Set the status window to the currently selected path.
				TCHAR szDir[MAX_PATH];
				if (SHGetPathFromIDList((LPITEMIDLIST) lParam , szDir)) 
					SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
				break;            
			}            
		default:
			break;         
	}         
	return 0;
}


void CBrowseForFolder::SetNewStyle(bool val)
{
	if (val) 
		flags |= 0x0040;
	else 
		flags &= ~0x0040;
}

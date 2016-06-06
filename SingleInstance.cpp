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
#include "SingleInstance.h"
#include "globals.h"

/////////////////////////////////////////////////////////////////////////////
// CSingleInstance
//

CSingleInstance::CSingleInstance()
{
	// Set our default values
	m_hMutex = NULL;
}

CSingleInstance::~CSingleInstance()
{
	if ( m_hMutex != NULL ) {
		ReleaseMutex( m_hMutex );
	}
}

BOOL CSingleInstance::CreateMutex(CCommandLineInfo &cmdInfo) {

	m_hMutex = ::CreateMutex(NULL, FALSE, m_strClassName);
	
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		m_hMutex = NULL;
		HWND hWnd = FindWindowEx(NULL, NULL, m_strClassName, NULL);
		if (hWnd != NULL)
		{
			HANDLE	hProcessDest = NULL;
			HANDLE	hMMFReceiver;
			HANDLE	hMMF;
			DWORD	dwProcessId;
			LPVOID	lpvFile;
			int		nSize;


			if (!cmdInfo.m_strFileName.IsEmpty())
			{
				nSize = (_tcslen(cmdInfo.m_strFileName) + 1) * sizeof(TCHAR);
				::GetWindowThreadProcessId(hWnd, &dwProcessId);
				hProcessDest = ::OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwProcessId);

				if (!hProcessDest)
					return FALSE;

				hMMF = ::CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, nSize, NULL);

				if (!hMMF)
					return FALSE;

				lpvFile = ::MapViewOfFile(hMMF, FILE_MAP_WRITE, 0, 0, nSize);

				if (!lpvFile) return FALSE;

				memcpy((LPTSTR)lpvFile, (LPCTSTR)cmdInfo.m_strFileName, nSize);

				::DuplicateHandle(::GetCurrentProcess(), hMMF,
					hProcessDest, &hMMFReceiver, 0, FALSE,
					DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE);
				::PostMessage(hWnd, WM_EXP_OPENFROMSHELL, 0, (LPARAM)hMMFReceiver);
				::UnmapViewOfFile(lpvFile);
			}

			WINDOWPLACEMENT wndpl;
			BOOL res = GetWindowPlacement(hWnd, &wndpl);
			BringWindowToTop(hWnd);
			SetForegroundWindow(hWnd);
			if (res)
				SetWindowPlacement(hWnd, &wndpl);
		}
		// Return failure
		return FALSE;
	}

	return TRUE;
}

BOOL CSingleInstance::Create( UINT nID, CCommandLineInfo &cmdInfo )
{
	CString strFullString;
	
	if ( strFullString.LoadString( nID ) )	
		AfxExtractSubString( m_strClassName, strFullString, 0 );
	
	m_strClassName += _T(" Class");
	
	// disable Single Instance

	//BOOL ret = CreateMutex(cmdInfo);

	//if (ret == FALSE)
	//	return FALSE;
	
	// Register the unique window class name so others can find it.
	WNDCLASS wndcls;    memset(&wndcls, 0, sizeof(WNDCLASS));
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = AfxWndProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = LoadIcon( wndcls.hInstance, MAKEINTRESOURCE( nID ) );//or AFX_IDI_STD_FRAME; 
	wndcls.hCursor = LoadCursor( wndcls.hInstance, IDC_ARROW );
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;//You may need to fix this
	wndcls.lpszClassName = m_strClassName; // my class name
	// Register name, exit if it fails
	if(!AfxRegisterClass(&wndcls)) {
		AfxMessageBox( _T("Failed to register window class!"), MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	// Return success
	return TRUE;
}

CString CSingleInstance::GetClassName( void ) const
{
	return m_strClassName;
}

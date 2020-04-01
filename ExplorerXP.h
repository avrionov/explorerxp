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


#ifndef __EXPLORER_XP_H__
#define __EXPLORER_XP_H__

#pragma once


#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "SingleInstance.h"

class CExplorerXPApp : public CWinAppEx
{
public:
	CExplorerXPApp();

    CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	CSingleInstance m_SingleInstance;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExplorerXPApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CExplorerXPApp)
	afx_msg void OnAppAbout();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnWindowCloseAll();

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXPLORER_XP_H__

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


#ifndef __STDAFX_H__
#define __STDAFX_H__

#pragma once

//#define _USING_V110_SDK71_

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
					
#include <SDKDDKVer.h>

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1

#include <afxwin.h>         // MFC core and standard components
#include <afxole.h>
#include <afxmt.h>			// MFC Multithreaded Extensions (Syncronization Objects)
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxtempl.h>	

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#pragma warning (disable : 4786)
#pragma warning (disable : 4100)


#include <map>
#include <vector>
#include <algorithm>
#include <time.h>
#include <tchar.h>


// Handy functions
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsAltpressed()  ( (GetKeyState(VK_MENU) & (1 << (sizeof(SHORT)*8-1))) != 0 )

#define EMPTYSTR _T("")
#define SLASH    _T('\\')
			 	 
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __STDAFX_H__

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


#ifndef __THEMES_H__

#include <uxtheme.h>

void InitThemes();
void FinThemes();
void PrepareImageRect(HWND hButtonWnd, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, RECT* rpImage);
void DrawTheIcon(HWND hButtonWnd, HICON hIcon, HDC* dc, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, BOOL bIsDisabled);

typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
							int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect);
typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, 
							int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
							DWORD dwTextFlags2, const RECT *pRect);
typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,  HDC hdc, 
							int iPartId, int iStateId,  const RECT *pBoundingRect, 
							RECT *pContentRect);

typedef BOOL (__stdcall * ISTHEMEACTIVE)(VOID);

extern PFNOPENTHEMEDATA zOpenThemeData;
extern PFNDRAWTHEMEBACKGROUND zDrawThemeBackground;
extern PFNCLOSETHEMEDATA zCloseThemeData;
extern PFNDRAWTHEMETEXT zDrawThemeText;
extern PFNGETTHEMEBACKGROUNDCONTENTRECT zGetThemeBackgroundContentRect;
extern ISTHEMEACTIVE zIsThemeActtive;

extern BOOL ThemeLibLoaded;

#define __THEMES_H__

#endif
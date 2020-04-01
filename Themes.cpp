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

#include "Themes.h"

#define ICON_HEIGHT 16
#define ICON_WIDTH 16

BOOL Themed = FALSE;
BOOL ThemeLibLoaded = FALSE;

PFNOPENTHEMEDATA zOpenThemeData = NULL;
PFNDRAWTHEMEBACKGROUND zDrawThemeBackground = NULL;
PFNCLOSETHEMEDATA zCloseThemeData = NULL;
PFNDRAWTHEMETEXT zDrawThemeText = NULL;
PFNGETTHEMEBACKGROUNDCONTENTRECT zGetThemeBackgroundContentRect = NULL;
ISTHEMEACTIVE zIsThemeActtive;

static HMODULE hModThemes = NULL;

void InitThemes()
{
	hModThemes = LoadLibrary(_T("UXTHEME.DLL"));
	if(hModThemes)
	{
		zIsThemeActtive = (ISTHEMEACTIVE) GetProcAddress(hModThemes, "IsThemeActive");
		if (zIsThemeActtive)
			if (!zIsThemeActtive()) 
			{
				ThemeLibLoaded = FALSE;
				return;
			}

		zOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(hModThemes, "OpenThemeData");
		zDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProcAddress(hModThemes, "DrawThemeBackground");
		zCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(hModThemes, "CloseThemeData");
		zDrawThemeText = (PFNDRAWTHEMETEXT)GetProcAddress(hModThemes, "DrawThemeText");
		zGetThemeBackgroundContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProcAddress(hModThemes, "GetThemeBackgroundContentRect");

		if(zOpenThemeData 
			&& zDrawThemeBackground
			&& zCloseThemeData
			&& zDrawThemeText
			&& zGetThemeBackgroundContentRect)
		{
			ThemeLibLoaded = TRUE;			
		} else {
			FreeLibrary(hModThemes);
			hModThemes = NULL;
		}
	}
}

void FinThemes()
{
	FreeLibrary(hModThemes);
	hModThemes = NULL;
	ThemeLibLoaded = FALSE;
	Themed = FALSE;
}

void PrepareImageRect(HWND hButtonWnd, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, RECT* rpImage)
{
	RECT rBtn;

	CopyRect(rpImage, rpItem);

	GetClientRect(hButtonWnd, &rBtn);
	if (bHasTitle == FALSE)
	{
		// Center image horizontally
		LONG rpImageWidth = rpImage->right - rpImage->left;
		rpImage->left += ((rpImageWidth - (long)dwWidth)/2);
	} else {
		// Image must be placed just inside the focus rect
		LONG rpTitleWidth = rpTitle->right - rpTitle->left;
		rpTitle->right = rpTitleWidth - dwWidth - 30;
		rpTitle->left = 30;
		rpImage->left = rBtn.right - dwWidth - 30;
		// Center image vertically
		LONG rpImageHeight = rpImage->bottom - rpImage->top;
		rpImage->top += ((rpImageHeight - (long)dwHeight)/2);
	}
		
	// If button is pressed then press image also
	if (bIsPressed && !Themed)
		OffsetRect(rpImage, 1, 1);
} // End of PrepareImageRect

void DrawTheIcon(HWND hButtonWnd, HICON hOwnerDrawIcon, HDC* dc, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, BOOL bIsDisabled)
{
	RECT	rImage;
	PrepareImageRect(hButtonWnd, bHasTitle, rpItem, rpTitle, bIsPressed, ICON_WIDTH, ICON_HEIGHT, &rImage);

	// Ole'!
	DrawState(	*dc,
				NULL,
				NULL,
				(LPARAM)hOwnerDrawIcon,
				0,
				rImage.left,
				rImage.top,
				ICON_WIDTH,
				ICON_HEIGHT, 
				(bIsDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_ICON);
} // End of DrawTheIcon


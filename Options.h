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

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#pragma once

#include "rsettings.h"

#include "globals.h"

// start settings
#define ONSTART_NOTHING		0
#define ONSTART_MYCOMP		1
#define ONSTART_LASTSAVED	2
#define ONSTART_DEFAULT		3


// MDI Child Windows state
#define WINDOW_FULL 0
#define WINDOW_CASCADE 1
#define WINDOW_TILEV   2
#define WINDOW_TILEH   3


#define SIZE_IN_BYTES  0
#define SIZE_IN_KBYTES 1
#define SIZE_IN_DEPEND 2


#define FOLDERSIZE_AUTOMATIC 0
#define FOLDERSIZE_MANUAL	 1
#define FOLDERSIZE_DELAYED	 2


class CStrOptions : public CRegSettings
{
public:
	CString m_Name;
	BEGIN_REG_MAP(CStrOptions)
		REG_ITEM(m_Name, "")				
	END_REG_MAP()

};

class COptions : public CRegSettings
{
	public:
		BOOL m_bTabsOnTop;
		BOOL m_bVLines;
		BOOL m_bHLines;
		std::vector<CStrOptions> m_LastTabs;
		std::vector<CStrOptions> m_DefaultTabs;
		DWORD m_OnStart;
		DWORD m_LastWindowMode;
		BOOL  m_bShowHidden;
		DWORD m_nCleanSel;
		DWORD m_SizeIn;
		CString m_LastTabsActiveView;
		DWORD m_FolderSizeMode;
		BOOL m_bStripPath;
		BOOL m_bLimitChars;
		DWORD m_CharLimit;

	BEGIN_REG_MAP(COptions)
		REG_ITEM(m_bTabsOnTop, TRUE)		
		REG_ITEM(m_bVLines, FALSE)
		REG_ITEM(m_bHLines, TRUE)
		REG_ITEM_VECTOR(m_LastTabs)
		REG_ITEM_VECTOR(m_DefaultTabs)
		REG_ITEM(m_OnStart, ONSTART_LASTSAVED);
		REG_ITEM(m_LastWindowMode, WINDOW_FULL);
		REG_ITEM(m_bShowHidden, TRUE);
		REG_ITEM(m_nCleanSel, ((DWORD)-1));
		REG_ITEM(m_SizeIn, SIZE_IN_DEPEND)
		REG_ITEM(m_LastTabsActiveView, "");
		REG_ITEM(m_FolderSizeMode, FOLDERSIZE_AUTOMATIC);
		REG_ITEM(m_bStripPath, TRUE);
		REG_ITEM(m_bLimitChars, TRUE);
		REG_ITEM(m_CharLimit, 30);
	END_REG_MAP()

};


extern COptions gOptions;
#endif
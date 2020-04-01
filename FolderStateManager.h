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

#pragma once

#include "globals.h"
#include "rsettings.h"

class CFileViewer;

#define MAX_FOLDERCOLONMS 16

class CFolderState 
{
public:
	CFolderState ();
	DWORD   m_Widths[MAX_FOLDERCOLONMS];	
	DWORD   m_Visible;
	bool	m_bAscending;
	int     m_nSortColumn;
	int		m_CalcSize;

	bool operator != (const CFolderState &state);
	void LimitedEq (const CFolderState &state);
//	BEGIN_REG_MAP(CFolderState)
//		REG_ITEM(m_SortColumn, -1)				
//	END_REG_MAP()
};

typedef std::map <CString, CFolderState> CFolderStateMap;
typedef CFolderStateMap::iterator CFolderState_it;
typedef CFolderStateMap::value_type CFolderState_value;

#include "PropContainer.h"

class CFolderStateManager
{
public:
	CFolderStateManager(void);
	~CFolderStateManager(void);
	void SaveState (const TCHAR *folder, CFolderState &state);
	void LoadState (const TCHAR *folder, CFolderState &state);
	bool IsIn (const TCHAR *folder);
	void InitDefaults ();
	void Load ();
	void Save ();
	char* GetStateFileName ();
	void ResetAllFolders ();
	void MakeAllAs (CFolderState &);
	void AddSection (PropContainer &folder_props, int count, const TCHAR *name, CFolderState &state);
	CFolderState GetDefault () { return m_Default;}
protected:
	CFolderStateMap m_Map;
	CFolderState m_Default;
};

extern CFolderStateManager gFolderStateMan;
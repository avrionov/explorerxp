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


#ifndef __GROUP_MANGER_H__
#define __GROUP_MANGER_H__
#pragma once

#include "globals.h"

#define GROUP_BASE_CMD 45000
#define MAX_GROUPS     30
#define GROUP_BASE_CMD_LAST  (GROUP_BASE_CMD+MAX_GROUPS)


class CGroup
{
	public:
		CGroup ():m_bOpen (FALSE) {};
		CGroup (const TCHAR *name);
		CGroup (const TCHAR *name, CStringAr &names);
		CString m_Name;
		CStringAr m_Folders;
		BOOL m_bOpen;
};

class CGroupManager
{
public:
	CGroupManager(void);
	~CGroupManager(void);

	std::vector <CGroup> m_Groups;
	bool  IsExist (const TCHAR *name);
	void  Add (const TCHAR *name, CStringAr &names);
	void  Add (const TCHAR *name);
	void  AddToGroup (const TCHAR *groupName, const TCHAR *folder);
	void  DeleteGroup (const TCHAR *name);
	void  DeleteItem (const TCHAR *name);
	void  RenameGroup (const TCHAR *groupName, const TCHAR *newGroupName);
	int   Find (const TCHAR *name);
	void  SetOpen (const TCHAR *name, bool bOpen);
	void  DeleteFromGroup (const TCHAR *groupName, const TCHAR *folder);
	void  Save (const char *fileName);
	void  Load (const char *fileName);
	void  Save();
	void  Load ();
	void OpenGroup (const TCHAR *groupName);
	void OpenGroup (int index);
	void CloseAllAndOpenGroup (const TCHAR *groupName);

	bool CheckName (TCHAR const *name);
	size_t size () {return m_Groups.size();	}
	CGroup & operator [] (int pos){ return m_Groups[pos];}
	
};

extern CGroupManager gGroupManager;
#endif
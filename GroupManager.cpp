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
#include "GroupManager.h"
#include "resource.h"
#include "PropContainer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGroupManager gGroupManager;



CGroup::CGroup (const TCHAR *name)
{
	m_Name = name;
	m_bOpen = FALSE;
}

CGroup::CGroup (const TCHAR *name, CStringAr &names)
{
	m_Name = name;
	m_Folders = names;
	m_bOpen = FALSE;
}

CGroupManager::CGroupManager(void)
{
}

CGroupManager::~CGroupManager(void)
{
}


void CGroupManager::Save (const char *fileName)
{
	TRFUN (_T("CGroupManager:Save"));
	PropContainer prop;
	prop.setDelim (DELIM);
	for (unsigned int i = 0; i < m_Groups.size(); i++)
	{
		CString label;

		label.Format(_T("Group%d"),i );
		PropSection *section = prop.addSectionSimple(label);			
		section->addTilte(_T("Var"));
		section->addTilte(_T("Name"));
		 //(m_Groups[i].m_Name);
		section->addValues(2, _T("Name"), (LPCTSTR)m_Groups[i].m_Name);
		section->addValue (_T("Open"), m_Groups[i].m_bOpen);
		for (unsigned int j = 0; j < m_Groups[i].m_Folders.size(); j++)
		{
			label.Format(_T("Folder%d"),j );
			section->addValues(2, (LPCTSTR)label, (LPCTSTR)m_Groups[i].m_Folders[j]);
		}
	}
	prop.save(fileName);
}


void CGroupManager::Load (const char *fileName)
{
	TRFUN (_T("CGroupManager:Load"));
	PropContainer prop;
	prop.setDelim (DELIM);
	prop.load (fileName);

	for (unsigned int i = 0; i < prop.size (); i++)
	{
		CGroup group;
		PropSection &section = prop[i];
		std::vector <CStringAr> & values = section.values ();
		for (unsigned int j = 0; j < values.size (); j++)
			if (values[j].size () >=2 )
			{
				if (values[j][0] == _T("Name"))
					group.m_Name = values[j][1];

				if (values[j][0] == _T("Open"))
					group.m_bOpen = values[j][1] == "1";

				if (values[j][0].Find(_T("Folder")) == 0)
				{
					group.m_Folders.push_back(values[j][1]);
				}
			}
		if (group.m_Folders.size() && !group.m_Name.IsEmpty())	
			m_Groups.push_back(group);
	}
}

void CGroupManager::Save()
{
	Save (GetLocalFile("Groups", ".dat"));
}

void CGroupManager::Load ()
{
	Load (GetLocalFile("Groups", ".dat"));
}

bool CGroupManager::IsExist (const TCHAR *name)
{
	int pos = Find( name);
	return pos != -1;
}

bool CGroupManager::CheckName (TCHAR const *name)
{
	CString csName = name;
	csName.TrimLeft();
	csName.TrimRight();
	if (csName.IsEmpty())
		return false;

	for (unsigned int i = 0; i < size(); i++)
		if (csName.CompareNoCase(m_Groups[i].m_Name) == 0)
			return false;

	return true;
}

void  CGroupManager::Add (const TCHAR *name, CStringAr &names)
{
	if (!CheckName(name))
		return;

	CGroup grp (name, names);
	m_Groups.push_back(grp);
}

void  CGroupManager::Add (const TCHAR *name)
{
	if (!CheckName(name))
		return;

	CGroup grp (name);
	m_Groups.push_back(grp);
}

void  CGroupManager::DeleteGroup (const TCHAR *name)
{
	int index = Find (name);

	if (index == -1)
		return;

	m_Groups.erase(m_Groups.begin() + index);
}

int CGroupManager::Find (const TCHAR *name)
{
	for (unsigned int i = 0; i < m_Groups.size(); i++)
		if (name == m_Groups[i].m_Name)
			return i;

	return -1;
}

void CGroupManager::SetOpen (const TCHAR *name, bool bOpen)
{
	int index = Find (name);

	if (index == -1)
		return;
	
	m_Groups[index].m_bOpen = bOpen;
}


void CGroupManager::AddToGroup (const TCHAR *groupName, const TCHAR *folder)
{
	int index = Find (groupName);

	if (index == -1)
		return;

	m_Groups[index].m_Folders.push_back(folder);
}

void  CGroupManager::RenameGroup (const TCHAR *groupName, const TCHAR *newGroupName)
{
	int index = Find (groupName);

	if (index == -1)
		return;

	m_Groups[index].m_Name = newGroupName;
}

void  CGroupManager::DeleteFromGroup (const TCHAR *groupName, const TCHAR *folder)
{
	int index = Find (groupName);

	if (index == -1)
		return;

	CStringAr &flds= m_Groups[index].m_Folders;
	for (unsigned int i = 0; i < flds.size(); i++)
		if ( flds[i].CompareNoCase (folder) == 0)
		{
			flds.erase(flds.begin()+ i);
			break;
		}
}

void CGroupManager::OpenGroup (int index)
{

	CStringAr &flds= m_Groups[index].m_Folders;

	for (unsigned int i = 0 ; i < flds.size (); i++)
		if (IsDirectory (flds[i]))
			OpenFolder (flds[i]);	
}

void CGroupManager::OpenGroup (const TCHAR *groupName)
{
	int index = Find (groupName);

	if (index == -1)
		return;

	OpenGroup (index);
}

void CGroupManager::CloseAllAndOpenGroup (const TCHAR *groupName)
{
	CWnd *pWnd = AfxGetMainWnd ();
	pWnd->SendMessage (WM_COMMAND, ID_WINDOW_CLOSE_ALL);		
	OpenGroup (groupName);
}
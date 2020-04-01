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
#include "Folderstatemanager.h"
#include "SimpleFile.h"

CFolderStateManager gFolderStateMan;

#define DEFAULT_FOLDER _T("**DEFAULT")


CFolderState::CFolderState ()
:m_bAscending (true),
 m_nSortColumn (0),
 m_Visible (0x1B)
{
	memset (m_Widths, 0, sizeof (m_Widths));
}

bool CFolderState::operator != (const CFolderState &state)
{
	if (m_bAscending != state.m_bAscending)
		return true;

	if (m_nSortColumn != state.m_nSortColumn)
		return true;

	if (m_Visible != state.m_Visible)
		return true;

	for (int i = 0; i < MAX_FOLDERCOLONMS; i++)
		if (m_Widths[i] != state.m_Widths[i])
			return true;

	return false;
}

void CFolderState::LimitedEq (const CFolderState &state) 
{
	memcpy (&m_Widths,  state.m_Widths, sizeof (m_Widths));
	m_Visible = state.m_Visible;
}

CFolderStateManager::CFolderStateManager(void)
{
	InitDefaults ();
}

CFolderStateManager::~CFolderStateManager(void)
{
}

void CFolderStateManager::InitDefaults ()
{
	m_Default.m_Widths[0] = 200;
	m_Default.m_Widths[1] = 120;
	m_Default.m_Widths[2] = 120;
	m_Default.m_Widths[3] = 140;
	m_Default.m_Widths[4] = 120;		
}

void CFolderStateManager::SaveState (const TCHAR *folder, CFolderState &state)
{
	CFolderState_it it= m_Map.find (folder);
	if (it != m_Map.end ())	
		m_Map.erase (it);
	
	if (state != m_Default)
		m_Map.insert (CFolderState_value (folder, state));			 
}


bool CFolderStateManager::IsIn (const TCHAR *folder)
{
	CFolderState_it it= m_Map.find (folder);
	if (it != m_Map.end ())	
		return true;
	return false;
}

void CFolderStateManager::LoadState (const TCHAR *folder, CFolderState &state)
{
	CFolderState_it it= m_Map.find (folder);
	if (it != m_Map.end ())	
		state.LimitedEq(it->second);
	else
		state.LimitedEq ( m_Default);
}

void CFolderStateManager::Save ()
{
	TRFUN (_T("CFolderStateManager:Save"));
/*	PropContainer folder_props;
	CFolderState_it it = m_Map.begin ();	
	int count = 1;
	while (it != m_Map.end ())
	{			
		AddSection (folder_props, count++, (LPCTSTR)it->first, it->second);
		it++;
	}
	AddSection (folder_props, count++, DEFAULT_FOLDER, m_Default);
	folder_props.setDelim (DELIM);
	folder_props.save (GetStateFileName());*/

	CSimpleFile file(GetStateFileName(), "wb");
	CFolderState_it it = m_Map.begin ();	

	while (it != m_Map.end ())
	{			
		//AddSection (folder_props, count++, (LPCTSTR)it->first, it->second);
		file.write_line("[Folder]\r\n \r\n");
		file.write_line("Folder|*|");
		char pszA[4096];

		::WideCharToMultiByte( CP_ACP, 0, it->first, it->first.GetLength()+1, pszA, 4096, NULL, NULL );
		
		file.write_line(pszA);
		file.write_line("\r\n");

		file.write_line("Visible|*|");
		sprintf (pszA, "%lX", it->second.m_Visible);
		file.write_line(pszA);
		file.write_line("\r\n");

		char *start = pszA;
		pszA[0] = 0;

		for (int i = 0; i < MAX_FOLDERCOLONMS; i++)
		{
			sprintf (start, "%04d ", it->second.m_Widths[i]);
			start += 5;			
		}

		file.write_line("Widths|*|");
		file.write_line(pszA);
		file.write_line("\r\n\r\n");
		++it;
	}
}

void CFolderStateManager::AddSection (PropContainer &folder_props, int count, const TCHAR *name, CFolderState &state) 
{	
	TCHAR buf[200];
	CString widths;
	wsprintf (buf, _T("Folder %d"), count);
	PropSection *section = folder_props.addSectionSimple (buf);
	section->addValues (2, _T("Folder"), name);
	section->addValue (_T("Visible"), state.m_Visible);	
	for (int i = 0; i < MAX_FOLDERCOLONMS; i++)
		{
			wsprintf (buf, _T("%04d "), state.m_Widths[i]);
			widths += buf;
		}
	section->addValues (2, _T("Widths"), widths);
}

void loadStrAr (CString &line, CStringAr &ar, CString delim) {
	CString rest = line;
	int  pos;

	while ( (pos =  rest.Find (delim)) != -1) 	{
		CString extr = rest.Left (pos);
		ar.push_back (extr);
		rest.Delete (0, pos + delim.GetLength ());
	}
	ar.push_back (rest);
}

void CFolderStateManager::Load ()
{
	InOut in(_T("CFolderStateManager:Load"));
	CSimpleFile file (GetStateFileName(), "rb");

	file.read_all();

	char *line = NULL;
	int len = 0;
	line = file.read_line(len);
	
	while (line != 0)
	{
		CFolderState state;
				
		CString folder ;	

		if (len < 1) goto next_line;

		if (line[0] == '[')
		{
			file.read_line(len);
		//	int len_folder, len_visible, len_widhts;
			line = file.read_line (len);
			if (line && len > 9)
				if (line[0] == 'F')
				{
					folder = (char*) (line+ 9);
					folder.TrimRight();
					folder.TrimLeft();
				}	

			line = file.read_line (len);

			if (line && len > 9)
				if (line[0] == 'V')
				{
					sscanf (line+10, "%lX", &state.m_Visible);
				}

			line = file.read_line (len);
			if (line && len > 9 )
				if (line[0] == 'W')
				{					
					char *start = line + 9;
					if ( (len - 9) >= (MAX_FOLDERCOLONMS * 5))
						for (unsigned int i = 0; i < MAX_FOLDERCOLONMS; i++) 
						{
							int width = (start[0] -'0') * 1000 +
										(start[1] -'0') * 100 +
										(start[2] -'0') * 10 +
										(start[3] -'0') * 1;

							state.m_Widths[i] = width;
							start += 5;
						}
				}

			if (!folder.IsEmpty ())
				m_Map.insert (CFolderState_value (folder, state));	
		}

next_line:		
		line = file.read_line(len);
	}
	/*PropContainer folder_props;
	folder_props.setDelim (DELIM);
	folder_props.load (GetStateFileName());

	for (unsigned int i = 0; i < folder_props.size (); i++)
	{
		CFolderState state;
		PropSection &section = folder_props[i];
		std::vector <CStringAr> & values = section.values ();
		CString folder ;
		folder.Empty ();
		if (values.size () >=3 )
		{
			if (values[0].size() >= 2)
				folder = values[0][1];

			if (values[1].size() >= 2)
				_stscanf (values[1][1], _T("%lX"), &state.m_Visible);

			if (values[2].size() >= 2)
			{
				CStringAr ar;
				loadStrAr (values[2][1], ar, " ");

				for (unsigned int i = 0; i < min (MAX_FOLDERCOLONMS, ar.size()); i++)					
					swscanf (ar[i], _T("%d"), &state.m_Widths[i]);	
			}
				
			
		}*/
		/*for (unsigned int j = 0; j < values.size (); j++)
			if (values[j].size () >=2 )
			{
				if (values[j][0] == _T("Folder"))
					folder = values[j][1];

				if (values[j][0] == _T("Visible"))
				{
					_stscanf (values[j][1], _T("%lX"), &state.m_Visible);					
				}

				if (values[j][0] == _T("Widths"))
				{
					CStringAr ar;
					loadStrAr (values[j][1], ar, " ");

					for (unsigned int i = 0; i < min (MAX_FOLDERCOLONMS, ar.size()); i++)					
						 swscanf (ar[i], _T("%d"), &state.m_Widths[i]);											
				}
			}*/

		/*	if (!folder.IsEmpty ())
				m_Map.insert (CFolderState_value (folder, state));			 
	}*/

	CFolderState_it it= m_Map.find (DEFAULT_FOLDER);
	if (it != m_Map.end ())	
	{
		m_Default.LimitedEq(it->second);
		bool allzero =  true;
		for (unsigned int i = 0; i < MAX_FOLDERCOLONMS; i++) 
			if (m_Default.m_Widths[i] != 0)
			{
				allzero = false;
				break;
			}

		if (allzero)
			InitDefaults();
	}
}

char* CFolderStateManager::GetStateFileName ()
{
	return GetLocalFile("FolderStates", ".dat");
}


void CFolderStateManager::ResetAllFolders () 
{
	CFolderStateMap map;

	CFolderState_it it= m_Map.find (CONST_MYCOMPUTER);
	if (it != m_Map.end ())	
		map.insert (CFolderState_value (CONST_MYCOMPUTER, it->second));

	it= m_Map.find (CONST_RECYCLEBIN);
	if (it != m_Map.end ())	
		map.insert (CFolderState_value (CONST_RECYCLEBIN, it->second));

	m_Map = map;

	CFolderState state;
	m_Default = state;
	InitDefaults ();
}



void CFolderStateManager::MakeAllAs (CFolderState &state)
{

	CFolderStateMap map;

	CFolderState_it it= m_Map.find (CONST_MYCOMPUTER);
	if (it != m_Map.end ())	
		map.insert (CFolderState_value (CONST_MYCOMPUTER, it->second));

	it= m_Map.find (CONST_RECYCLEBIN);
	if (it != m_Map.end ())	
		map.insert (CFolderState_value (CONST_RECYCLEBIN, it->second));

	m_Map = map;

	m_Default = state;
}

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
#include <set>
#include <math.h>
#include "ExplorerXP.h"
#include "FindMissing.h"
#include "globals.h"
#include "fast_array.h"
#include "FileShellAttributes.h"


// CFindMissing dialog

IMPLEMENT_DYNAMIC(CFindMissing, CDialog)
CFindMissing::CFindMissing(CWnd* pParent /*=NULL*/)
	: CDialog(CFindMissing::IDD, pParent)
{
}

CFindMissing::~CFindMissing()
{
}

void CFindMissing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CFindMissing, CDialog)
END_MESSAGE_MAP()


// CFindMissing message handlers

void GetFolder (fast_array <WIN32_FIND_DATA> &array, const TCHAR *path);

BOOL CFindMissing::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcl;
	m_List.GetClientRect(rcl);

	m_List.SetImageList (GetSysImageList (), LVSIL_SMALL  );

	m_List.InsertColumn(0, _T("File"), LVCFMT_LEFT, rcl.Width ());
	
	// fix it;
		
	fast_array <WIN32_FIND_DATA> array(100, 100);

	CStringAr group;

	CString pattern;

	GetFolder (array, m_Path);
	
	CString match = m_File;

	match.MakeLower();
	
	std::set <int> numbers;
	int min_num = 99999, max_num = 0, number;
	int max_len = 0;
	int last_dif = -1;
	for (unsigned int i = 0; i < array.size(); i++)
	{
		CString file = array[i].cFileName;

		file.MakeLower();

		int dif = 0;
		dif += abs (match.GetLength() - file.GetLength());
		int len = min (match.GetLength(), file.GetLength());
		for (int k = 0; k < len ; k++)
		{
			if (match[k] != file[k])
			{

				dif++;
				if ( (last_dif != -1) && (last_dif != (k-1)))
						dif++;
				last_dif = k;
			}
		}

		if (dif <= 4)
		{
			group.push_back(file);
			max_len = max (max_len, file.GetLength());
		/*	int pos = file.FindOneOf(_T("0123456789"));
			if (pos != -1)
			{
				CString num;
				while (pos < file.GetLength() && isdigit(file[pos]))
				{
					num = num + file[pos] ;
					pos ++;
				}

				if (!num.IsEmpty())
				{
					number = _ttoi(num);
				//	m_List.InsertItem(0,num);
					min_num = min (number, min_num);
					max_num = max (number, max_num);
					numbers.insert (number);
					TRACE(_T("%s\n"), (LPCTSTR)num);
				}
			}*/
		}
	}

	for (int i = 0; i < max_len; i++)
		pattern += _T("*");
	
	// build pattern
	for (unsigned int i= 0; i < group.size();i++)
	{	
		int len = min (match.GetLength(), group[i].GetLength());
		for (int k = 0; k < len ; k++)		
			if (match[k] != group[i][k])
				pattern.SetAt(k,_T('?'));			
	}

	for (unsigned int i= 0; i < group.size();i++)
	{
		int pos = 0;
		while (pos < group[i].GetLength() && !(isdigit(group[i][pos]) && pattern[pos] == _T('?')))
			pos ++;

		if (pos < group[i].GetLength())
		{
			CString num;
			while (pos < group[i].GetLength() && isdigit(group[i][pos]))
			{
				num = num + group[i][pos] ;
				pos ++;
			}

			if (!num.IsEmpty())
			{
				number = _ttoi(num);
				//	m_List.InsertItem(0,num);
				min_num = min (number, min_num);
				max_num = max (number, max_num);
				numbers.insert (number);
				TRACE(_T("%s\n"), (LPCTSTR)num);
			}
		}
	}
	// prepare mask
	//int pos = match.FindOneOf(_T("0123456789"));

	match = m_File;
	int pos = 0;
	while (pos < match.GetLength() && !(isdigit(match[pos]) && pattern[pos] == _T('?')))
		pos ++;

	if (pos < match.GetLength())
		while (pos < match.GetLength() && isdigit(match[pos]))
		{
			match.Delete(pos);
		}

	int count = 0;
	int digits = (int)(log10 ((double)max_num))+1;

	CString full_name = m_Path;
	SureBackSlash(full_name);
	full_name += m_File;

	int icon = CFileShellAttributes::GetFileIcon (full_name);
	for (int i =  min_num; i <= max_num; i++)
	{
		if (numbers.find (i) == numbers.end ())
		{
			CString num ;
			num.Format(_T("%0*d"),digits, i);
			if (pos == -1)
				m_List.InsertItem(count++, num);
			else
			{
				CString tmp;
				tmp = match;
				tmp.Insert(pos, num);
				m_List.InsertItem(count++, tmp, icon);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFindMissing::Init (const TCHAR *path, const TCHAR *file)
{
	m_Path = path;
	m_File = file;
}
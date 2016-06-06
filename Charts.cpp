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
#include "ExplorerXP.h"
#include "Charts.h"
#include "FileShellAttributes.h"

// CCharts dialog

extern CDirSize dirs;

IMPLEMENT_DYNAMIC(CCharts, CDialog)

CCharts::CCharts(CWnd* pParent /*=NULL*/)
	: CDialog(CCharts::IDD, pParent)
{

}

CCharts::~CCharts()
{
}

void CCharts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CCharts, CDialog)
END_MESSAGE_MAP()


// CCharts message handlers

bool sort_by_folder_size (const dir_cache d1, const dir_cache d2)
{		
		if (d1.dirinfo.m_Size <= d2.dirinfo.m_Size)
			return false;

		//if (d1.dirinfo.m_Size > d2.dirinfo.m_Size)
		return true;		
}

BOOL CCharts::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CRect rc;

	GetClientRect (rc);

	m_List.InsertColumn (0, _T("Folder"), LVCFMT_LEFT, rc.Width ()/2 );
	m_List.InsertColumn (1, _T("Size"), LVCFMT_LEFT, rc.Width () /3 );
	m_List.InsertColumn (2, _T("%"), LVCFMT_LEFT, rc.Width () /6 );
	m_List.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER);
	
	m_List.SetImageList (GetSysImageList (), LVSIL_SMALL  );

	dirs.GetDirInfoArrray (m_Array);
	
	std::sort (m_Array.begin(), m_Array.end(), sort_by_folder_size);

	LVITEM item;
	CString tmpString;

	size_t i = 0 ;
	int listIndex = 0;
	while (i < m_Array.size() && listIndex < 50)
	{
		
		if (IsDirectory (m_Array[i].folder))
		{
			item.mask = LVIF_TEXT | LVIF_IMAGE;
			item.iItem = i;
			item.iSubItem = 0;	
			item.iImage = CFileShellAttributes::GetFileIcon (m_Array[i].folder);
			item.pszText = (LPTSTR)(LPCTSTR)m_Array[i].folder;
			m_List.InsertItem (&item);

			tmpString = size_to_string (m_Array[i].dirinfo.m_Size);
			
			m_List.SetItem (listIndex, 1, LVIF_TEXT ,tmpString, 0, 0,0,0);
			listIndex++;
		}
		i++;		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

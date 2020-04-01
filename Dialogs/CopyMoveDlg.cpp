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
#include "explorerxp.h"
#include "CopyMoveDlg.h"
#include "FileShellAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCopyMoveDlg dialog




CCopyMoveDlg::CCopyMoveDlg(const TCHAR *Title, const TCHAR* current_path,  CWnd* pParent /*=NULL*/)
	: CResizableDialog(CCopyMoveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCopyMoveDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Title = Title;
	m_Path = current_path;
}


void CCopyMoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopyMoveDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_FOLDERS, m_Folders);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCopyMoveDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CCopyMoveDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnKeydownList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCopyMoveDlg message handlers

BOOL CCopyMoveDlg::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	AddAnchor (IDC_FOLDERS,  TOP_LEFT, MIDDLE_RIGHT);
	AddAnchor (IDC_LIST, MIDDLE_LEFT, BOTTOM_RIGHT );
	
	AddAnchor (IDC_LAST_SEP, BOTTOM_LEFT, BOTTOM_RIGHT);

	AddAnchor (IDOK, BOTTOM_LEFT);
	AddAnchor (IDCANCEL, BOTTOM_LEFT);

	SetWindowText (m_Title);

	m_Folders.Fill ();
	m_Folders.Find (m_Path, true);

	CRect rc;
	m_List.GetClientRect (rc);

	m_List.InsertColumn (0, _T("Path"),LVCFMT_LEFT, rc.Width () );	
	m_List.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES| LVS_EX_DOUBLEBUFFER);

	LVITEM item;
	m_List.SetImageList (GetSysImageList (), LVSIL_SMALL  );
	
	LoadPaths ();

	for (unsigned int i = 0; i < m_Paths.size (); i++)
	{
		item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		item.iItem = i;
		item.iSubItem = 0;
		item.lParam = i;
		item.iImage = CFileShellAttributes::GetFileIcon (m_Paths[i]);
		
		item.pszText = (LPTSTR)(LPCTSTR)m_Paths[i];
		m_List.InsertItem (&item);

	}

	EnableSaveRestore (_T("CopySaveDlg")); 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCopyMoveDlg::OnOK() 
{
	HTREEITEM hItem = m_Folders.GetSelectedItem ();
	if (hItem != NULL)
	{
		m_NewPath = m_Folders.GetPathFromItem (hItem);
		m_Paths.push_back (m_NewPath);
		SavePaths ();
	}
	
	CResizableDialog::OnOK();
}

void CCopyMoveDlg::LoadPaths() 
{
	CString key="Path0";

	CString path = AfxGetApp()->GetProfileString(_T("CopyMovePaths"), key);
	
	while (!path.IsEmpty() )	
	{
		m_Paths.push_back (path);
		key.Format (_T("Path%d"), (int)m_Paths.size ());
		path = AfxGetApp()->GetProfileString(_T("CopyMovePaths"), key);
	}
	RemoveDups ();
}
	
void CCopyMoveDlg::RemoveDups ()
{
	std::sort( m_Paths.begin(), m_Paths.end() );

	loop:
	if ( m_Paths.size () > 0)	
		for( unsigned int i = 0;  i < m_Paths.size () -1; i++)
		{
			if (m_Paths[i].CompareNoCase (m_Paths[i+1]) == 0)
			{				
				m_Paths.erase(m_Paths.begin () + i + 1 );
				goto loop;
			}
		}
}

void CCopyMoveDlg::SavePaths ()
{
	RemoveDups ();

	CString key;

	for (unsigned int i = 0; i < m_Paths.size (); i++)
	{
		key.Format (_T("Path%d"), i);
		AfxGetApp()->WriteProfileString(_T("CopyMovePaths"), key, m_Paths[i]);
	}

	key.Format (_T("Path%d"), (int)m_Paths.size ());
	AfxGetApp()->WriteProfileString(_T("CopyMovePaths"), key, EMPTYSTR);
}

void CCopyMoveDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->uNewState &LVIS_SELECTED)
	{
		CString path = m_List.GetItemText (pNMListView->iItem, 0);
		m_Folders.Find (path, true);

	}
	*pResult = 0;
}

void CCopyMoveDlg::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	if (pLVKeyDow->wVKey == VK_DELETE)	
	{
		int item = m_List.GetNextItem (-1, LVNI_SELECTED );
		if (item != -1)
		{
			m_List.DeleteItem (item);
			m_Paths.erase (m_Paths.begin ()+item);	
		}
	}
	*pResult = 0;
}

void CCopyMoveDlg::OnCancel() 
{	
	SavePaths ();
	CResizableDialog::OnCancel();
}

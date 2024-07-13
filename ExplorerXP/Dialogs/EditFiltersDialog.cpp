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

#include "stdafx.h"
#include "explorerxp.h"
#include "EditFiltersDialog.h"
#include "EditFilterDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditFiltersDialog dialog


CEditFiltersDialog::CEditFiltersDialog(CFilterMan &filterMan, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CEditFiltersDialog::IDD, pParent),
      m_FilterMan (filterMan)
{
	//{{AFX_DATA_INIT(CEditFiltersDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEditFiltersDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditFiltersDialog)
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_LIST, m_FilterList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditFiltersDialog, CResizableDialog)
	//{{AFX_MSG_MAP(CEditFiltersDialog)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditFiltersDialog message handlers

BOOL CEditFiltersDialog::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();

	AddAnchor (IDC_STATIC_FILTERS, TOP_CENTER);
	AddAnchor (IDC_STATIC_RENAME, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_LIST, TOP_LEFT, BOTTOM_RIGHT);	
	AddAnchor (IDC_ADD, BOTTOM_LEFT);
	AddAnchor (IDC_EDIT, BOTTOM_LEFT);
	AddAnchor (IDC_DELETE, BOTTOM_LEFT);
	AddAnchor (IDC_STATIC_DOWN, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDOK, BOTTOM_LEFT);
	AddAnchor (IDCANCEL, BOTTOM_LEFT);
	//AddAnchor (IDC_EDITFILTERS, BOTTOM_LEFT);
	
	CRect rc;
	m_FilterList.GetClientRect (rc);
	m_FilterList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, rc.Width()/3);
	m_FilterList.InsertColumn(1, _T("Extensions"), LVCFMT_LEFT, 2*rc.Width()/3);
	m_FilterList.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	
	EnableSaveRestore (_T("EditFiltersDialog")); 
	Fill ();
	EnableControls ();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditFiltersDialog::OnAdd() 
{
	CEditFilterDialog dlg (m_FilterMan, -1);
	if (dlg.DoModal () == IDOK)
	{
		m_FilterMan.addFilter (dlg.GetFilter ());
		Fill ();
	}
}

void CEditFiltersDialog::OnEdit() 
{	
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	
	if (!pos)
		return;

	int nSel = m_FilterList.GetNextSelectedItem(pos);
	
	CEditFilterDialog dlg (m_FilterMan, nSel);	
	if (dlg.DoModal () == IDOK)
	{
		m_FilterMan[nSel] = dlg.GetFilter ();
		Fill ();
	}
}

void CEditFiltersDialog::OnDelete() 
{
	
}

void CEditFiltersDialog::EnableControls ()
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	if (pos)
	{
		m_Delete.EnableWindow (TRUE);
		m_Edit.EnableWindow (TRUE);		
	}
	else
	{
		m_Delete.EnableWindow (FALSE);
		m_Edit.EnableWindow (FALSE);	
	}
}

void CEditFiltersDialog::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	EnableControls ();	
	*pResult = 0;
}

void CEditFiltersDialog::Fill ()
{
	m_FilterList.DeleteAllItems ();
	for (int i = 0; i < m_FilterMan.size (); i++)
	{
		m_FilterList.InsertItem (i, m_FilterMan[i].getName ());
		CString exts;
		m_FilterMan[i].getExts (exts);
		m_FilterList.SetItemText(i, 1, exts);
	}
	EnableControls ();
}

void CEditFiltersDialog::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit ();
	*pResult = 0;
}

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
#include "CleanDialog.h"
#include "EditFilterDialog.h"
#include "EditFiltersDialog.h"
#include "Options.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;	
#endif

/////////////////////////////////////////////////////////////////////////////
// CCleanDialog dialog


CCleanDialog::CCleanDialog (CWnd* pParent /*=NULL*/)
	: CResizableDialog(CCleanDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCleanDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_FilterMan = gFilterMan;
}


void CCleanDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCleanDialog)
	DDX_Control(pDX, IDC_EDITFILTERS, m_EditFilters);
	DDX_Control(pDX, IDOK, m_Clean);
	DDX_Control(pDX, IDC_LIST, m_FilterList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCleanDialog, CResizableDialog)
	//{{AFX_MSG_MAP(CCleanDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_EDITFILTERS, OnEditfilters)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCleanDialog message handlers

BOOL CCleanDialog::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	AddAnchor (IDC_STATIC_FILTERS, TOP_CENTER);
	AddAnchor (IDC_STATIC_RENAME, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_LIST, TOP_LEFT, BOTTOM_RIGHT);	
//	AddAnchor (IDC_ADD, BOTTOM_LEFT);
//	AddAnchor (IDC_EDIT, BOTTOM_LEFT);
//	AddAnchor (IDC_DELETE, BOTTOM_LEFT);
	AddAnchor (IDC_STATIC_DOWN, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDOK, BOTTOM_LEFT);
	AddAnchor (IDCANCEL, BOTTOM_LEFT);
	AddAnchor (IDC_EDITFILTERS, BOTTOM_LEFT);
	
	EnableSaveRestore (_T("CleanDlg")); 

	CRect rc;
	m_FilterList.GetClientRect (rc);
	m_FilterList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, rc.Width()/3);
	m_FilterList.InsertColumn(1, _T("Extensions"), LVCFMT_LEFT, 2*rc.Width()/3);
	m_FilterList.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	Fill ();	
	if (gOptions.m_nCleanSel >= 0)
		m_FilterList.SetItemState (gOptions.m_nCleanSel, LVIS_SELECTED, LVIS_SELECTED);
	EnableControls ();	
	m_FilterList.SetFocus ();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCleanDialog::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	EnableControls ();	
	*pResult = 0;
}

void CCleanDialog::EnableControls ()
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	if (pos)	
		m_Clean.EnableWindow (TRUE);		
	else	
		m_Clean.EnableWindow (FALSE);		
}

void CCleanDialog::OnEdit() 
{
	//CEditFilterDialog dlg (0);
	//dlg.DoModal ();
}

void CCleanDialog::Fill ()
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



void CCleanDialog::OnEditfilters() 
{
	CEditFiltersDialog dlg (m_FilterMan);
	
	if (dlg.DoModal () == IDOK)
	{
		gFilterMan = m_FilterMan = dlg.GetFilterMan ();
		Fill ();
		gFilterMan.save ();
	}
}

void CCleanDialog::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEditfilters ();	
	*pResult = 0;
}


void CCleanDialog::OnOK() 
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	
	if (!pos)
		return;

	gOptions.m_nCleanSel = m_nSel = m_FilterList.GetNextSelectedItem(pos);	
	
	CResizableDialog::OnOK();
}

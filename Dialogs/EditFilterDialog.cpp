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
#include "EditFilterDialog.h"
#include "AddEditPatternDlg.h"
#include "FileFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEditFilterDialog::CEditFilterDialog(CFilterMan &filterman, int ef, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CEditFilterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditFilterDialog)
	m_Name = EMPTYSTR;
	//}}AFX_DATA_INIT
	m_EF = ef;

	if (m_EF < filterman.size () && m_EF >= 0)
		m_Filter = filterman[m_EF];
}


void CEditFilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditFilterDialog)
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_LIST, m_FilterList);
	DDX_Text(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditFilterDialog, CResizableDialog)
	//{{AFX_MSG_MAP(CEditFilterDialog)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditFilterDialog message handlers

BOOL CEditFilterDialog::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
		
	AddAnchor (IDC_STATIC_FILTERS, TOP_LEFT);
	AddAnchor (IDC_LIST, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDC_ADD, BOTTOM_LEFT);
	AddAnchor (IDC_EDIT, BOTTOM_LEFT);
	AddAnchor (IDC_DELETE, BOTTOM_LEFT);
	AddAnchor (IDC_STATIC_DOWN, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDOK, BOTTOM_LEFT);
	AddAnchor (IDCANCEL, BOTTOM_LEFT);
	AddAnchor (IDC_NAME, TOP_LEFT, TOP_RIGHT);
	EnableSaveRestore (_T("EditFilterDlg")); 

	CRect rc;
	m_FilterList.GetClientRect (rc);
	m_FilterList.InsertColumn(0, _T("Extensions"), LVCFMT_LEFT, rc.Width() - 30);
	m_FilterList.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_Name = m_Filter.getName ();
	Fill ();
	UpdateData (FALSE);
	EnableControls ();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEditFilterDialog::Fill ()
{
	m_FilterList.DeleteAllItems();
	for (int i = 0; i < m_Filter.size (); i++)
		m_FilterList.InsertItem (i, m_Filter[i]);
}

void CEditFilterDialog::OnAdd() 
{
	CAddEditPatternDlg dlg (TRUE, this);
	if (dlg.DoModal () == IDOK)
	{
		m_Filter.addExt (dlg.m_sPattern);
		Fill ();
	}
}

void CEditFilterDialog::OnDelete() 
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nSel = m_FilterList.GetNextSelectedItem(pos);
		m_Filter.deleteExt (nSel);		
		Fill ();
	}	
}

void CEditFilterDialog::OnEdit() 
{
	POSITION pos = m_FilterList.GetFirstSelectedItemPosition();
	while (pos)
	{	
		CAddEditPatternDlg dlg (FALSE, this);
		int nSel = m_FilterList.GetNextSelectedItem(pos);
		dlg.m_sPattern = m_Filter[nSel];
		if (dlg.DoModal () == IDOK)
			m_Filter[nSel] = dlg.m_sPattern;
		Fill();
	}
}

void CEditFilterDialog::EnableControls ()
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

void CEditFilterDialog::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	EnableControls ();		
	*pResult = 0;
}

void CEditFilterDialog::OnChangeName() 
{
	UpdateData (TRUE);
	m_Filter.setName (m_Name);	
}

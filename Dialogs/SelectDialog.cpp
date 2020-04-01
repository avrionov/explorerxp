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
#include "SelectDialog.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectDialog dialog


CSelectDialog::CSelectDialog(const TCHAR *title, const TCHAR *file_name, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSelectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectDialog)	
	m_Wildcards = EMPTYSTR;
	//}}AFX_DATA_INIT
	m_Title = title;
	if (file_name)
		m_Wildcards = file_name;	
}


void CSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDialog)	
	DDX_Text(pDX, IDC_WILDCARDS, m_Wildcards);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectDialog, CResizableDialog)
	//{{AFX_MSG_MAP(CSelectDialog)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDialog message handlers

BOOL CSelectDialog::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	SetWindowText (m_Title);

	CRect rc;
	GetWindowRect (rc);
	CSize sz;
	sz.cy  = rc.Height ();
	sz.cx = ::GetSystemMetrics (SM_CXFULLSCREEN);
	SetMaxTrackSize (sz);

	AddAnchor (IDC_WILDCARDS, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_STATIC_DOWN, TOP_LEFT, TOP_RIGHT);
	//AddAnchor (IDC_LIST, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDOK, BOTTOM_LEFT);
	AddAnchor (IDCANCEL, BOTTOM_LEFT);
	//AddAnchor (IDC_STATIC_DOWN, BOTTOM_LEFT, BOTTOM_RIGHT);
	
	EnableSaveRestore (_T("SelectDialog")); 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




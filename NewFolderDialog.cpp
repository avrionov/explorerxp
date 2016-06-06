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
#include "explorerxp.h"
#include "NewFolderDialog.h"
#include "TextUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewFolderDialog dialog


CNewFolderDialog::CNewFolderDialog(const TCHAR *name, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CNewFolderDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewFolderDialog)
	m_FolderName = name;
	//}}AFX_DATA_INIT
}


void CNewFolderDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewFolderDialog)
	DDX_Control(pDX, IDC_CONVERT, m_Convert);
	DDX_Control(pDX, IDC_CASE, m_Case);
	DDX_Text(pDX, IDC_FOLDERNAME, m_FolderName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewFolderDialog, CResizableDialog)
	//{{AFX_MSG_MAP(CNewFolderDialog)
	ON_BN_DOUBLECLICKED(IDC_CASE, OnDoubleclickedCase)
	ON_BN_DOUBLECLICKED(IDC_CONVERT, OnDoubleclickedConvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewFolderDialog message handlers

BOOL CNewFolderDialog::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	CRect rc;
	GetWindowRect (rc);
	CSize sz;
	sz.cy  = rc.Height ();
	sz.cx = ::GetSystemMetrics (SM_CXFULLSCREEN);
	SetMaxTrackSize (sz);
	
	AddAnchor (IDC_FOLDERNAME, TOP_LEFT, TOP_RIGHT);

	AddAnchor (IDC_STATIC_DOWN, TOP_LEFT, TOP_RIGHT);

	EnableSaveRestore (_T("NewFolderDlg"));
	
	m_Case.SetMenu (IDR_CASE);
	m_Convert.SetMenu (IDR_CONVERT);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CNewFolderDialog::OnDoubleclickedCase() 
{
	UpdateData ();
	ChangeCase (m_Case.GetCommand (), m_FolderName);
	UpdateData (FALSE);
}




void CNewFolderDialog::OnDoubleclickedConvert() 
{
	UpdateData ();
	ConvertSpaces (m_Convert.GetCommand (), m_FolderName, false);
	UpdateData (FALSE);		
}

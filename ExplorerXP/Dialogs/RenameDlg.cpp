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
#include "RenameDlg.h"
#include "TextUtil.h"
#include "SplitPath.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRenameDlg::CRenameDlg (const TCHAR *name, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CRenameDlg::IDD, pParent)
	, m_NewExt(EMPTYSTR)
{
	//{{AFX_DATA_INIT(CRenameDlg)	
	m_OrgName = name;
	//}}AFX_DATA_INIT

	CSplitPath path (name);
	m_NewName = path.GetFName();
	m_NewExt = path.GetExt();
	m_bHideExt = false;	
}


void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameDlg)
	DDX_Control(pDX, IDC_CONVERT, m_Convert);
	DDX_Control(pDX, IDC_CASE, m_Case);
	DDX_Text(pDX, IDC_NEW_NAME, m_NewName);
	DDX_Text(pDX, IDC_ORG_NAME, m_OrgName);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_NEW_EXT, m_NewExt);
}


BEGIN_MESSAGE_MAP(CRenameDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CRenameDlg)
	ON_WM_SIZE()
	ON_BN_DOUBLECLICKED(IDC_CASE, OnDoubleclickedCase)
	ON_BN_DOUBLECLICKED(IDC_CONVERT, OnDoubleclickedConvert)
	//}}AFX_MSG_MAP	
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg message handlers

BOOL CRenameDlg::OnInitDialog() 
{
	
	CResizableDialog::OnInitDialog();
	
	CRect rc;
	GetWindowRect (rc);
	CSize sz;
	sz.cy  = rc.Height ();
	sz.cx = ::GetSystemMetrics (SM_CXFULLSCREEN);
	SetMaxTrackSize (sz);
	
	// TODO: Add extra initialization here
	AddAnchor (IDC_ORG_NAME, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_NEW_NAME, TOP_LEFT, TOP_RIGHT);

	AddAnchor (IDC_STATIC_EXT, TOP_RIGHT);
	AddAnchor (IDC_NEW_EXT, TOP_RIGHT);
	AddAnchor (IDC_STATIC_RENAME, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_STATIC_TO, TOP_LEFT, TOP_RIGHT);

	AddAnchor (IDC_STATIC_DOWN, TOP_LEFT, TOP_RIGHT);

	AddAnchor (IDC_TO, MIDDLE_CENTER);
	AddAnchor (IDC_RENAME, MIDDLE_CENTER);

	AddAnchor(IDOK, TOP_LEFT);

	EnableSaveRestore (_T("RenameDlg")); 
	
	if (m_bHideExt)
	{
		GetDlgItem(IDC_NEW_EXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_EXT)->ShowWindow(SW_HIDE);
	}
	m_Case.SetMenu (IDR_CASE);
	m_Convert.SetMenu (IDR_CONVERT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRenameDlg::OnSize(UINT nType, int cx, int cy) 
{
	CResizableDialog::OnSize(nType, cx, cy);
	
}


void CRenameDlg::OnDoubleclickedCase() 
{
	UpdateData ();
	ChangeCase (m_Case.GetCommand (), m_NewName);
	UpdateData (FALSE);
}

void CRenameDlg::OnDoubleclickedConvert() 
{
	UpdateData ();
	ConvertSpaces (m_Convert.GetCommand (), m_NewName, false);
	UpdateData (FALSE);		
}

void CRenameDlg::OnBnClickedOk()
{	
	OnOK();
	m_NewName += m_NewExt;
	m_NewName.TrimLeft();
}

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
#include "resource.h"
#include "AddEditPatternDlg.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAddEditPatternDlg::CAddEditPatternDlg ( BOOL bAddDlg, CWnd* pParent )
	: CDialog(CAddEditPatternDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddEditPatternDlg)
	m_sPattern = EMPTYSTR;
	//}}AFX_DATA_INIT

    m_bAddDlg = bAddDlg;
}


void CAddEditPatternDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEditPatternDlg)
	DDX_Text(pDX, IDC_PATTERN, m_sPattern);
	DDV_MaxChars(pDX, m_sPattern, 259);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddEditPatternDlg, CDialog)
	//{{AFX_MSG_MAP(CAddEditPatternDlg)
	//}}AFX_MSG_MAP
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
    ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddEditPatternDlg message handlers

BOOL CAddEditPatternDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    if ( m_bAddDlg )    
        SetWindowText ( _T("Add Wildcard") );    
    else        
        SetWindowText ( _T("Edit Wildcard") );
        
    return TRUE; 
}

void CAddEditPatternDlg::OnKickIdle()
{
    UpdateDialogControls ( this, FALSE );
}

void CAddEditPatternDlg::OnUpdateOK ( CCmdUI* pCmdUI )
{
    pCmdUI->Enable ( GetDlgItem(IDC_PATTERN)->GetWindowTextLength() > 0 );
}

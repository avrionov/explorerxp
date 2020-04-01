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
#include "ExplorerXP.h"
#include "RenaneReplace.h"
#include "Globals.h"
#include ".\renanereplace.h"


CRenaneReplace::CRenaneReplace(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CRenaneReplace::IDD, pParent)
	, m_Find(EMPTYSTR)
	, m_Replace(EMPTYSTR)
	, m_CaseSensentive(FALSE)
	, m_ReplaceAll(FALSE)
{
}

CRenaneReplace::~CRenaneReplace()
{
}

void CRenaneReplace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FIND, m_Find);
	DDX_Text(pDX, IDC_REPLACE, m_Replace);
	DDX_Check(pDX, IDC_CHECK1, m_CaseSensentive);
	DDX_Check(pDX, IDC_REPLACEALL, m_ReplaceAll);
}


BEGIN_MESSAGE_MAP(CRenaneReplace, CResizableDialog)
	ON_EN_CHANGE(IDC_FIND, OnChange)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCaseSensetive)
	ON_EN_CHANGE(IDC_REPLACE, OnChange)
	ON_BN_CLICKED(IDC_REPLACEALL, OnBnClickedReplaceall)
END_MESSAGE_MAP()


// CRenaneReplace message handlers

BOOL CRenaneReplace::OnInitDialog()
{
	CResizableDialog::OnInitDialog();
	AddAnchor (IDC_FIND, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_REPLACE, TOP_LEFT, TOP_RIGHT);

	EnableSaveRestore (_T("RenameReplace")); 

	return TRUE;
}

void CRenaneReplace::OnChange()
{
	UpdateData();
	GetParent()->SendMessage(WM_REN_VALUECNAGED);	
}

void CRenaneReplace::OnBnClickedCaseSensetive()
{
	UpdateData();
	GetParent()->SendMessage(WM_REN_VALUECNAGED);
}


void CRenaneReplace::OnCancel()
{	
	///((CDialog*)GetParent())->OnCancel ();
	GetParent()->SendMessage(WM_COMMAND, IDCANCEL, 0);
}

void CRenaneReplace::OnOK()
{		
	GetParent()->SendMessage(WM_COMMAND, IDOK, 0);
}

void CRenaneReplace::OnBnClickedReplaceall()
{
	UpdateData();
	GetParent()->SendMessage(WM_REN_VALUECNAGED);
}

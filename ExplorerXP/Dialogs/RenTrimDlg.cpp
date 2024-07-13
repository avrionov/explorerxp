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
#include "ExplorerXP.h"
#include "RenTrimDlg.h"
#include "globals.h"
#include ".\rentrimdlg.h"


IMPLEMENT_DYNAMIC(CRenTrimDlg, CDialog)
CRenTrimDlg::CRenTrimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenTrimDlg::IDD, pParent)
	, m_Position(0)
	, m_Count(0)
	, m_Left(0)
	, m_bInit (false)
	, m_bFromRight(FALSE)
{
}

CRenTrimDlg::~CRenTrimDlg()
{
}

void CRenTrimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Position);
	DDV_MinMaxInt(pDX, m_Position, 0, 255);
	DDX_Text(pDX, IDC_EDIT2, m_Count);
	DDV_MinMaxInt(pDX, m_Count, 0, 255);
	DDX_Control(pDX, IDC_SPIN1, m_spinPos);
	DDX_Control(pDX, IDC_SPIN2, m_spinCount);
	DDX_Check(pDX, IDC_FROMRIGH, m_bFromRight);
}


BEGIN_MESSAGE_MAP(CRenTrimDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangePos)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeCount)
	ON_BN_CLICKED(IDC_FROMRIGH, OnBnClickedFromrigh)
END_MESSAGE_MAP()

// CRenTrimDlg message handlers

void CRenTrimDlg::OnCancel()
{
	GetParent()->SendMessage(WM_COMMAND, IDCANCEL);
}

void CRenTrimDlg::OnEnChangePos()
{
	if (!m_bInit)
		return;

	UpdateData();
	GetParent()->SendMessage(WM_REN_VALUECNAGED);
}

void CRenTrimDlg::OnEnChangeCount()
{
	if (!m_bInit)
		return;

	UpdateData();
	GetParent()->SendMessage(WM_REN_VALUECNAGED);
}

BOOL CRenTrimDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spinPos.SetRange(0, 255);
	m_spinCount.SetRange(0,255);
	m_bInit = true;
	
	return TRUE;  
}

void CRenTrimDlg::OnBnClickedFromrigh()
{
	if (!m_bInit)
		return;

	UpdateData();
	GetParent()->SendMessage(WM_REN_VALUECNAGED);
}

void CRenTrimDlg::OnOK()
{	
	///((CDialog*)GetParent())->OnCancel ();
	GetParent()->SendMessage(WM_COMMAND, IDOK, 0);
}
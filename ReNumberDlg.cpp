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
#include "ExplorerXP.h"
#include "ReNumberDlg.h"
#include "globals.h"
#include ".\renumberdlg.h"


IMPLEMENT_DYNAMIC(CReNumberDlg, CDialog)
CReNumberDlg::CReNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReNumberDlg::IDD, pParent)
	, m_bInit(false)
	, m_Digits(3)
	, m_Start(1)
	, m_Step(1)
	//, m_Pattern(_T("<NUM> <FILENAME><EXT>"))
{
}

CReNumberDlg::~CReNumberDlg()
{
}

void CReNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_Digits);
	DDX_Text(pDX, IDC_EDIT1, m_Start);
	DDX_Text(pDX, IDC_EDIT2, m_Step);
	DDV_MinMaxInt(pDX, m_Step, 1, 10000);	
	DDX_Control(pDX, IDC_SPIN1, m_spinStart);
	DDX_Control(pDX, IDC_SPIN2, m_spinStep);
	DDX_Control(pDX, IDC_SPIN3, m_spinDigit);
	DDX_Control(pDX, IDC_PATTERNS, m_Patterns);
}


BEGIN_MESSAGE_MAP(CReNumberDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeStart)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeStart)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeStart)	
	ON_CBN_SELCHANGE(IDC_PATTERNS, OnCbnSelchangePatterns)
	ON_CBN_EDITCHANGE(IDC_PATTERNS, OnCbnEditchangePatterns)
	ON_CBN_EDITUPDATE(IDC_PATTERNS, OnCbnEditupdatePatterns)
	ON_CBN_SELENDOK(IDC_PATTERNS, OnCbnSelendokPatterns)
END_MESSAGE_MAP()


// CReNumberDlg message handlers

BOOL CReNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_spinStart.SetRange(0, 10000);
	m_spinStep.SetRange(1, 10000);
	m_spinDigit.SetRange(1, 10);
	m_Patterns.AddString(_T("<NUM> <FILENAME><EXT>"));
	m_Patterns.AddString(_T("<NUM><EXT>"));	
	m_Patterns.AddString(_T(" <FILENAME> <NUM><EXT>"));
	m_Patterns.SetCurSel(0);
	int curSel = m_Patterns.GetCurSel();

	if (curSel != LB_ERR)	
		m_Patterns.GetLBText( curSel, m_Pattern);

//	OnCbnSelchangePatterns ();
	m_bInit = true;
	return TRUE;  // return TRUE unless you set the focus to a control	
}

void CReNumberDlg::OnEnChangeStart()
{
	if (!m_bInit)
		return;

	UpdateData();

	GetParent ()->SendMessage(WM_REN_VALUECNAGED);
}

void CReNumberDlg::OnCancel()
{		
	GetParent()->SendMessage(WM_COMMAND, IDCANCEL, 0);
}

void CReNumberDlg::OnOK()
{	
	///((CDialog*)GetParent())->OnCancel ();
	GetParent()->SendMessage(WM_COMMAND, IDOK, 0);
}
void CReNumberDlg::OnCbnSelchangePatterns()
{
	int curSel = m_Patterns.GetCurSel();

	if (curSel != LB_ERR)
	{
		m_Patterns.GetLBText( curSel, m_Pattern);
		GetParent ()->SendMessage(WM_REN_VALUECNAGED);
	}
	

	//m_Patterns.GetWindowText(m_Pattern);
	//
}

void CReNumberDlg::OnCbnEditchangePatterns()
{
	m_Patterns.GetWindowText(m_Pattern);
	GetParent ()->SendMessage(WM_REN_VALUECNAGED);
}

void CReNumberDlg::OnCbnEditupdatePatterns()
{
	
}

void CReNumberDlg::OnCbnSelendokPatterns()
{

	m_Patterns.GetWindowText(m_Pattern);
	GetParent ()->SendMessage(WM_REN_VALUECNAGED);
}

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
#include "TabOptionsDlg.h"
#include "Options.h"
#include "MainFrm.h"

// CTabOptionsDlg dialog

IMPLEMENT_DYNAMIC(CTabOptionsDlg, CPropertyPage)
CTabOptionsDlg::CTabOptionsDlg()
	: CPropertyPage(CTabOptionsDlg::IDD)	
{
	m_bStripPath = gOptions.m_bStripPath;
	m_bLimitChars = gOptions.m_bLimitChars;
	m_CharLimit = gOptions.m_CharLimit;
}

CTabOptionsDlg::~CTabOptionsDlg()
{
}

void CTabOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_STRIP, m_bStripPath);
	DDX_Check(pDX, IDC_LIMIT, m_bLimitChars);
	DDX_Text(pDX, IDC_LIMITCHAR, m_CharLimit);
	DDV_MinMaxInt(pDX, m_CharLimit, 10, 1024);
}


BEGIN_MESSAGE_MAP(CTabOptionsDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_LIMIT, OnBnClickedLimit)
	ON_EN_CHANGE(IDC_LIMITCHAR, OnEnChangeLimitchar)
	ON_BN_CLICKED(IDC_STRIP, OnBnClickedStrip)
END_MESSAGE_MAP()


// CTabOptionsDlg message handlers

BOOL CTabOptionsDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if (!m_bLimitChars)
		GetDlgItem(IDC_LIMITCHAR)->EnableWindow(FALSE);
		

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTabOptionsDlg::OnBnClickedLimit()
{
	UpdateData();

	
	GetDlgItem(IDC_LIMITCHAR)->EnableWindow(m_bLimitChars);
	

	SetModified();
}

void CTabOptionsDlg::OnEnChangeLimitchar()
{
	SetModified();
}

void CTabOptionsDlg::OnBnClickedStrip()
{
	SetModified();
}


BOOL CTabOptionsDlg::OnApply()
{
	GetFromDlg ();

	return CPropertyPage::OnApply();
}

void CTabOptionsDlg::OnOK()
{
	GetFromDlg ();

	CPropertyPage::OnOK();
}

void CTabOptionsDlg::GetFromDlg()
{
	BOOL bNeedUpdate = FALSE;

	UpdateData ();

	if (m_bStripPath != gOptions.m_bStripPath)
	{
		gOptions.m_bStripPath = m_bStripPath;
		bNeedUpdate = true;
	}

	if (m_bLimitChars != gOptions.m_bLimitChars)
	{
		gOptions.m_bLimitChars = m_bLimitChars;
		bNeedUpdate = true;
	}

	if (m_CharLimit != gOptions.m_CharLimit)
	{
		gOptions.m_CharLimit = m_CharLimit;
		bNeedUpdate = true;
	}

	if (bNeedUpdate)
	{
		CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();		
		pFrame->UpdateTabs();
	}
}
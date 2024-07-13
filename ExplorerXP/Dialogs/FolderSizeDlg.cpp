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
#include "FolderSizeDlg.h"
#include "Options.h"
#include "dirsize.h"
#include "globals.h"

extern CDirSize dirs;

// CFolderSizeDlg dialog

IMPLEMENT_DYNAMIC(CFolderSizeDlg, CPropertyPage)
CFolderSizeDlg::CFolderSizeDlg()
	: CPropertyPage(CFolderSizeDlg::IDD)
{
	m_SizeIn = gOptions.m_SizeIn;
	m_FolderMode = gOptions.m_FolderSizeMode;
	m_psp.dwFlags &= ~PSP_HASHELP;  // Lose the Help button
}

CFolderSizeDlg::~CFolderSizeDlg()
{
}

void CFolderSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_SIZE1, m_SizeIn);
	DDX_Radio(pDX, IDC_FOLDERMODE_AUTO, m_FolderMode);
}


BEGIN_MESSAGE_MAP(CFolderSizeDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_SIZE1, OnClicked)
	ON_BN_CLICKED(IDC_SIZE2, OnClicked)
	ON_BN_CLICKED(IDC_SIZE3, OnClicked)
	ON_BN_CLICKED(IDC_FOLDERMODE_AUTO, OnClicked)
	ON_BN_CLICKED(IDC_FOLDERMODE_DELAYED, OnClicked)
	ON_BN_CLICKED(IDC_FOLDERMODE_MAN, OnClicked)
END_MESSAGE_MAP()



void CFolderSizeDlg::GetFromDlg()
{
	BOOL bNeedUpdate = FALSE;

	if (gOptions.m_SizeIn != static_cast<DWORD>(m_SizeIn))
	{
		gOptions.m_SizeIn = m_SizeIn;
		bNeedUpdate = TRUE;
	}

	if (gOptions.m_FolderSizeMode != static_cast<DWORD>(m_FolderMode))
	{
		gOptions.m_FolderSizeMode = m_FolderMode;
		if (gOptions.m_FolderSizeMode == FOLDERSIZE_MANUAL)
			StopSizeThread ();

		bNeedUpdate = TRUE;
	}

	if (bNeedUpdate)	
	{
		dirs.Notify (WM_EXP_HARDUPDATE, NULL, NULL);				
	}

	dirs.Notify (WM_EXP_OPTIONS, 0, NULL);
}

BOOL CFolderSizeDlg::OnApply()
{
	GetFromDlg ();

	return CPropertyPage::OnApply();
}

void CFolderSizeDlg::OnOK()
{
	GetFromDlg ();

	CPropertyPage::OnOK();
}

void CFolderSizeDlg::OnClicked()
{
	SetModified();
}


// CFolderSizeDlg message handlers

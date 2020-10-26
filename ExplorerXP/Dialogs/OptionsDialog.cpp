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
#include "OptionsDialog.h"
#include "Options.h"
#include "DirSize.h"
#include "globals.h"
#include "MainFolder.h"

extern CDirSize dirs;
extern CMainFolder m_DriveTree;

// COptionsDialog dialog

IMPLEMENT_DYNAMIC(COptionsDialog, baseOptions)

COptionsDialog::COptionsDialog()
    : baseOptions(COptionsDialog::IDD), m_bHiddenFiles(FALSE) {
  m_bVert = gOptions.m_bVLines;
  m_bHorz = gOptions.m_bHLines;
  m_bAtStartNone = gOptions.m_OnStart;
  m_bHiddenFiles = gOptions.m_bShowHidden;
  m_bShowExeIcons = gOptions.m_bShowExeIcons;
  m_psp.dwFlags &= ~PSP_HASHELP; // Lose the Help button
}

COptionsDialog::~COptionsDialog() {}

void COptionsDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_VERTICAL, m_bVert);
  DDX_Check(pDX, IDC_HORIZONTAL, m_bHorz);
  DDX_Radio(pDX, IDC_ATSTARTNONE, m_bAtStartNone);
  DDX_Check(pDX, IDC_HIDDENFILES, m_bHiddenFiles);
  DDX_Check(pDX, IDC_NOEXECUTABLESICONS, m_bShowExeIcons);
}

BEGIN_MESSAGE_MAP(COptionsDialog, baseOptions)
ON_BN_CLICKED(IDC_HIDDENFILES, OnBnClickedHiddenFiles)
ON_BN_CLICKED(IDC_HORIZONTAL, OnBnClickedHorizontal)
ON_BN_CLICKED(IDC_VERTICAL, OnBnClickedVertical)
ON_BN_CLICKED(IDC_ATSTARTNONE, OnBnClickedAtstartnone)
ON_BN_CLICKED(IDC_NOEXECUTABLESICONS, OnBnClickedShowExeIcons)
END_MESSAGE_MAP()

// COptionsDialog message handlers

void COptionsDialog::GetFromDlg() {
  BOOL bNeedUpdate = FALSE;

  UpdateData();
  gOptions.m_bVLines = m_bVert;
  gOptions.m_bHLines = m_bHorz;
  gOptions.m_OnStart = m_bAtStartNone;
  gOptions.m_bShowExeIcons = m_bShowExeIcons;

  if (gOptions.m_bShowHidden != m_bHiddenFiles) {
    gOptions.m_bShowHidden = m_bHiddenFiles;
    bNeedUpdate = TRUE;
    m_DriveTree.MarkAll();
  }

  if (bNeedUpdate) {
    dirs.Notify(WM_EXP_HARDUPDATE, NULL, NULL);
  }

  dirs.Notify(WM_EXP_OPTIONS, 0, NULL);
}

BOOL COptionsDialog::OnApply() {
  GetFromDlg();

  return CPropertyPage::OnApply();
}

void COptionsDialog::OnOK() {
  GetFromDlg();

  CPropertyPage::OnOK();
}

void COptionsDialog::OnBnClickedHiddenFiles() { SetModified(); }

void COptionsDialog::OnBnClickedHorizontal() { SetModified(); }

void COptionsDialog::OnBnClickedVertical() { SetModified(); }

void COptionsDialog::OnBnClickedAtstartnone() { SetModified(); };

void COptionsDialog::OnBnClickedShowExeIcons() { SetModified(); };



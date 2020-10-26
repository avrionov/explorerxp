/* Copyright Nikolay Avrionov. All Rights Reserved.
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*  http://www.apache.org/licenses/LICENSE-2.0
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/


#include "stdafx.h"
#include "ExplorerXP.h"
#include "RenameInsert.h"
#include "MultiRenameDlg.h"


IMPLEMENT_DYNAMIC(CRenameInsert, CDialog)
CRenameInsert::CRenameInsert(CWnd *pParent /*=NULL*/)
    : CDialog(CRenameInsert::IDD, pParent), m_bRight(FALSE), m_Text(EMPTYSTR),
      m_bInit(false), m_Pos(0) {}

CRenameInsert::~CRenameInsert() {}

void CRenameInsert::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_FROMRIGH, m_bRight);
  DDX_Text(pDX, IDC_WHAT, m_Text);
  DDX_Control(pDX, IDC_SPIN1, m_Spin);
  DDX_Text(pDX, IDC_EDIT1, m_Pos);
}

BEGIN_MESSAGE_MAP(CRenameInsert, CDialog)
ON_EN_CHANGE(IDC_WHAT, OnEnChangeWhat)
ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
ON_BN_CLICKED(IDC_FROMRIGH, OnBnClickedFromrigh)
END_MESSAGE_MAP()

// CRenameInsert message handlers

void CRenameInsert::OnEnChangeWhat() {
  if (!m_bInit)
    return;

  UpdateData();

  GetParent()->SendMessage(WM_REN_VALUECNAGED);
}

void CRenameInsert::OnEnChangeEdit1() {
  if (!m_bInit)
    return;

  UpdateData();

  GetParent()->SendMessage(WM_REN_VALUECNAGED);
}

BOOL CRenameInsert::OnInitDialog() {
  CDialog::OnInitDialog();

  m_Spin.SetRange(0, 256);
  m_bInit = true;

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CRenameInsert::OnBnClickedFromrigh() {
  if (!m_bInit)
    return;

  UpdateData();

  GetParent()->SendMessage(WM_REN_VALUECNAGED);
}

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
#include "AddressBar.h"

#include "debug_new.h"

CAddressBar::CAddressBar(CWnd *pParent /*=NULL*/) : CDialogBar() {
  //{{AFX_DATA_INIT(CAddressBar)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

LRESULT CAddressBar::OnInitDialog(WPARAM wParam, LPARAM lParam) {
  // BOOL bRet = HandleInitDialog(wParam, lParam);
  Default();
  if (!UpdateData(FALSE)) {
    TRACE0("Warning: UpdateData failed during dialog init.\n");
  }

  ModifyStyle(0, WS_CLIPCHILDREN);

  m_Address.Init(0, 300);
  m_Address.SetFolder(NULL);
  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddressBar::DoDataExchange(CDataExchange *pDX) {
  CDialogBar::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAddressBar)
  DDX_Control(pDX, IDC_ADDRESS, m_Address);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddressBar, CDialogBar)
//{{AFX_MSG_MAP(CAddressBar)
ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBar message handlers

void CAddressBar::OnSize(UINT nType, int cx, int cy) {
  CDialogBar::OnSize(nType, cx, cy);

  CWnd *pWnd = GetDlgItem(IDC_ADDRESS);

  if (!pWnd)
    return;

  CRect rcAddr;

  CRect rcClient;
  GetClientRect(rcClient);

  pWnd->GetWindowRect(rcAddr);

  ScreenToClient(rcAddr);

  int width = rcClient.Width() - rcAddr.left;

  //pWnd->MoveWindow(rcAddr.left, rcAddr.top, width, rcAddr.Height());
  
  //pWnd->MoveWindow(rcAddr.left, rcAddr.top + 1, width, rcAddr.Height()-4);
  //ScreenToClient(rcClient);
  //rcClient.OffsetRect(1, 1);

  pWnd->MoveWindow(1, 5, width -2, rcAddr.Height());
  
}

void CAddressBar::SetFolder(const TCHAR *folder) {
  m_Address.SetFolder(folder);
  m_Address.Invalidate();
}

void CAddressBar::FocusOnAddress() { 
	m_Address.SetFocus(); 
}

void CAddressBar::ShowDropDown() { 
	m_Address.DisplayTree(); 
}
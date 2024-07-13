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
#include "PatternMatchBar.h"


CPatternMatchBar::CPatternMatchBar(CWnd* pParent /*=NULL*/) : CDialogBar() {
    //{{AFX_DATA_INIT(CPatternMatchBar)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

LRESULT CPatternMatchBar::OnInitDialog(WPARAM wParam, LPARAM lParam) {
    // BOOL bRet = HandleInitDialog(wParam, lParam);
    Default();
    if (!UpdateData(FALSE)) {
        TRACE0("Warning: UpdateData failed during dialog init.\n");
    }

    ModifyStyle(0, WS_CLIPCHILDREN);

    //m_Address.Init(0, 300);
    //m_Address.SetFolder(NULL);
    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatternMatchBar::DoDataExchange(CDataExchange* pDX) {
    CDialogBar::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPatternMatchBar)
    //DDX_Control(pDX, IDC_ADDRESS, m_Address);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPatternMatchBar, CDialogBar)
    //{{AFX_MSG_MAP(CPatternMatchBar)
    ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatternMatchBar message handlers

void CPatternMatchBar::OnSize(UINT nType, int cx, int cy) {
    CDialogBar::OnSize(nType, cx, cy);

    CWnd* pWnd = GetDlgItem(IDC_ADDRESS);

    if (!pWnd)
        return;

    CRect rcAddr;

    CRect rcClient;
    GetClientRect(rcClient);

    pWnd->GetWindowRect(rcAddr);

    ScreenToClient(rcAddr);

    int width = rcClient.Width() - rcAddr.left;

    //pWnd->MoveWindow(rcAddr.left, rcAddr.top, width, rcAddr.Height());
    pWnd->MoveWindow(1, 5, width - 2, rcAddr.Height());
}

BOOL CPatternMatchBar::OnEraseBkgnd(CDC* pDC)
{
    CWnd* pParent = GetParent();
    ASSERT_VALID(pParent);
    CPoint pt(0, 0);
    MapWindowPoints(pParent, &pt, 1);
    pt = pDC->OffsetWindowOrg(pt.x, pt.y);
    LRESULT lResult = pParent->SendMessage(WM_ERASEBKGND,
        (WPARAM)pDC->m_hDC, 0L);
    pDC->SetWindowOrg(pt.x, pt.y);
    return (BOOL)lResult;
}

void CPatternMatchBar::OnMove(int x, int y)
{
    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);

    Invalidate();
}
//
//void CPatternMatchBar::SetFolder(const TCHAR* folder) {
//    m_Address.SetFolder(folder);
//    m_Address.Invalidate();
//}
//
//void CPatternMatchBar::FocusOnAddress() {
//    m_Address.SetFocus();
//}
//
//void CPatternMatchBar::ShowDropDown() {
//    m_Address.DisplayTree();
//}

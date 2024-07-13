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

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_RIGHTSCROLLBAR;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	
	cs.style |= WS_BORDER;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

static int first = true;
void CChildFrame::ActivateFrame(int nCmdShow) 
{
	if (first)
	{
		nCmdShow = SW_SHOWMAXIMIZED;	
		first = false;
	}
	
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



// prevent flicker when a tab window is activated
LRESULT CChildFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_NCPAINT || message == WM_SIZE)
	{
		BOOL bMax;
		CMDIFrameWnd* pParentFrame = (CMDIFrameWnd*)GetParentFrame();
		if (pParentFrame)
		{
			CMDIChildWnd* pChildFrame = pParentFrame->MDIGetActive(&bMax);
			if (bMax)
			{
				if (message == WM_NCPAINT) // non client area
					return 0;

				if (message == WM_SIZE) // client area
				{
					if (wParam == SIZE_MAXIMIZED &&
						pChildFrame == this) // active and maximized
						return CMDIChildWnd::WindowProc(message, wParam, lParam);

					SetRedraw(FALSE);
					LRESULT ret =
						CMDIChildWnd::WindowProc(message, wParam, lParam);
					SetRedraw();
					return  ret;
				}
			}
		}
	}
	return CMDIChildWnd::WindowProc(message, wParam, lParam);
}
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
#include "PersistMDIFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////
// CPersistMDIFrame

IMPLEMENT_DYNAMIC(CPersistMDIFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CPersistMDIFrame, CMDIFrameWnd)
    //{{AFX_MSG_MAP(CPersistMDIFrame)
    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////

CPersistMDIFrame::CPersistMDIFrame()
{
	m_persistImpl.SetWindow( this );
	m_persistImpl.SetProfileHeading( _T("Main Frame"));
}
///////////////////////////////////////////////////////////////

CPersistMDIFrame::~CPersistMDIFrame()
{
}
///////////////////////////////////////////////////////////////

BOOL CPersistMDIFrame::ShowWindow( int nCmdShow )
{
	m_persistImpl.Load();
	return	CMDIFrameWnd::ShowWindow( m_persistImpl.getCmdShow() );
}
///////////////////////////////////////////////////////////////

void CPersistMDIFrame::OnDestroy()
{
	m_persistImpl.Save();
    CMDIFrameWnd::OnDestroy();
}
///////////////////////////////////////////////////////////////

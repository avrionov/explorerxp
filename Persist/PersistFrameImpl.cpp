
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
#include "PersistFrameImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// static
TCHAR CPersistFrameImpl::s_profileHeading[]		= _T( "Window size" );
TCHAR CPersistFrameImpl::s_profileRect[]		= _T( "Rectangle" );
TCHAR CPersistFrameImpl::s_profileIcon[]		= _T( "Iconized" );
TCHAR CPersistFrameImpl::s_profileMax[]			= _T( "Maximized" );
TCHAR CPersistFrameImpl::m_profileBarState[]	= _T( "BarState" );

//////////////////////////////////////////////////////////////////////

CPersistFrameImpl::CPersistFrameImpl( CFrameWnd* pWnd, LPCTSTR szHeading /*= NULL*/, bool bBarStates /*= false*/ )
{
	m_pWnd = pWnd;

	m_wndpl.length = sizeof(WINDOWPLACEMENT);
	
	if( szHeading != NULL ) {
		SetProfileHeading( szHeading );
	}
	else {
		SetProfileHeading( s_profileHeading );
	}

	m_bManageBarStates = false;
}
//////////////////////////////////////////////////////////////////////

CPersistFrameImpl::~CPersistFrameImpl()
{
}
//////////////////////////////////////////////////////////////////////

//	Load the window state from registry and apply the new state on the window
void	CPersistFrameImpl::Load()
{
	ASSERT( m_pWnd != NULL );

    WINDOWPLACEMENT	m_wndpl;
	CString			strText;

	m_pWnd->GetWindowPlacement( &m_wndpl );
	CRect	rect = m_wndpl.rcNormalPosition;

	//	Gets current window position and minimized/maximized status from Registry
	strText = AfxGetApp()->GetProfileString( m_profileHeading, s_profileRect );
	if( ! strText.IsEmpty() ) {
		rect.left	= _ttoi( (const TCHAR*) strText.Mid(0,4) );
		rect.top	= _ttoi( (const TCHAR*) strText.Mid(5,4) );
		rect.right	= _ttoi( (const TCHAR*) strText.Mid(10,4) );
		rect.bottom = _ttoi( (const TCHAR*) strText.Mid(15,4) );
	}

	BOOL	bMaximized = AfxGetApp()->GetProfileInt( m_profileHeading, s_profileMax, 0 );   
    UINT flags;
		
	if( bMaximized ) {
		m_nCmdShow = SW_SHOWMAXIMIZED;
		flags = WPF_RESTORETOMAXIMIZED;
	}
	else {
		m_nCmdShow = SW_SHOWNORMAL;
		flags = WPF_SETMINPOSITION;
	}


	RECT rMaxWA;
 
    // Get Min/Max Screen Information
    ::SystemParametersInfo( SPI_GETWORKAREA, 0, &rMaxWA, 0 );

	//	Make the window have new settings
	m_wndpl.showCmd = m_nCmdShow;
	m_wndpl.flags = flags;
	m_wndpl.ptMinPosition = CPoint(0, 0);
	m_wndpl.ptMaxPosition = CPoint( -::GetSystemMetrics(SM_CXBORDER),
									-::GetSystemMetrics(SM_CYBORDER) );
	m_wndpl.rcNormalPosition = rect;

	 if( ( m_wndpl.rcNormalPosition.left >= rMaxWA.left ) &&
                ( m_wndpl.rcNormalPosition.top >= rMaxWA.top ) &&
                ( m_wndpl.rcNormalPosition.right <= rMaxWA.right ) &&
                ( m_wndpl.rcNormalPosition.bottom <= rMaxWA.bottom ) )
	 {
			m_pWnd->SetWindowPlacement( &m_wndpl );
	 }


	if( m_bManageBarStates )
	{
		CString	cTmp;
		cTmp.Format(_T("%s\\%s"), m_profileHeading, m_profileBarState );
		m_pWnd->LoadBarState( cTmp );
	}
}
//////////////////////////////////////////////////////////////////////

//	Save the window state into registry
void	CPersistFrameImpl::Save()
{
	ASSERT( m_pWnd != NULL );

	CString			strText;
	BOOL			bIconic = FALSE;
	BOOL			bMaximized = FALSE;

	WINDOWPLACEMENT	m_wndpl;

	//	Gets current window position and minimized/maximized status
	m_pWnd->GetWindowPlacement( &m_wndpl );
	if( m_wndpl.showCmd == SW_SHOWNORMAL ) {
		bIconic = FALSE;
		bMaximized = FALSE;
	}
	else if( m_wndpl.showCmd == SW_SHOWMAXIMIZED) {
		bIconic = FALSE;
		bMaximized = TRUE;
	} 
	else if( m_wndpl.showCmd == SW_SHOWMINIMIZED) {
		bIconic = TRUE;
		if( m_wndpl.flags ) {
			bMaximized = TRUE;
		}
		else {
			bMaximized = FALSE;
		}
	}
	
	//	Save the state into Registry
	strText.Format( _T("%04d %04d %04d %04d"),
			m_wndpl.rcNormalPosition.left,
			m_wndpl.rcNormalPosition.top,
			m_wndpl.rcNormalPosition.right,
			m_wndpl.rcNormalPosition.bottom );
	AfxGetApp()->WriteProfileString( m_profileHeading, s_profileRect, strText );
	AfxGetApp()->WriteProfileInt( m_profileHeading, s_profileIcon, bIconic );
	AfxGetApp()->WriteProfileInt( m_profileHeading, s_profileMax, bMaximized );

	if( m_bManageBarStates )
	{
		CString	cTmp;
		cTmp.Format( _T("%s\\%s"), m_profileHeading, m_profileBarState );
		m_pWnd->SaveBarState( cTmp );
	}
}
//////////////////////////////////////////////////////////////////////

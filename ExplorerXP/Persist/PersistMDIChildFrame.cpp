
/*************************************************************************
 FILE       : PersistMDIChildFrame.cpp

 Author		: Stefan Chekanov (stefant@iname.com)

 Description: Implementation of CPersistMDIChildFrame class used to make Child
			  frames persistent.

Copyright(c) 1998,1999,2000
by Stefan Chekanov (stefant@iname.com)

This code may be used in compiled form in any way you desire. This
file may be redistributed unmodified by any means PROVIDING it is 
not sold for profit without the authors written consent, and 
providing that this notice and the authors name is included. If 
the source code in this file is used in any commercial application 
then a simple email to the author would be nice.

This file is provided "as is" with no expressed or implied warranty.
The author accepts no liability if it causes any damage.

*************************************************************************/
/* #    Revisions    # */

/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PersistMDIChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPersistMDIChildFrame

IMPLEMENT_DYNCREATE(CPersistMDIChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CPersistMDIChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPersistMDIChildFrame)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CPersistMDIChildFrame::CPersistMDIChildFrame()
{
	m_persistImpl.SetWindow( this );
	m_persistImpl.SetProfileHeading( "Child Windows" );
}
/////////////////////////////////////////////////////////////////////////////

CPersistMDIChildFrame::~CPersistMDIChildFrame()
{
}
/////////////////////////////////////////////////////////////////////////////

void CPersistMDIChildFrame::ActivateFrame(int nCmdShow) 
{
	m_persistImpl.Load();
	CMDIChildWnd::ActivateFrame( m_persistImpl.getCmdShow() );
}
/////////////////////////////////////////////////////////////////////////////

void CPersistMDIChildFrame::OnDestroy() 
{
	m_persistImpl.Save();
	CMDIChildWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////

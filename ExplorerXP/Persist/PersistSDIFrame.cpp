
/*************************************************************************
 FILE       : PersistSDIFrame.cpp

 Author		: Stefan Chekanov (stefant@iname.com)

 Description: Implementation of CPersistSDIFrame class used to make SDI frames 
			  persistent.

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

///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PersistSDIFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////
// CPersistSDIFrame

IMPLEMENT_DYNAMIC(CPersistSDIFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CPersistSDIFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CPersistSDIFrame)
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////

CPersistSDIFrame::CPersistSDIFrame()
{
	m_persistImpl.SetWindow( this );
	m_persistImpl.SetProfileHeading( "Main Frame" );
}
///////////////////////////////////////////////////////////////

CPersistSDIFrame::~CPersistSDIFrame()
{
}
///////////////////////////////////////////////////////////////

void CPersistSDIFrame::ActivateFrame(int nCmdShow)
{
	m_persistImpl.Load();
    CFrameWnd::ActivateFrame( m_persistImpl.getCmdShow() );
}
///////////////////////////////////////////////////////////////

void CPersistSDIFrame::OnDestroy()
{
	m_persistImpl.Save();
    CFrameWnd::OnDestroy();
}
///////////////////////////////////////////////////////////////


#ifndef __PERSISTMDICHILDFRAME_H__
#define __PERSISTMDICHILDFRAME_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/*************************************************************************
 FILE       : PersistMDIChildFrame.h

 Author		: Stefan Chekanov (stefant@iname.com)

 Description: Definition of CPersistMDIChildFrame class used to make Child
			  frames persistent.
			  To use:
			  1. Derive a class from it instead of CMDIChildWnd.
			  2. In the constructor call SetProfileHeading() to set the
			     registry key where to save the state. Its default value
				 is "Child Windows".
			  3. In the constructor call SetManageBarStates to set whether
			     bar states should be persistent too. Its default value is false.

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

#include "PersistFrameImpl.h"

/////////////////////////////////////////////////////////////////////////////

class CPersistMDIChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPersistMDIChildFrame)

protected:
	CPersistMDIChildFrame();
	virtual	~CPersistMDIChildFrame();


// Attributes
protected:
	//	Set the registry section where the state info to be saved/loaded from
	void	SetProfileHeading( LPCTSTR szHeading ) {
		m_persistImpl.SetProfileHeading( szHeading );
	}
	
	//	Specify whether positions of control bars should be saved/restored too.
	void	SetManageBarStates( bool bManage ) {
		m_persistImpl.SetManageBarStates( bManage );
	}

	CPersistFrameImpl	m_persistImpl;


// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPersistMDIChildFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CPersistMDIChildFrame)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

#endif	//	__PERSISTMDICHILDFRAME_H__

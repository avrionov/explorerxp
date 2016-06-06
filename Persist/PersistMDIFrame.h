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


#ifndef __PERSISTMDIFRAME_H__
#define __PERSISTMDIFRAME_H__

#pragma once

#include "PersistFrameImpl.h"

/////////////////////////////////////////////////////////////////////////////

class CPersistMDIFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CPersistMDIFrame)

protected:
    CPersistMDIFrame();
    virtual	~CPersistMDIFrame();


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
    //{{AFX_VIRTUAL(CPersistMDIFrame)
    //}}AFX_VIRTUAL

// Generated message map functions
protected:
    //{{AFX_MSG(CPersistMDIFrame)
    afx_msg void OnDestroy();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
	virtual	BOOL	ShowWindow( int nCmdShow );
};
/////////////////////////////////////////////////////////////////////////////

#endif	//	__PERSISTMDIFRAME_H__

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


#ifndef __PERSISTWNDIMPL_H__
#define __PERSISTWNDIMPL_H__

#pragma once

class CPersistFrameImpl
{
public:
	CPersistFrameImpl( CFrameWnd* pWnd = NULL, LPCTSTR szHeading = NULL, bool bBarStates = false );
	virtual ~CPersistFrameImpl();

	virtual	void	Load();
	virtual	void	Save();

	UINT				getCmdShow() {
		return	m_nCmdShow;
	}

	LPWINDOWPLACEMENT	getWindowsPlacement() {
		return	&m_wndpl;
	}

	//	Set the window for which this object load/save state info
	void	SetWindow( CFrameWnd* pWnd ) {
		m_pWnd = pWnd;
	}

	//	Set the registry section where the state info to be saved/loaded from
	void	SetProfileHeading( LPCTSTR szHeading ) {
		m_profileHeading = szHeading;
	}

	//	Specify whether positions of control bars should be saved/restored too.
	void	SetManageBarStates( bool bManage ) {
		m_bManageBarStates = bManage;
	}

private:
	CFrameWnd*		m_pWnd;
	UINT			m_nCmdShow;
    WINDOWPLACEMENT	m_wndpl;

	CString		m_profileHeading;			//	The assigned Profile Heading
	bool		m_bManageBarStates;			//	True if we should manage bar states too
											//	By default it is false

    static TCHAR	s_profileHeading[];		//	Default profile heading
    static TCHAR	s_profileRect[];
    static TCHAR	s_profileIcon[];
    static TCHAR	s_profileMax[];
	static TCHAR	m_profileBarState[];
};
//////////////////////////////////////////////////////////////////////

#endif	//	__PERSISTWNDIMPL_H__

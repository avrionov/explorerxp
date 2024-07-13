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

#ifndef __SINGLEINSTANCE_H__
#define __SINGLEINSTANCE_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


/////////////////////////////////////////////////////////////////////////////
// CSingleInstance
// See SingleInstance.cpp for the implementation of this class
//

class CSingleInstance
{
public:
	CSingleInstance();
	~CSingleInstance();

	BOOL	Create( UINT nID, CCommandLineInfo &cmdInfo );
	CString	GetClassName( void ) const;
	BOOL CreateMutex(CCommandLineInfo &cmdInfo);

protected:
	HANDLE	m_hMutex;
	CString	m_strClassName;
};

/////////////////////////////////////////////////////////////////////////////

#endif // __SINGLEINSTANCE_H__

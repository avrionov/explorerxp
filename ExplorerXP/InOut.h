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


#ifndef __INOUT_H__

#pragma once

class InOut 
{
public:	
	InOut (const TCHAR *funcName,...)
	{
		 va_list argptr;

		va_start(argptr, funcName);
		
		TCHAR *ptr = m_FuncName.GetBuffer (2560);

		_vsntprintf (ptr, 2550, funcName, argptr);
		
		m_FuncName.ReleaseBuffer ();
		
		//TRACE(_T("-> %s\n"), (LPCTSTR)m_FuncName);
		wsprintf (buf, _T("-> %s\n"), (LPCTSTR)m_FuncName);
		OutputDebugString (buf);
		m_TimeOut = m_timeIn = GetTickCount64 ();
	}

	~InOut ()
	{
		m_TimeOut = GetTickCount64 ();
		
		double span = (m_TimeOut  - m_timeIn) / 1000.0f;

		if (span != 0.0)
		{
			_stprintf (buf, _T("<- %s %6.3lf \n"), (LPCTSTR)m_FuncName, span);
			OutputDebugString (buf);			
			//TRACE(_T("<- %s %6.3lf \n"), (LPCTSTR)m_FuncName, span);			
		}			
		else 
		{
		//	TRACE(_T("<- %s \n"), (LPCTSTR)m_FuncName);
			_stprintf (buf, _T("<- %s \n"), (LPCTSTR)m_FuncName);
			OutputDebugString (buf);	
		}
	}
	
protected:
	CString m_FuncName;
	ULONGLONG m_timeIn;
	ULONGLONG m_TimeOut;
	TCHAR buf[4096];
};


#endif //__INOUT_H__

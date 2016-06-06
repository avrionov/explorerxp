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
#include "ThreadPool.h"
#include "Globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadPool::CThreadPool()
{

}

CThreadPool::~CThreadPool()
{
}


void CThreadPool::AddThread (int iID, HANDLE hThread)
{
	TRACE(_T("Add Th \n"));
	CGuard guard(m_lock);
	m_Threads.insert (CI2HVal (iID, hThread));
}

bool CThreadPool::IsThreadWorking (int iID)
{
	HANDLE hThread = FindThread (iID);
	if (hThread == NULL)
		return FALSE;

	DWORD dwExitCode = 0;
	

	if (::GetExitCodeThread (hThread, &dwExitCode))
	{
		if (dwExitCode != STILL_ACTIVE)
		{
			RemoveThread (iID);
			return false;
		}
		return true;
	}
	return false;
}

void CThreadPool::StopThread (int iID)
{
	HANDLE hThread = FindThread (iID);

	if (hThread == NULL)
		return;
	
	TerminateThread (hThread, 0);
	//CloseHandle (hThread);
	RemoveThread (iID);
}
	
void CThreadPool::StopAll ()
{
	TRACE(_T("Stop All \n"));
	CGuard guard(m_lock);

	CI2HIt it = m_Threads.begin ();
	HANDLE hThread;

	while (it != m_Threads.end ())
	{
		hThread = it->second;
		TerminateThread (hThread, 0);
		CloseHandle (hThread);
		++it;		
	}
	m_Threads.clear ();
}

void CThreadPool::RemoveThread (int iID)
{
	CGuard guard(m_lock);

	CI2HIt it = m_Threads.find (iID);
	if (it != m_Threads.end ())
	{
		CloseHandle (it->second);
		m_Threads.erase (it);
	}	
}

HANDLE CThreadPool::FindThread (int iID)
{
	CGuard guard(m_lock);

	HANDLE hThread = NULL;
	CI2HIt it = m_Threads.find (iID);
	if (it != m_Threads.end ())
		hThread =it->second;	
	return hThread;
}

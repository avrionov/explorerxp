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


#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#pragma once

#include "MyLock.h"

template <class T>
struct ThreadData
{
public:
	typedef void (T::*TFunc)();
	HANDLE hEvent;
	T* pThreadObject;
	TFunc pThreadFunc;
	static DWORD _ThreadFunc(ThreadData<T>* pThis)
	{
		//copying data, because after SetEvent is called,
		//the caller thread could restart and delete the
		//local data
		ThreadData<T> td=*pThis;
		SetEvent(td.hEvent);
		((*(td.pThreadObject)).*(td.pThreadFunc))();
		return 0;
	}
};

template <class T>
HANDLE CreateMemberThread(T* p,void (T::*func)())
{
	ThreadData<T> td;
	td.pThreadObject=p;
	td.pThreadFunc=func;
	td.hEvent=CreateEvent(NULL,0,0,NULL);
	DWORD Dummy;   //To make win 9x Happy with the lpThreadId param
	HANDLE ThreadHandle=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ThreadData<T>::_ThreadFunc,&td,NULL,&Dummy);
	//SetThreadPriority (ThreadHandle, THREAD_PRIORITY_HIGH);
	WaitForSingleObject(td.hEvent,INFINITE);
	::CloseHandle(td.hEvent);
	return ThreadHandle;
}

typedef std::map <int , HANDLE> CI2HMap;
typedef CI2HMap::iterator CI2HIt;
typedef CI2HIt::value_type CI2HVal;

class CThreadPool  
{
public:
	CThreadPool();
	virtual ~CThreadPool();
	void AddThread (int iID, HANDLE hThread);
	
	template <class T> void  StartThread (int iID, T* p, void (T::*func)())
	{
		ThreadData<T> td;
		td.pThreadObject=p;
		td.pThreadFunc=func;
		td.hEvent= CreateEvent(NULL,0,0, _T("StartThread"));
		DWORD Dummy;   //To make win 9x Happy with the lpThreadId param
		HANDLE ThreadHandle = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ThreadData<T>::_ThreadFunc,&td,NULL,&Dummy);
		if (ThreadHandle != NULL) {
			WaitForSingleObject(td.hEvent,INFINITE);
			::CloseHandle(td.hEvent);
			SetThreadPriority (ThreadHandle, THREAD_PRIORITY_BELOW_NORMAL);
			AddThread (iID, ThreadHandle);
		}
	}	

	template <class T> void  StartThreadAndWait (int iID, T* p, void (T::*func)())
	{
		ThreadData<T> td;
		td.pThreadObject=p;
		td.pThreadFunc=func;
		td.hEvent= CreateEvent(NULL,0,0, _T("StartThread"));
		DWORD Dummy;   //To make win 9x Happy with the lpThreadId param
		HANDLE ThreadHandle= CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ThreadData<T>::_ThreadFunc,&td,NULL,&Dummy);		
		if (ThreadHandle != NULL) {
			WaitForSingleObject(td.hEvent,INFINITE);
			::CloseHandle(td.hEvent);
			SetThreadPriority (ThreadHandle, THREAD_PRIORITY_BELOW_NORMAL);
			AddThread (iID, ThreadHandle);
			WaitForSingleObject (ThreadHandle, INFINITE);
		}
	}	

	bool IsThreadWorking (int iID);
	void StopThread (int iID);
	void StopAll ();
	HANDLE FindThread (int iDI);
	void RemoveThread (int iID);
	CI2HMap m_Threads;
  CMyLock m_lock;
};

#endif // __THREAD_POOL_H__

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

#pragma once

//classes declrared in other files:
class CDirectoryChangeWatcher;
class CDirectoryChangeHandler;
//classes declared in this file:
class CDirChangeNotification;
class CDelayedDirectoryChangeHandler;

class CDelayedNotificationWindow;
class CDelayedNotificationThread;

/*******************************************************************
	The classes in this file implement methods to ensure that file change 
	notifications are fired in a thread other than the worker thread used
	by CDirectoryChangeWatcher.

	Dispatching the notifications in to a different thread improves the performance
	of CDirectoryChangeWatcher so that it can process more notifications faster
	and notifications aren't 'lost'.


	There are two methods of dispatching functions to another thread.

		1)  One is to use the message pump associated w/ the main thread by posting notifications
			to a hidden window. This is implemented w/ the class CDelayedNotificationWindow.

		2)	The other is to create a worker thread that implements a message pump.  This is 
			implemented w/ the class CDelayedNotificationThread.


	If your app uses a GUI then it has a already has message pump.  
	You can make sure that CDelayedNotificationWindow is used in this case.
	The advantage here being that there is one less worker thread used in your program.

	If your app is a command line app or otherwise doesn't have a GUI,
	then you will want to make sure that you are using the CDelayedNotificationThread
	to dispatch notifications to another thread.

	This is determined by a flag passed is passed to the constructor of CDirecotryChangeWatcher

********************************************************************/

class CDelayedNotifier
//
//	Abstract base class for ensuring notifications are fired in a thread 
//
//
{
public:
	virtual ~CDelayedNotifier(){}
	virtual void PostNotification(CDirChangeNotification * pNotification) = 0;

};

class CDelayedNotificationWindow : public CDelayedNotifier
//
//	A class that implements a
//	there will always be only one of the actual windows 
//	in existance. 
//
{
public:
		CDelayedNotificationWindow(){  AddRef(); }
		virtual ~CDelayedNotificationWindow(){ Release(); }
		

		void PostNotification(CDirChangeNotification * pNotification);
private:
		long AddRef();		//	the window handle is reference counted
		long Release();		//

		static long s_nRefCnt;
		static HWND s_hWnd; //there's only one window no matter how many instances of this class there are.... this means that all notifications are handled by the same thread.
		static BOOL s_bRegisterWindow;
		BOOL RegisterWindowClass(LPCTSTR szClassName);
		BOOL CreateNotificationWindow();
};

class CDelayedNotificationThread : public CDelayedNotifier
//
//	Class that implements a worker thread w/ a message pump.
//	CDirectoryChangeWatcher posts notifications to this thread, where they are dispatched.
//	This thread executes CDirectoryChangeHandler notifications.
//
{
public:
	CDelayedNotificationThread()
		:m_hThreadStartEvent(NULL)
	{ 
		m_hThreadStartEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		ASSERT( m_hThreadStartEvent );
		AddRef(); 
	}
	virtual ~CDelayedNotificationThread()
	{ 
		Release(); 
		if( m_hThreadStartEvent ) 
			CloseHandle(m_hThreadStartEvent), m_hThreadStartEvent = NULL;
	}

	void PostNotification(CDirChangeNotification * pNotification);

private:
	long AddRef();					// The thread handle is reference
	long Release();					// counted so that only one thread is used
									// so that there's only one worker thread(performing this functino)
	static long		s_nRefCnt;		// no matter how many directories are being watched
	static HANDLE	s_hThread;		//	
	static DWORD	s_dwThreadID;	//  
										
	static UINT __stdcall ThreadFunc(LPVOID lpvThis);

	bool StartThread();
	bool StopThread();

	BOOL WaitForThreadStartup(){ return WaitForSingleObject(m_hThreadStartEvent, INFINITE) == WAIT_OBJECT_0; };
	BOOL SignalThreadStartup(){ return SetEvent( m_hThreadStartEvent ) ; }

	HANDLE m_hThreadStartEvent;//signals that the worker thread has started. this fixes a bug condition.
		
};


class CDirChangeNotification
//
//	 A class to help dispatch the change notifications to the main thread.
//
//	 This class holds the data in memory until the notification can be dispatched.(ie: this is the time between when the notification is posted, and the clients notification code is called).
//
//
{
private:
	CDirChangeNotification();//not implemented
public:
	explicit CDirChangeNotification(CDelayedDirectoryChangeHandler * pDelayedHandler, DWORD dwPartialPathOffset);
	~CDirChangeNotification();

	//
	//
	void PostOn_FileAdded(LPCTSTR szFileName);
	void PostOn_FileRemoved(LPCTSTR szFileName);
	void PostOn_FileNameChanged(LPCTSTR szOldName, LPCTSTR szNewName);
	void PostOn_FileModified(LPCTSTR szFileName);
	void PostOn_ReadDirectoryChangesError(DWORD dwError, LPCTSTR szDirectoryName);
	void PostOn_WatchStarted(DWORD dwError, LPCTSTR szDirectoryName);
	void PostOn_WatchStopped(LPCTSTR szDirectoryName);

	void DispatchNotificationFunction();


	enum eFunctionToDispatch{	eFunctionNotDefined = -1,
								eOn_FileAdded		= FILE_ACTION_ADDED, 
								eOn_FileRemoved		= FILE_ACTION_REMOVED, 
								eOn_FileModified	= FILE_ACTION_MODIFIED,
								eOn_FileNameChanged	= FILE_ACTION_RENAMED_OLD_NAME,
								eOn_ReadDirectoryChangesError,
								eOn_WatchStarted,
								eOn_WatchStopped
	};	
protected:
	void PostNotification();
	
private:
	friend class CDelayedDirectoryChangeHandler;
	CDelayedDirectoryChangeHandler * m_pDelayedHandler;

	//
	//	Members to help implement DispatchNotificationFunction
	//
	//

	eFunctionToDispatch m_eFunctionToDispatch;
	//Notification Data:
	TCHAR *	m_szFileName1;//<-- is the szFileName parameter to On_FileAdded(),On_FileRemoved,On_FileModified(), and is szOldFileName to On_FileNameChanged(). Is also strDirectoryName to On_ReadDirectoryChangesError(), On_WatchStarted(), and On_WatchStopped()
	TCHAR *	m_szFileName2;//<-- is the szNewFileName parameter to On_FileNameChanged()
	DWORD m_dwError;	  //<-- is the dwError parameter to On_WatchStarted(), and On_ReadDirectoryChangesError()
	//

	DWORD m_dwPartialPathOffset;//helps support FILTERS_CHECK_PARTIAL_PATH...not passed to any functions other than may be used during tests in CDelayedDirectoryChangeHandler::NotifyClientOfFileChange()


	friend class CDirChangeNotification;
	friend class CDirectoryChangeWatcher;
	friend DWORD GetPathOffsetBasedOnFilterFlags(CDirChangeNotification*,DWORD);//a friend function
};


//////////////////////////////////////////////////////////////////////////
//
//	This class makes it so that a file change notification is executed in the
//	context of the main thread, and not the worker thread.
//
//
//	It works by creating a hidden window.  When it receieves a notification
//	via one of the On_Filexxx() functions, a message is posted to this window.
//	when the message is handled, the notification is fired again in the context
//	of the main thread, or whichever thread that called CDirectoryChangeWatcher::WatchDirectory()
//
//
/////////////////////////////////////////////////////////////////////////////


class CDelayedDirectoryChangeHandler : public CDirectoryChangeHandler
//
//	Decorates an instance of a CDirectoryChangeHandler object.
//	Intercepts notification function calls and posts them to 
//	another thread through a method implemented by a class derived from 
//	CDelayedNotifier
//	
//
//	This class implements dispatching the notifications to a thread
//	other than CDirectoryChangeWatcher::MonitorDirectoryChanges()
//
//	Also supports the include and exclude filters for each directory
//
{
private:
	CDelayedDirectoryChangeHandler();//not implemented.
public:
	CDelayedDirectoryChangeHandler( CDirectoryChangeHandler * pRealHandler, bool bAppHasGUI, LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter, DWORD dwFilterFlags);
	virtual ~CDelayedDirectoryChangeHandler();

	
	CDirectoryChangeHandler * GetRealChangeHandler()const { return m_pRealHandler; }
	CDirectoryChangeHandler * & GetRealChangeHandler(){ return m_pRealHandler; }//FYI: PCLint will give a warning that this exposes a private/protected member& defeats encapsulation.  

	void PostNotification(CDirChangeNotification * pNotification);
	void DispatchNotificationFunction(CDirChangeNotification * pNotification);


protected:
	//These functions are called when the directory to watch has had a change made to it
	void On_FileAdded(const CString & strFileName);
	void On_FileRemoved(const CString & strFileName);
	void On_FileModified(const CString & strFileName);
	void On_FileNameChanged(const CString & strOldFileName, const CString & strNewFileName);
	void On_ReadDirectoryChangesError(DWORD dwError, const CString & strDirectoryName);

	void On_WatchStarted(DWORD dwError, const CString & strDirectoryName);
	void On_WatchStopped(const CString & strDirectoryName);
	

	void SetChangedDirectoryName(const CString & strChangedDirName);
	const CString & GetChangedDirectoryName()const;

	BOOL WaitForOnWatchStoppedDispatched();//see comments in .cpp


	bool NotifyClientOfFileChange(CDirChangeNotification * pNot);

	CDirChangeNotification * GetNotificationObject();
	void DisposeOfNotification(CDirChangeNotification * pNotification);

	CDelayedNotifier * m_pDelayNotifier;
	CDirectoryChangeHandler * m_pRealHandler;	

						// m_bAppHasGUI: 
						//   This flag, if set to true, indicates that the app has a message
	bool m_bAppHasGUI;	//	 pump, and that functions are dispatched to the main thread.
						//   Otherwise, functions are dispatched to a separate worker thread.
						//
	DWORD m_dwFilterFlags;

	DWORD m_dwPartialPathOffset; //helps support FILTERS_CHECK_PARTIAL_PATH
	void SetPartialPathOffset(const CString & strWatchedDirName);

	friend class CDirectoryChangeWatcher;
	friend class CDirectoryChangeWatcher::CDirWatchInfo;

private:
	HANDLE m_hWatchStoppedDispatchedEvent;//supports WaitForOnWatchStoppedDispatched()

	//
	//	Load PathMatchSpec dynamically because it's only supported if IE 4.0 or greater is
	//	installed.
	static HMODULE s_hShlwapi_dll;//for the PathMatchSpec() function
	static BOOL s_bShlwapi_dllExists;//if on NT4.0 w/out IE 4.0 or greater, this'll be false
	static long s_nRefCnt_hShlwapi;

};
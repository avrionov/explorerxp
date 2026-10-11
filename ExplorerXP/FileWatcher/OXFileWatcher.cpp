// ==========================================================================
//				Class Implementation : COXFileWatcher
// ==========================================================================

// Source file : OXFileWatcher.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXFileWatcher.h"
#include "OXMainRes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#ifdef _DEBUG
// Trace a message when the RESULT specifies failure
#define CONDITIONAL_TRACE_RESULT(TEXT, RESULT)										\
	{ if (FAILED(RESULT)) {															\
		TRACE(_T("%s : Failed (%u == 0x%X, Code : %u) :\n\t%s\n"),					\
		_T(TEXT), RESULT, RESULT, HRESULT_CODE(RESULT), COXFileWatcher::GetResultMessage(RESULT));	\
	} }
#else
// Do not trace in Release build
#define CONDITIONAL_TRACE_RESULT(TEXT, RESULT)
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// COXFileWatcher

// Data members -------------------------------------------------------------
// public:             

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

const DWORD COXFileWatcher::OXFileWatchChangeFileName	=	FILE_NOTIFY_CHANGE_FILE_NAME;
// --- Any filename change in the watched directory or subtree causes a change notification

const DWORD COXFileWatcher::OXFileWatchChangeDirName	=	FILE_NOTIFY_CHANGE_DIR_NAME;
// --- Any directory-name change in the watched directory or subtree causes a change notification

const DWORD COXFileWatcher::OXFileWatchChangeAttributes	=	FILE_NOTIFY_CHANGE_ATTRIBUTES;
// --- Any attribute change in the watched directory or subtree causes a change notification

const DWORD COXFileWatcher::OXFileWatchChangeSize		=	FILE_NOTIFY_CHANGE_SIZE;
// --- Any file-size change in the watched directory or subtree causes a change notification
//     The operating system detects a change in file size only when the file is written to the disk.
//     For operating systems that use extensive caching, detection occurs only when the cache is 
//     sufficiently flushed

const DWORD COXFileWatcher::OXFileWatchChangeLastWrite	=	FILE_NOTIFY_CHANGE_LAST_WRITE;
// --- Any change to the last write-time of files in the watched directory or subtree causes a 
//     change notification. The operating system detects a change to the last write-time only when 
//     the file is written to the disk. For operating systems that use extensive caching, detection 
//     occurs only when the cache is sufficiently flushed

const DWORD COXFileWatcher::OXFileWatchChangeLastAccess	=	FILE_NOTIFY_CHANGE_LAST_ACCESS;
// --- Any change to the last access time of files in the watched directory or subtree causes
//     a change notification

const DWORD COXFileWatcher::OXFileWatchChangeCreation	=	FILE_NOTIFY_CHANGE_CREATION;
// --- Any change to the creation time of files in the watched directory or subtree causes 
//	   a change notification

const DWORD COXFileWatcher::OXFileWatchChangeSecurity	=	FILE_NOTIFY_CHANGE_SECURITY;
// --- Any security-descriptor change in the watched directory or subtree causes a change notification


#if defined(_WIN32_WINNT)
BOOL COXFileWatcher::g_bBackupPrivilegeGranted=FALSE;
#endif

// protected:
	// CArray<COXWatchedDir*, COXWatchedDir*>	m_arWatchedDirs;
	// --- Array with the watched directories

	// COXIDManager<COXFileWatchNotifier>	m_arFileWatchNotifiers;
	// --- Array with the waiting notifier objects

	// HRESULT	m_hrError;
	// --- the HRESULT value of the last error

	// BOOL		m_bThreadIsRunning;
	// --- TRUE if the thread which	is used to watch file change notifications is running

	// CMutex		m_mxThreadShouldStop;
	// --- Is used to stop the watching thread

	// CMutex		m_mxThreadStopped;
	// --- Is used to inform the main thread that the watcher thread is stopped
	//     and it is safe to make changes in m_arWatchedDirs

	// CWinThread*	m_pWatcherThread;
	// --- A pointer to the watcher thread

	// HANDLE*		m_pHandles;
	// --- Array with the watched handles

	// HANDLE*		m_pNewHandles;
	// --- Array with the new handles after any changes in m_arWatchedDirs. 

	// WORD		m_nNumHandles;
	// --- the Number of watched handles

// private:

// Member functions ---------------------------------------------------------
// public:

COXFileWatcher::COXFileWatcher(BOOL bAddBackupPrivilege/*=TRUE*/) :
	m_hrError(ERROR_SUCCESS),
	m_bThreadIsRunning(FALSE),
	m_mxThreadShouldStop(TRUE),
	m_mxThreadStopped(FALSE),
	m_mxThreadCanContinue(FALSE),
	m_eventStartThread(FALSE,TRUE,NULL,NULL),
	m_pHandles(NULL),
	m_pNewHandles(NULL),
	m_pWatcherThread(NULL),
	m_nNumHandles(0),
	m_bDeletingThread(FALSE)
{
#if defined(_WIN32_WINNT)
	if(bAddBackupPrivilege)
	{
		if(!SetBackupPrivilege(TRUE))
		{
			AfxThrowNotSupportedException();
		}
	}
#else
	UNREFERENCED_PARAMETER(bAddBackupPrivilege);
#endif
}


COXFileWatcher::~COXFileWatcher()
{
	// Will stop thread if it's running and will delete handles
	RemoveAllWatches();		
	// ... m_pWatcherThread may already be NULL
	delete m_pWatcherThread;
}


#if defined(_WIN32_WINNT)
BOOL COXFileWatcher::SetBackupPrivilege(BOOL bAdd)
{
	if(g_bBackupPrivilegeGranted==bAdd)
	{
		// already done
		return TRUE;
	}

	TOKEN_PRIVILEGES NewState;
	LUID luid;
	HANDLE hToken=NULL;
	BOOL bRtnStatus=TRUE;
	
	// Open the process token for this process.
	if(::OpenProcessToken(
		::GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		// Get the local unique id for the privilege.
		if(::LookupPrivilegeValue(NULL,_T("SeBackupPrivilege"),&luid))
		{
			// Assign values to the TOKEN_PRIVILEGE structure.
			NewState.PrivilegeCount=1;
			NewState.Privileges[0].Luid=luid;
			NewState.Privileges[0].Attributes=(bAdd ? SE_PRIVILEGE_ENABLED : 0);

			// Adjust the token privilege
			if(!::AdjustTokenPrivileges(
				hToken,FALSE,&NewState,sizeof(NewState),NULL,NULL))
			{
				bRtnStatus = FALSE;
			}
		}
		else
		{
			bRtnStatus = FALSE;
		}
	}
	else
	{
		bRtnStatus = FALSE;
	}

	if(hToken!=NULL)
	{
		::CloseHandle(hToken);
	}

	if(bRtnStatus)
	{
		g_bBackupPrivilegeGranted=bAdd;
	}

	return bRtnStatus;
}
#endif


BOOL COXFileWatcher::AddWatch(LPCTSTR pszPath, BOOL bWatchSubtree, DWORD dwWatchFilter)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	ASSERT(pszPath != NULL);

	// Clear error flag
	m_hrError = ERROR_SUCCESS;

	COXWatchedDir* pwdNewEntry;

	// Make the class a Thread-Safe Class
	CSingleLock lockObj(&m_crArrayWatchedDirGuard);
	VERIFY(lockObj.Lock());

	// Fisrt check if we are not watching for the same path
	for(int i=0; i<m_arWatchedDirs.GetSize(); i++)
	{
		if((m_arWatchedDirs[i])->m_sPath == pszPath)
		{
			// ... OK. The path is already here
			COXWatchedDir* pwdPresentEntry = m_arWatchedDirs[i];

			if(bWatchSubtree == pwdPresentEntry->m_bWatchSubTree &&
			   dwWatchFilter == pwdPresentEntry->m_dwWatchFilter)
			{
				// No modifications
				return TRUE;
			}

			pwdNewEntry = new COXWatchedDir(
				pszPath,										// directory to be watched
				bWatchSubtree|pwdPresentEntry->m_bWatchSubTree,	// flag for monitoring directory or directory tree 
				dwWatchFilter|pwdPresentEntry->m_dwWatchFilter,	// filter conditions to watch for
				IsExtendedInfoSupported());						// flag for extended info

			// Create new handle
			if(!pwdNewEntry->FindFirstHandle(m_hrError))
			{
				goto error;
			}

			// Prepares the handles for the watcher thread
			m_pNewHandles = m_pHandles;

			VERIFY(m_mxThreadCanContinue.Lock());
			// Informs the watcher thread that it can change the handles
			// and waits for the watcher thread to stop
			VERIFY(m_mxThreadShouldStop.Unlock());
			// wait until the watcher thread stops
			VERIFY(m_mxThreadStopped.Lock());

			// Update the entry for the given path
			m_arWatchedDirs.SetAt(i, pwdNewEntry);

			// Close old entry. This will close the old handle
			delete pwdPresentEntry;

			// Replace the old handle with the new one and start the 
			// thread again
			m_pHandles[i+1]=pwdNewEntry->m_hEvent;

			VERIFY(m_mxThreadStopped.Unlock());
			VERIFY(m_mxThreadShouldStop.Lock());
			// notify watcher thread that it can go on executing
			VERIFY(m_mxThreadCanContinue.Unlock());

			return TRUE;
		}
	}

	pwdNewEntry = new COXWatchedDir(pszPath,		// directory to be watched
									bWatchSubtree,	// flag for monitoring directory or directory tree 
									dwWatchFilter,	// filter conditions to watch for
									IsExtendedInfoSupported());// flag for extended info

	if(pwdNewEntry->FindFirstHandle(m_hrError))
	{
		// Handles are m_nNumHandles, so we must allocate (m_nNumHandles+2) : one for the StartStopEvent
		// and one for the new entry
		m_pNewHandles= new HANDLE[m_nNumHandles + 2];
		// Informs the watcher thread that it can change the handles
		if(m_bThreadIsRunning) 
		{
			VERIFY(m_mxThreadCanContinue.Lock());
			// notify watcher thread that it should stop
			VERIFY(m_mxThreadShouldStop.Unlock());
			// wait until it stops

			DWORD dwResult=::WaitForSingleObject(m_mxThreadStopped.m_hObject, 0xFFFFFFFF);
			ASSERT(dwResult==NULL);
			//VERIFY(m_mxThreadStopped.Lock());
		}

		// update the array of directories
		m_arWatchedDirs.Add(pwdNewEntry);

		if(0 < m_nNumHandles)
		{
			memcpy(m_pNewHandles,m_pHandles,(m_nNumHandles+1)*sizeof(HANDLE));
		}
		else
		{
			m_pNewHandles[0]=HANDLE(m_mxThreadShouldStop);
		}
		m_pNewHandles[m_nNumHandles+1] = pwdNewEntry->m_hEvent;

		// ... m_pHandles may be NULL
		delete [] m_pHandles;
		m_pHandles = m_pNewHandles;
		m_nNumHandles++;

		// Starts the watcher thread
		if(m_bThreadIsRunning) 
		{
			VERIFY(m_mxThreadStopped.Unlock());
			VERIFY(m_mxThreadShouldStop.Lock());
			// Unlocks the watcher thread
			VERIFY(m_mxThreadCanContinue.Unlock());
			return TRUE;
		}
		else
		{
			// ... m_pWatcherThread may already be NULL
			delete m_pWatcherThread;
			VERIFY(m_eventStartThread.ResetEvent());
			m_pWatcherThread=AfxBeginThread(FileWatchThreadFunction,this);
			if(m_pWatcherThread!=NULL)
			{
				m_pWatcherThread->m_bAutoDelete = FALSE;
				::WaitForSingleObject(m_eventStartThread,INFINITE);
				return TRUE;
			}
		}
	}
	
error:
	delete pwdNewEntry;
	CONDITIONAL_TRACE_RESULT("COXFileWatcher::AddWatch", m_hrError)
	return FALSE;
}


HRESULT	COXFileWatcher::GetLastError() const
{
	return m_hrError;
}

BOOL COXFileWatcher::IsExtendedInfoSupported() const
{
#if defined(_UNICODE) && (_WIN32_WINNT >= 0x400)
// Extended info is supported
	OSVERSIONINFO OSVersion;

	OSVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	VERIFY(::GetVersionEx(&OSVersion));

	// At the moment extended information is only supported on Windows NT 4.0
	return (OSVersion.dwPlatformId==VER_PLATFORM_WIN32_NT && 
		4<=OSVersion.dwMajorVersion);

#else
// Extended info is not supported
	
	return FALSE;

#endif //defined(_UNICODE) && (_WIN32_WINNT >= 0x400)
}

void COXFileWatcher::RemoveWatch(LPCTSTR pszPath)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	ASSERT(pszPath != NULL);
	
	// Make the class a Thread-Safe Class
	CSingleLock lockObj(&m_crArrayWatchedDirGuard);
	VERIFY(lockObj.Lock());

	// Fisrt find the given path in the local array
	for(int i=0; i<m_arWatchedDirs.GetSize(); i++)
	{
		if((m_arWatchedDirs[i])->m_sPath==pszPath)
		{
			// If the handle is only one then stop the watcher thread
			if(m_nNumHandles == 1) 
				m_pNewHandles = NULL;
			else 
				m_pNewHandles=m_pHandles;

			VERIFY(m_mxThreadCanContinue.Lock());
			// notify watcher thread that it should stop
			VERIFY(m_mxThreadShouldStop.Unlock());
			// wait until it stops
//			VERIFY(m_mxThreadStopped.Lock());
			m_mxThreadStopped.Lock();

			// Move all handles after the current one forward
			for(int k=i+1; k<m_nNumHandles; k++)
			{
				m_pHandles[k]=m_pHandles[k+1];
			}
			m_nNumHandles--;

			COXWatchedDir* pwdEntry = m_arWatchedDirs[i];
			m_arWatchedDirs.RemoveAt(i);
			delete (COXWatchedDir*)pwdEntry;

			//critical section, before let the thread to go on
			//set m_bDeletingThread to TRUE to inform the thread
			//that we are deleting it. Otherwise, may came notification
			//from windows exactly in the period time we have deleted
			//the list watches, but thread had no time to finished,
			//so, the thread receives notification while
			//::WaitForSingleObject() in this function 
			if (m_pNewHandles==NULL)
				m_bDeletingThread=TRUE;

			VERIFY(m_mxThreadStopped.Unlock());
			VERIFY(m_mxThreadShouldStop.Lock());
			// notify watcher thread that it can go on executing
			VERIFY(m_mxThreadCanContinue.Unlock());

			if(m_pNewHandles!=NULL)
			{
				return;
			}

			// Wait for the watcher thread to stops
			::WaitForSingleObject(m_pWatcherThread->m_hThread, INFINITE);

			delete m_pWatcherThread;
			m_pWatcherThread=NULL;

			delete [] m_pHandles;
			m_pHandles=NULL;

			//end of the critical section, restore m_bDeletingThread
			m_bDeletingThread=FALSE;

			return; 
		}
	}
}

void COXFileWatcher::RemoveAllWatches()
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	// Make the class a Thread-Safe Class
	CSingleLock lockObj(&m_crArrayWatchedDirGuard);
	VERIFY(lockObj.Lock());

	StopThread();
	
	if(m_arWatchedDirs.GetSize() == 0) 
		return;
	for(int i=0; i<=m_arWatchedDirs.GetUpperBound(); i++)
	{
		delete m_arWatchedDirs[i];
	}
	m_arWatchedDirs.RemoveAll();

	if(m_pHandles!=NULL)
	{
		delete [] m_pHandles;
		m_pHandles = NULL;
	}
	m_nNumHandles = 0;
}

BOOL COXFileWatcher::OnNotify(COXFileWatchNotifier fileWatchNotifier)
{
	return FALSE;
}

void COXFileWatcher::EnableWindowNotification(LPCTSTR pszPath, CWnd* pWnd, BOOL bPost)
{
	ASSERT(pszPath!=NULL);
	ASSERT(pWnd!=NULL && pWnd->m_hWnd!=NULL);
	
	// Make the class a Thread-Safe Class
	CSingleLock lockObj(&m_crArrayWatchedDirGuard);
	VERIFY(lockObj.Lock());

	// ... Looking for the path
	for(int i=0; i<=m_arWatchedDirs.GetUpperBound(); i++)
	{
		if((m_arWatchedDirs[i])->m_sPath == pszPath)
		{
			COXWatchedDir* pwdEntry = m_arWatchedDirs[i];
			pwdEntry->m_hwndWindowToNotify = pWnd->m_hWnd;
			pwdEntry->m_bPost = bPost;
			break;
		}
	}
}

COXFileWatchNotifier COXFileWatcher::GetFileWatchNotifier(DWORD fileWatchNotifierID)
{
	COXFileWatchNotifier* pfwnRequestedNotifier=m_arFileWatchNotifiers.GetItemPtr(fileWatchNotifierID);
	COXFileWatchNotifier aFileWatchNotifier;

	if(pfwnRequestedNotifier != NULL) 
	{
		aFileWatchNotifier = *pfwnRequestedNotifier;

		// Remove the notifier object, so it is no more accessable
		m_arFileWatchNotifiers.RemoveItem(fileWatchNotifierID);
		delete pfwnRequestedNotifier;
	}
	return aFileWatchNotifier;
}

void COXFileWatcher::DisableWindowNotification(LPCTSTR pszPath)
{
	ASSERT(pszPath!=NULL);

	// Make the class a Thread-Safe Class
	CSingleLock lockObj(&m_crArrayWatchedDirGuard);
	VERIFY(lockObj.Lock());

	if(pszPath == NULL) 
		return;

	// ... Looking for the path again
	for(int i=0; i<=m_arWatchedDirs.GetUpperBound(); i++)
	{
		if((m_arWatchedDirs[i])->m_sPath == pszPath)
		{
			COXWatchedDir* pwdEntry = m_arWatchedDirs[i];
			pwdEntry->m_hwndWindowToNotify = NULL;
			break;
		}
	}
}

void COXFileWatcher::Empty()
{
	RemoveAllWatches();
	m_arFileWatchNotifiers.DeleteAll();
}

#ifdef _DEBUG
void 	COXFileWatcher::AssertValid()
{
	// Make the class a Thread-Safe Class
	CSingleLock lockObj(&m_crArrayWatchedDirGuard);
	VERIFY(lockObj.Lock());

	CObject::AssertValid();

	for(int i=0; i<m_arWatchedDirs.GetSize(); i++)
	{
		COXWatchedDir* pwd = m_arWatchedDirs[i];
		pwd->AssertValid();
	}
}

void COXFileWatcher::Dump(CDumpContext& dc)
{
	// Make the class a Thread-Safe Class
	CSingleLock lockObj(&m_crArrayWatchedDirGuard);
	VERIFY(lockObj.Lock());

	CObject::Dump(dc);

	dc << _T("\nNumber of watched paths: ");
	dc << m_arWatchedDirs.GetSize() << _T("\n");
	
	if (dc.GetDepth() > 0)
	{
		for(int i=0; i<m_arWatchedDirs.GetSize(); i++)
		{
			COXWatchedDir* pwd = m_arWatchedDirs[i];
			pwd->Dump(dc);
		}
	}
}


static TCHAR szUnknownError[] = _T("*** Unknown Error ***");
static DWORD dwLangID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT); 

CString COXFileWatcher::GetResultMessage(HRESULT hResult)
	// --- In  : hResult : The result code
	// --- Out : 
	// --- Returns : A string containing a message of the specified code
	// --- Effect : Retrieves the error message corresponding to the given hResult
{
	CString sResultMessage;
	LPTSTR pszMsgBuf = NULL;
	BOOL bUnknown = FALSE;
	DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;

	// ... Remove the facility part if FACILITY_WIN32
	if (HRESULT_FACILITY(hResult) == FACILITY_WIN32)
		hResult = HRESULT_CODE(hResult);

	// ... Get the actual message 
	if (::FormatMessage(dwFlags, NULL, hResult, dwLangID,
	      (LPTSTR)&pszMsgBuf, 0, NULL) == 0)
	{
		TRACE2("COXFileWatcher::GetResultMessage : No message was found for result code %i == 0x%8.8X\n",
			hResult, hResult);
	  	//pszMsgBuf = szUnknownError;
		VERIFY(sResultMessage.LoadString(IDS_OX_FILEWATCHERUNKERROR));
		bUnknown = TRUE;
	}
	else
		sResultMessage = pszMsgBuf;

	// ... Clean up
	if (!bUnknown)
		LocalFree(pszMsgBuf);

	return sResultMessage;
}

#endif // _DEBUG

// protected:

UINT COXFileWatcher::FileWatchThreadFunction(LPVOID pParam)
// --- In  : pParam: a pointer to the current COXFileWatcher object 
	// --- Out : 
	// --- Returns :
	// --- Effect : Used as a worker thread function
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	ASSERT(pParam!=NULL);

	COXFileWatcher* pThis=(COXFileWatcher*)pParam;
	HANDLE*			pHandles=pThis->m_pHandles;
	
	pThis->m_bThreadIsRunning = TRUE;

	VERIFY(pThis->m_eventStartThread.SetEvent());

#if defined(_UNICODE) && (_WIN32_WINNT >= 0x400)
// Extended info is supported

	static TCHAR	szFileName[_MAX_PATH];
	// --- the name of the file if extended info is available
	static TCHAR	szNewFileName[_MAX_PATH];
	// --- the name of the new file if extended info is available and action is rename
	
	FILE_NOTIFY_INFORMATION* pfnInfo=NULL;
	// -- pointer to a buffer with extended information (if available)

#endif // defined(_UNICODE) && (_WIN32_WINNT >= 0x400)

	COXFileWatchNotifier* pfwnNotifier;
	// --- the notifier object that will be posted/sended

	DWORD nCount=PtrToLong(pThis->m_arWatchedDirs.GetSize()+1);
	
	// Exit from the loop only when the StartStopEvent as signaled
	while(TRUE)
	{
		DWORD dwRetValue = WaitForMultipleObjects(nCount, pHandles, 
			FALSE, INFINITE);

		// Check for error
		if(dwRetValue == WAIT_FAILED || dwRetValue<WAIT_OBJECT_0 || 
			dwRetValue > (WAIT_OBJECT_0 + nCount - 1))
		{
			pThis->m_hrError = HRESULT_FROM_WIN32(::GetLastError());
			break;
		}
		else  
		{
			// To stop the thread or a notification is signaled
			if(dwRetValue == WAIT_OBJECT_0)
			{
				VERIFY(pThis->m_mxThreadShouldStop.Unlock());
				// Informs the main thread that we are waiting
//				VERIFY(pThis->m_mxThreadStopped.Unlock());
				pThis->m_mxThreadStopped.Unlock();
				// Wait for the main thread
				VERIFY(pThis->m_mxThreadCanContinue.Lock());
				VERIFY(pThis->m_mxThreadStopped.Lock());
				VERIFY(pThis->m_mxThreadCanContinue.Unlock());

				// In this point the main thread should prepare the handles

				if(pThis->m_pNewHandles==NULL) 
				{
					break; // Stops the thread
				}

				// Set new handles
				pHandles=pThis->m_pNewHandles;
				nCount = PtrToLong(pThis->m_arWatchedDirs.GetSize()+1);

				continue;
			}
			else
			{ 
				// ... OK. New notification

				//if notification comes in the period of the time
				//that this thread is to be deleted, so do not
				//process the notification.
				if (pThis->m_bDeletingThread)
					break;
				
				// Find the directory entry
				int nSignaledPath=dwRetValue-WAIT_OBJECT_0-1;
				
				COXWatchedDir* pwdPath = (pThis->m_arWatchedDirs)[nSignaledPath];

#if defined(_UNICODE) && (_WIN32_WINNT >= 0x400)
// Extended info is supported
				DWORD dwNumberOfBytesTransferred;
				BOOL bValid;

				if(pwdPath->m_bExtended)
				{
					// ... OK. We have extended info. Take a pointer to it.
					pfnInfo=(FILE_NOTIFY_INFORMATION*)pwdPath->m_lpBuffer;

					// ... we have to check if the notification is valid or not
					// The check must be done before FindNextHandle()
					bValid= ::GetOverlappedResult(pwdPath->m_hDirectory,      // handle of the directory
								  				  pwdPath->m_pOverlapped,	  // address of overlapped structure 
 												  &dwNumberOfBytesTransferred, // address of actual bytes count 
												  FALSE);					  /// wait flag 

					if(bValid == FALSE || dwNumberOfBytesTransferred == 0) 
					{
						// Something is wrong
#ifdef _DEBUG
						if(bValid == FALSE)
						{
							pThis->m_hrError = HRESULT_FROM_WIN32(::GetLastError());
							CONDITIONAL_TRACE_RESULT("COXFileWatcher::FileWatchThreadFunction", pThis->m_hrError)
						}
						else // dwNumberOfBytesTransferred == 0
						{
							TRACE(_T("\nThe buffer passed to ReadDirectoryChangesW() is too small!\n"));
						}
#endif // _DEBUG

						// ... update the handle
						if(!pwdPath->FindNextHandle((pThis->m_pHandles)[nSignaledPath+1]))
						{
							pThis->m_hrError=
								HRESULT_FROM_WIN32(::GetLastError());
							break;
						}

						// The extended information is available, but it is not valid
						continue;
					}
				}

				while(TRUE)
				{
					if(pwdPath->m_bExtended)
					{
						// Extended information is available
						memcpy(szFileName,(LPCTSTR)&pfnInfo->FileName[0], 
							pfnInfo->FileNameLength);
						szFileName[pfnInfo->
							FileNameLength/sizeof(TCHAR)]=_T('\0');

						if(pfnInfo->Action == FILE_ACTION_RENAMED_OLD_NAME) 
						{
							pfnInfo=(FILE_NOTIFY_INFORMATION*)(((char*)pfnInfo)+
								pfnInfo->NextEntryOffset);
							memcpy(szNewFileName, (LPCTSTR)&pfnInfo->FileName[0],
								pfnInfo->FileNameLength);
							szNewFileName[pfnInfo->
								FileNameLength/sizeof(TCHAR)]=_T('\0');
						}
						else
						{
							szNewFileName[0]=_T('\0');
						}
						pfwnNotifier= new COXFileWatchNotifier(pwdPath->m_sPath,
							pwdPath->m_bWatchSubTree,pwdPath->m_dwWatchFilter, 
							TRUE,pfnInfo->Action,szFileName,szNewFileName);
					}
					else
#endif // defined(_UNICODE) && (_WIN32_WINNT >= 0x400)
					{
						// ... there is no extended information
						pfwnNotifier= new COXFileWatchNotifier(pwdPath->m_sPath,
							pwdPath->m_bWatchSubTree,pwdPath->m_dwWatchFilter, 
							FALSE,0,NULL,NULL);
					}

					// Notify the derived class or ...
					if(!pThis->OnNotify(*pfwnNotifier) && 
						pwdPath->m_hwndWindowToNotify!=NULL)
					{
						// ... post/send message
						UINT nItemIndex=pThis->m_arFileWatchNotifiers.
							AddItem(pfwnNotifier);
						if(pwdPath->m_bPost)
						{
							::PostMessage(pwdPath->m_hwndWindowToNotify, 
								WM_OX_FILE_NOTIFY,nItemIndex,LPARAM(0));
						}
						else
						{
							::SendMessage(pwdPath->m_hwndWindowToNotify, 
								WM_OX_FILE_NOTIFY,nItemIndex,LPARAM(0));
						}
					}
					else 
					{
						// the notifier object is not queued so delete it
						delete pfwnNotifier;
					}

#if defined(_UNICODE) && (_WIN32_WINNT >= 0x400)
// Extended info is supported
					if(!pwdPath->m_bExtended || pfnInfo->NextEntryOffset == 0) 
					{
						break;
					}
					else
					{
						// ... go to the next entry in the buffer...
						pfnInfo = (FILE_NOTIFY_INFORMATION*)(((char*)pfnInfo)+
							pfnInfo->NextEntryOffset);
					}
				} // while(TRUE);

#endif // defined(_UNICODE) && (_WIN32_WINNT >= 0x400)

				// ... update the handle
				if(!pwdPath->FindNextHandle((pThis->m_pHandles)[nSignaledPath+1]))
				{
					pThis->m_hrError = HRESULT_FROM_WIN32(::GetLastError());
					break;
				}

			}
		}
	}

	// The thread is stopping
	pThis->m_bThreadIsRunning = FALSE;
	return 0;
}

void COXFileWatcher::StopThread()
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Stops the thread
{
	if(!m_bThreadIsRunning) 
		return;

	VERIFY(m_mxThreadCanContinue.Lock());
	// Informs the watcher thread that it can change the handles
	// and waits for the watcher thread to stop
	VERIFY(m_mxThreadShouldStop.Unlock());
	// wait until the watcher thread stops
//	VERIFY(m_mxThreadStopped.Lock());
	m_mxThreadStopped.Lock();

	// The watcher thread will stop if there are no more handles to watch
	m_pNewHandles = NULL;

	VERIFY(m_mxThreadStopped.Unlock());
	VERIFY(m_mxThreadShouldStop.Lock());
	// notify watcher thread that it can go on executing
	VERIFY(m_mxThreadCanContinue.Unlock());

	// Wait for the watcher thread to stops
	::WaitForSingleObject(m_pWatcherThread->m_hThread, INFINITE);

	delete m_pWatcherThread;
	m_pWatcherThread = NULL;
}

// private:


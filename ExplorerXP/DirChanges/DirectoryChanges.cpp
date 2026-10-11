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


#include "stdafx.h"
#include "DirectoryChanges.h"
#include "DelayedDirectoryChangeHandler.h"
#include "PrivilegeEnabler.h"
#include "FileNotifyInformation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//#define TRACE_FUN TRACE(_T("Entering function: %s\n"), __FUNCTIONW__);
#define TRACE_FUN ;

static bool IsDirectory(const CString & strPath)
{
	DWORD dwAttrib	= GetFileAttributes( strPath );
	return static_cast<bool>( ( dwAttrib != 0xffffffff 
							&&	(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) );		
}

CDirectoryChangeHandler::CDirectoryChangeHandler()
: m_nRefCnt( 1 ),
  m_pDirChangeWatcher( NULL ),
  m_nWatcherRefCnt( 0L )
{
}

CDirectoryChangeHandler::~CDirectoryChangeHandler()
{
  TRACE_FUN;
  UnwatchDirectory();
}

long CDirectoryChangeHandler::AddRef()
{ 
	return InterlockedIncrement(&m_nRefCnt);	
}

long CDirectoryChangeHandler::Release()
{  
	long nRefCnt = -1;
	if( (nRefCnt = InterlockedDecrement(&m_nRefCnt)) == 0 )
		delete this;
	return nRefCnt;
}
long CDirectoryChangeHandler::CurRefCnt()const 
{ 
	return m_nRefCnt;
}

BOOL CDirectoryChangeHandler::UnwatchDirectory() {
  TRACE_FUN;
  CSingleLock lock(&m_csWatcher, TRUE);
  ASSERT(lock.IsLocked());

  if (m_pDirChangeWatcher) return m_pDirChangeWatcher->UnwatchDirectory(this);
  return TRUE;
}


long  CDirectoryChangeHandler::ReferencesWatcher(CDirectoryChangeWatcher * pDirChangeWatcher)
{
	ASSERT( pDirChangeWatcher );
	CSingleLock lock(&m_csWatcher, TRUE);
	if( m_pDirChangeWatcher 
	&&  m_pDirChangeWatcher != pDirChangeWatcher )
	{
		TRACE(_T("CDirectoryChangeHandler...is becoming used by a different CDirectoryChangeWatcher!\n"));
		TRACE(_T("Directories being handled by this object will now be unwatched.\nThis object is now being used to ")
			  _T("handle changes to a directory watched by different CDirectoryChangeWatcher object, probably on a different directory"));
		
		if( UnwatchDirectory() )
		{
			m_pDirChangeWatcher = pDirChangeWatcher;
			m_nWatcherRefCnt = 1; //when this reaches 0, set m_pDirChangeWatcher to NULL
			return m_nWatcherRefCnt;
		}
		else
		{
			ASSERT( FALSE );//shouldn't get here!
		}
	}
	else
	{
		ASSERT( !m_pDirChangeWatcher || m_pDirChangeWatcher == pDirChangeWatcher );
		
		m_pDirChangeWatcher = pDirChangeWatcher;	
		
		if( m_pDirChangeWatcher )
			return InterlockedIncrement(&m_nWatcherRefCnt);
		
	}
	return m_nWatcherRefCnt;
}

long CDirectoryChangeHandler::ReleaseReferenceToWatcher(CDirectoryChangeWatcher * pDirChangeWatcher)
{
	ASSERT( m_pDirChangeWatcher == pDirChangeWatcher );
	CSingleLock lock(&m_csWatcher, TRUE);
	long nRef;
	if( (nRef = InterlockedDecrement(&m_nWatcherRefCnt)) <= 0L )
	{
		m_pDirChangeWatcher = NULL; //Setting this to NULL so that this->UnwatchDirectory() which is called in the dtor
									//won't call m_pDirChangeWatcher->UnwatchDirecotry(this).
									//m_pDirChangeWatcher may point to a destructed object depending on how
									//these classes are being used.
		m_nWatcherRefCnt = 0L;
	}
	return nRef;
}


void CDirectoryChangeHandler::On_FileAdded(const CString & strFileName)
{ 
	TRACE(_T("The following file was added: %s\n"), static_cast<LPCWSTR>(strFileName));
}

void CDirectoryChangeHandler::On_FileRemoved(const CString & strFileName)
{
	TRACE(_T("The following file was removed: %s\n"), static_cast<LPCWSTR>(strFileName));
}

void CDirectoryChangeHandler::On_FileModified(const CString & strFileName)
{
	TRACE(_T("The following file was modified: %s\n"), static_cast<LPCWSTR>(strFileName));
}

void CDirectoryChangeHandler::On_FileNameChanged(const CString & strOldFileName, const CString & strNewFileName)
{
	TRACE(_T("The file %s was RENAMED to %s\n"), static_cast<LPCWSTR>(strOldFileName), static_cast<LPCWSTR>(strNewFileName));
}
void CDirectoryChangeHandler::On_ReadDirectoryChangesError(DWORD dwError, const CString & strDirectoryName)
{
	TRACE(_T("WARNING!!!!!\n") );
	TRACE(_T("An error has occurred on a watched directory!\n"));
	TRACE(_T("This directory has become unwatched! -- %s \n"), static_cast<LPCWSTR>(strDirectoryName));
	TRACE(_T("ReadDirectoryChangesW has failed! %d"), dwError);
	ASSERT( FALSE );//you should override this function no matter what. an error will occur someday.
}

void CDirectoryChangeHandler::On_WatchStarted(DWORD dwError, const CString & strDirectoryName)
{	
	if( dwError == 0 )
		TRACE(_T("A watch has begun on the following directory: %s\n"), static_cast<LPCWSTR>(strDirectoryName));
	else
		TRACE(_T("A watch failed to start on the following directory: (Error: %d) %s\n"),dwError, static_cast<LPCWSTR>(strDirectoryName));
}

void CDirectoryChangeHandler::On_WatchStopped(const CString & strDirectoryName)
{
	TRACE(_T("The watch on the following directory has stopped: %s\n"), static_cast<LPCWSTR>(strDirectoryName));
}

bool CDirectoryChangeHandler::On_FilterNotification(DWORD /*dwNotifyAction*/, LPCTSTR /*szFileName*/, LPCTSTR /*szNewFileName*/)
//
//	bool On_FilterNotification(DWORD dwNotifyAction, LPCTSTR szFileName, LPCTSTR szNewFileName);
//
//	This function gives your class a chance to filter unwanted notifications.
//
//	PARAMETERS: 
//			DWORD	dwNotifyAction	-- specifies the event to filter
//			LPCTSTR szFileName		-- specifies the name of the file for the event.
//			LPCTSTR szNewFileName	-- specifies the new file name of a file that has been renamed.
//
//	RETURN VALUE:
//			return true from this function, and you will receive the notification.
//			return false from this function, and your class will NOT receive the notification.
//
//	Valid values of dwNotifyAction:
//		FILE_ACTION_ADDED			-- On_FileAdded() is about to be called.
//		FILE_ACTION_REMOVED			-- On_FileRemoved() is about to be called.
//		FILE_ACTION_MODIFIED		-- On_FileModified() is about to be called.
//		FILE_ACTION_RENAMED_OLD_NAME-- On_FileNameChanged() is about to be call.
//
//	  
//	NOTE:  When the value of dwNotifyAction is FILE_ACTION_RENAMED_OLD_NAME,
//			szFileName will be the old name of the file, and szNewFileName will
//			be the new name of the renamed file.
//
//  The default implementation always returns true, indicating that all notifications will 
//	be sent.
//	
{
	return true;
}

void CDirectoryChangeHandler::SetChangedDirectoryName(const CString & strChangedDirName)
{
	m_strChangedDirectoryName = strChangedDirName;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

CDirectoryChangeWatcher::CDirectoryChangeWatcher(bool bAppHasGUI /*= true*/, DWORD dwFilterFlags/*=FILTERS_CHECK_FILE_NAME_ONLY*/)
: m_hCompPort( NULL )
 ,m_hThread( NULL )
 ,m_dwThreadID( 0UL )
 ,m_bAppHasGUI( bAppHasGUI )
 ,m_dwFilterFlags( dwFilterFlags == 0? FILTERS_DEFAULT_BEHAVIOR : dwFilterFlags)
{
	//NOTE:  
	//	The bAppHasGUI variable indicates that you have a message pump associated
	//	with the main thread(or the thread that first calls CDirectoryChangeWatcher::WatchDirectory() ).
	//	Directory change notifications are dispatched to your main thread.
	//	
	//	If your app doesn't have a gui, then pass false.  Doing so causes a worker thread
	//	to be created that implements a message pump where it dispatches/executes the notifications.
	//  It's ok to pass false even if your app does have a GUI.
	//	Passing false is required for Console applications, or applications without a message pump.
	//	Note that notifications are fired in a worker thread.
	//

	//NOTE:
	//
	//
}

CDirectoryChangeWatcher::~CDirectoryChangeWatcher()
{
    TRACE_FUN;
	UnwatchAllDirectories();

	if( m_hCompPort )
	{
		CloseHandle( m_hCompPort );
		m_hCompPort = NULL;
	}
}

DWORD CDirectoryChangeWatcher::SetFilterFlags(DWORD dwFilterFlags)
//
//	SetFilterFlags()
//	
//	sets filter behavior for directories watched AFTER this function has been called.
//
//
//
{
	DWORD dwOld = m_dwFilterFlags;
	m_dwFilterFlags = dwFilterFlags;
	if( m_dwFilterFlags == 0 )
		m_dwFilterFlags = FILTERS_DEFAULT_BEHAVIOR;//the default.
	return dwOld;
}

BOOL CDirectoryChangeWatcher::IsWatchingDirectory(const CString & strDirName)const
/*********************************************
  Determines whether or not a directory is being watched

  be carefull that you have the same name path name, including the backslash
  as was used in the call to WatchDirectory().

	  ie:	
			"C:\\Temp"
		is different than
			"C:\\Temp\\"
**********************************************/
{
	CSingleLock lock( const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	ASSERT( lock.IsLocked() );
	int i;
	if( GetDirWatchInfo(strDirName, i) )
		return TRUE;
	return FALSE;
}

int	CDirectoryChangeWatcher::NumWatchedDirectories()const
{
	CSingleLock lock(const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	ASSERT( lock.IsLocked() );
	int nCnt(0),max = static_cast<int>(m_DirectoriesToWatch.GetSize());
	for(int i(0); i < max; ++i)
	{
		if( m_DirectoriesToWatch[i] != NULL )//array may contain NULL elements.
			nCnt++;
	}

	return nCnt;
}
DWORD CDirectoryChangeWatcher::WatchDirectory(
    const CString& strDirToWatch, DWORD dwChangesToWatchFor,
    CDirectoryChangeHandler* pChangeHandler, BOOL bWatchSubDirs /*=FALSE*/,
    LPCTSTR szIncludeFilter /*=NULL*/, LPCTSTR szExcludeFilter /*=NULL*/
) {
  ASSERT(dwChangesToWatchFor != 0);

  if (strDirToWatch.IsEmpty() || dwChangesToWatchFor == 0 ||
      pChangeHandler == NULL) {
    TRACE(_T("ERROR: Invalid parameters passed to WatchDirectory()\n"));
    ::SetLastError(ERROR_INVALID_PARAMETER);
    return ERROR_INVALID_PARAMETER;
  }

  if (!IsDirectory(strDirToWatch)) {
    TRACE(_T("ERROR: WatchDirectory() -- %s is not a directory!\n"),
          (LPCTSTR)strDirToWatch);
    ::SetLastError(ERROR_BAD_PATHNAME);
    return ERROR_BAD_PATHNAME;
  }

  if (IsWatchingDirectory(strDirToWatch)) {
    VERIFY(UnwatchDirectory(strDirToWatch));
  }

  CPrivilegeEnabler::Instance();

  HANDLE hDir = CreateFile(
      strDirToWatch, FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
      OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  if (hDir == INVALID_HANDLE_VALUE) {
    DWORD dwError = GetLastError();
    TRACE(_T("WatchDirectory() -- Couldn't open directory. Error: %d\n"),
          dwError);
    ::SetLastError(dwError);
    return dwError;
  }

  CDirWatchInfo* pDirInfo = new CDirWatchInfo(
      hDir, strDirToWatch, pChangeHandler, dwChangesToWatchFor, bWatchSubDirs,
      m_bAppHasGUI, szIncludeFilter, szExcludeFilter, m_dwFilterFlags);

  if (!pDirInfo) {
    CloseHandle(hDir);
    ::SetLastError(ERROR_OUTOFMEMORY);
    return ERROR_OUTOFMEMORY;
  }

  // 64-bit safe completion key cast (ULONG_PTR)
  m_hCompPort =
      CreateIoCompletionPort(hDir, m_hCompPort, (ULONG_PTR)pDirInfo, 0);

  if (m_hCompPort == NULL) {
    DWORD dwError = GetLastError();
    pDirInfo->DeleteSelf(NULL);
    ::SetLastError(dwError);
    return dwError;
  }

  // Start thread if not already running
  if (m_hThread == NULL) {
    CWinThread* pThread =
        AfxBeginThread(MonitorDirectoryChanges, this, THREAD_PRIORITY_LOWEST, 0,
                       CREATE_SUSPENDED);

    if (!pThread) {
      pDirInfo->DeleteSelf(NULL);
      ::SetLastError(ERROR_MAX_THRDS_REACHED);
      return ERROR_MAX_THRDS_REACHED;
    }

    // Prevent auto-delete race condition
    pThread->m_bAutoDelete = FALSE;
    m_dwThreadID = pThread->m_nThreadID;

    // Duplicate handle to maintain ownership
    BOOL bDup = ::DuplicateHandle(::GetCurrentProcess(), pThread->m_hThread,
                                  ::GetCurrentProcess(), &m_hThread, 0, FALSE,
                                  DUPLICATE_SAME_ACCESS);

    if (!bDup) {
      m_hThread = pThread->m_hThread;
    }

    pThread->ResumeThread();
  }

  DWORD dwStarted = pDirInfo->StartMonitor(m_hCompPort);

  if (dwStarted != ERROR_SUCCESS) {
    pDirInfo->DeleteSelf(NULL);
    ::SetLastError(dwStarted);
    return dwStarted;
  }

  pChangeHandler->ReferencesWatcher(this);
  VERIFY(AddToWatchInfo(pDirInfo));
  ::SetLastError(ERROR_SUCCESS);
  return ERROR_SUCCESS;
}

BOOL CDirectoryChangeWatcher::UnwatchAllDirectories() {
  TRACE_FUN;
  HANDLE hThreadToWait = NULL;
  HANDLE hCompPortToClose = NULL;

  CSimpleArray<CDirWatchInfo*> itemsToUnwatch;

  {
    CSingleLock lock(&m_csDirWatchInfo, TRUE);
    if (m_hThread == NULL) return FALSE;

    INT_PTR count = m_DirectoriesToWatch.GetSize();
    for (INT_PTR i = 0; i < count; ++i) {
      CDirWatchInfo* pDirInfo = m_DirectoriesToWatch[i];
      if (pDirInfo != NULL) {
        itemsToUnwatch.Add(pDirInfo);
        m_DirectoriesToWatch.SetAt(i, NULL);
      }
    }
    m_DirectoriesToWatch.RemoveAll();

    hThreadToWait = m_hThread;
    hCompPortToClose = m_hCompPort;

    m_hThread = NULL;
    m_dwThreadID = 0;
  }  // Release m_csDirWatchInfo lock HERE (keep m_hCompPort valid while thread finishes)

  // Mark all directories stopped and close handles to abort any pending ReadDirectoryChangesW
  int nItems = itemsToUnwatch.GetSize();
  for (int i = 0; i < nItems; ++i) {
    CDirWatchInfo* pDirInfo = itemsToUnwatch[i];
    if (pDirInfo != NULL) {
      pDirInfo->LockProperties();
      pDirInfo->m_RunningState = CDirWatchInfo::RUNNING_STATE_STOPPED;
      pDirInfo->CloseDirectoryHandle();
      pDirInfo->UnlockProperties();

      if (pDirInfo->GetChangeHandler()) {
        pDirInfo->GetChangeHandler()->On_WatchStopped(pDirInfo->m_strDirName);
      }
    }
  }

  // Signal worker thread to exit
  if (hCompPortToClose != NULL) {
    ::PostQueuedCompletionStatus(hCompPortToClose, 0, 0, NULL);
  }

  // Wait for worker thread to exit completely BEFORE destroying any CDirWatchInfo objects
  if (hThreadToWait != NULL) {
    ::WaitForSingleObject(hThreadToWait, INFINITE);
    ::CloseHandle(hThreadToWait);  // Close the duplicated handle
  }

  if (hCompPortToClose != NULL) {
    ::CloseHandle(hCompPortToClose);
    m_hCompPort = NULL;
  }

  // Delete all CDirWatchInfo objects now that worker thread is completely terminated
  for (int i = 0; i < nItems; ++i) {
    CDirWatchInfo* pDirInfo = itemsToUnwatch[i];
    if (pDirInfo != NULL) {
      pDirInfo->DeleteSelf(this);
    }
  }

  return TRUE;
}

BOOL CDirectoryChangeWatcher::UnwatchDirectory(
    const CString& strDirToStopWatching)

/***************************************************************
FUNCTION:	UnwatchDirectory(const CString & strDirToStopWatching)
- Stops watching the specified directory and safely destroys pDirInfo.
****************************************************************/
{
  TRACE_FUN;

  if (m_hCompPort == NULL || strDirToStopWatching.IsEmpty()) return FALSE;

  CDirWatchInfo* pDirInfo = NULL;

  {
    CSingleLock lock(&m_csDirWatchInfo, TRUE);
    ASSERT(lock.IsLocked());

    int nIdx = -1;
    pDirInfo = GetDirWatchInfo(strDirToStopWatching, nIdx);
    if (pDirInfo != NULL && nIdx != -1) {
      // Null out the slot in the watching list so no other thread can retrieve
      // it
      m_DirectoriesToWatch.SetAt(nIdx, NULL);
    } else {
      return FALSE;
    }
  }  // <-- Release m_csDirWatchInfo lock HERE

  // Signal worker thread to stop monitoring and WAIT for worker thread
  // handshake
  VERIFY(pDirInfo->UnwatchDirectory(m_hCompPort));

  // Now that worker thread is completely done with pDirInfo, safe to delete
  pDirInfo->DeleteSelf(this);

  return TRUE;
}

BOOL CDirectoryChangeWatcher::UnwatchDirectory(
    CDirectoryChangeHandler* pChangeHandler)
/************************************
Unwatches all directories associated with the specified pChangeHandler.
************************************/
{
  TRACE_FUN;
  if (pChangeHandler == NULL || m_hCompPort == NULL) return FALSE;

  CSimpleArray<CDirWatchInfo*> itemsToUnwatch;

  {
    CSingleLock lock(&m_csDirWatchInfo, TRUE);
    ASSERT(lock.IsLocked());

    int nIdx = -1;
    CDirWatchInfo* pDirInfo = NULL;

    // Extract all matching pDirInfo pointers and null out their array entries
    // first
    while ((pDirInfo = GetDirWatchInfo(pChangeHandler, nIdx)) != NULL) {
      itemsToUnwatch.Add(pDirInfo);
      m_DirectoriesToWatch.SetAt(nIdx, NULL);
    }
  }  // <-- Release m_csDirWatchInfo lock HERE

  // Process shutdown and deletion outside the critical section lock
  int nUnwatched = itemsToUnwatch.GetSize();
  for (int i = 0; i < nUnwatched; ++i) {
    CDirWatchInfo* pDirInfo = itemsToUnwatch[i];
    if (pDirInfo != NULL) {
      VERIFY(pDirInfo->UnwatchDirectory(m_hCompPort));
      pDirInfo->DeleteSelf(this);
    }
  }

  return (BOOL)(nUnwatched != 0);
}


BOOL CDirectoryChangeWatcher::UnwatchDirectoryBecauseOfError(CDirWatchInfo * pWatchInfo)
//
//	Called in the worker thread in the case that ReadDirectoryChangesW() fails
//	during normal operation. One way to force this to happen is to watch a folder
//	using a UNC path and changing that computer's IP address.
//	
{
	ASSERT( pWatchInfo );
	ASSERT( m_dwThreadID == GetCurrentThreadId() );//this should be called from the worker thread only.
	BOOL bRetVal = FALSE;
	if( pWatchInfo )
	{
		CSingleLock lock(&m_csDirWatchInfo, TRUE);
		
		ASSERT( lock.IsLocked() );
		int nIdx = -1;
		if( GetDirWatchInfo(pWatchInfo, nIdx) == pWatchInfo )
		{
			// we are actually watching this....

			//
			//	Remove this CDirWatchInfo object from the list of watched directories.
			//
			m_DirectoriesToWatch.SetAt(nIdx, NULL);//mark the space as free for the next watch...

			//
			//	and delete it...
			//

			pWatchInfo->DeleteSelf(this);
		
		}

	}
	return bRetVal;
}

int	CDirectoryChangeWatcher::AddToWatchInfo(CDirectoryChangeWatcher::CDirWatchInfo * pWatchInfo)
//
//	
//	To add the CDirWatchInfo  * to an array.
//	The array is used to keep track of which directories 
//	are being watched.
//
//	Add the ptr to the first non-null slot in the array.
{
	CSingleLock lock( &m_csDirWatchInfo, TRUE);
	ASSERT( lock.IsLocked() );
	
	//first try to add it to the first empty slot in m_DirectoriesToWatch
	int max = static_cast<int>(m_DirectoriesToWatch.GetSize());
	int i = 0;

	for( i = 0; i < max; ++i)
	{
		if( m_DirectoriesToWatch[i] == NULL )
		{
			m_DirectoriesToWatch[i] = pWatchInfo;
			break;
		}
	}
	if( i == max )
	{
		// there where no empty slots, add it to the end of the array
		//try{
			i = static_cast<int>(m_DirectoriesToWatch.Add( pWatchInfo ));
		/*}
		catch(CMemoryException * e){
			e->ReportError();
			e->Delete();//??? delete this? I thought CMemoryException objects where pre allocated in mfc? -- sample code in msdn does, so will i
			i = -1;
		}*/
	}

	return (BOOL)(i != -1);
}

//
//	functions for retrieving the directory watch info based on different parameters
//
CDirectoryChangeWatcher::CDirWatchInfo * CDirectoryChangeWatcher::GetDirWatchInfo(const CString & strDirName, int & ref_nIdx)const
{
	if( strDirName.IsEmpty() )// can't be watching a directory if it's you don't pass in the name of it...
		return FALSE;		  //
	
	CSingleLock lock(const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);

	int max = static_cast<int>(m_DirectoriesToWatch.GetSize());
	CDirWatchInfo * p = NULL;
	for(int i = 0; i < max; ++i )
	{
		if( (p = m_DirectoriesToWatch[i]) != NULL
		&&	p->m_strDirName.CompareNoCase( strDirName ) == 0 )
		{
			ref_nIdx = i;
			return p;
		}
	}
			
	return NULL;//NOT FOUND
}

CDirectoryChangeWatcher::CDirWatchInfo * CDirectoryChangeWatcher::GetDirWatchInfo(CDirectoryChangeWatcher::CDirWatchInfo * pWatchInfo, int & ref_nIdx)const
{
	ASSERT( pWatchInfo != NULL );

	CSingleLock lock( const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	int i(0), max = static_cast<int>(m_DirectoriesToWatch.GetSize());
	CDirWatchInfo * p;
	for(; i < max; ++i)
	{
		if( (p = m_DirectoriesToWatch[i]) != NULL
		&&	 p == pWatchInfo )
		{
			ref_nIdx = i;
			return p;
		}
	}
	return NULL;//NOT FOUND
}

CDirectoryChangeWatcher::CDirWatchInfo * CDirectoryChangeWatcher::GetDirWatchInfo(CDirectoryChangeHandler * pChangeHandler, int & ref_nIdx)const
{
	ASSERT( pChangeHandler != NULL );
	CSingleLock lock( const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	int i(0),max = static_cast<int>(m_DirectoriesToWatch.GetSize());
	CDirWatchInfo * p;
	for( ; i < max; ++i)
	{
		if( (p = m_DirectoriesToWatch[i]) != NULL
		&&	p->GetRealChangeHandler() == pChangeHandler )
		{
			ref_nIdx = i;
			return p;
		}
	}
	return NULL;//NOT FOUND
}

long CDirectoryChangeWatcher::ReleaseReferenceToWatcher(CDirectoryChangeHandler * pChangeHandler)
{
	ASSERT( pChangeHandler );
	return pChangeHandler->ReleaseReferenceToWatcher(this);
}

CDirectoryChangeWatcher::CDirWatchInfo::CDirWatchInfo(HANDLE hDir, 
													  const CString & strDirectoryName, 
													  CDirectoryChangeHandler * pChangeHandler,
													  DWORD dwChangeFilter, 
													  BOOL bWatchSubDir,
													  bool bAppHasGUI,
													  LPCTSTR szIncludeFilter,
													  LPCTSTR szExcludeFilter,
													  DWORD dwFilterFlags)
 :	m_pChangeHandler( NULL ), 
	m_hDir(hDir),
	m_dwChangeFilter( dwChangeFilter ),
	m_bWatchSubDir( bWatchSubDir ),
	m_strDirName( strDirectoryName ),
	m_dwBufLength(0),
	m_dwReadDirError(ERROR_SUCCESS),
	m_StartStopEvent(FALSE, TRUE), //NOT SIGNALLED, MANUAL RESET
	m_RunningState( RUNNING_STATE_NOT_SET )
{ 
	
	ASSERT( hDir != INVALID_HANDLE_VALUE 
		&& !strDirectoryName.IsEmpty() );
	
	//
	//	This object 'decorates' the pChangeHandler passed in
	//	so that notifications fire in the context a thread other than
	//	CDirectoryChangeWatcher::MonitorDirectoryChanges()
	//
	//	Supports the include and exclude filters
	//
	//
	m_pChangeHandler = new CDelayedDirectoryChangeHandler( pChangeHandler, bAppHasGUI, szIncludeFilter, szExcludeFilter, dwFilterFlags );
	if( m_pChangeHandler ) 
		m_pChangeHandler->SetPartialPathOffset( m_strDirName );//to support FILTERS_CHECK_PARTIAL_PATH..this won't change for the duration of the watch, so set it once... HERE!
	ASSERT( m_pChangeHandler );

	ASSERT( GetChangeHandler() );
	ASSERT( GetRealChangeHandler() );
	if( GetRealChangeHandler() )
		GetRealChangeHandler()->AddRef();
	
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
	//memset(m_Buffer, 0, sizeof(m_Buffer));
}

CDirectoryChangeWatcher::CDirWatchInfo::~CDirWatchInfo()
{
	if( GetChangeHandler() )
	{//If this call to CDirectoryChangeHandler::Release() causes m_pChangeHandler to delete itself,
		//the dtor for CDirectoryChangeHandler will call CDirectoryChangeWatcher::UnwatchDirectory( CDirectoryChangeHandler * ),
		//which will make try to delete this object again.
		//if m_pChangeHandler is NULL, it won't try to delete this object again...
		CDirectoryChangeHandler * pTmp = SetRealDirectoryChangeHandler( NULL );
		if( pTmp )
			pTmp->Release();
		else{
			ASSERT( FALSE );
		}
	}
	
	CloseDirectoryHandle();

	delete m_pChangeHandler;
	m_pChangeHandler = NULL;
	
}


void CDirectoryChangeWatcher::CDirWatchInfo::DeleteSelf(CDirectoryChangeWatcher * pWatcher)
//
//	There's a reason for this function!
//
//	the dtor is private to enforce that it is used.
//
//
//	pWatcher can be NULL only if CDirecotryChangeHandler::ReferencesWatcher() has NOT been called.
//	ie: in certain sections of WatchDirectory() it's ok to pass this w/ NULL, but no where else.
//
{
	//ASSERT( pWatcher != NULL );


	ASSERT( GetRealChangeHandler() );
	if( pWatcher )
	{
	//
	//
	//	Before this object is deleted, the CDirectoryChangeHandler object
	//	needs to release it's reference count to the CDirectoryChangeWatcher object.
	//	I might forget to do this since I getting rid of CDirWatchInfo objects
	//	in more than one place...hence the reason for this function.
	//
		pWatcher->ReleaseReferenceToWatcher( GetRealChangeHandler() );
	}
	
	delete this;
}

CDelayedDirectoryChangeHandler* CDirectoryChangeWatcher::CDirWatchInfo::GetChangeHandler() const 
{ 
	return m_pChangeHandler; 
}

CDirectoryChangeHandler * CDirectoryChangeWatcher::CDirWatchInfo::GetRealChangeHandler() const
//
//	The 'real' change handler is the CDirectoryChangeHandler object 
//	passed to CDirectoryChangeWatcher::WatchDirectory() -- this is the object
//	that really handles the changes.
//
{	
	ASSERT( m_pChangeHandler ); 
	return m_pChangeHandler->GetRealChangeHandler(); 
}

CDirectoryChangeHandler * CDirectoryChangeWatcher::CDirWatchInfo::SetRealDirectoryChangeHandler(CDirectoryChangeHandler * pChangeHandler)
//
//	Allows you to switch out, at run time, which object really handles the change notifications.
//
{
	CDirectoryChangeHandler * pOld = GetRealChangeHandler();
	m_pChangeHandler->GetRealChangeHandler() = pChangeHandler; 
	return pOld;
}

BOOL CDirectoryChangeWatcher::CDirWatchInfo::CloseDirectoryHandle() {
  // Atomically grab the handle and swap m_hDir to INVALID_HANDLE_VALUE in one
  // CPU operation. This prevents any other thread from trying to close the same
  // handle.
  HANDLE hDirToClose = (HANDLE)InterlockedExchangePointer(
      (PVOID*)&m_hDir, (PVOID)INVALID_HANDLE_VALUE);

  // Only the thread that successfully retrieved the valid handle will perform
  // CloseHandle
  if (hDirToClose != INVALID_HANDLE_VALUE && hDirToClose != NULL) {
    return ::CloseHandle(hDirToClose);
  }

  return TRUE;  // Already closed by another thread
}

DWORD CDirectoryChangeWatcher::CDirWatchInfo::StartMonitor(HANDLE hCompPort) {
  // Runtime check prevents static analyzer (C6387) warnings
  if (hCompPort == NULL || hCompPort == INVALID_HANDLE_VALUE) {
    TRACE(_T("ERROR: Invalid hCompPort handle passed to StartMonitor()\n"));
    return ERROR_INVALID_HANDLE;
  }

  // Guard properties during state transition
  VERIFY(LockProperties());

  m_RunningState = RUNNING_STATE_START_MONITORING;
  m_StartStopEvent.ResetEvent();

  // Static analysis now knows hCompPort is guaranteed non-null here
  BOOL bPosted = ::PostQueuedCompletionStatus(
      hCompPort, 0, reinterpret_cast<ULONG_PTR>(this), NULL);

  VERIFY(UnlockProperties());

  if (!bPosted) {
    DWORD dwError = ::GetLastError();
    TRACE(
        _T("ERROR: PostQueuedCompletionStatus failed in StartMonitor(). ")
        _T("Error: %d\n"),
        dwError);
    return dwError;
  }

  // Wait for the worker thread to call ReadDirectoryChangesW
  const DWORD dwTimeoutMs = 10000;  // 10-second safety timeout
  DWORD dwWait = ::WaitForSingleObject(m_StartStopEvent, dwTimeoutMs);

  if (dwWait == WAIT_TIMEOUT) {
    TRACE(_T("ERROR: Timeout waiting for worker thread in StartMonitor()\n"));
    return ERROR_TIMEOUT;
  } else if (dwWait != WAIT_OBJECT_0) {
    DWORD dwError = ::GetLastError();
    TRACE(
        _T("ERROR: WaitForSingleObject failed in StartMonitor(). Error: %d\n"),
        dwError);
    return dwError;
  }

  return m_dwReadDirError;
}


BOOL CDirectoryChangeWatcher::CDirWatchInfo::UnwatchDirectory(HANDLE hCompPort)


/*******************************************

    Sets the running state of the object to stop monitoring a directory,
	Causes the worker thread to wake up and to stop monitoring the dierctory
	
********************************************/
{
  TRACE_FUN;
	ASSERT( hCompPort );
	//
	// Signal that the worker thread is to stop watching the directory
	//
	if( SignalShutdown(hCompPort) )
	{
		//and wait for the thread to signal that it has shutdown
		return WaitForShutdown();

	}
	return FALSE;
}


BOOL CDirectoryChangeWatcher::CDirWatchInfo::SignalShutdown(HANDLE hCompPort) {
  // 1. Validate parameters for static analysis (C6387) and runtime safety
  if (hCompPort == NULL || hCompPort == INVALID_HANDLE_VALUE) {
    TRACE(_T("ERROR: Invalid hCompPort handle passed to SignalShutdown()\n"));
    ::SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  // 2. Lock properties briefly to update running state
  VERIFY(LockProperties());

  // Avoid re-signaling if already shutting down or terminated
  if (m_RunningState == RUNNING_STATE_STOP ||
      m_RunningState == RUNNING_STATE_STOP_STEP2 ||
      m_RunningState == RUNNING_STATE_STOPPED) {
    VERIFY(UnlockProperties());
    return TRUE;
  }

  m_RunningState = CDirectoryChangeWatcher::CDirWatchInfo::RUNNING_STATE_STOP;
  VERIFY(UnlockProperties());

  // Post completion packet to wake worker thread; the worker thread closes the directory handle
  // in RUNNING_STATE_STOP, triggering the cancellation of ReadDirectoryChangesW for STEP2.
  BOOL bRetVal = ::PostQueuedCompletionStatus(
      hCompPort, 0, reinterpret_cast<ULONG_PTR>(this), NULL);

  if (!bRetVal) {
    DWORD dwError = ::GetLastError();
    TRACE(
        _T("ERROR: PostQueuedCompletionStatus() failed in SignalShutdown()! ")
        _T("GetLastError(): %d\n"),
        dwError);
    CloseDirectoryHandle();
    ::SetLastError(dwError);
  }

  return bRetVal;
}

BOOL CDirectoryChangeWatcher::CDirWatchInfo::WaitForShutdown()
//
//	This is to be called some time after SignalShutdown().
//	
//
{
	ASSERT_VALID(&m_StartStopEvent);
	
	//Wait for the Worker thread to indicate that the watch has been stopped
	DWORD dwWait;
	bool bWMQuitReceived = false;
	do{
		dwWait	= MsgWaitForMultipleObjects(1, &m_StartStopEvent.m_hObject, FALSE, 5000, QS_ALLINPUT);//wait five seconds
		switch( dwWait )
		{
		case WAIT_OBJECT_0:
			//handle became signalled!
			break;
		case WAIT_OBJECT_0 + 1:
			{
				//This thread awoke due to sent/posted message
				//process the message Q
				//
				//	There is a message in this thread's queue, so 
				//	MsgWaitForMultipleObjects returned.
				//	Process those messages, and wait again.

				MSG msg;
				while( PeekMessage(&msg, NULL, 0,0, PM_REMOVE ) ) 
				{
					if( msg.message != WM_QUIT)
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else
					{
						/****
						This appears to be causing quite a lot of pain, to quote Mustafa.

						//it's the WM_QUIT message, put it back in the Q and
						// exit this function
						PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam );
						bWMQuitReceived = true;

						****/
						break;
					}
				}
			}break;
		case WAIT_TIMEOUT:
			{
				TRACE(_T("WARNING: Possible Deadlock detected! ThreadID: %d File: %s Line: %d\n"), GetCurrentThreadId(), _T(__FILE__), __LINE__);
			}break;
		}//end switch(dwWait)
	}while( dwWait != WAIT_OBJECT_0 && !bWMQuitReceived );
		
	
	
	ASSERT( dwWait == WAIT_OBJECT_0 || bWMQuitReceived);

	m_StartStopEvent.ResetEvent();
	
	return (BOOL) (dwWait == WAIT_OBJECT_0 || bWMQuitReceived);
}

UINT __cdecl CDirectoryChangeWatcher::MonitorDirectoryChanges(LPVOID lpvThis) {
  DWORD numBytes = 0;
  CDirWatchInfo* pdi = NULL;
  LPOVERLAPPED lpOverlapped = NULL;

  CDirectoryChangeWatcher* pThis =
      reinterpret_cast<CDirectoryChangeWatcher*>(lpvThis);
  ASSERT(pThis);

  HANDLE hCompPort = pThis->m_hCompPort;
  if (hCompPort == NULL) return 0;

  pThis->On_ThreadInitialize();

  while (true) {
    pdi = NULL;
    lpOverlapped = NULL;
    numBytes = 0;

    BOOL bSuccess =
        GetQueuedCompletionStatus(hCompPort, &numBytes,
                                  (PULONG_PTR)&pdi, &lpOverlapped, INFINITE);

    // NULL pdi signals thread shutdown via PostQueuedCompletionStatus(port, 0,
    // 0, NULL)
    if (pdi == NULL) {
      break;
    }

    // If port failed and no I/O was pending, port is closed or invalid
    if (!bSuccess && lpOverlapped == NULL) {
      break;
    }

    if (!AfxIsValidAddress(pdi,
                           sizeof(CDirectoryChangeWatcher::CDirWatchInfo))) {
      continue;
    }

    // Lock during state evaluation AND updates
    pdi->LockProperties();
    CDirWatchInfo::eRunningState Run_State = pdi->m_RunningState;

    switch (Run_State) {
      case CDirWatchInfo::RUNNING_STATE_START_MONITORING: {
        ZeroMemory(&pdi->m_Overlapped, sizeof(OVERLAPPED));
        if (!ReadDirectoryChangesW(
                pdi->m_hDir, pdi->m_Buffer, READ_DIR_CHANGE_BUFFER_SIZE,
                pdi->m_bWatchSubDir, pdi->m_dwChangeFilter, &pdi->m_dwBufLength,
                &pdi->m_Overlapped, NULL)) {
          pdi->m_dwReadDirError = GetLastError();
          pdi->UnlockProperties();

          if (pdi->GetChangeHandler())
            pdi->GetChangeHandler()->On_WatchStarted(pdi->m_dwReadDirError,
                                                     pdi->m_strDirName);
        } else {
          pdi->m_RunningState = CDirWatchInfo::RUNNING_STATE_NORMAL;
          pdi->m_dwReadDirError = ERROR_SUCCESS;
          pdi->UnlockProperties();

          if (pdi->GetChangeHandler())
            pdi->GetChangeHandler()->On_WatchStarted(ERROR_SUCCESS,
                                                     pdi->m_strDirName);
        }
        pdi->m_StartStopEvent.SetEvent();
      } break;

      case CDirWatchInfo::RUNNING_STATE_NORMAL: {
        pdi->UnlockProperties();  // Unlock while processing notifications

        if (!bSuccess || numBytes == 0) {
          DWORD dwErr = ::GetLastError();
          if (dwErr != ERROR_OPERATION_ABORTED && dwErr != ERROR_SUCCESS) {
            if (pdi->GetChangeHandler())
              pdi->GetChangeHandler()->On_ReadDirectoryChangesError(
                  dwErr, pdi->m_strDirName);

            pThis->UnwatchDirectoryBecauseOfError(pdi);
          }
          break;
        }

        if (pdi->GetChangeHandler())
          pdi->GetChangeHandler()->SetChangedDirectoryName(pdi->m_strDirName);

        DWORD dwReadBuffer_Offset = 0UL;
        CFileNotifyInformation notify_info((LPBYTE)pdi->m_Buffer,
                                           READ_DIR_CHANGE_BUFFER_SIZE);

        pThis->ProcessChangeNotifications(notify_info, pdi,
                                          dwReadBuffer_Offset);

        // Ensure DWORD (4-byte) alignment for ReadDirectoryChangesW buffer
        // offset
        dwReadBuffer_Offset = (dwReadBuffer_Offset + 3) & ~3;

        ZeroMemory(&pdi->m_Overlapped, sizeof(OVERLAPPED));

        if (!ReadDirectoryChangesW(
                pdi->m_hDir, pdi->m_Buffer + dwReadBuffer_Offset,
                READ_DIR_CHANGE_BUFFER_SIZE - dwReadBuffer_Offset,
                pdi->m_bWatchSubDir, pdi->m_dwChangeFilter, &pdi->m_dwBufLength,
                &pdi->m_Overlapped, NULL)) {
          DWORD dwErr = GetLastError();
          if (pdi->GetChangeHandler())
            pdi->GetChangeHandler()->On_ReadDirectoryChangesError(
                dwErr, pdi->m_strDirName);

          pThis->UnwatchDirectoryBecauseOfError(pdi);
        }
      } break;

      case CDirWatchInfo::RUNNING_STATE_STOP: {
        pdi->m_RunningState = CDirWatchInfo::RUNNING_STATE_STOP_STEP2;

        // Call CloseDirectoryHandle inside or outside the lock safely now
        BOOL bHadValidHandle =
            (pdi->m_hDir != INVALID_HANDLE_VALUE && pdi->m_hDir != NULL);
        pdi->CloseDirectoryHandle();

        pdi->UnlockProperties();

        if (bHadValidHandle) {
          if (pdi->GetChangeHandler())
            pdi->GetChangeHandler()->On_WatchStopped(pdi->m_strDirName);
        } else {
          pdi->m_StartStopEvent.SetEvent();
        }
      } break;

      case CDirWatchInfo::RUNNING_STATE_STOP_STEP2: {
        pdi->m_RunningState = CDirWatchInfo::RUNNING_STATE_STOPPED;
        pdi->UnlockProperties();
        pdi->m_StartStopEvent.SetEvent();
      } break;

      case CDirWatchInfo::RUNNING_STATE_STOPPED: {
        pdi->UnlockProperties();
      } break;

      default:
        TRACE(_T("CDirectoryChangeWatcher::MonitorDirectoryChanges() -- unexpected state %d\n"), Run_State);
        pdi->UnlockProperties();
        break;
    }
  }

  pThis->On_ThreadExit();
  return 0;
}

void CDirectoryChangeWatcher::ProcessChangeNotifications(IN CFileNotifyInformation & notify_info, 
														 IN CDirWatchInfo * pdi,
														 OUT DWORD & ref_dwReadBuffer_Offset//used in case ...see case for FILE_ACTION_RENAMED_OLD_NAME
														 )
/////////////////////////////////////////////////////////////
//
//	Processes the change notifications and dispatches the handling of the 
//	notifications to the CDirectoryChangeHandler object passed to WatchDirectory()
//
/////////////////////////////////////////////////////////////
{
	//
	//	Sanity check...
	//	this function should only be called by the worker thread.
	//	
	ASSERT( m_dwThreadID == GetCurrentThreadId() );

	//	Validate parameters...
	//	
	ASSERT( pdi );
	ASSERT( AfxIsValidAddress(pdi, sizeof(CDirectoryChangeWatcher::CDirWatchInfo) ) );

	if( !pdi || !AfxIsValidAddress(pdi, sizeof(CDirectoryChangeWatcher::CDirWatchInfo)) )
	{
		TRACE(_T("Invalid arguments to CDirectoryChangeWatcher::ProcessChangeNotifications() -- pdi is invalid!\n"));
		TRACE(_T("File: %s Line: %d"), _T( __FILE__ ), __LINE__ );
		return;
	}

	DWORD dwLastAction = 0;
	ref_dwReadBuffer_Offset = 0UL;
	

	CDirectoryChangeHandler * pChangeHandler = pdi->GetChangeHandler();
	//CDelayedDirectoryChangeHandler * pChangeHandler = pdi->GetChangeHandler();
	ASSERT( pChangeHandler );
	ASSERT( AfxIsValidAddress(pChangeHandler, sizeof(CDirectoryChangeHandler)) );
	//ASSERT( AfxIsValidAddress(pChangeHandler, sizeof(CDelayedDirectoryChangeHandler)) );
	if( !pChangeHandler )
	{
		TRACE(_T("CDirectoryChangeWatcher::ProcessChangeNotifications() Unable to continue, pdi->GetChangeHandler() returned NULL!\n"));
		TRACE(_T("File: %s  Line: %d\n"), _T( __FILE__ ), __LINE__ );
		return;
	}


	// Store state across iterations or as class member variables if split
        // across buffers:
        CString strPendingOldFileName;

        do {
          switch (notify_info.GetAction()) {
            case FILE_ACTION_ADDED:
              pChangeHandler->On_FileAdded(
                  notify_info.GetFileNameWithPath(pdi->m_strDirName));
              break;

            case FILE_ACTION_REMOVED:
              pChangeHandler->On_FileRemoved(
                  notify_info.GetFileNameWithPath(pdi->m_strDirName));
              break;

            case FILE_ACTION_MODIFIED:
              pChangeHandler->On_FileModified(
                  notify_info.GetFileNameWithPath(pdi->m_strDirName));
              break;

            case FILE_ACTION_RENAMED_OLD_NAME:
              // Store the old file name and wait for the NEW_NAME record in the
              // next iteration/buffer
              strPendingOldFileName =
                  notify_info.GetFileNameWithPath(pdi->m_strDirName);
              break;

            case FILE_ACTION_RENAMED_NEW_NAME: {
              CString strNewFileName =
                  notify_info.GetFileNameWithPath(pdi->m_strDirName);

              if (!strPendingOldFileName.IsEmpty()) {
                pChangeHandler->On_FileNameChanged(strPendingOldFileName,
                                                   strNewFileName);
                strPendingOldFileName.Empty();
              } else {
                // Fallback if OLD_NAME was lost due to buffer overflow
                pChangeHandler->On_FileAdded(strNewFileName);
              }
            } break;

            default:
              TRACE(
                  _T("CDirectoryChangeWatcher::ProcessChangeNotifications() ")
                  _T("-- unknown action: %d\n"),
                  notify_info.GetAction());
              break;
          }

          dwLastAction = notify_info.GetAction();

        } while (notify_info.GetNextNotifyInformation());
}
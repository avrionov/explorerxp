// ==========================================================================
// 					Class Specification : COXFileWatcher
// ==========================================================================

// Header file : OXFileWatcher.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CObject

//	NO	Is a CWnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This organizes file change notifications.
//  The AddWatch() function is used to request a certain directory to be watched.

//  The OnNotify() function can be overriden in derived classes in order to catch
//  notifications. Another way is also possible. Just supply a window 
//  handle in the function EnableWindowNotification() and this window will receive
//  the WM_OX_FILE_NOTIFY message. In this case the GetFileWatchNotifier() function
//  should be used to retrieve the notifier object

// Prerequisites (necessary conditions):
//	To use the extended capabilities of the file watcher
//	 define _UNICODE and define _WIN32_WINNT as 0x400.
//	This works only on WIndows NT and in Unicode versions.


/////////////////////////////////////////////////////////////////////////////

#ifndef __OXFILEWATCHER_H__
#define __OXFILEWATCHER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "OXDllExt.h"
/*
#ifndef __AFXTEMPL_H__
#include <AfxTempl.h>
#define __AFXTEMPL_H__
#endif
#ifndef __AFXMT_H__
#include <afxmt.h>
#define __AFXMT_H__
#endif*/

#include "OXWatchedDir.h"
#include "OXFileWatchNotifier.h"
//#include "OXIDManager.h"


/////////////////////////////////////////////////////////////////////////////
// COXFileWatcher:


class OX_CLASS_DECL COXFileWatcher : public CObject
{
// Data members -------------------------------------------------------------
public:             
	// Notification types ==>

	const static DWORD OXFileWatchChangeFileName;
	// --- Any filename change in the watched directory or subtree causes a change notification

	const static DWORD OXFileWatchChangeDirName;
	// --- Any directory-name change in the watched directory or subtree causes a change notification

	const static DWORD OXFileWatchChangeAttributes;
	// --- Any attribute change in the watched directory or subtree causes a change notification

	const static DWORD OXFileWatchChangeSize;
	// --- Any file-size change in the watched directory or subtree causes a change notification
	//     The operating system detects a change in file size only when the file is written to the disk.
	//     For operating systems that use extensive caching, detection occurs only when the cache is 
	//     sufficiently flushed

	const static DWORD OXFileWatchChangeLastWrite;
	// --- Any change to the last write-time of files in the watched directory or subtree causes a 
	//     change notification. The operating system detects a change to the last write-time only when 
	//     the file is written to the disk. For operating systems that use extensive caching, detection 
	//     occurs only when the cache is sufficiently flushed

	const static DWORD OXFileWatchChangeLastAccess;
	// --- Any change to the last access time of files in the watched directory or subtree causes
	//     a change notification

	const static DWORD OXFileWatchChangeCreation;
	// --- Any change to the creation time of files in the watched directory or subtree causes 
	//	   a change notification

	const static DWORD OXFileWatchChangeSecurity;
	// --- Any security-descriptor change in the watched directory or subtree causes a change notification

// <== End of Notification types

protected:
	CArray<COXWatchedDir*, COXWatchedDir*>	m_arWatchedDirs;
	COXIDManager<COXFileWatchNotifier>	m_arFileWatchNotifiers;

	HRESULT		m_hrError;
	BOOL		m_bThreadIsRunning;
	CWinThread*	m_pWatcherThread;

	CMutex		m_mxThreadStopped;
	CMutex		m_mxThreadShouldStop;
	CMutex		m_mxThreadCanContinue;

	CEvent		m_eventStartThread;

	HANDLE*		m_pHandles;
	HANDLE*		m_pNewHandles;
	WORD		m_nNumHandles;

	BOOL		m_bDeletingThread;

	CCriticalSection	m_crArrayWatchedDirGuard;

private:

// Member functions ---------------------------------------------------------
public:     
	// --- In  :	bAddBackupPrivilege	-	if TRUE then the running application 
	//										will be granted the "backup file and 
	//										directories" user right. It is required 
	//										if you need to watch for remote 
	//										directories
	// --- Out : 
	// --- Returns: 
	// --- Effect : Constructs the object. If this object is intented to watch for 
	//				remote directories then you need to set bAddBackupPrivilege
	//				to TRUE in order to guarantee that notifications will reach 
	//				this application (refer MSDN article 
	//				"http://support.microsoft.com/support/kb/articles/Q188/3/21.ASP"
	//				for details). Special thanks goes to Don Sanders who 
	//				suggested the fix for this problem
	COXFileWatcher(BOOL bAddBackupPrivilege=TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Object destructor
	virtual ~COXFileWatcher();


protected:
#if defined(_WIN32_WINNT)
	static BOOL g_bBackupPrivilegeGranted;
	static BOOL SetBackupPrivilege(BOOL bAdd);
#endif

public:
	BOOL AddWatch(LPCTSTR pszPath, BOOL bWatchSubtree = FALSE, DWORD dwWatchFilter = 
		OXFileWatchChangeFileName | OXFileWatchChangeLastWrite);
	// --- In  : pszPath: the path to be watched
	//			 bWatchSubtree: indicates whether to watch subtree or not
	//			 dwWatchFilter: specifys which type of changes should be reported
	//							For possible values see public data members Notification types
	// --- Out : 
	// --- Returns : FALSE: if fails
	// --- Effect : This function is used to request a certain directory to be watched

	HRESULT GetLastError() const;
	// --- In  :
	// --- Out : 
	// --- Returns : HRESULT of the last error of ERROR_SUCCESS if there is no error
	// --- Effect : This function returns the error code of the last error that has occurred

	BOOL IsExtendedInfoSupported() const;
	// --- In  :
	// --- Out : 
	// --- Returns : TRUE if extended information is supported
	// --- Effect : This function returns whether extended information is supported. 
	//				At the moment this is only supported on Windows NT 4.0

	void RemoveWatch(LPCTSTR pszPath);
	// --- In  : pszPath: directory to be removed from he watch list
	// --- Out : 
	// --- Returns :
	// --- Effect : Removes the watch on the given directory

	void RemoveAllWatches();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Removes all watches

	virtual BOOL OnNotify(COXFileWatchNotifier fileWatchNotifier);
	// --- In  : fileWatchNotifier: watch notifier object with information about 
	//			 file change notification
	// --- Out : 
	// --- Returns : FALSE if the WM_OX_FILE_NOTIFY message should be posted/sended
	// --- Effect : This function is called when a file change notification is 
	//				received by this file watch object

	void EnableWindowNotification(LPCTSTR pszPath, CWnd* pWnd, BOOL bPost = TRUE);
	// --- In  : pszPath: directory for witch the WM_OX_FILE_NOTIFY message should be
	//					  posted/sended
	//			 pWnd:	  window object to witch the message should be posted/sended
	//			 bPost:	  if TRUE the message should be posted, if FALSE the message 
	//					  should be sended
	// --- Out : 
	// --- Returns :
	// --- Effect : This function connects a future notification of a file watch to 
	//				a message to a particular window

	COXFileWatchNotifier GetFileWatchNotifier(DWORD fileWatchNotifierID);
	// --- In  : fileWatchNotifierID: ID of the retrieved notifier object
	// --- Out : 
	// --- Returns : The copy of the requested notifier object. This notifier object
	//				 will be removed from the COXFileWatcher. Any other requests for the
	//				 same ID will return other information.
	// --- Effect : This function returns a specific file watch notifier object

	void DisableWindowNotification(LPCTSTR pszPath);
	// --- In  : pszPath: directory for witch no more messages will be posted/sended
	// --- Out : 
	// --- Returns :
	// --- Effect : Remove the connection between the notification of a specific path and a window

	void Empty();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Clear the entire object (this will end all watched paths and all associated windows)

#ifdef _DEBUG
	virtual void AssertValid() ;
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : AssertValid performs a validity check on this object 
	//				by checking its internal state. 
	//				In the Debug version of the library, AssertValid may assert and 
	//				thus terminate the program.

	virtual void Dump(CDumpContext& dc) ;
	// --- In  : dc : The diagnostic dump context for dumping, usually afxDump.
	// --- Out : 
	// --- Returns :
	// --- Effect : Dumps the contents of the object to a CDumpContext object. 
	//				It provides diagnostic services for yourself and 
	//				 other users of your class. 

	static CString GetResultMessage(HRESULT hResult);
	// --- In  : hResult : The result code
	// --- Out : 
	// --- Returns : A string containing a message of the specified code
	// --- Effect : The GetResultMessage function can be used to obtain error message strings

#endif // _DEBUG

protected:      
	static UINT FileWatchThreadFunction(LPVOID pParam);
	void StopThread();

private:
};


#endif // __OXFILEWATCHER_H__
// ==========================================================================


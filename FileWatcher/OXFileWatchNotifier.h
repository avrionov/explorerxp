// ==========================================================================
// 					Class Specification : COXFileWatchNotifier
// ==========================================================================

// Header file : OXFileWatchNotifier.h

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

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :
//  This class encapsulates the file change notification parameters         

// Prerequisites (necessary conditions):

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXFILEWATCHNOTIFIER_H__
#define __OXFILEWATCHNOTIFIER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "OXDllExt.h"


class OX_CLASS_DECL COXFileWatchNotifier : public CObject
{
// Data members -------------------------------------------------------------
public:             

// Action codes (when extended info is avilable)
	static const DWORD OXFileWatchActionAdded;
	// --- The file was added to the directory

	static const DWORD OXFileWatchActionRemoved;
	// --- The file was removed from the directory

	static const DWORD OXFileWatchActionModified;
	// --- The file was modified. This can be a change in the time stamp or attributes

	static const DWORD OXFileWatchActionRename;
	// --- The file was renamed

protected:
	CString	m_sPath;
	BOOL	m_bWatchSubTree;
	DWORD	m_dwWatchFilter;
	DWORD	m_dwAction;

	CString	m_sFileName;
	CString m_sNewFileName;

	BOOL	m_bExtendedInfoAvailable;
	CTime	m_tActionTime;

private:

// Member functions ---------------------------------------------------------
public:     
	COXFileWatchNotifier();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs an empty object

	COXFileWatchNotifier( LPCTSTR pszPath, BOOL bWatchSubtree, DWORD dwWatchFilter, BOOL bExtendedInfo,
						  DWORD dwAction, LPCTSTR pszFileName, LPCTSTR pszNewFileName);
	// --- In  : pszPath: the path to be watched
	//			 bWatchSubtree: indicates whether to watch subtree or not
	//			 dwWatchFilter: specifys which type of changes should be reported
	//							For possible values see public data members Notification types
	//			 bExtendedInfo: Extended info is available
	//			 dwAction: The action code (when extended information is available)
	//			 pszFileName: The file name (when extended information is available)
	//			 pszNewFileName: The new file name (when extended information is available)
	//							 if the action is OXFileWatchActionRename
	// --- Out : 
	// --- Returns :
	// --- Effect :  Constructs the object

	COXFileWatchNotifier(const COXFileWatchNotifier& anOtherNotifier);
	// --- In  : anOtherNotifier: source object
	// --- Out : 
	// --- Returns :
	// --- Effect : copy constructor

	COXFileWatchNotifier& operator=(const COXFileWatchNotifier& anOtherNotifier);
	// --- In  : anOtherNotifier: source object
	// --- Out : 
	// --- Returns :
	// --- Effect : the assignment operator

	CTime	GetNotificationTime() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The time when the notifiaction was received
	// --- Effect : Returns the time when the notification was received

	BOOL	IsEmpty() const;
	// --- In  :
	// --- Out : 
	// --- Returns : If TRUE then the object does not contain valid information
	// --- Effect : Returns whether this is an empty object

	CString	GetPath() const;
	// --- In  :
	// --- Out : 
	// --- Returns : Watched path
	// --- Effect : Returns the original path specification that is being watched

	BOOL	GetWatchSubtree() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The subtree is watched if TRUE
	// --- Effect : Returns whether subtrees are being watched

	DWORD	GetWatchFilter() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The watch filter
	// --- Effect : Returns the watch filter that is being used

	BOOL	IsExtendedInfoAvailable() const;
	// --- In  :
	// --- Out : 
	// --- Returns : TRUE if extended info is available
	// --- Effect : Returns whether extended information (action code and possible 
	//				old and new file name) is available

	DWORD	GetAction() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The action code. For possible values see the public data members
	// --- Effect : Returns the action code (when extended information is available)

	CString	GetFileName() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The name of the file
	// --- Effect : Returns the file name (when extended information is available)

	CString	GetNewFileName() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The new file name
	// --- Effect : Returns the new file name (when extended information is available
	//				and action is OXFileWatchActionRename)


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
	//				Note  The Dump function does not print a newline character
	//				 at the end of its output. 

#endif

protected:      

private:
};


#endif // __OXFILEWATCHNOTIFIER_H__

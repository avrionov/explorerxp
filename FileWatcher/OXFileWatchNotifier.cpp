// ==========================================================================
//				Class Implementation : COXFileWatchNotifier
// ==========================================================================

// Source file : COXFileWatchNotifier.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXFileWatchNotifier.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


// Data members -------------------------------------------------------------
//public:             

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

	const DWORD COXFileWatchNotifier::OXFileWatchActionAdded	= 1;
	// --- The file was added to the directory

	const DWORD COXFileWatchNotifier::OXFileWatchActionRemoved	= 2;
	// --- The file was removed from the directory

	const DWORD COXFileWatchNotifier::OXFileWatchActionModified	= 4;
	// --- The file was modified. This can be a change in the time stamp or attributes

	const DWORD COXFileWatchNotifier::OXFileWatchActionRename	= 8;
	// --- The file was renamed

//protected:
	// CString	m_sPath;
	// --- Path to be watched
	//			 bExtendedInfo: Extended info is available
	//			 dwAction: The action code (when extended information is available)
	//			 pszFileName: The file name (when extended information is available)
	//			 pszNewFileName: The new file name (when extended information is available)
	//							 if the action is OXFileWatchActionRename

	// BOOL		m_bWatchSubTree;
	// --- Indicates whether to watch subtree or not

	// DWORD	m_dwWatchFilter;
	// --- Specifys which type of changes should be reported

	// DWORD	m_dwAction;
	// --- The action code (when extended information is available)

	// CString	m_sFileName;
	// --- The file name (when extended information is available)
	
	// CString  m_sNewFileName;
	// --- The new file name (when extended information is available)
	//	   if the action is OXFileWatchActionRename

	// BOOL	    m_bExtendedInfoAvailable;
	// --- Extended info is available
	
	// CTime	m_tActionTime;
	// --- The time when the notifiaction was received

//private:

// Member functions ---------------------------------------------------------
// public:

COXFileWatchNotifier::COXFileWatchNotifier()
	:m_sPath(					_T("")),
	 m_bWatchSubTree(			FALSE),
	 m_dwWatchFilter(			0),
	 m_bExtendedInfoAvailable(	FALSE),
	 m_dwAction(				0),
	 m_sFileName(				_T("")),
	 m_sNewFileName(			_T("")),
	 m_tActionTime(				CTime::GetCurrentTime())
	{
	}

COXFileWatchNotifier::COXFileWatchNotifier(LPCTSTR pszPath, 
										    BOOL    bWatchSubTree, 
											DWORD   dwWatchFilter, 
										    BOOL    bExtendedInfo,
											DWORD   dwAction,  
											LPCTSTR pszFileName, 
											LPCTSTR pszNewFileName)
	:m_sPath(					pszPath),
	 m_bWatchSubTree(			bWatchSubTree),
	 m_dwWatchFilter(			dwWatchFilter),
	 m_bExtendedInfoAvailable(	bExtendedInfo),
	 m_dwAction(				0),
	 m_sFileName(				pszFileName),
	 m_sNewFileName(			pszNewFileName),
	 m_tActionTime(				CTime::GetCurrentTime())
	{
	switch(dwAction)
		{
		case FILE_ACTION_ADDED:				m_dwAction = OXFileWatchActionAdded;
			break;
		case FILE_ACTION_REMOVED:			m_dwAction = OXFileWatchActionRemoved;
			break;
		case FILE_ACTION_MODIFIED:			m_dwAction = OXFileWatchActionModified;
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
		case FILE_ACTION_RENAMED_NEW_NAME:	m_dwAction = OXFileWatchActionRename;
			break;
		}
	}

COXFileWatchNotifier::COXFileWatchNotifier(const COXFileWatchNotifier& anOtherNotifier)
	:m_sPath(					anOtherNotifier.m_sPath),
	 m_bWatchSubTree(			anOtherNotifier.m_bWatchSubTree),
	 m_dwWatchFilter(			anOtherNotifier.m_dwWatchFilter),
	 m_dwAction(				anOtherNotifier.m_dwAction),
	 m_sFileName(				anOtherNotifier.m_sFileName),
	 m_sNewFileName(			anOtherNotifier.m_sNewFileName),
	 m_bExtendedInfoAvailable(	anOtherNotifier.m_bExtendedInfoAvailable),
	 m_tActionTime(				anOtherNotifier.m_tActionTime)
{
}

COXFileWatchNotifier& COXFileWatchNotifier::operator=(const COXFileWatchNotifier& anOtherNotifier)
{
	if(this==&anOtherNotifier)
		return *this;
		
	m_sPath						= anOtherNotifier.m_sPath;
	m_bWatchSubTree				= anOtherNotifier.m_bWatchSubTree;
	m_dwWatchFilter				= anOtherNotifier.m_dwWatchFilter;
	m_dwAction					= anOtherNotifier.m_dwAction;
	m_sFileName					= anOtherNotifier.m_sFileName;
	m_sNewFileName				= anOtherNotifier.m_sNewFileName;
	m_bExtendedInfoAvailable	= anOtherNotifier.m_bExtendedInfoAvailable;
	m_tActionTime				= anOtherNotifier.m_tActionTime;

	return *this;
}

CTime	COXFileWatchNotifier::GetNotificationTime() const
	{
	return m_tActionTime;
	}

BOOL	COXFileWatchNotifier::IsEmpty() const
	{
	return m_sPath==_T("");
	}

CString	COXFileWatchNotifier::GetPath() const
	{
	return m_sPath;
	}

BOOL	COXFileWatchNotifier::GetWatchSubtree() const
	{
	return m_bWatchSubTree;
	}

DWORD	COXFileWatchNotifier::GetWatchFilter() const
	{
	return m_dwWatchFilter;
	}

BOOL	COXFileWatchNotifier::IsExtendedInfoAvailable() const
	{
	return m_bExtendedInfoAvailable;
	}

DWORD	COXFileWatchNotifier::GetAction() const
	{
	return m_dwAction;
	}

CString	COXFileWatchNotifier::GetFileName() const
	{
	return m_sFileName;
	}

CString	COXFileWatchNotifier::GetNewFileName() const
	{
	return m_sNewFileName;
	}

#ifdef _DEBUG
void 	COXFileWatchNotifier::AssertValid()
	{
	CObject::AssertValid();
	}

void 	COXFileWatchNotifier::Dump(CDumpContext& dc)
	{
	CObject::Dump(dc);

	dc << _T("\nPath: ") << m_sPath;
	dc << _T("\nSubTree: ") << ((m_bWatchSubTree)?_T("YES"):_T("NO"));
	dc << _T("\nFilter: ") << m_dwWatchFilter;
	dc << _T("\nTime: ") << m_tActionTime.Format(_T("%H:%M:%S"));
	dc << _T("\nExtInfo: ") << ((m_bExtendedInfoAvailable)?_T("YES"):_T("NO"));

	if(m_bExtendedInfoAvailable)
		{
		dc << _T("\nAction: ") << m_dwAction;
		dc << _T("\nFile Name: ") << m_sFileName;
		dc << _T("\nNew File Name: ") << m_sNewFileName;
		}
	}

#endif

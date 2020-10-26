// ==========================================================================
// 					Class Specification : COXWatchedDir
// ==========================================================================

// Header file : OXWatchedDir.h

// Version: 9.3

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
//	A helper class used to store information about the watched directories

// Prerequisites (necessary conditions):


/////////////////////////////////////////////////////////////////////////////

#ifndef __OXWATCHEDDIR_H__
#define __OXWATCHEDDIR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "OXDllExt.h"
#define OX_CLASS_DECL

class OX_CLASS_DECL COXWatchedDir : public CObject
{
friend class COXFileWatcher;

// Data members -------------------------------------------------------------
public:
protected:
	CString	m_sPath;				
	BOOL	m_bWatchSubTree;		
	DWORD	m_dwWatchFilter;		
	HWND	m_hwndWindowToNotify;	
	BOOL	m_bPost;				
	HANDLE	m_hEvent;				

#if defined(_UNICODE) && (_WIN32_WINNT >= 0x400)
// Extended info is supported
	BOOL	m_bExtended;			
	HANDLE	m_hDirectory;			
	LPVOID	m_lpBuffer;				
	DWORD	m_nBufferLength;		
	OVERLAPPED* m_pOverlapped;		
	static DWORD dwBytesReturned;	
#endif // defined(_UNICODE) && (_WIN32_WINNT >= 0x400)

protected:
private:

// Member functions ---------------------------------------------------------
public:
protected:
	COXWatchedDir(CString sPath, BOOL bWatchSubTree, DWORD dwWatchFilter, BOOL bExtended);
	virtual ~COXWatchedDir();

	BOOL FindFirstHandle(HRESULT& rhrResult);
	BOOL FindNextHandle(HANDLE& hHandle);

private:
};

#endif // __OXWATCHEDDIR_H__
// ==========================================================================


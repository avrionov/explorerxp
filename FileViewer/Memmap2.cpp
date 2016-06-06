/*
Module : MEMMAP.CPP
Purpose: Implementation for an MFC class to wrap memory mapped files
Created: PJN / MEMMAP/1 / 19-03-1997
History: PJN / 31-03-1998 1) Class now avoids trying to lock the mutex if only read access is required
                          2) User now has the option of specifying whether a file should be mapped with 
                             A Null terminator at the end. Can prove helpful when you want to use some
                             of the "C" runtime functions on the pointer returned.
         PJN / 20-04-1998 1) Now uses GetFileSize SDK call instead of GetFileInformationByHandle as a
                             more "reliable" way to determine file length.
                          2) Included TRACE statements to call GetLastError in all places where 
                             SDK functions fail
         PJN / 29-05-1998 1) Mapping a file now has the option of making it named or not.
         PJN / 22-10-1998 1) Fixed a bug in a number of calls to CreateMappingName when the classes were 
                             being used to share memory.
                          2) Tidy up of the demo app including:
                             a) Made the amount of text being shared a constant of MAX_EDIT_TEXT instead
                             of hardcoding it to 20 everywhere in the sample.
                             b) Changed where the timer is being created to OnInitDialog
                             c) Tidied up the initialisation sequence in OnInitDialog
                             d) Now using _tcscpy instead of _tcsncpy to ensure array is null terminated
                             e) Fixed resource.h which was causing the resources to fail to compile
                             f) Removed unnecessary symbols from resource.h
                             g) Optimized the way the OnTimer code works to only update the text when it
                             has changed in the MMF. This means that you can type continuously into the
                             edit control.
                          3) New documentation in the form of a HTML file.
                          4) Sample now ships as standard with VC 5 workspace files
         PJN / 30-3-1999  1) Code is now unicode compliant
                          2) Code now supports growable MMF's
                          3) Addition of accessor functions for file handle and file mapping handle
         PJN / 21-4-1999  1) Works around a Window bug where you try to memory map a zero length file on
                             Windows 95 or 98.
         PJN / 24-4-2000  1) Fixed a simple typo problem in a TRACE statement when compiled for UNICODE
         PJN / 07-3-2001  1) Updated copyright information   
                          2) Fixed problem where mutex was not being locked when read only access to 
                          the memory mapped file was desired. Access to the MMF should be synchronised
                          irrespective of the access mode.
         PJN / 02-4-2001  1) Now supports passing in a security descriptor to MapFile and MapMemory
                          2) Now supports opening a specified portion of a file, rather than always mapping
                          all of the file
         PJN / 05-10-2003 1) Updated copyright details.
                          2) Fixed a small typo in the description of the FSCTL_SET_SPARSE IOCTL. Thanks to 
                          amores perros for reporting this.
                          3) Fixed a minor tab indentation problem at the start of the MapFile method. Again
                          thanks to amores perros for reporting this.
                          4) Removed the unnecessary AssertValid function. Again thanks to amores perros for 
                          reporting this.
         PJN / 17-11-2003 1) Fixed a memory leak in UnMap as reported by Bart Duijndam using 
                          "Memory Validator". The memory leak may in fact not be real, but the code change 
                          avoids the reported problem.
         PJN / 06-06-2004 1) Fixed an issue in MapHandle where the wrong value was sent to CreateFileMapping. 
                          This issue only occurs when you are not mapping the whole of a file, but instead
                          decide to perform the mapping a chunk at a time. Thanks to Nicolas Stohler for
                          reporting this problem.
                          2) Removed the AppendNull option as it is incompatible with general use of memory 
                          mapped files.
                          3) Reviewed all the TRACE statements throughout the class
                          4) Added ASSERT validation at the top of functions which modify member variables
                          5) Failing to create mutex in MapHandle and MapExistingMemory not fails the function



Copyright (c) 1997 - 2004 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 




********************************************************************
24-Aug-2004

CMemMapFile2 is a modified and stripped version of CMemMapFile created by PJ Naughter. (thats way all his copyright& history still is there.)
This class is modified to support 64bit integer and is able to map view of files that are bigger then 4GB. 
And since I did not have time to get everything in PJ.N's version of CMemMapFile to work with int64 I removed everything I did not need like MemMapMemory.
and it also opening the files in READONLY mode only.
Removed Mutex stuff becouse with it I could not memmap the the same files from two instances.  and I want that since im only doing readonly mapping.
I renamed the class to CMemMapFile2 so it will not conflict with the original if I use both in the same project.

/ Mathias S,  ( email : ms@resutl42.com )

********************************************************************


*/



/////////////////////////////////  Includes  / Defines ////////////////////////

#include "stdafx.h"
#include "memmap2.h"

#ifndef _WINIOCTL_
#pragma message("To avoid this message, please put winioctl.h in your PCH (normally stdafx.h)")
#include <winioctl.h>
#endif

//Define taken from the Platform SDK, means that CMemMapFile will compile 
//correctly even on systems which do not have the Platform SDK installed
#ifndef FSCTL_SET_SPARSE
#define FSCTL_SET_SPARSE                CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 49, METHOD_BUFFERED, FILE_WRITE_DATA)
#endif



///////////////////////////////// Implementation //////////////////////////////

IMPLEMENT_DYNAMIC(CMemMapFile2, CObject)

CMemMapFile2::CMemMapFile2()
{
  //Initialise variables to sane values
  m_hFile = INVALID_HANDLE_VALUE;
  m_hMapping = NULL;
//  m_bReadOnly = TRUE;
  m_lpData = NULL;
  m_bOpen = FALSE;
  m_hMutex = NULL;
  m_dwMapLength = 0;
  m_dwMapSize = 0;

  m_nFileSize = 0;
  m_nOffset = 0;
  
  SYSTEM_INFO SystemInfo;
  GetSystemInfo(&SystemInfo);
  m_dwAlign = SystemInfo.dwAllocationGranularity;
}

CMemMapFile2::~CMemMapFile2()
{
  UnMap();
}

BOOL CMemMapFile2::MapFile(const CString& sFilename, DWORD dwShareMode,  __int64 nStartOffset , DWORD dwNumberOfBytesToMap, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  //Validate our state
  ASSERT(m_hFile == INVALID_HANDLE_VALUE);

  //Work out the file access flags
//  m_bReadOnly = TRUE;
  //Open the real file on the file system
  m_hFile = CreateFile(sFilename, GENERIC_READ, dwShareMode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (m_hFile == INVALID_HANDLE_VALUE)
  {
    TRACE(_T("CMemMapFile2::MapFile, Failed in call to CreateFile, Error:%d\n"), GetLastError());
    UnMap();
    return FALSE;
  }

	//Get the size of the file we are mapping
  DWORD dwFileSizeHigh=0;
  DWORD dwFileSizeLow = GetFileSize(m_hFile, &dwFileSizeHigh);
 
  if (dwFileSizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR ) 
  {
		//There was an error calling GetFileSize
		TRACE(_T("CMemMapFile2::MapFile, Failed in call to GetFileSize, Error:%d\n"), GetLastError());
		UnMap();
		return FALSE;
  }

  ULARGE_INTEGER ul;
  ul.LowPart = dwFileSizeLow;
  ul.HighPart = dwFileSizeHigh;
  m_nFileSize = ul.QuadPart;

  //Fail if file is 0 length in size, calling CreateFileMapping on a 
  //zero length file on 95/98 can cause problems
  if (dwFileSizeHigh == 0 && dwFileSizeLow == 0)
  {
    TRACE(_T("CMemMapFile2::MapFile, File size is 0, not attempting to memory map the file\n"));
    UnMap();
    return FALSE;
  }

  //Find out the size of the mapping we will be performing
  if(dwNumberOfBytesToMap)
  {
	  // if map area is bigger then filesize.. then shrink to fit filesize
	  if( dwNumberOfBytesToMap > m_nFileSize )
			m_dwMapLength = dwFileSizeLow;
	  else
			m_dwMapLength = dwNumberOfBytesToMap;

	  m_dwMapSize = dwNumberOfBytesToMap;
  }
  else
  {
	  if( m_nFileSize > 1073741824 ) // Allow MAX 1G of mapping.
		m_dwMapLength = 1073741824;
	  else
		m_dwMapLength = dwFileSizeLow;

	  m_dwMapSize = 0;
  }

  //Do the actual mapping
  m_sMappingName = CreateMappingName(sFilename, TRUE );
  return MapHandle(m_hFile, lpSecurityAttributes, m_nFileSize , nStartOffset); // nFileMappingSize (3 param ) is what area of the file that should be able to be mapped.
}

BOOL CMemMapFile2::MapHandle(HANDLE hHandle, LPSECURITY_ATTRIBUTES lpSecurityAttributes, __int64 nFileMappingSize, __int64 nStartOffset)
{
  //Validate our state
  ASSERT(m_hMapping == NULL);
  ASSERT(m_lpData == NULL);
  ASSERT(m_hMutex == NULL);

  //Create the file mapping object
  ULARGE_INTEGER ul;
  ul.QuadPart = nFileMappingSize;

//  m_hMapping = CreateFileMapping(hHandle, lpSecurityAttributes, PAGE_READONLY , ul.HighPart , ul.LowPart , m_sMappingName);
  m_hMapping = CreateFileMapping(hHandle, lpSecurityAttributes, PAGE_READONLY , 0 ,0 , m_sMappingName);
  if (m_hMapping == NULL)
  {
    TRACE(_T("CMemMapFile2::MapHandle, Failed in call to CreateFileMapping, Error:%d\n"), GetLastError());
    UnMap();
    return FALSE;
  }

  if( m_dwMapSize == 0 )
	  m_dwMapLength = nFileMappingSize;

  if( nStartOffset > 0 )
  {
	nStartOffset -= nStartOffset % m_dwAlign;
	if( nStartOffset < 0 ) // just to make sure
		nStartOffset = 0;

	if( (nStartOffset + m_dwMapLength ) > m_nFileSize )
	{
		m_dwMapLength = (DWORD)(m_nFileSize - nStartOffset); // Okey to cast to DWORD since we to the size check in the "if" above..
	}
	ul.QuadPart = nStartOffset;
  }
  else
  {
	  if( m_dwMapLength > m_nFileSize )
		  m_dwMapLength = m_nFileSize;

	  ul.QuadPart = nStartOffset;
  }
	

  m_nOffset = nStartOffset;

  m_lpData = MapViewOfFile(m_hMapping, FILE_MAP_READ, ul.HighPart , ul.LowPart , m_dwMapLength );

  /*
  //Create the mutex to sync access
  m_hMutex = CreateMutex(lpSecurityAttributes, FALSE, CreateMutexName());
  if (m_hMutex == NULL)
  {
    TRACE(_T("CMemMapFile2::MapHandle, Failed in call to CreateMutex, Error:%d\n"), GetLastError());
    UnMap();
    return FALSE;
  }
*/
  return (m_lpData != NULL);
}

LPVOID CMemMapFile2::Open(DWORD /*dwTimeout*/)
{
  if (m_lpData == NULL)
    return NULL;

  //Synchronise access to the MMF using the named mutex
//	DWORD dwResult = WaitForSingleObject(m_hMutex, dwTimeout);
//	if (dwResult == WAIT_OBJECT_0)
	{
		m_bOpen = TRUE;
		return m_lpData;
	}

  return NULL;
}

BOOL CMemMapFile2::Close()
{
  //Release our interest in this MMF
  if (!m_bOpen)
    return FALSE;

  //ASSERT(m_hMutex); //Mutex should be valid
  m_bOpen = FALSE;
  if( m_hMutex )
	ReleaseMutex(m_hMutex);
  
  return TRUE;
}

BOOL CMemMapFile2::Flush()
{
  //No mapping open, so nothing to do
  if (m_lpData == NULL)
		return FALSE;

  return FlushViewOfFile(m_lpData, 0);
}

CString CMemMapFile2::CreateMutexName() const
{
  return m_sMappingName + _T("MUTEX");
}

BOOL CMemMapFile2::Reload( INT64 nOffset )
{
	HANDLE hFile = GetFileHandle();
	DWORD dwFileSizeHigh=0;
	DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);

	if (dwFileSizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR ) 
		return FALSE;

	ULARGE_INTEGER ul;
	ul.LowPart = dwFileSizeLow;
	ul.HighPart = dwFileSizeHigh;
	if( (__int64)ul.QuadPart == m_nFileSize )
		return FALSE;

    m_nFileSize = ul.QuadPart;

	if (m_hMapping != NULL)
	{
		CloseHandle(m_hMapping);
		m_hMapping = NULL;
	}
	//Close the mutex we have been using
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
	BOOL bStatus = FlushViewOfFile(m_lpData, 0);
	bStatus = UnmapViewOfFile(m_lpData);
	m_lpData = NULL;
	m_dwMapLength = m_dwMapSize;
	if( MapHandle( hFile , NULL , m_nFileSize , nOffset ) )
		return TRUE;
    return FALSE;
}

LPVOID CMemMapFile2::Remap( INT64 nOffset , DWORD dwMapSize )
{
	BOOL bStatus = FlushViewOfFile(m_lpData, 0);
	bStatus = UnmapViewOfFile(m_lpData);
	m_lpData = NULL;

	//  Offset but be align to m_dwAlign. ( normaly 64k )
	//  offset += offset % m_dwAlign

	if( nOffset > m_nFileSize )
		nOffset = m_nFileSize - (dwMapSize/2);
   
	nOffset -= nOffset % m_dwAlign;
	
	if( (nOffset + dwMapSize ) > m_nFileSize )
	{
		dwMapSize = (DWORD)(m_nFileSize - nOffset); // Okey to cast to DWORD since we to the size check in the "if" above..
		if( dwMapSize < m_dwAlign && ( dwMapSize + m_dwAlign) < m_nFileSize && nOffset > m_dwAlign )
		{
			dwMapSize += m_dwAlign;
			nOffset -= m_dwAlign;
		}
	}


	ULARGE_INTEGER ul;
	ul.QuadPart = nOffset;

	m_lpData = MapViewOfFile(m_hMapping, FILE_MAP_READ , ul.HighPart , ul.LowPart  , dwMapSize );
	if( m_lpData == NULL )
	{
		m_nOffset = 0;
	}

	if( m_lpData )
	{
		m_nOffset = nOffset;
		m_dwMapLength = dwMapSize;
		return m_lpData;
	}
	return NULL;

}
void CMemMapFile2::UnMap()
{
  //Close any views which may be open
	Close();

  //unmap the view
	if (m_lpData != NULL)
	{
		FlushViewOfFile(m_lpData, 0);
		UnmapViewOfFile(m_lpData);
		m_lpData = NULL;
	}

  //remove the file mapping
	if (m_hMapping != NULL)
	{
		CloseHandle(m_hMapping);
		m_hMapping = NULL;
	}

  //close the file system file if its open
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

  //Close the mutex we have been using
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

  //Reset the remaining member variables
  m_sMappingName.Empty();
  m_dwMapLength = 0;
}
CString CMemMapFile2::CreateMappingName(const CString& sName, BOOL bNamed)
{
	CString rVal;
	if (bNamed)
	{
		rVal = sName;

		//Replace all '\' by '_'
		for (int i=0; i<sName.GetLength(); i++)
		{
			if (rVal.GetAt(i) == _T('\\'))
				rVal.SetAt(i, _T('_'));
		}

		rVal.MakeUpper();
	}

	return rVal;
}


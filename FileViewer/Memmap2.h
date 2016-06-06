/*
Module : MEMMAP.H
Purpose: Interface for an MFC class to wrap memory mapped files
Created: PJN / 30-07-1997


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
////////////////////////////////// Macros ///////////////////////////

#ifndef __MEMMAP_H__
#define __MEMMAP_H__



/////////////////////////// Classes /////////////////////////////////

class CMemMapFile2 : public CObject
{
public:
//Constructors / Destructors
  CMemMapFile2();
  ~CMemMapFile2();

//Methods
  BOOL    MapFile(const CString& sFilename, DWORD dwShareMode = 0, __int64 nStartOffset=0, DWORD dwNumberOfBytesToMap=0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);

  void    UnMap();
  LPVOID  Open(DWORD dwTimeout = INFINITE);
  BOOL	  Close();
  BOOL	  Flush();

  BOOL    Reload( INT64 nOffset );
//Accessors
  CString GetMappingName() const { return m_sMappingName; }   ;
  HANDLE  GetFileHandle() const { return m_hFile; };
  HANDLE  GetFileMappingHandle() const { return m_hMapping; };
  BOOL    IsOpen() const { return m_bOpen; };
  DWORD   GetFileMapLength() const { return m_dwMapLength; };

  LPVOID  GetData() { return m_lpData; }
  __int64 GetTotalFileSize() const { return m_nFileSize; }
  __int64 GetOffset() { return m_nOffset; }
  LPVOID  Remap( INT64 nOffset , DWORD dwMapSize);


protected:
  DECLARE_DYNAMIC(CMemMapFile2)

  BOOL MapHandle(HANDLE hHandle, LPSECURITY_ATTRIBUTES lpSecurityAttributes, __int64 nFileMappingSize , __int64 nStartOffset );
  CString CreateMappingName(const CString& sName, BOOL bNamed);
  CString CreateMutexName() const;

  __int64	m_nFileSize;
  __int64   m_nOffset;
  DWORD     m_dwAlign;

  HANDLE  m_hFile;
  HANDLE  m_hMapping;

  LPVOID  m_lpData;
  CString m_sMappingName;
  BOOL    m_bOpen;
  HANDLE  m_hMutex;

  DWORD   m_dwMapLength;	// How big area are Mapped .
  DWORD   m_dwMapSize;      // How big area we going to try to map when remaping
};

#endif //__MEMMAP_H__

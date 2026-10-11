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

#include <afxmt.h>

///////////////////////////////////////////////
// Helper class:

class CFileNotifyInformation
/*******************************

A Class to more easily traverse the FILE_NOTIFY_INFORMATION records returned
by ReadDirectoryChangesW().

FILE_NOTIFY_INFORMATION is defined in Winnt.h as:

 typedef struct _FILE_NOTIFY_INFORMATION {
    DWORD NextEntryOffset;
        DWORD Action;
    DWORD FileNameLength;
    WCHAR FileName[1];
} FILE_NOTIFY_INFORMATION, *PFILE_NOTIFY_INFORMATION;

  ReadDirectoryChangesW basically puts x amount of these records in a
  buffer that you specify.
  The FILE_NOTIFY_INFORMATION structure is a 'dynamically sized' structure (size
depends on length of the file name (+ sizeof the DWORDs in the struct))

  Because each structure contains an offset to the 'next' file notification
  it is basically a singly linked list.  This class treats the structure in that
way.


  Sample Usage:
  BYTE Read_Buffer[ 4096 ];

  ...
  ReadDirectoryChangesW(...Read_Buffer, 4096,...);
  ...

  CFileNotifyInformation notify_info( Read_Buffer, 4096);
  do{
            switch( notify_info.GetAction() )
                {
                case xx:
                    notify_info.GetFileName();
                }

  while( notify_info.GetNextNotifyInformation() );

********************************/
{
 public:
  CFileNotifyInformation(BYTE* lpFileNotifyInfoBuffer, DWORD dwBuffSize)
      : m_pBuffer(lpFileNotifyInfoBuffer), m_dwBufferSize(dwBuffSize) {
    ASSERT(lpFileNotifyInfoBuffer && dwBuffSize);

    m_pCurrentRecord = (PFILE_NOTIFY_INFORMATION)m_pBuffer;
  }

  BOOL GetNextNotifyInformation();

  BOOL CopyCurrentRecordToBeginningOfBuffer(
      OUT DWORD& ref_dwSizeOfCurrentRecord);

  DWORD GetAction() const;  // gets the type of file change notifiation
  CString GetFileName()
      const;  // gets the file name from the FILE_NOTIFY_INFORMATION record
  CString GetFileNameWithPath(const CString& strRootPath)
      const;  // same as GetFileName() only it prefixes the strRootPath into the
              // file name

 protected:
  BYTE* m_pBuffer;  //<--all of the FILE_NOTIFY_INFORMATION records 'live' in
                    //the buffer this points to...
  DWORD m_dwBufferSize;
  PFILE_NOTIFY_INFORMATION
      m_pCurrentRecord;  // this points to the current FILE_NOTIFY_INFORMATION
                         // record in m_pBuffer
};

BOOL CFileNotifyInformation::GetNextNotifyInformation()
/***************
  Sets the m_pCurrentRecord to the next FILE_NOTIFY_INFORMATION record.

  Even if this return FALSE, (unless m_pCurrentRecord is NULL)
  m_pCurrentRecord will still point to the last record in the buffer.
****************/
{
  if (m_pCurrentRecord && m_pCurrentRecord->NextEntryOffset !=
                              0UL)  // is there another record after this one?
  {
    // set the current record to point to the 'next' record
    PFILE_NOTIFY_INFORMATION pOld = m_pCurrentRecord;
    m_pCurrentRecord =
        (PFILE_NOTIFY_INFORMATION)((LPBYTE)m_pCurrentRecord +
                                   m_pCurrentRecord->NextEntryOffset);

    ASSERT((DWORD)((BYTE*)m_pCurrentRecord - m_pBuffer) <
           m_dwBufferSize);  // make sure we haven't gone too far

    if ((DWORD)((BYTE*)m_pCurrentRecord - m_pBuffer) > m_dwBufferSize) {
      // we've gone too far.... this data is hosed.
      //
      //  This sometimes happens if the watched directory becomes deleted...
      //  remove the FILE_SHARE_DELETE flag when using CreateFile() to get the
      //  handle to the directory...
      m_pCurrentRecord = pOld;
    }

    return (BOOL)(m_pCurrentRecord != pOld);
  }
  return FALSE;
}

BOOL CFileNotifyInformation::CopyCurrentRecordToBeginningOfBuffer(
    OUT DWORD& ref_dwSizeOfCurrentRecord) {
  ASSERT(m_pBuffer && m_pCurrentRecord);
  if (!m_pBuffer || !m_pCurrentRecord) return FALSE;

  // Calculate exact byte size of current FILE_NOTIFY_INFORMATION record
  ref_dwSizeOfCurrentRecord = FIELD_OFFSET(FILE_NOTIFY_INFORMATION, FileName) +
                              m_pCurrentRecord->FileNameLength;

  // Cast pointers to BYTE* for offset arithmetic and validation
  BYTE* pBufferStart = reinterpret_cast<BYTE*>(m_pBuffer);
  BYTE* pBufferEnd = pBufferStart + m_dwBufferSize;
  BYTE* pRecordStart = reinterpret_cast<BYTE*>(m_pCurrentRecord);
  BYTE* pRecordEnd = pRecordStart + ref_dwSizeOfCurrentRecord;

  ASSERT(pRecordStart >= pBufferStart && pRecordEnd <= pBufferEnd);
  if (pRecordStart < pBufferStart || pRecordEnd > pBufferEnd) {
    TRACE(
        _T("ERROR: Record out of bounds in ")
        _T("CopyCurrentRecordToBeginningOfBuffer()\n"));
    return FALSE;
  }

  // Safely compare raw pointer addresses
  if (static_cast<const void*>(m_pBuffer) !=
      static_cast<const void*>(m_pCurrentRecord)) {
    // Use memmove to handle overlapping memory regions safely
    ::memmove(m_pBuffer, m_pCurrentRecord, ref_dwSizeOfCurrentRecord);

    // Reset NextEntryOffset on the copied record
    PFILE_NOTIFY_INFORMATION pCopiedRecord =
        reinterpret_cast<PFILE_NOTIFY_INFORMATION>(m_pBuffer);
    pCopiedRecord->NextEntryOffset = 0;

    // Repoint m_pCurrentRecord to the buffer start
    m_pCurrentRecord = pCopiedRecord;
  }

  return TRUE;
}

DWORD CFileNotifyInformation::GetAction() const {
  ASSERT(m_pCurrentRecord);
  if (m_pCurrentRecord) return m_pCurrentRecord->Action;
  return 0UL;
}

CString CFileNotifyInformation::GetFileName() const {
  //
  // BUG FIX:
  //		File Name's longer than 130 characters are truncated
  //
  //		Thanks Edric @ uo_edric@hotmail.com for pointing this out.
  if (m_pCurrentRecord) {
    WCHAR wcFileName[MAX_PATH + 1] = {0};  // L"";
    memcpy(wcFileName, m_pCurrentRecord->FileName,
           // min( MAX_PATH, m_pCurrentRecord->FileNameLength) <-- buggy line
           min((MAX_PATH * sizeof(WCHAR)), m_pCurrentRecord->FileNameLength));

    return CString(wcFileName);
  }
  return CString();
}

static inline bool HasTrailingBackslash(const CString& str) {
  if (str.GetLength() > 0 && str[str.GetLength() - 1] == SLASH) return true;
  return false;
}
CString CFileNotifyInformation::GetFileNameWithPath(
    const CString& strRootPath) const {
  CString strFileName(strRootPath);
  if (!HasTrailingBackslash(strRootPath)) strFileName += _T("\\");

  strFileName += GetFileName();
  return strFileName;
}
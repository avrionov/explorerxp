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
#include "FileShellAttributes.h"
#include "SplitPath.h"
#include "globals.h"

CShellInfoMap CFileShellAttributes::m_IconMap;
CFileInfoMap CFileShellAttributes::m_FileTypeMap;
CMyLock CFileShellAttributes::m_lock;

CFileShellAttributes::CFileShellAttributes(void) {}

CFileShellAttributes::~CFileShellAttributes(void) {}

int CFileShellAttributes::GetFileIcon(const TCHAR *file_name, bool bFolder) {

  {
    CGuard guard(m_lock);

    icon_it it = m_IconMap.find(file_name);

    if (it != m_IconMap.end())
      return it->second.iIcon;
  }

  CSplitPath path(file_name);
  const TCHAR *ext = path.GetExt();

  if (_tcsicmp(ext, TEXT(".exe")) == 0) {
    return 0;
  }

  SHFILEINFO sfi;

  DWORD attr = 0;
  if (bFolder)
    attr = FILE_ATTRIBUTE_DIRECTORY;
  else
    attr = FILE_ATTRIBUTE_NORMAL;

  SHGetFileInfo(file_name, attr, &sfi, sizeof(SHFILEINFO),
                SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
  {
    CGuard guard(m_lock);
    m_IconMap.insert(iconmap_value(file_name, sfi));
  }

  return sfi.iIcon;
}

int CFileShellAttributes::GetFileIcon(const TCHAR *file_name) {

  {
    CGuard guard(m_lock);
    icon_it it = m_IconMap.find(file_name);

    if (it != m_IconMap.end())
      return it->second.iIcon;
  }

  CSplitPath path(file_name);
  const TCHAR *ext = path.GetExt();
  if (_tcsicmp(ext, TEXT(".exe")) == 0) {
    return 0;
  }

  SHFILEINFO sfi;

  SHGetFileInfo(file_name, 0, &sfi, sizeof(SHFILEINFO),
                SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
   {
    CGuard guard(m_lock);
	m_IconMap.insert(iconmap_value(file_name, sfi));
   }

  return sfi.iIcon;
}

const CString CFileShellAttributes::GetFileType(const TCHAR *parent_folder,
                                               const TCHAR *file_name) {
  CString path = parent_folder;
  path += file_name;

  return GetFileType(path);
}

const CString CFileShellAttributes::GetFileType(const TCHAR *file_name) {

  CSplitPath path(file_name);
  const TCHAR *ext = path.GetExt();

  if (!ext[0]) {
    if (IsDirectory(file_name))
      return _T("File Folder");
    else
      return _T("File");
  }

  {
    CGuard guard(m_lock);
    filetype_it it = m_FileTypeMap.find(ext);
    if (it != m_FileTypeMap.end())
      return it->second;
  }

  SHFILEINFO sfi;

  if (_tcsicmp(ext, TEXT(".exe")) == 0) {
    return TEXT("Application");
  }

  SHGetFileInfo(file_name, 0, &sfi, sizeof(SHFILEINFO),
                SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_TYPENAME);
  if (!sfi.szTypeName[0]) {
    if (ext[0]) {
      sfi.szTypeName[0] = 0;
      _tcsncat(sfi.szTypeName, ext + 1, 70);
      _tcsupr(sfi.szTypeName);
      _tcsncat(sfi.szTypeName, _T(" File"), 79);
    } else
      _tcscpy(sfi.szTypeName, _T("File"));
  }

  {
    CGuard guard(m_lock);
    m_IconMap.insert(iconmap_value(file_name, sfi));
    m_FileTypeMap.insert(filetype_value(ext, sfi.szTypeName));
  }

  return sfi.szTypeName;
}

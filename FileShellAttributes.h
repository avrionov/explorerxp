/* Copyright 2002-2020 Nikolay Avrionov. All Rights Reserved.
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

#include "MyLock.h"

typedef std::map<CString, SHFILEINFO, std::less<CString> > CShellInfoMap;
typedef CShellInfoMap::iterator icon_it;
typedef CShellInfoMap::value_type iconmap_value;

typedef std::map<CString, CString, std::less<CString> > CFileInfoMap;
typedef CFileInfoMap::iterator filetype_it;
typedef CFileInfoMap::value_type filetype_value;



class CFileShellAttributes {
public:
  CFileShellAttributes(void);
  ~CFileShellAttributes(void);

  static int GetFileIcon (const TCHAR* file_name, bool bFolder);
  static int GetFileIcon (const TCHAR* file_name);
  static const CString GetFileType (const TCHAR* file_name);
  static const CString GetFileType(const TCHAR* parent_folder, const TCHAR* file_name);

protected:
	static CShellInfoMap m_IconMap;
	static CFileInfoMap  m_FileTypeMap;
	static CMyLock		 m_lock;
};

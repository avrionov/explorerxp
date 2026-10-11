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

/////////////////////////////////////////////////////////////////////////////////
class CPrivilegeEnabler
//
//	Enables privileges for this process
//	first time CDirectoryChangeWatcher::WatchDirectory() is called.
//
//	It's a singleton.
//
{
 private:
  CPrivilegeEnabler();  // ctor
 public:
  ~CPrivilegeEnabler() {};

  static CPrivilegeEnabler& Instance();
  // friend static CPrivilegeEnabler & Instance();
};

/////////////////////////////////////////////////////////////////////
//	Helper functions.
BOOL EnablePrivilege(LPCTSTR pszPrivName, BOOL fEnable /*= TRUE*/)
//
//	I think this code is from a Jeffrey Richter book...
//
//	Enables user priviledges to be set for this process.
//
//	Process needs to have access to certain priviledges in order
//	to use the ReadDirectoryChangesW() API.  See documentation.
{
  BOOL fOk = FALSE;
  // Assume function fails
  HANDLE hToken;
  // Try to open this process's access token
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
    // privilege
    TOKEN_PRIVILEGES tp = {1};

    if (LookupPrivilegeValue(NULL, pszPrivName, &tp.Privileges[0].Luid)) {
      tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;

      AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

      fOk = (GetLastError() == ERROR_SUCCESS);
    }
    CloseHandle(hToken);
  }
  return (fOk);
}


CPrivilegeEnabler::CPrivilegeEnabler() {
  LPCTSTR arPrivelegeNames[] = {
      SE_BACKUP_NAME,   //	these two are required for the
                        //FILE_FLAG_BACKUP_SEMANTICS flag used in the call to
      SE_RESTORE_NAME,  //  CreateFile() to open the directory handle for
                        //  ReadDirectoryChangesW

      SE_CHANGE_NOTIFY_NAME  // just to make sure...it's on by default for all
                             // users.
      //<others here as needed>
  };
  for (int i = 0; i < sizeof(arPrivelegeNames) / sizeof(arPrivelegeNames[0]);
       ++i) {
    if (!EnablePrivilege(arPrivelegeNames[i], TRUE)) {
      TRACE(
          _T("Unable to enable privilege: %s	--	GetLastError(): %d\n"),
          arPrivelegeNames[i], GetLastError());
      TRACE(
          _T("CDirectoryChangeWatcher notifications may not work as intended ")
          _T("due to insufficient access rights/process privileges.\n"));
      TRACE(_T("File: %s Line: %d\n"), _T(__FILE__), __LINE__);
    }
  }
}

CPrivilegeEnabler& CPrivilegeEnabler::Instance() {
  static CPrivilegeEnabler
      theInstance;  // constructs this first time it's called.
  return theInstance;
}
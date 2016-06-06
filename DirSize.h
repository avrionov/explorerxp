/* Copyright 2002-2016 Nikolay Avrionov. All Rights Reserved.
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

#ifndef __DIR_SIZE__
#define __DIR_SIZE__

#pragma once

#include "DirChanges\DirectoryChanges.h"
#include "MyLock.h"
#include "globals.h"

struct CDirInfo
{
	CDirInfo (const ULONGLONG &size, const ULONGLONG &sizeOnDisk, FILETIME &fTime, bool bSubs)
	{
		m_Size = size;
		m_Time = fTime;
		m_SizeOnDisk = sizeOnDisk;
		m_bSubFolders = bSubs;
	}
	CDirInfo () 
	{
		m_Size = 0;
		m_Time.dwHighDateTime = 0;
		m_Time.dwLowDateTime  = 0;
		m_SizeOnDisk = 0;
		m_bSubFolders = 0;
	};

	ULONGLONG m_Size;
	ULONGLONG m_SizeOnDisk;
	FILETIME m_Time;
	bool	 m_bSubFolders;
};


struct dir_cache
{
	TCHAR folder[MAX_FOLDER_SIZE];
	CDirInfo dirinfo;
};

typedef std::map <CString, CDirInfo, std::less <CString> > CDirInfoMap;

typedef CDirInfoMap::iterator dirinfo_it;
typedef CDirInfoMap::value_type dirinfo_value;

typedef std::vector <dir_cache> CDirInfoArray;

struct CDirStackEl
{
	CDirStackEl (const TCHAR *path, const TCHAR *file, FILETIME &time)
	{
		m_Path = path;
		m_FileName = file;
		m_Time = time;
	}

	CString m_Path;
	CString m_FileName;
	FILETIME m_Time;
};


class CDirSizeChangeHandler;
class CExplorerXPView;

typedef std::vector <CDirStackEl> CDirStack; 
typedef std::vector <CDirSizeChangeHandler*> CDirSizHandlerAr;
typedef std::vector <CExplorerXPView*> CViewAr;

#define UPDATE_ADD      0
#define UPDATE_DELETE	1
#define UPDATE_RENAME	2
#define UPDATE_MODIFY	3



class CDirSize  
{
public:
	CDirSize();
	virtual ~CDirSize();
	bool GetSize (const TCHAR *folder, const TCHAR *file, ULONGLONG &size, ULONGLONG &sizeOnDisk, FILETIME &fTime);
	bool GetSizeInternal (const TCHAR *folder, const TCHAR *file, ULONGLONG &size, ULONGLONG &sizeOnDisk, FILETIME &fTime);
	bool GetSizeLight (const TCHAR *folder, const TCHAR *file, ULONGLONG &size, ULONGLONG &sizeOnDisk, FILETIME &fTime);
	bool GetSizeLight (const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeOnDisk, FILETIME &fTime);
	
	//void GetFolder (std::vector <WIN32_FIND_DATA> &array, const TCHAR *path);
	void Cycler ();
	void AddDir (const TCHAR *path, const TCHAR *file, FILETIME &time);
	bool GetDir (CString &path, CString &file, FILETIME &time);
	void Clear ();
	void Notify (int msg, WPARAM wp, LPARAM lp);
	void AddSize (const TCHAR *path, const TCHAR *name, ULONGLONG size, ULONGLONG sizeOnDisk, FILETIME &fTime, bool bSub);
	void AddWindow (HWND hWnd);
	void DeleteWindow (HWND  hWnd);

	void AddWindow (CExplorerXPView *pView);
	void DeleteWindow (CExplorerXPView *pView);

	void Start ();
	void Stop ();
	void Invalidate (const TCHAR *path, bool bDelete = false);
	void InvalidateName (const TCHAR *old_name, const TCHAR *new_name);
	bool IsInStack (const TCHAR *path, const TCHAR *name);
	void NotifyViews (const TCHAR *filename);
	bool IsStackEmpty ();
	bool GetDirInfoLock (const TCHAR *path, CDirInfo &dirinfo);
	void MarkDrives ();
	void CheckNewDrives ();
	void SortStack ();
	void ClearStack ();
	void Save ();
	void Load ();
	void GetDirInfoArrray (CDirInfoArray & arr);

	CDirInfoMap m_Map;
	CDirStack	m_Stack;
	
	std::vector <HWND> m_Wnds;
	CViewAr   m_Views;
	CDirectoryChangeWatcher m_Watcher;
	CMyLock m_SizeLock;
	CMyLock m_wndLock;	
	CMyLock m_StackLock;
	CDirSizHandlerAr m_Handlers;
	CDriveArray m_Drives;
};

#endif // __DIR_SIZE__

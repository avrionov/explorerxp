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


#include "stdafx.h"
#include "DirSize.h"
#include "DirSizeChangeHandler.h"
#include "Viewers\DriveViewer.h"
#include "ExplorerXPView.h"
#include "fast_array.h"
#include "SimpleFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDirSize::CDirSize()
:m_Watcher (true, CDirectoryChangeWatcher::FILTERS_DONT_USE_FILTERS | CDirectoryChangeWatcher::FILTERS_NO_WATCH_STARTSTOP_NOTIFICATION)
{
	m_Wnds.reserve (1000);	 
}

CDirSize::~CDirSize()
{
	
}

void GetFolder (fast_array <WIN32_FIND_DATA> &array, const TCHAR *path)
{
	CString mask = path;
	SureBackSlash (mask);
	mask += "*.*";

	HANDLE hFind;
	
	hFind = FindFirstFile(mask, &array[0]);
	
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	//array.add();

	BOOL bNext = TRUE;

	while (bNext)
	{
		if (ifNotJunctionPoint(array.last().dwFileAttributes)) {
			bool bDir = (array.last().dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==FILE_ATTRIBUTE_DIRECTORY;

			if (!(bDir && IsDots (array.last().cFileName)))
			
			array.add();
		}

		bNext = FindNextFile (hFind, &array.last());
	}				

	FindClose(hFind);	
}

/*
void GetFolder (std::vector <WIN32_FIND_DATA> &array, const TCHAR *path)
{
	CString mask = path;
	SureBackSlash (mask);
	mask += "*.*";

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	array.clear ();
	
	hFind = FindFirstFile(mask, &FindFileData);

	array.reserve(2048);

	if (hFind == INVALID_HANDLE_VALUE)
		return;
		
	BOOL bNext = TRUE;

	while (bNext)
	{
		bool bDir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==FILE_ATTRIBUTE_DIRECTORY;

		if (!(bDir && IsDots (FindFileData.cFileName)))
			array.push_back ( FindFileData);
		
		bNext = FindNextFile (hFind, &FindFileData);
	}				

	FindClose(hFind);
}
*/

inline ULONGLONG GetSizeOnDisk2 (const TCHAR *parent_path, WIN32_FIND_DATA &fileinfo, ULONGLONG &size)
{
	ULONGLONG sizeonDisk;

	if ((fileinfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) == FILE_ATTRIBUTE_COMPRESSED)
		{
			DWORD size_low, size_hi;
			CString cs = parent_path;
			SureBackSlash (cs);
			cs += fileinfo.cFileName;
			size_low = GetCompressedFileSize (cs, &size_hi);
			//sizeonDisk = UInt32x32To64 (size_hi, (ULONGLONG)MAXDWORD+1);
			sizeonDisk = (((ULONGLONG)size_hi) << 32) + (ULONGLONG)size_low;			
		}
		else
		{
			sizeonDisk = size;
			sizeonDisk /= 4096 ;
			sizeonDisk +=  (size % 4096) != 0; // check the real value;
			sizeonDisk *= 4096;
		}
	
	return sizeonDisk;
}

bool CDirSize::GetDirInfoLock (const TCHAR *path, CDirInfo &dirinfo) {
	CGuard guard (m_SizeLock);
	
	dirinfo_it it = m_SizeMap.find ((LPCTSTR)path);
		
	if (it != m_SizeMap.end ()) {
		dirinfo = it->second;			
		return true;
	}	
	return false;
}

inline bool EqualFileTime (const FILETIME* pFT1, const FILETIME* pFT2)
{
	if ((pFT1->dwLowDateTime == pFT2->dwLowDateTime) &&
		(pFT1->dwHighDateTime == pFT2->dwHighDateTime))
		return true;

	return false;
}

bool CDirSize::GetSizeInternal (const TCHAR *folder, const TCHAR *file, ULONGLONG &size, ULONGLONG &sizeOnDisk, FILETIME &fTime)
{
	CString path = folder;

	SureBackSlash (path);

	path += file;

	CDirInfo dirInfo;
	TRACE(_T("GetSizeInternal GetDirInfoLock\n"));
	if (GetDirInfoLock(path,dirInfo))
	{
	  //if (CompareFileTime (&dirInfo.m_Time, &fTime)  == 0)
		if (EqualFileTime (&dirInfo.m_Time, &fTime))
		{
//		  TRACE (_T("Cached %s\n"), (LPCTSTR)path);
		  size = dirInfo.m_Size;
		  sizeOnDisk = dirInfo.m_SizeOnDisk;				
		  return true;
		}	
	}

	//std::vector <WIN32_FIND_DATA> array;
	//GetFolder (array, path);

	fast_array <WIN32_FIND_DATA> array (1024, 256);
	GetFolder (array, path);

	bool bHasDirs = false;

	ULONGLONG lSize = 0, lSizeodisk = 0;
	//TRACE (L"%s\n", file);
	for ( UINT i = 0; i < array.size (); i++)
	{
		bool bDir = (array[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==FILE_ATTRIBUTE_DIRECTORY;
		if (bDir)
		{
			ULONGLONG sizeTemp = 0, sizeondisktemp = 0;
			GetSizeInternal (path, array[i].cFileName, sizeTemp, sizeondisktemp, array[i].ftLastWriteTime);
			lSize += sizeTemp;
			lSizeodisk += sizeondisktemp;
			bHasDirs = true;
		}
		else 
		{
			//ULONGLONG sz = UInt32x32To64 (array[i].nFileSizeHigh, (ULONGLONG)MAXDWORD+1);	;
			//sz +=  array[i].nFileSizeLow;			
			ULONGLONG sz = (((ULONGLONG)array[i].nFileSizeHigh) << 32) + (ULONGLONG)array[i].nFileSizeLow;			
			lSize += sz;			
			lSizeodisk += GetSizeOnDisk2 (path, array[i], sz);
		}		
	}
	
	AddSize (folder, file, lSize, lSizeodisk, fTime, bHasDirs);
	size = lSize;
	sizeOnDisk = lSizeodisk;
	return true;
}

bool GetLastWriteTime (const TCHAR *path, FILETIME &writetime)
{
	WIN32_FILE_ATTRIBUTE_DATA  data;
	if (!GetFileAttributesEx (path, GetFileExInfoStandard, (void *) &data))
		return false;

	writetime = data.ftLastWriteTime;

	return true;
}


void CDirSize::AddDir (const TCHAR *path, const TCHAR *file, FILETIME &time)
{	
	CGuard guard (m_StackLock);	
	m_Stack.push_back (CDirStackEl (path, file, time));		
}

bool CDirSize::GetDir (CString &path, CString &file, FILETIME &time)
{	
	CGuard guard (m_StackLock);	
	bool ret = false;

	if (! m_Stack.empty ())
	{	
		unsigned int end = m_Stack.size () -1;
		path = m_Stack[end].m_Path;
		file = m_Stack[end].m_FileName;
		time = m_Stack[end].m_Time;
		m_Stack.pop_back ();
		ret = true;
	}	
	return ret;
}

void CDirSize::Clear ()
{	
	CGuard guard (m_StackLock);	
	m_Stack.clear ();	
}


void CDirSize::Notify (int msg, WPARAM wp, LPARAM lp)
{
	for (unsigned int i = 0; i < m_Wnds.size (); i++)
		::SendMessage (m_Wnds[i], msg, wp, lp);
		//	::PostMessage (m_Wnds[i], msg, wp, lp);

	for (unsigned int i = 0; i < m_Views.size (); i++)
		switch (msg)
		{			
			case WM_EXP_SYNC: m_Views[i]->OnSync (wp, lp); break;
			case WM_EXP_UPDATE: m_Views[i]->OnFolderUpdate (wp, lp); break;
			case WM_EXP_CLIPCHANGED: m_Views[i]->OnClipChanged (wp, lp); break;
			case WM_EXP_RECYCLEUPDATE: m_Views[i]->OnRecBinUpdate (wp, lp); break;
			case WM_EXP_FILERENAMED: m_Views[i]->OnFileRenamed (wp, lp); break;
			case WM_EXP_OPTIONS: m_Views[i]->OnOptions (wp, lp); break;
			case WM_EXP_HARDUPDATE: m_Views[i]->OnHardUpdate (wp, lp); break;
			case WM_EXP_SETSTATE: m_Views[i]->OnSetState (wp, lp); break;
		}
}

void CDirSize::Cycler ()
{
	static CString path, file;
	ULONGLONG size, sizeOndisk;
	bool bShowReady = false;
	static FILETIME fTime = {0,0};
	while (true)
	{
		if (!IsNotInFOP())
			return;

		if (!m_StackLock.isLocked())
			if (GetDir (path, file, fTime))
			{	
				TRACE(_T("Cycler GetSizeInternal %s %s\n"), path, file); 	
				GetSizeInternal (path, file, size, sizeOndisk, fTime);
				bShowReady = false;
			}
			else
			{
				if (!bShowReady)
					bShowReady = true;			
				TRACE(_T("Cycler Thread Ended\n")); 	
				return;
			}		
	}

	TRACE(_T("Cycler Thread Ended\n")); 	
}


void CDirSize::AddSize (const TCHAR* folder, const TCHAR* name , ULONGLONG size, ULONGLONG sizeondisk, FILETIME &fTime, bool bSub)
{
	CDirInfo dr_info (size, sizeondisk, fTime, bSub);
	CString path = folder;
	SureBackSlash (path);
	CDirStackEl dir (path, name, fTime);

	path += name;		

	TRACE(_T("Add Size - %s %d %d\n"), (LPCTSTR)path, fTime.dwHighDateTime, fTime.dwLowDateTime);
	{
		CGuard guard (m_SizeLock);

		std::pair <dirinfo_it, bool> ret =  m_SizeMap.insert (dirinfo_value((LPCTSTR)path, dr_info));
		if (ret.second == false)
			ret.first->second  = dr_info;
	}
	

	Notify (WM_EXP_SYNC, NULL, (LPARAM)&dir);	
}



bool CDirSize::GetSizeLight (const TCHAR *folder, const TCHAR *file, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime)
{	
	CString path = folder;
	SureBackSlash (path);
	
	path += file;
	CDirInfo dirinfo;

	if (GetDirInfoLock(path, dirinfo))
	{
		if (EqualFileTime (&dirinfo.m_Time, &fTime))
		{
			size = dirinfo.m_Size;
			sizeondisk	 = dirinfo.m_SizeOnDisk;
			return true;
		}
	}

	return false;
}

bool CDirSize::GetSize (const TCHAR *folder, const TCHAR *file, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime)
{		
	CString path = folder;

	SureBackSlash (path);
	
	path += file;
	
	CDirInfo dirinfo;

	if (GetDirInfoLock(path, dirinfo))
	{
		if (EqualFileTime (&dirinfo.m_Time, &fTime))
		{
			size = dirinfo.m_Size;
			sizeondisk	 = dirinfo.m_SizeOnDisk;	
			return true;
		}		
	}	
	AddDir (folder, file,fTime);	
	return false;
}

void RemoveBackSlash (CString &cs)
{
	cs.TrimRight(SLASH);
}

bool CDirSize::GetSizeLight (const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeOnDisk, FILETIME &fTime)
{
	CString path = folder;
	RemoveBackSlash (path);
	
	CDirInfo dirinfo;

	if (GetDirInfoLock(path, dirinfo))
	{		
		if (EqualFileTime (&dirinfo.m_Time, &fTime))
		{
			size = dirinfo.m_Size;
			sizeOnDisk	 = dirinfo.m_SizeOnDisk;
			return true;
		}
	}
	return false;
}

void CDirSize::AddWindow (HWND hWnd)
{	
	CGuard guard (m_wndLock);	
	m_Wnds.push_back (hWnd);	
}

void CDirSize::AddWindow (CExplorerXPView *pView)
{
	CGuard guard (m_wndLock);	
	m_Views.push_back (pView);	
}

void CDirSize::DeleteWindow (CExplorerXPView *pView)
{
	CGuard guard (m_wndLock);	

	for (unsigned int i = 0; i < m_Views.size (); i++) 
	{
		if (pView == m_Views[i])
		{
			m_Views.erase(m_Views.begin()+i);
			break;
		}	
	}
}

void CDirSize::DeleteWindow (HWND  hWnd)
{

}


void CDirSize::Start ()
{	
	CDriveArray ar;
	GetDrives (ar);
	for (unsigned int i = 0; i < ar.size (); i++)
	{
		bool bFlopy = false;
		CString type = ar[i].m_Type;
		type.MakeLower ();

		if (type.Find (_T("floppy")) != -1)
			bFlopy = true;		

		if (!bFlopy && (ar[i].m_nType == DRIVE_FIXED || ar[i].m_nType == DRIVE_REMOVABLE))
		{
			CDirSizeChangeHandler *pHandler = new CDirSizeChangeHandler (this, ar[i].m_Path);
			m_Watcher.WatchDirectory (ar[i].m_Path, 
									  FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_DIR_NAME, 
									  pHandler, 
									  TRUE);
		
			m_Handlers.push_back (pHandler);
		}		
	}
}

void CDirSize::Stop ()
{
	m_Watcher.UnwatchAllDirectories();
	for (unsigned int i = 0; i < m_Handlers.size (); i++)
	{		
		//if (GetDriveType (m_Handlers[i]->m_Path) == DRIVE_FIXED || GetDriveType (m_Handlers[i]->m_Path) == DRIVE_REMOVABLE)	
		delete m_Handlers[i];
	}
}

void Split (CString &path, CString &parent, CString &name)
{
	int len = path.GetLength ();

	if (!len)
		return;

	if (path[len-1] == '\\')
	{
		path.Delete (len-1);
		len --;
	}
	
	int pos = path.ReverseFind ('\\');

	if (!pos)
		return;

	parent = path.Left (pos);
	name = path.Right (len - pos-1);
}

void CDirSize::InvalidateName (const TCHAR *old_name, const TCHAR *new_name) {
	{
		CGuard guard (m_SizeLock);

		CString path = old_name;

		//SureBackSlash (path);

		dirinfo_it it = m_SizeMap.find ((LPCTSTR)path);
		if (it != m_SizeMap.end ()) {
			CString parent, name;
			CDirInfo dr_info = it->second;
			m_SizeMap.erase (it);
			dirinfo_it it2 = m_SizeMap.find (new_name);

			if (it2 != m_SizeMap.end ())
				m_SizeMap.erase(it2);

			m_SizeMap.insert (dirinfo_value(new_name, dr_info));
		}
	}

	RENAMEFILESTRUCT rnStruct = {old_name, new_name};
	Notify (WM_EXP_FILERENAMED, 0, (LPARAM)&rnStruct);
}


void CDirSize::Invalidate (const TCHAR *invalidate_path, bool bDelete) {
		
	CString path = invalidate_path;
		
	// ignore Windows\config\software.log
	if (path.CompareNoCase (csSoftwareLogPath) == 0) 		
		return;
		
	CString parent, name;

	if (bDelete) {
		Split (path, parent, name);

		CGuard guard(m_SizeLock);

		dirinfo_it it = m_SizeMap.find ((LPCTSTR)path);
		if (it != m_SizeMap.end ())		
			m_SizeMap.erase (it);
		path = parent;
	}

	if (path.GetLength () > 3)
		do
		{
			Split (path, parent, name);

			// ignore Windows\Prefetch
			if (path.CompareNoCase(csWindowsPrefetch) == 0) 							
				return;			
				
			CGuard guard(m_SizeLock);
			dirinfo_it it = m_SizeMap.find ((LPCTSTR)path);
			if (it != m_SizeMap.end ())
			{	
				CGuard guard2 (m_StackLock);	
				m_SizeMap.erase (it);				
				if (!IsInStack (parent, name)) {
					FILETIME fTime;
					GetLastWriteTime (parent, fTime);
					m_Stack.push_back (CDirStackEl (parent, name, fTime));
				}				
			}
			path = parent;
		} while (path.GetLength () > 3);
	

	gDriveViewer.MarkForUpdate ();
	Notify (WM_EXP_UPDATE, 0, (LPARAM)invalidate_path);	
}

bool CDirSize::IsInStack (const TCHAR *path, const TCHAR *name) {

	for (unsigned int i = 0; i < m_Stack.size (); i++) {
		if (!_tcsicmp (name, m_Stack[i].m_FileName))
			if (!_tcsicmp (path, m_Stack[i].m_Path))
				return true;
	}	

	return false;
}


void CDirSize::NotifyViews (const TCHAR *filename)  {	
	Notify (WM_EXP_UPDATE, 0, (LPARAM)filename);	
}

bool CDirSize::IsStackEmpty () {	
	CGuard guard (m_StackLock);	
	bool ret = m_Stack.size () == 0;
	return ret;
}

struct StackSort 
{
	bool operator () (const CDirStackEl &el1, const CDirStackEl &el2) 
	{ 
		return el1.m_Path.GetLength() < el2.m_Path.GetLength();
	}
};

void CDirSize::SortStack ()
{
	CGuard guard (m_StackLock);	
	std::sort (m_Stack.begin(), m_Stack.end(), StackSort());
}

void CDirSize::ClearStack ()
{
	CGuard guard (m_StackLock);	
	m_Stack.clear();
}




void CDirSize::Save ()
{	
	TRFUN (_T("DirSize:Save"));

	CSimpleFile file (GetLocalFile("DirSizeCache", "dat"), "w+b");
	
	if (! file.isValid())	
		return;

	CGuard guard (m_SizeLock);

	dirinfo_it 	it = m_SizeMap.begin(); 

	while (it != m_SizeMap.end()) {		
		if (!it->second.m_bSubFolders)
		{
			CString tmp = it->first;
			dir_cache cache;
			ZeroMemory (&cache, sizeof (dir_cache));
			cache.dirinfo = it->second;
			_tcsncpy (cache.folder, it->first, MAX_FOLDER_SIZE);
			file.write (&cache, sizeof (dir_cache),1);
		}
		++it;
	}			
}

void CDirSize::GetDirInfoArrray (CDirInfoArray & arr)
{
	CGuard guard (m_SizeLock);

	dirinfo_it 	it = m_SizeMap.begin(); 
	dir_cache cache;
	while (it != m_SizeMap.end()) {			
		{
			ZeroMemory (&cache, sizeof (dir_cache));
			cache.dirinfo = it->second;
			_tcsncpy (cache.folder, it->first, MAX_FOLDER_SIZE);
			arr.push_back (cache);		
		}		
		++it;
	}	
}


void CDirSize::Load () {	

	InOut log (_T("DirSize:Load"));

	CSimpleFile file (GetLocalFile("DirSizeCache", "dat"), "rb");
	
	if (!file.isValid())
		return;

	CGuard guard (m_SizeLock);
	dir_cache cache;
	while (file.read (&cache, sizeof(dir_cache), 1) == 1) {	
		int size = _tcslen (cache.folder);

		if (size)
			m_SizeMap.insert (dirinfo_value(cache.folder, cache.dirinfo));
	}	
}
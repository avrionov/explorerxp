/*  Copyright 2002-2020 Nikolay Avrionov. All Rights Reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
*/

#ifndef __DRIVEVIEWER_H__
#define __DRIVEVIEWER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Viewer.h"
#include "globals.h"
#include "MyLock.h"


class CDriveViewer : public CViewer  
{
public:
	CDriveViewer();
	virtual ~CDriveViewer();
	void Fill (const TCHAR *root) ;
	void GetDrives ();
	const TCHAR * GetTitle () ;
	void GridCallBack (GV_DISPINFO *pDispInfo) ;
	void Sort () ;
	bool Sync (const TCHAR* folder , const TCHAR *name) ;
	//int GetColumnCount () { return 4;};
	int GetCount ();
	bool GetObjectSize (int iRow, ULONGLONG &size, ULONGLONG &sizeOnDisk);

	bool CanPaste () { return false;};
	bool CanCopy ()  { return true;};
	bool CanCut ()   { return false;};
	bool CanDelete ()   { return false;};
	bool CanUp () { return false;};
	bool CanExecute () { return true;};
	bool CanDrag()  { return false;};

	CString GetText (int row, int col);

	virtual bool CanDropToView () { return false;};
	virtual bool CanDropToSubs () { return true;};

	void MarkForUpdate () { m_bNeedUpdate = true;}
	void CheckUpdate ();
	
	bool CanChangeTo (int iRow) { return true;};
	CString  GetPath (int iRow) { return m_Array[iRow].m_Path;};
  CString  GetPath(int iRow, bool &bFolder) { bFolder = false;  return m_Array[iRow].m_Path; }; // ToDo check this

	const TCHAR*  GetName (int iRow) { return m_Array[iRow].m_Path;};

	void getDrives (CDriveArray& drives_array);
	void setDrives (const CDriveArray& drives_array);

	void OnClose ();

	CDriveArray m_Array;
	void FillHeader();
	CString & GetDriveInfoStr(const TCHAR *path);

	bool			m_bNeedUpdate;
	bool			m_bGetState;
	CMyLock		m_lock;

};

extern CDriveViewer gDriveViewer;

#endif // __DRIVEVIEWER_H__

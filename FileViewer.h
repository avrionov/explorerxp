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

#ifndef __FILEVIEWER_H__
#define __FILEVIEWER_H__

#pragma once

#include "Viewer.h"
#include "FileInfo.h"
#include "fast_array.h"

class CFileViewer : public CViewer  
{
public:
	CFileViewer();
	virtual ~CFileViewer();
	void Fill (const TCHAR *root) ;
	const TCHAR * GetTitle () ;
	void GridCallBack (GV_DISPINFO *pDispInfo) ;
	void Sort () ;
	bool Sync (const TCHAR* folder , const TCHAR *name) ;
	bool CanChangeTo (int iRow) ;
	CString  GetPath (int iRow) ;
  CString  GetPath(int iRow, bool& bFolder);
	const TCHAR *  GetName (int iRow) ;
	
	int GetCount () { return m_Array.size ();}
	bool GetObjectSize (int iRow, ULONGLONG &size, ULONGLONG &sizeOnDisk);

	bool CanPaste () { return true;};
	bool CanCopy ()  { return true;};
	bool CanCut ()   { return true;};
	bool CanDelete ()   { return true;};
	bool CanUp () { return true;};
	bool CanExecute () { return true;};
	bool CanDrag () { return true;};

	bool CanDropToView () { return true;};
	bool CanDropToSubs () { return true;};
	CString GetText (int row, int col);
		
	virtual void Rename (CString &oldName, CString &newName);

	void	GetFolder (fast_array <CFileInfo> &array, const TCHAR *path);
	boolean CalculateSizes (const TCHAR *path);
	void	FillHeader();
	void	OnClose ();

	protected:
	fast_array <CFileInfo> m_Array;
	CString m_Root;
	bool m_bCheckForCutFiles;
};

#endif // __FILEVIEWER_H__
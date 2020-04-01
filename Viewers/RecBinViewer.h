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

#ifndef __RECBIN_VIEWER_H__
#define __RECBIN_VIEWER_H__

#pragma once

#include "Viewer.h"

typedef IShellDetails	FAR*	PSHELLDETAILS;
typedef IShellFolder2	FAR*	LPSHELLFOLDER2;


typedef struct {
	DWORD dwItem1;
	DWORD dwItem2;
} SHNOTIFYSTRUCT;


struct SHFILEINFOBYTE{
	long hIcon;
	long iIcon;
	long dwAttributes;
	byte szDisplayName[MAX_PATH];
	byte szTypeName[80];
};



class CRecycleItem 
{
	public:
		CRecycleItem (CStringAr &ar, LPITEMIDLIST pidl = NULL)
			:m_PIDL (pidl)
		{
			m_Ar.assign ( ar.begin (), ar.end ());
		};

		const CString &operator [] (int pos) const { return m_Ar[pos];};
		
	LPITEMIDLIST m_PIDL;
	CStringAr m_Ar;
};


typedef std::vector <CRecycleItem> CFileList;


class CRecBinViewer : public CViewer  
{
public:
	CRecBinViewer();
	virtual ~CRecBinViewer();
    
	void Fill (const TCHAR *root) ;
	const TCHAR * GetTitle () ;
	void GridCallBack (GV_DISPINFO *pDispInfo) ;
	void Sort () ;
	bool Sync (const TCHAR* folder , const TCHAR *name) ;
	bool CanChangeTo (int iRow) ;
	CString  GetPath (int iRow) ;
  CString  GetPath(int iRow, bool &bFolder);

	const TCHAR*  GetName (int iRow) ;
	//int GetColumnCount () { return m_Headers.size ();};
	int GetCount () { return m_List.size ();}
	bool GetObjectSize (int iRow, ULONGLONG &size, ULONGLONG &sizeOnDisk);

	bool CanPaste () { return false;};
	bool CanCopy ()  { return false;};
	bool CanCut ()   { return false;};
	bool CanDelete ()   { return true;};
	bool CanUp () { return false;}
	bool CanExecute () { return false;}
	bool CanDrag () { return false;};
	void Empty ();

	bool CanDropToView () { return true;};
	bool CanDropToSubs () { return false;};
	CString GetText (int row, int col);
	
	bool OnDrop (COleDataObject *object, DROPEFFECT dwEffect , const TCHAR *pTo);

	void Install (HWND hWnd);
	void Remove  ();
	BOOL ExecCommand (int iItem, LPCTSTR lpszCommand);
	BOOL ExecCommandOnSelection (LPCTSTR lpszCommand);
	BOOL ExecCommand (LPCONTEXTMENU pCtxMenu,  LPCTSTR lpszCommand);
	void UndeleteAll ();
	void Undelete (int iPos);
	void DeleteSelection ();
	
	afx_msg LRESULT OnShellNotify (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyRBinDir (WPARAM wParam, LPARAM lParam);

	DROPEFFECT CalcDropEffect (CSelRowArray &ar, const TCHAR * root, const TCHAR * drop_source,  DWORD dwKeyState) { return DROPEFFECT_MOVE;};
		
	void MarkForUpdate () { m_bNeedUpdate = true;}
	void CheckUpdate ();

	void ContextMenu (CView *pView, CSelRowArray &ar, CPoint &pt);
	void Properties (CSelRowArray &ar);
	void FillHeader();
	void OnClose ();

//protected:
public:

   
protected:

	void InitInterfaces (void);
	void UpdateList (void);
	void GetName (STRRET str, CString &cs);
	BOOL GetFolder (void);
	void HeaderFolder (void);
	void FillFolder (void);
	BOOL GetFolder2 (void);
	void HeaderFolder2 (void);
	void FillFolder2 (void);

	void InstallRBinNotify ();
	void InstallShellNotify ();

	void RemoveRBinNotify ();
	void RemoveShellNotify ();
	void RemovePIDL ();
	void EmptyHelper();


    HWND m_hWnd;
//	std::vector <CFileInfo> m_Array;
	CString m_Root;
          
	ULONG			m_hNotifyRBin;
	ULONG			m_hNotifyDrives[32];
	LPITEMIDLIST	m_pidlDrives[32];
	LPSHELLFOLDER2	m_pFolder2;
	LPSHELLFOLDER	m_pRecycleBin;
    CFileList       m_List;
	bool			m_bNeedUpdate;
};

extern CRecBinViewer gRecBinViewer;

#endif // __RECBIN_VIEWER_H__

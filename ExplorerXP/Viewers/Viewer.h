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


#ifndef __VIEWER_H__
#define __VIEWER_H__

#pragma once


#include "GridCtrl_src/GridCtrl.h"
#include "globals.h"
#include "FolderStateManager.h"

class CViewer  
{
public:
	CViewer();
	virtual ~CViewer();
	virtual void Fill (const TCHAR *root) = 0;
	virtual const TCHAR * GetTitle () = 0;
	virtual void GridCallBack (GV_DISPINFO *pDispInfo) =0;
	virtual void Sort () = 0;
	virtual bool Sync (const TCHAR* folder , const TCHAR *name) = 0;
	virtual bool CanChangeTo (int iRow)=0;
	virtual CString  GetPath (int iRow)=0;
  virtual CString  GetPath(int iRow, bool &bFolder) = 0;
	virtual const TCHAR*  GetName (int iRow)=0;
	virtual int GetCount () = 0;
	virtual bool GetObjectSize (int iRow, ULONGLONG &size, ULONGLONG &sizeOnDisk) = 0;
// base folder operation
	virtual bool CanPaste () = 0;
	virtual bool CanCopy ()  = 0;
	virtual bool CanCut ()  = 0;
	virtual bool CanDelete ()  = 0;
	virtual bool CanUp () = 0;
	virtual bool CanExecute () = 0;
	virtual bool CanDropToView () = 0;
	virtual bool CanDropToSubs () = 0;
	virtual bool CanDrag() = 0;
	virtual CString GetText (int row, int col) = 0;
	virtual void Rename (CString &oldName, CString &newName) {};
	virtual void ContextMenu (CView *pView, CSelRowArray &ar, CPoint &pt);
	virtual void Properties (CSelRowArray &ar);
	virtual void ContextMenuOnHeader(CView *pView, CPoint &pt, NM_GRIDVIEW* pItem);
	virtual void FillHeader() = 0;
	virtual void SetupGrid ();
	virtual void OnClose ();
	virtual void SaveState(const TCHAR *name);
	virtual void GetState (CFolderState &state);
	virtual void SetState (CFolderState &state);
// virtual 
	
	virtual DROPEFFECT CalcDropEffect (CSelRowArray &ar, const TCHAR * root, const TCHAR * drop_source,  DWORD dwKeyState);
	virtual bool OnDrop (COleDataObject *object, DROPEFFECT dwEffect , const TCHAR *pTo);
	
	void SetGrid (CGridCtrl*pGrid) { m_pGridCtrl = pGrid;}
	void SetSortColumn (int nColumn) { m_State.m_nSortColumn = nColumn;}
	void SetSortDir (bool bDir)  { m_State.m_bAscending = bDir;	}
	virtual size_t GetColumnCount () { return m_Headers.size () -1;};
	
	COLORREF GetSelectedColorBackground ();
	COLORREF GetSelectedColorText();
	
protected:
	CGridCtrl* m_pGridCtrl;
	CStringAr m_Headers;

	CFolderState m_State;
};

#endif // __VIEWER_H__

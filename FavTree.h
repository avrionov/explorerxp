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


#pragma once

#include "TreeCtrlEx.h"
#include "groupmanager.h"
#include "GenericDropTarget.h"

#define baseTree CTreeCtrlEx


class CFavTree : public baseTree, public CGenericDropClass
{
//	DECLARE_DYNAMIC(CFavTree)

public:
	CFavTree();
	virtual ~CFavTree();
	void GetOpen ();
	void Reload ();
	void AddGroup (CGroup &group);
	bool m_bInit;
	void Init ();
	int m_FavIndex;
	int m_FolderIndex;
	bool m_FoldersOnly;

protected:
	DECLARE_MESSAGE_MAP()
	CGenericDropTarget m_Target;
	HTREEITEM m_hLastDropItem;
	CSelRowArray m_DropFiles;
	CString m_LastDropFolder;
	bool m_bDragDataAcceptable;
	CString m_DropFolder;
	
public:
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);

	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual HWND GetSafeHwnd ();	


};

extern CFavTree gFavs;


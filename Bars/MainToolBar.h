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

#ifndef __MAINTOOLBAR_H_
#define __MAINTOOLBAR_H_

#pragma once

#include "ToolbarEx.h"
#include  "MainBarDropTarget.h"
#include "globals.h"

class CMainToolBar : public CToolBarEx
{
    //DECLARE_DYNAMIC( CMainToolBar )
public:
    CMainToolBar();
	
// Overrides
public:
    virtual void Init();

	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	int HitTestButton (CPoint &pt);
	
protected:
    virtual bool HasButtonText( UINT nID );
	bool PassThroughTip (UINT nID);
	CMainBarDropTarget  m_DropTarget;

	CSelRowArray m_DropFiles;	
	CString m_DropFolder;
	CString m_LastDropFolder;	
};



#endif //__MAINTOOLBAR_H_

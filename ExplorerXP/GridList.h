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


#ifndef __GRID_LIST_H__
#define __GRID_LIST_H__

#pragma once

#include "GRIDCTRL_SRC\GridCtrl.h"

class CGridList : public CGridCtrl  
{
public:
	CGridList();
	virtual ~CGridList();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL SetVertScroll (int nPos, BOOL bRedraw = TRUE) { return SetScrollPos32(SB_VERT, nPos, bRedraw );}
	virtual void OnBeginDrag();
	int HitTest (CPoint &pt);
	CString m_LookStr;

	DECLARE_MESSAGE_MAP()

};

#endif // __GRID_LIST_H__

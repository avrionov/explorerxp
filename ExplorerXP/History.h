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


#ifndef __HISTORY_H__
#define __HISTORY_H__

#pragma once

#include "globals.h"


class CHistoryObj 
{
	public:
	CHistoryObj (const TCHAR *path, int pos, CSelRowArray &sel)
	{
		m_Text = path;
		m_iPos = pos;
		m_Selection = sel;
	};
	
	CString m_Text;
	int m_iPos;
	CSelRowArray m_Selection;
	
};

typedef std::vector <CHistoryObj> CHistoAr;
typedef CHistoAr::iterator CHistoIt;

class CHistory  
{
public:
	CHistory();
	virtual ~CHistory();

	bool CanBack ();
	bool CanFore ();

	void MoveForward (int amount, CString &cs, int &iPos, CSelRowArray &sel);
	void MoveBack (int amount, CString &cs, int &iPos,  CSelRowArray &sel, const TCHAR *current_path, int curent_pos,  CSelRowArray &_current_sel);

	void Push (const TCHAR *path, int iPos, CSelRowArray &sel);
	void GetForeAr (CHistoAr &ar);
	void GetBackAr (CHistoAr &ar);

protected:
	CHistoAr	m_Ar;
	int m_iPos;
	int m_ForeSize;
	int m_BackSize;

};

#endif // __HISTORY_H__

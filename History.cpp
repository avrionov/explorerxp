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

#include "stdafx.h"
#include "explorerxp.h"
#include "History.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHistory::CHistory()
:m_iPos (0),
m_ForeSize(0),
m_BackSize(0)
{
	
}

CHistory::~CHistory()
{

}

bool CHistory::CanBack ()
{
	return m_BackSize >= 1;
}

bool CHistory::CanFore ()
{
	return m_ForeSize != 0;
}

void CHistory::MoveForward (int amount,  CString &cs, int &iPos, CSelRowArray &sel)
{
	if (!CanFore ())
		return;


	m_iPos += amount;
	
	cs = m_Ar[m_iPos].m_Text;
	iPos = m_Ar[m_iPos].m_iPos;
	sel = m_Ar[m_iPos].m_Selection;

	m_BackSize += amount ;
	m_ForeSize -= amount ;

}

void CHistory::MoveBack (int amount, CString &cs, int &iPos,   CSelRowArray &sel, const TCHAR *current_path, int curent_pos, CSelRowArray &cur_sel)
{
	if (!CanBack ())
		return;

	if (m_ForeSize == 0)
	{
		CHistoryObj obj (current_path, curent_pos, cur_sel);

		if (m_iPos < static_cast<int>(m_Ar.size()))
			m_Ar[m_iPos] = obj;
		else
			m_Ar.push_back (obj);
	}
	
	m_iPos -= amount;

	cs = m_Ar[m_iPos].m_Text;
	iPos = m_Ar[m_iPos].m_iPos;
	sel = m_Ar[m_iPos].m_Selection;

	m_BackSize -= amount;
	m_ForeSize += amount;
}

void CHistory::Push (const TCHAR *path, int iPos,  CSelRowArray &sel)
{
	CHistoryObj obj (path, iPos, sel);

	if (m_iPos < static_cast<int>(m_Ar.size()))
		m_Ar[m_iPos] = obj;
	else
		m_Ar.push_back (obj);
		
	m_BackSize++;
	m_iPos ++;
	m_ForeSize = 0;
	
}

void CHistory::GetForeAr (CHistoAr &ar)
{
	int iFore = m_ForeSize;
	int iPos = m_iPos+1;
	
	ar.clear ();

	while ( (iFore != 0) && (iPos < static_cast<int>(m_Ar.size())))
	{
		ar.push_back (m_Ar[iPos]);
		iPos++;
		iFore--;
	}
}

void CHistory::GetBackAr (CHistoAr &ar)
{
	int iBack = m_BackSize;
	int iPos = m_iPos;

	while (iBack != 0)
	{
		iBack--;
		iPos --;
		TRACE (_T("%d %s\n"), m_Ar[iPos].m_iPos, (LPCTSTR)m_Ar[iPos].m_Text);
		ar.push_back (m_Ar[iPos]);
	}
}
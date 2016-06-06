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
#include "FileFilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilter::CFilter()
{

}

CFilter::~CFilter()
{

}

const TCHAR *szSection = _T("Filters");
const TCHAR *szCount = _T("FilterCount");
const TCHAR *szFilterName= _T("FilterName%d");
const TCHAR *szFilter=_T("Filter%d");
	
void CFilter::setName (const TCHAR *name)
{
	m_Name = name;
}

void CFilter::addExt  (const TCHAR *ext)
{
	CString ext_str (ext);
	ext_str.TrimLeft ();
	ext_str.TrimRight ();
	if (!ext_str.IsEmpty ())
	{
		m_Ext.push_back (ext_str);
		removeDups ();
	}	
}

const TCHAR * CFilter::getName () const 
{
	return m_Name;
}

void CFilter::getExts (CString & ext)
{
	ext.Empty ();
	for (unsigned int i = 0; i < m_Ext.size (); i++)
	{
		ext += m_Ext[i];
		if (i != (m_Ext.size () - 1))
			ext += "; ";
	}
}

void CFilter::parseExt (const TCHAR *ext_string)
{
	CString ext_str = ext_string;
	int pos = ext_str.Find (_T(";"));
	while (pos != -1)
	{
		CString tmp = ext_str.Left (pos);		
		addExt (tmp);
		ext_str.Delete (0, pos+1);
		pos = ext_str.Find (_T(";"));
	}
	addExt (ext_str);	
}

void CFilter::clearExt ()
{
	m_Ext.clear ();
}

int CFilter::size ()
{
	return m_Ext.size ();
}

void CFilter::removeDups ()
{
	std::sort (m_Ext.begin (), m_Ext.end ());

	loop:
    if ( m_Ext.size () > 0)
	{
		for( unsigned int i = 0;  i < m_Ext.size () -1; i++)
		{
			if (m_Ext[i].CompareNoCase (m_Ext[i+1]) == 0)
			{				
				m_Ext.erase(m_Ext.begin () + i+1 );
				goto loop;
			}
		}
	}
}

void CFilter::deleteExt (int pos)
{
	if (pos > static_cast<int>(m_Ext.size ()))
		return;

	if (pos < 0)
		return;

	m_Ext.erase (m_Ext.begin () + pos);
}

void CFilter::setExt (int pos, const TCHAR *ext)
{
	if (pos > static_cast<int>(m_Ext.size ()))
		return;

	if (pos < 0)
		return;

	m_Ext[pos] = ext;
}
	
//----------------- FilterMan -----------------------------------------

void CFilterMan::addDefault ()
{
	addFilter (_T("C++ Temp Files"), _T("*.obj; *.pch; *.pdb; *.ilk; *.exp; *.res; *.trg; *.idb; *.tmp; *.plg; *.log; *.ncb; *.aps; *.bsc; *.sbr"));
	addFilter (_T("HTML Files"), _T("*.js; *.jsp; *.php; *.swf; *.asx; *.gif"));
}

void CFilterMan::writeString(const TCHAR *entry, const TCHAR* string)
{
	AfxGetApp()->WriteProfileString (szSection, entry, string);
}

void CFilterMan::writeInt (const TCHAR *entry, int number)
{
	AfxGetApp()->WriteProfileInt (szSection, entry, number);
}

int  CFilterMan::getInt (const TCHAR *entry)
{
	return 	AfxGetApp()->GetProfileInt(szSection, entry, -1);
}

CString CFilterMan::getString (const TCHAR *entry)
{
  	return AfxGetApp()->GetProfileString(szSection, entry);
}

int CFilterMan::size ()
{
	return m_Filters.size ();
}

void CFilterMan::save ()
{
	writeInt (szCount, m_Filters.size ());
	for (unsigned int i = 0; i < m_Filters.size (); i++)
	{
	   CString name; name.Format (szFilterName, i);
	   CString filter; filter.Format (szFilter, i);
	   writeString (name, m_Filters[i].getName());
	   CString exts;
	   m_Filters[i].getExts (exts);	
	   writeString (filter, exts);
	}
}

void CFilterMan::load ()
{
	m_Filters.clear ();
	
	int count = getInt (szCount);
	for (int i = 0; i < count; i++)
	{
		CString name; name.Format (szFilterName, i);
	    CString filter; filter.Format (szFilter, i);
		CString name_value = getString (name);
		CString filter_value = getString (filter);
		if (!name_value.IsEmpty () && !filter_value.IsEmpty ())
			addFilter (name_value, filter_value);
	}
	
	if (m_Filters.size () == 0)
		addDefault ();
}

void CFilterMan::addFilter (const TCHAR *name, const TCHAR *ext)
{
	CFilter flt;
	flt.setName (name);
	flt.parseExt (ext);
	m_Filters.push_back (flt);
	removeDups ();
}

void CFilterMan::addFilter (CFilter &filter)
{
	m_Filters.push_back (filter);
	removeDups ();
}

void CFilterMan::removeDups ()
{
	std::sort (m_Filters.begin (), m_Filters.end ());

	loop:
    if ( m_Filters.size () > 0)
	{
		for( unsigned int i = 0;  i < m_Filters.size () -1; i++)
		{
			if (m_Filters[i] == m_Filters[i+1])
			{
				
				m_Filters.erase(m_Filters.begin () + i+1 );
				goto loop;
			}
		}
	}
}


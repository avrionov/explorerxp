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

#ifndef __FILTER_H__
#define __FILTER_H__

#pragma once

class CFilter  
{
public:
	CFilter();
	virtual ~CFilter();
	
	void setName (const TCHAR *);
	void addExt  (const TCHAR *);
	const TCHAR * getName () const;
	void getExts (CString & ext);
	void parseExt (const TCHAR *);
	void clearExt ();
	void deleteExt (int pos);
	void setExt (int pos, const TCHAR *ext);
	int size ();
	void removeDups ();
	bool operator < (const CFilter &filter) { return m_Name <= filter.getName (); }
	bool operator== (const CFilter &filter) { return m_Name.CompareNoCase (filter.getName ()) == 0;};
	CString & operator [] (int i ) { return m_Ext[i];}
	protected:
		CString m_Name;
		std::vector <CString> m_Ext;

};

typedef std::vector <CFilter> CFilterAr;
typedef CFilterAr::iterator CFilterIt;

class CFilterMan 
{	
public:
	int size ();
	void save ();
	void load ();
	void addFilter (const TCHAR *name, const TCHAR *ext);
	void addFilter (CFilter &filter);
	void removeDups ();
	CFilter & operator [] (int i ) { return m_Filters[i];}
	protected:
		CFilterAr m_Filters;	
   void addDefault ();
   void writeString (const TCHAR *entry, const TCHAR* string);
   void writeInt (const TCHAR *entry, int number);
   int  getInt (const TCHAR *entry);
   CString getString (const TCHAR *entry);
};

extern CFilterMan gFilterMan;


#endif // __FILTER_H__

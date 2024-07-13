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


#ifndef __PROPCONTAINER_H__
#define __PROPCONTAINER_H__

#pragma once

#include "Globals.h"

#include <fstream>

class PropSection
{
	public:
		PropSection (const TCHAR* name);
		void clear ();
		CString  get_name ();
		void addTilte (const TCHAR * titleName);
		void addTilte (int titleID);
		void addPair (int valueID, const TCHAR* value);
		void addValues (int count, ...);
		void addValue (const TCHAR* label, DWORD value);
		void save (std::wofstream &file);
		bool load (std::wifstream &file);
		void loadStrAr (CString &line, CStringAr &ar);
		void setDelim (const TCHAR * delim);
		CString getDelim ();
		CStringAr & titles () { return _titles;}
		std::vector <CStringAr> & values () { return _values;};
		
		void getAsStr (CString &str);

	protected:
		CString _name;
		int		 _imageID;
		CStringAr _titles;
		std::vector <CStringAr>  _values;
		CString _delim;
};

typedef std::vector <PropSection*> SectionArray;

class PropContainer  
{
public:
	PropContainer();
	virtual ~PropContainer();
	PropSection* addSection (int secName);	
	PropSection* addSection (const TCHAR* secName);
	PropSection* addSectionSimple (const TCHAR* secName);
	PropSection* findSection (int secName);
	PropSection* findSection (const TCHAR * secName);
	size_t size () {return _sections.size ();};
	PropSection& operator [] (int iPos) { return *_sections[iPos];	}
	void clear ();
	void save (const char * fileName);
	void load (const char * fileName);
	void setDelim (const TCHAR * delim);
	CString getDelim ();

	void getAsStr (CString &str);

protected:
	CString _delim;
	SectionArray _sections;

};

#endif //__PROPCONTAINER_H__

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
#include "PropContainer.h"
#include "SimpleFile.h"


#define EOLN "\r\n"
 
// ---------------------------  prop section -----------------------------------

PropSection::PropSection (const TCHAR * name) {
	_name = name;
}

void PropSection::clear () {
	_titles.clear ();
	_values.clear ();
}

void PropSection::addTilte (const TCHAR * titleName) {
	_titles.push_back (titleName);
}


CString  PropSection::get_name () {
	return _name;
}


void PropSection::addValues (int count, ...) {
	 
   va_list marker;

   va_start( marker, count );  

   CStringAr  strArr;
   while( count )
   {
      count--;
      const TCHAR *value = va_arg( marker, const TCHAR *);
	  strArr.push_back (value);

   }
   va_end( marker );    
   _values.push_back (strArr);
}

void PropSection::addValue (const TCHAR* label, DWORD value)
{
	CStringAr  strArr;
	strArr.push_back (label);
	TCHAR buf[100];
	wsprintf (buf, _T("%lX"), value);
	strArr.push_back (buf);
    _values.push_back (strArr);
}

void PropSection::save (std::wofstream &file) {

	file << "[";
	file << (LPCTSTR) _name;
	file << "]";
	
	file << std::endl;

	for (unsigned int i = 0; i < _titles.size (); i++) {
		file << (LPCTSTR)_titles[i];		
		if (i !=  (_titles.size () -1 ))			 
			 file << (LPCTSTR) _delim;
	}

	file << " " << std::endl;
	
	for (size_t i = 0; i < _values.size (); i++) {
		for (int unsigned j = 0; j < _values[i].size (); j++) {
			file << (LPCTSTR) _values[i][j];

			if (j !=  (_values[i].size () - 1 ))			
				file << (LPCTSTR) _delim;
		}

		file << " " << std::endl;
	}
}

void PropSection::loadStrAr (CString &line, CStringAr &ar) {
	CString rest = line;
	int  pos;

	while ( (pos =  rest.Find (_delim)) != -1) 	{
		CString extr = rest.Left (pos);
		ar.push_back (extr);
		rest.Delete (0, pos + _delim.GetLength ());
	}
	ar.push_back (rest);
}

bool PropSection::load (std::wifstream &file) {
	const int buf_size = 4096;
	TCHAR buf[buf_size];

	CString line;

	while (!file.eof ())
	{
	   //while (file.readLine (line) ==  ECError::eNoError) {

		if (file.fail())
			return false;

		file.getline (buf, buf_size);
		line = buf;

		line.TrimLeft ();

		if (line.IsEmpty ())
			continue;

		_name = line; 
		_name.TrimLeft ('[');
		_name.TrimRight (']');
	

		file.getline (buf, buf_size);
		//if (file.readLine (line) !=  ECError::eNoError)
		if (file.eof ())
		  return false;

		_titles.clear ();

		//loadStrAr (line, _titles);

		while (!file.eof ())
		{
			file.getline (buf, buf_size);
			line = buf;

			line.TrimLeft ();
			line.TrimRight ();

			if (line.IsEmpty ())
				return true;

			CStringAr ar;
			ar.clear ();
			
			loadStrAr (line, ar);
			_values.push_back (ar);			
		}
		
		return true;
	}
	return false;
}
void PropSection::setDelim (const TCHAR *delim) {
	_delim = delim;
}

void PropSection::getAsStr (CString &str) {

	str += "[";
	str += _name;
	str +=  "]";
	str += EOLN;
	
	for (unsigned int i = 0; i < _titles.size (); i++) {
		str +=  _titles[i];		
		if (i !=  (_titles.size () -1 ))			 
			 str += _delim;
	}

	str += EOLN;
		
	for (size_t i = 0; i < _values.size (); i++) {
		for (unsigned int j = 0; j < _values[i].size (); j++) {
			str += _values[i][j];

			if (j !=  (_values[i].size () - 1 ))			
				str += _delim;
		}		
		str += EOLN;
	}
}


//--------------------------- prop container -----------------------------
PropContainer::PropContainer() {
_delim = "|-|";
}

PropContainer::~PropContainer()
{
	for (size_t i = 0; i < _sections.size (); i++)
		delete _sections[i];
	_sections.clear(); 
}

/*
PropSection * PropContainer::findSection (int secName) {
	CString tmp;
	tmp.fromRes (secName);
	return findSection (tmp.c_str());
}*/

PropSection* PropContainer::findSection (const TCHAR* secName) {
	for (unsigned int i = 0; i < _sections.size (); i++)
		if (secName == _sections[i]->get_name ())
		  return  _sections[i];
    return NULL;
}


PropSection* PropContainer::addSection (const TCHAR* secName) {
	PropSection *section = findSection (secName);

	if (section)
		return section;

	PropSection* pSection = new	PropSection (secName);	
	_sections.push_back (pSection);
	return pSection;
}

PropSection* PropContainer::addSectionSimple (const TCHAR* secName)
{
	PropSection* pSection = new	PropSection (secName);	
	_sections.push_back (pSection);
	return pSection;
}

void PropContainer::save (const char * fileName) {

	std::wofstream file (fileName);

	if (file.fail ())
		return;
	
	for (unsigned int i = 0; i < _sections.size (); i++) {
		_sections[i]->setDelim (_delim);
		_sections[i]->save (file);
		file << " " << std::endl;
	}	
}
	
void PropContainer::load (const char * fileName) {

	std::wifstream file (fileName);

	if (file.fail ())
		return;

	//PropSection sec (_T("NoName"));
	//sec.setDelim ( _delim);
	_sections.clear ();

	/*while (sec.load (file)) {
		_sections.push_back (sec);
		sec.clear ();
	}*/

	while (true)
	{
		PropSection *pSec  = new PropSection(_T("NoName"));
		pSec->setDelim ( _delim);
		if (!pSec->load(file))
		{
			delete pSec;
			return;
		}
		_sections.push_back(pSec);
	}
}


void PropContainer::getAsStr (CString &str) {

	for (unsigned int i = 0; i < _sections.size (); i++) {
		_sections[i]->setDelim (_T(" "));
		_sections[i]->getAsStr (str);
		str += EOLN;
	}
}

void PropContainer::setDelim (const TCHAR *delim) {
	_delim = delim;
}
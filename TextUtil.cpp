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
#include "TextUtil.h"
#include "SplitPath.h"
#include "Resource.h"
#include "globals.h"

void  MakeUpper (CString &str)
{
	int last = str.GetLength ();

	/*int ext_pos = str.ReverseFind ('.');

	if (ext_pos != -1)
		last = ext_pos;*/

	for (int i = 0; i < last; i++)	
			str.SetAt (i, _totupper (str[i]));
				
}

void TitleCase (CString& str)
{
	if (str.GetLength () == 0)
		return;

	str.MakeLower ();	
	bool bNewWord = true;

	int last = str.GetLength ();

/*	int ext_pos = str.ReverseFind ('.');

	if (ext_pos != -1)
		last = ext_pos;*/

	for (int i = 0; i < last; i++)
	{
		if (_istalpha (str[i]))
		{
			if (bNewWord)
			{
				bNewWord = false;
				str.SetAt (i, _totupper (str[i]));
			}
		}
		else
			bNewWord = true;
	}
}

void ToggleCase (CString& str)
{
	if (str.GetLength () == 0)
		return;

	MakeUpper (str);	
	bool bNewWord = true;

	int last = str.GetLength ();

	/*int ext_pos = str.ReverseFind ('.');

	if (ext_pos != -1)
		last = ext_pos;*/

	for (int i = 0; i < last; i++)	
	{
		if (_istalpha (str[i]))
		{
			if (bNewWord)
			{
				bNewWord = false;
				str.SetAt (i, _tolower (str[i]));
			}
		}
		else
			bNewWord = true;
	}
}

void SentenceCase (CString& str)
{
	if (str.GetLength () == 0)
		return;

	str.MakeLower ();	
	str.SetAt (0, _toupper (str[0]));
}


void ChangeCase (int iCmd , CString &str)
{
 switch	(iCmd)
	{
		case ID_SENTENCECASE:
			SentenceCase (str);
			break;
			
		case ID_LOWERCASE:	
			str.MakeLower ();
			break;

		case ID_UPPERCASE:
			MakeUpper (str);
			break;

		case ID_TITLECASE:
			TitleCase (str);
			break;

		case ID_TOGGLECASE:
			ToggleCase (str);
			break;
	}
}


void Space2Underscore (CString & str)
{
	while (str.Replace (' ','_'));
}

void Underscore2Space (CString & str)
{
	while (str.Replace ('_',' '));
}

void Convert202Space (CString & str)
{
	while (str.Replace (_T("%20"), _T(" ")));
}

void ConvertPoint2Space (CString & str, bool bExtentions)
{
	if (bExtentions)
	{
		CSplitPath path (str);
		CString tmp;
		tmp = path.GetDrive();
		tmp += path.GetDir();
		tmp += path.GetFName();
		while (tmp.Replace (_T("."), _T(" ")));

		tmp += path.GetExt();
		str = tmp;
	}
	else
		while (str.Replace (_T("."), _T(" ")));		
}
void ConvertSpaces (int iCmd, CString &str, bool bExtentions)
{
	switch	(iCmd)
	{
		case ID_CONVERT20TOSPACE:
			Convert202Space (str);
			break;
			
		case ID_CONVERTUNDERSCORETOSPACE:	
			Underscore2Space (str);
			break;

		case ID_CONVERTSPACETOUNDERSCORE:
			Space2Underscore (str);
			break;	
		case ID_CONVERTPOINTTOSPACE:
			ConvertPoint2Space (str, bExtentions);
			break;
	}
}

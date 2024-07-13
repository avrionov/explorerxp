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


#include "stdafx.h"
#include "ExplorerXP.h"
#include "FileInfo.h"
#include "globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
#define max_chars 15 

void memset (TCHAR *buf, TCHAR val, int size)
{
	for (int i = 0; i < size; i++)
		buf[i] = val;
}

const TCHAR *filesize_to_str (ULONGLONG  filesize)
{
	static TCHAR buf[15];
	
	memset (buf, _T(' '), max_chars-1);
	
	buf[max_chars-1]=0;

	if (filesize == 0)
	{
		buf[max_chars-2] = _T('0');
		return buf;
	}
	
	int mode = filesize % 1024;
	filesize /= 1024;
	
	if (mode)
		filesize++;
	
	
	int k = 1;	
	while (filesize)
	{
		if (((k) % 4) == 0)
		{
			buf[max_chars - k-1] = _T(' ');
		}		
		else
		{
			buf[max_chars - k-1] = (TCHAR) (filesize % 10) + _T('0');
			filesize /= 10;
		}
		k++;
	}

	return buf;
}

const TCHAR *filesize_to_str2 (ULONGLONG  filesize)
{
	static TCHAR buf[15];
	
	memset (buf, _T(' '), max_chars-1);
	
	buf[max_chars-1]=0;

	if (filesize == 0)
	{
		buf[max_chars-2] = _T('0');
		return buf;
	}
	
	int k = 1;
	int p = 0;
	while (filesize)
	{
		if (((k) % 4) == 0)
		{
			p++;
			buf[max_chars - k-1] = _T(' ');
		}
		
		else
		{
			buf[max_chars - k-1] = (filesize % 10) + _T('0');
			filesize /= 10;
		}
		k++;
	}

	return buf;
}*/

inline ULONGLONG GetSizeOnDisk (const TCHAR *full_path, WIN32_FIND_DATA &fileinfo, ULONGLONG &size)
{
	ULONGLONG sizeonDisk;

	if ((fileinfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) == FILE_ATTRIBUTE_COMPRESSED)
		{
			DWORD size_low = 0, size_hi  =0;
			size_low = GetCompressedFileSize (full_path, &size_hi);
			//sizeonDisk = UInt32x32To64 (size_hi, (ULONGLONG)MAXDWORD+1);
			//sizeonDisk +=  size_low;
			sizeonDisk = (((ULONGLONG)size_hi) << 32) + (ULONGLONG)size_low;			
		}
		//else
		{
			sizeonDisk = size;
			sizeonDisk /= 4096 ;
			sizeonDisk +=  (size % 4096) != 0; // check the real value;
			sizeonDisk *= 4096;
		}	
	return sizeonDisk;
}

CFileInfo::CFileInfo (const TCHAR *full_path, WIN32_FIND_DATA &fileinfo)
{
	m_FileInfo = fileinfo;
	m_bDir = (m_FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==  FILE_ATTRIBUTE_DIRECTORY;	
	m_Size = (((ULONGLONG)m_FileInfo.nFileSizeHigh) << 32) + (ULONGLONG)m_FileInfo.nFileSizeLow;
	m_bNA = false;
	if (m_bDir)
	{
		m_bNA = true;		
		m_Size = 0;
		m_SizeOnDisk = 0;
	}
	else
		m_SizeOnDisk = ::GetSizeOnDisk (full_path, fileinfo, m_Size);
	
	m_nIcon = -1;
	m_Type[0] = 0;
	//m_Type = GetFileType (full_path);
	//m_Type = "xo";
}

void CFileInfo::Init (const TCHAR *full_path) 
{
	m_bDir = (m_FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==  FILE_ATTRIBUTE_DIRECTORY;	
	m_Size = (((ULONGLONG)m_FileInfo.nFileSizeHigh) << 32) + (ULONGLONG)m_FileInfo.nFileSizeLow;
	m_bNA = false;

	if (m_bDir)
	{
		m_bNA = true;		
		m_Size = 0;
		m_SizeOnDisk = 0;
	}
	else
		m_SizeOnDisk = ::GetSizeOnDisk (full_path, m_FileInfo, m_Size);

	m_nIcon = -1;
	m_Type[0] = 0;
}

CFileInfo::~CFileInfo()
{

}

const TCHAR* CFileInfo::as_text (int i)
{
	static CString tmp_str;
	switch (i)
	{
		case 0:
			return m_FileInfo.cFileName;

		case 1:
			if (m_bNA)
				return _T("  ");			
			tmp_str =  size_to_string (m_Size);
			return (LPCTSTR)tmp_str;

		case 2:
			if (m_bNA)
				return _T("  ");			
			tmp_str =   size_to_string (m_SizeOnDisk);
			return (LPCTSTR)tmp_str;

		case 3:						
			return m_Type;

		case 4:
			{
				//CTime time (m_FileInfo.ftLastWriteTime);
				//tmp_str = time.Format (" %x  %X ");
				SYSTEMTIME st;
				FILETIME ft = m_FileInfo.ftLastWriteTime;
				TCHAR szLocalDate[255], szLocalTime[255];
				FileTimeToLocalFileTime( &ft, &ft );
				FileTimeToSystemTime( &ft, &st );
				GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, szLocalDate, 255 );
				GetTimeFormat( LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, szLocalTime, 255 );
				tmp_str.Format( _T("%s %s"), szLocalDate, szLocalTime);
				return (LPCTSTR)tmp_str;
			}	
		case 5:
			{
				DWORD atr = m_FileInfo.dwFileAttributes;
				tmp_str.Empty();
				
				if (atr & FILE_ATTRIBUTE_ARCHIVE)
					tmp_str += _T('A');
				
				if (atr & FILE_ATTRIBUTE_COMPRESSED)
					tmp_str += _T('C');
				
				if (atr & FILE_ATTRIBUTE_DIRECTORY)
					tmp_str += _T('D');
				
				if (atr & FILE_ATTRIBUTE_ENCRYPTED)
					tmp_str += _T('E');
				
				if (atr & FILE_ATTRIBUTE_OFFLINE)
					tmp_str += _T('O');
				
				if (atr & FILE_ATTRIBUTE_READONLY)
					tmp_str += _T('R');
				
				if (atr & FILE_ATTRIBUTE_SYSTEM)
					tmp_str += _T('S');
				
				return (LPCTSTR)tmp_str;
			}
		default:
			return _T("xx");									
	}	
}

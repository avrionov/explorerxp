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


#ifndef __FILEINFO_H__
#define __FILEINFO_H__

#pragma once

#define ULONGLONGMAX 0xffffffffffffffffui64


class CFileInfo  
{
public:
	CFileInfo (const TCHAR *full_path, WIN32_FIND_DATA &fileinfo);	
	virtual ~CFileInfo();
	WIN32_FIND_DATA m_FileInfo;	
	const TCHAR *as_text (int i);
	bool IsDir () const { return m_bDir;}
	void SetSize (ULONGLONG size, ULONGLONG sizeondisk)
	{ 
		m_Size = size; 
		m_SizeOnDisk = sizeondisk;
		m_bNA = false;
	}

	void Init (const TCHAR *full_path);

	inline ULONGLONG GetSize () const { return m_Size;};
	inline ULONGLONG GetSizeOnDisk () const { return m_SizeOnDisk;};
	inline bool IsNA () const { return m_bNA;}; 
	
	TCHAR m_Type[80];	
	int m_nIcon;

private:
	bool m_bDir;
	ULONGLONG m_Size;
	ULONGLONG m_SizeOnDisk;
//	CString m_ParentDir;
	bool m_bNA;
};

#endif // !defined(AFX_FILEINFO_H__30B526ED_A48B_4CFC_8E63_3B2A657E96E5__INCLUDED_)

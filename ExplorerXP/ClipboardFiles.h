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

#ifndef __CLIPBOARDFILES_H__
#define __CLIPBOARDFILES_H__

#pragma once

typedef std::map<CString, int> CFilesMap;
typedef CFilesMap::value_type CFilesValue;


class CClipboardFiles  
{
public:
	CClipboardFiles();
	virtual ~CClipboardFiles();
	bool IsSameRoot (const TCHAR *root);
	bool IsFileIn (CString& file);
	void ReadClipboard ();

protected:
	CFilesMap m_Map;
	CString m_Root;

};

extern CClipboardFiles gClipboard;

#endif // __CLIPBOARDFILES_H__

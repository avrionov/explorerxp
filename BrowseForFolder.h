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


#ifndef __BROWSEFORFOLDER_H__
#define __BROWSEFORFOLDER_H__

#pragma once

class CBrowseForFolder  
{
public:
	void SetNewStyle(bool val);
	
	HWND hWndOwner;	
	CString strStartupDir;
	void SetStatusBar(bool val);
	void SetEditBox(bool val);
	
	CString strTitle;	
	int Image;	
	CString strDirRoot;
	bool GetFolder(CString &returnPath);
	CBrowseForFolder();
	virtual ~CBrowseForFolder();

protected:
	UINT flags;	
	static int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};


#endif // __BROWSEFORFOLDER_H__

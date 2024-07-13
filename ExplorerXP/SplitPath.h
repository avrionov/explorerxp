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


#ifndef __SPLIT_PATH__
#define __SPLIT_PATH__

#pragma once

#define SAFE_FACTOR 8


#define _SPLIT_MAX_PATH     (_MAX_PATH  * SAFE_FACTOR)
#define _SPLIT_MAX_DIR      (_MAX_DIR   * SAFE_FACTOR)
#define _SPLIT_MAX_DRIVE    (_MAX_DRIVE * SAFE_FACTOR)
#define _SPLIT_MAX_EXT		(_MAX_PATH  * SAFE_FACTOR)
#define _SPLIT_MAX_FNAME	(_MAX_FNAME * SAFE_FACTOR)

class CSplitPath  
{
public:
	CSplitPath (const TCHAR *path);
	virtual ~CSplitPath();
	const TCHAR* GetPath ()	{ return path_buffer;};
	const TCHAR* GetDrive () { return drive;};
	const TCHAR* GetDir ()	{ return dir;}
	const TCHAR* GetFName () { return fname;}
	const TCHAR* GetExt ()	{ return ext;}


protected:
   TCHAR path_buffer	[_SPLIT_MAX_PATH];
   TCHAR drive			[_MAX_PATH];
   TCHAR dir			[_SPLIT_MAX_DIR];
   TCHAR fname			[_SPLIT_MAX_FNAME];
   TCHAR ext			[_MAX_PATH];
};

#endif // __SPLIT_PATH__

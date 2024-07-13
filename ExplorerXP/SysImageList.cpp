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
#include "SysImageList.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSysImageList gSysImageList;

CSysImageList::CSysImageList()
{
	SHFILEINFO    ssfi; 
   
	TCHAR path[2094];
	SHGetSpecialFolderPath (NULL, path, CSIDL_WINDOWS , FALSE);

    m_hSystemSmallImageList =  (HIMAGELIST)SHGetFileInfo( path, 0, &ssfi, sizeof(SHFILEINFO),  SHGFI_SYSICONINDEX | SHGFI_SMALLICON); 
    m_ImageList.Attach (m_hSystemSmallImageList);   
}

CSysImageList::~CSysImageList()
{
	m_ImageList.Detach ();
}

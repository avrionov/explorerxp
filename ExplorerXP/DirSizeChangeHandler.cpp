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
#include "DirSizeChangeHandler.h"
#include "DirSize.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CDirSizeChangeHandler::CDirSizeChangeHandler (CDirSize *pDirSize, const TCHAR *path)
:m_Path (path),
m_pDirSize (pDirSize)
{
	
}

CDirSizeChangeHandler::~CDirSizeChangeHandler()
{

}

void CDirSizeChangeHandler::On_FileAdded(const CString & strFileName)
{
	m_pDirSize->Invalidate (strFileName);
}

void CDirSizeChangeHandler::On_FileRemoved(const CString & strFileName)
{
	m_pDirSize->Invalidate (strFileName, true);
}

void CDirSizeChangeHandler::On_FileModified(const CString & strFileName)
{
	m_pDirSize->Invalidate (strFileName);
}

void CDirSizeChangeHandler::On_FileNameChanged(const CString & strOldFileName, const CString & strNewFileName)
{
	m_pDirSize->InvalidateName (strOldFileName, strNewFileName);
}

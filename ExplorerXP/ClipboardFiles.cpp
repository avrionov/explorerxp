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
#include "ClipboardFiles.h"
#include "globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClipboardFiles gClipboard;

CClipboardFiles::CClipboardFiles()
{

}

CClipboardFiles::~CClipboardFiles()
{


}

bool CClipboardFiles::IsSameRoot (const TCHAR *root)
{
   if (m_Root.CompareNoCase (root) == 0)
	   return true;

   return false;
}

bool CClipboardFiles::IsFileIn (CString& file)
{
	if (m_Map.empty () == true)
		return false;
	
	CString csTemp = file;

	//csTemp.MakeLower ();
		
	CFilesMap::iterator it = m_Map.find (csTemp);
	if (it != m_Map.end ())
		return true;
	
	return false;
}

void CClipboardFiles::ReadClipboard ()
{
	COleDataObject object;

	m_Map.clear ();
	m_Root.Empty ();
	
	
	if ( !object.AttachClipboard() )
		return;

	DROPEFFECT drop_effect;

	if (!ReadDropEffect (&object, drop_effect))
		return;

		
	if (drop_effect != DROPEFFECT_MOVE)
	  return;
	
	CSelRowArray ar;

	if (!ReadHDropData (&object, ar, m_Root))
		return;

	for (unsigned int i =0; i < ar.size (); i++)
		m_Map.insert (CFilesValue (ar[i].m_Path,  1));	    
}


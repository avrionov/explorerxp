/*  Copyright 2002-2020 Nikolay Avrionov. All Rights Reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
*/

#include "stdafx.h"
#include "DriveViewer.h"
#include "globals.h"
#include "ThreadPool.h"

#include <shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CThreadPool gPool;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static TCHAR *Headers[] = {_T("Name"), _T("Type"), _T("Total Size"),
                           _T("Free Size"), _T("  ")};
static int HEADER_COLUMNS = 5;

CDriveViewer gDriveViewer;

CDriveViewer::CDriveViewer() {
	FillHeader();
	m_bGetState = false;
}

CDriveViewer::~CDriveViewer() {

}

void CDriveViewer::GetDrives () {	
	
	CDriveArray drArray;
	::GetDrives (drArray);

	setDrives(drArray);
}

void CDriveViewer::setDrives(const CDriveArray& drives_array) {
	CGuard guard(m_lock);
	m_Array.assign(drives_array.begin(), drives_array.end());
}

void CDriveViewer::getDrives(CDriveArray& drives_array) {
  CGuard guard(m_lock);
  drives_array.assign(m_Array.begin(), m_Array.end());  
}


void CDriveViewer::Fill (const TCHAR *root)  {
	
	gPool.StopThread(DRIVES_THREAD);
	gPool.StartThreadAndWait(DRIVES_THREAD, this, &CDriveViewer::GetDrives);
	
	if (!m_pGridCtrl)
		return;

	m_pGridCtrl->SetRedraw(FALSE);
	TRACE("Drive Viewer %d, %d, %d, %d, %d", m_State.m_Widths[0], m_State.m_Widths[1], m_State.m_Widths[2], m_State.m_Widths[3], m_State.m_Widths[4]);

	if (!m_bGetState) {
        loadState();
		m_bGetState = true;
	}
	else {
		CFolderState state;
		GetState(state);
		gFolderStateMan.SaveState(CONST_MYCOMPUTER, state);
		m_State = state;
	}
	
	CGuard guard(m_lock);
	Sort ();
	m_pGridCtrl->SetRowCount(static_cast<int>(m_Array.size ()) +1);
	SetupGrid ();	

	TRACE("Drive Viewer %d, %d, %d, %d, %d", m_State.m_Widths[0], m_State.m_Widths[1], m_State.m_Widths[2], m_State.m_Widths[3], m_State.m_Widths[4]);

	m_pGridCtrl->SetRedraw(TRUE);
}

const TCHAR * CDriveViewer::GetTitle ()  {
	return CONST_MYCOMPUTER;
}

void CDriveViewer::GridCallBack (GV_DISPINFO *pDispInfo)  {

  CGuard guard(m_lock);

	pDispInfo->item.nState |= GVIS_READONLY;

	if (pDispInfo->item.row == 0) {
		pDispInfo->item.strText = Headers[pDispInfo->item.col];	
		return ;
	}

	int row = pDispInfo->item.row-1;
	int col = pDispInfo->item.col;

	
	if (row > static_cast<int>(m_Array.size())) {
		TRACE (_T("Asking for %d from %d !!!!\n"), row, m_Array.size ());
		return;
	}
	
	switch (pDispInfo->item.col) {

		case 0:
			{
				pDispInfo->item.iImage = m_Array[row].m_nImage;
				pDispInfo->item.strText = m_Array[row].as_text (col);
			}
			break;
		
		case 3:
		case 2:
			pDispInfo->item.nFormat |= DT_RIGHT;
			pDispInfo->item.strText.Format (_T("%s "), m_Array[row].as_text (col));			
		break;
		
		case 1:
			pDispInfo->item.strText = m_Array[row].as_text (col);					
			break;		

		default:
			pDispInfo->item.strText = m_Array[row].as_text (col);			
			break;

	}
	
	if (pDispInfo->item.col == m_State.m_nSortColumn)
		pDispInfo->item.crBkClr = //RGB(247,247,247);             
		HLS_TRANSFORM (GetSysColor(COLOR_WINDOW),0, 5);

	if (pDispInfo->item.nState & GVIS_SELECTED)
			pDispInfo->item.crBkClr = GetSelectedColorBackground();

}


struct SortByName
{

	SortByName (bool bAscending )
		:m_bAscending (bAscending)
	{	}

	bool operator () (CDriveInfo &d1, CDriveInfo &d2) {
		if (m_bAscending)	 		{
				return _tcsicmp (d1.m_Path, d2.m_Path) <= 0;
		}	else {
			return _tcsicmp (d1.m_Path, d2.m_Path) >= 0;						
		}				
	}

	bool m_bAscending;
};

struct SortByType
{

	SortByType (bool bAscending )
		:m_bAscending (bAscending)
	{	}

	bool operator () (CDriveInfo &d1, CDriveInfo &d2) {
		if (m_bAscending) 		{
				int result = _tcsicmp (d1.m_Type, d2.m_Type);
				if (result == 0)
					return _tcsicmp (d1.m_Path, d2.m_Path) <= 0;

				return result <= 0;
		}	else {
			int result = _tcsicmp (d1.m_Type, d2.m_Type);
			if (result == 0)
				return _tcsicmp (d1.m_Path, d2.m_Path) >= 0;						

			return result >= 0;
		}				
	}

	bool m_bAscending;
};


bool sort_by_free_size_a (const CDriveInfo d1, const CDriveInfo d2) {

	if (d1.m_FreeSpace < d2.m_FreeSpace)
		return true;

	if (d1.m_FreeSpace > d2.m_FreeSpace)
		return false;

	return _tcsicmp (d1.m_Path, d2.m_Path) <= 0;
}

bool sort_by_free_size_d (const CDriveInfo d1, const CDriveInfo d2)
{		
		if (d1.m_FreeSpace < d2.m_FreeSpace)
			return false;

		if (d1.m_FreeSpace > d2.m_FreeSpace)
			return true;

		return _tcsicmp (d1.m_Path, d2.m_Path) <= 0;
}

bool sort_by_total_size_a (const CDriveInfo d1, const CDriveInfo d2) {

	if (d1.m_TotalSize < d2.m_TotalSize)
		return true;

	if (d1.m_TotalSize > d2.m_TotalSize)
		return false;

	return _tcsicmp (d1.m_Path, d2.m_Path) <= 0;
}

bool sort_by_total_size_d (const CDriveInfo d1, const CDriveInfo d2) {		

		if (d1.m_TotalSize < d2.m_TotalSize)
			return false;

		if (d1.m_TotalSize > d2.m_TotalSize)
			return true;

		return _tcsicmp (d1.m_Path, d2.m_Path) <= 0;
}

void CDriveViewer::Sort () {

	switch (m_State.m_nSortColumn)
	{
		case 0:	
			std::sort (m_Array.begin (), m_Array.end (), SortByName(m_State.m_bAscending));
			break;
		 case 1:
			 std::sort (m_Array.begin (), m_Array.end (), SortByType(m_State.m_bAscending));
		 case 2:
			if (m_State.m_bAscending)
				std::stable_sort (m_Array.begin (), m_Array.end (), sort_by_total_size_a);
			else
				std::stable_sort (m_Array.begin (), m_Array.end (), sort_by_total_size_d);
			break;		
		 case 3:
			if (m_State.m_bAscending)
				std::stable_sort (m_Array.begin (), m_Array.end (), sort_by_free_size_a);
			else
				std::stable_sort (m_Array.begin (), m_Array.end (), sort_by_free_size_d);
			break;		
	}
}

bool CDriveViewer::Sync (const TCHAR* folder , const TCHAR *name) {
	return false;
}

std::wstring formatDriveName(const std::wstring driveString) {
  // Find the opening parenthesis
  size_t openParenPos = driveString.rfind('(');
  // Find the closing parenthesis
  size_t closeParenPos = driveString.rfind(')');

  // If both parentheses are found and in the correct order
  if (openParenPos != std::wstring::npos && closeParenPos != std::wstring::npos &&
      openParenPos < closeParenPos) {
    // Extract the drive letter part (e.g., "C:")
    auto driveLetterPart = driveString.substr(
        openParenPos + 1, closeParenPos - (openParenPos + 1));

    // Extract the OS name part (e.g., "OS ")
    auto osNamePart = driveString.substr(0, openParenPos);

    // Remove any trailing space from the OS name if present
    if (!osNamePart.empty() && osNamePart.back() == ' ') {
      osNamePart.pop_back();
    }

    // Concatenate in the desired format
    return driveLetterPart + L" " + osNamePart;
  } else {
    // Return the original string if the format is not as expected
    return driveString;
  }
}

void GetDrives(CDriveArray& array) {

	array.clear();

	char driveLetter = 'A';

	while (driveLetter <= 'Z') {
		CString path = CString(driveLetter) + ":\\";
		
		SHFILEINFO     sfi;
		UINT nType = GetDriveType(path);
		//	if( DRIVE_REMOVABLE < nType && nType <= DRIVE_RAMDISK )
		if (nType != DRIVE_UNKNOWN && nType != DRIVE_NO_ROOT_DIR)
			if (SHGetFileInfo(path, 0, &sfi, sizeof(sfi),  SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY))
			{
				CDriveInfo info;
                info.m_Name = formatDriveName (sfi.szDisplayName).c_str();
				info.m_Path = path;
				info.m_Type = sfi.szTypeName;
				info.m_nImage = sfi.iIcon;
				info.m_nType = nType;

				DWORD SectorsPerCluster;     // sectors per cluster
				DWORD BytesPerSector;        // bytes per sector
				DWORD NumberOfFreeClusters;  // free clusters
				DWORD TotalNumberOfClusters; // total clusters
				//	TRACE (L"%s %s\n", sfi.szDisplayName, path);		
				if (nType != DRIVE_REMOVABLE)
					if (GetDiskFreeSpace(path, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
					{
						DWORD BytesPerCluster = BytesPerSector * SectorsPerCluster;
						info.m_FreeSpace = UInt32x32To64(NumberOfFreeClusters, BytesPerCluster);
						info.m_TotalSize = UInt32x32To64(TotalNumberOfClusters, BytesPerCluster);
					}
				array.push_back(info);
			}
		driveLetter++;
	}
}

void _GetDrives (CDriveArray &array) {

	array.clear ();

	
	IShellFolder   *psfDesktop;

	HRESULT errDesktop = SHGetDesktopFolder(&psfDesktop);
	if(psfDesktop == NULL)
		return;
	
	

	LPITEMIDLIST   pidlMyComputer;

	HRESULT errDrives = SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidlMyComputer);

	if(pidlMyComputer == NULL)
	{
		psfDesktop->Release();
		return;
	}
	
	
	IShellFolder   *psfMyComputer;

	psfDesktop->BindToObject(pidlMyComputer, NULL, IID_IShellFolder, (LPVOID*)&psfMyComputer);
	
	if(psfMyComputer) {
			IEnumIDList* pEnum;
			if(SUCCEEDED(psfMyComputer->EnumObjects(NULL, SHCONTF_FOLDERS|SHCONTF_INCLUDEHIDDEN, &pEnum))) {
				ITEMIDLIST* pidl;
				DWORD  dwFetched = 1;
				TCHAR  path[MAX_PATH];

				while(SUCCEEDED(pEnum->Next(1, &pidl, &dwFetched)) && dwFetched) {
				//	SHFILEINFO     sfi;	
				//	
				//	//LPITEMIDLIST pidl_full = Pidl_Concatenate (pidlMyComputer, pidl);
				//	LPITEMIDLIST pidl_full = ILCombine (pidlMyComputer, pidl);					
				//	SHGetPathFromIDList (pidl_full, path);

				//	UINT nType = GetDriveType( path);
				////	if( DRIVE_REMOVABLE < nType && nType <= DRIVE_RAMDISK )
				//	if( nType != DRIVE_UNKNOWN && nType != DRIVE_NO_ROOT_DIR )
				//	if(SHGetFileInfo((LPCTSTR)pidl_full, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY))
				//	{
				//		CDriveInfo info;
				//		info.m_Name = sfi.szDisplayName;
				//		info.m_Path = path;
				//		info.m_Type = sfi.szTypeName;
				//		info.m_nImage = sfi.iIcon;
				//		info.m_nType = nType;
				//		
				//		DWORD SectorsPerCluster;     // sectors per cluster
				//		DWORD BytesPerSector;        // bytes per sector
				//		DWORD NumberOfFreeClusters;  // free clusters
				//		DWORD TotalNumberOfClusters; // total clusters
				//	//	TRACE (L"%s %s\n", sfi.szDisplayName, path);		
				//		if (nType != DRIVE_REMOVABLE )
				//		if (GetDiskFreeSpace (path, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
				//			{	
				//					DWORD BytesPerCluster = BytesPerSector * SectorsPerCluster;								
				//					info.m_FreeSpace = UInt32x32To64(NumberOfFreeClusters, BytesPerCluster);
				//					info.m_TotalSize= UInt32x32To64(TotalNumberOfClusters, BytesPerCluster);
				//			}
				//		array.push_back (info);
				//	}
				}
				pEnum->Release ();
			}
		 psfMyComputer->Release();
	 }

	
	CoTaskMemFree(pidlMyComputer);
	
	psfDesktop->Release();    	
}


const TCHAR* CDriveInfo::as_text (int i)
{
	static CString tmpStr;
	switch (i)
	{
		case 0:
			return m_Name;
		case 1:
			return m_Type;
			
		case 2:
			if (m_TotalSize)
			{
				tmpStr = size_to_string (m_TotalSize);
				return 	(LPCTSTR)  tmpStr;		
			}
			else
				return EMPTYSTR;

		case 3:
			if (m_TotalSize)
			{
				tmpStr = size_to_string (m_FreeSpace);		
				return 	(LPCTSTR)  tmpStr;
			}
			else
				return EMPTYSTR;
		
		default:
			return EMPTYSTR;			
	}	
}

bool CDriveViewer::GetObjectSize (int iRow, ULONGLONG &size, ULONGLONG &sizeOnDisk) {

	CGuard guard(m_lock);
	size = m_Array[iRow].m_TotalSize;
	sizeOnDisk = 0;	
	return true;
}

void CDriveViewer::CheckUpdate () {

	if (m_bNeedUpdate) {
//		TRACE (_T("Check Update Dr Viewer\n"));
		Fill (EMPTYSTR);
		m_bNeedUpdate = false;
	}
}

CString CDriveViewer::GetText (int row, int col) {
  CGuard guard(m_lock);
	return m_Array[row].as_text (col);
}

void CDriveViewer::FillHeader() {	

	m_State.m_Visible = 0;
   for (int i = 0; i < HEADER_COLUMNS; i++) {
		m_Headers.push_back (Headers[i]);
		m_State.m_Visible |= 1 << i;
	}

	m_State.m_Widths[0] = 200;
	m_State.m_Widths[1] = 120;
	m_State.m_Widths[2] = 120;
	m_State.m_Widths[3] = 140;
	m_State.m_Widths[4] = 120;	

	if (gFolderStateMan.IsIn(CONST_MYCOMPUTER))
        loadState();
	else
		gFolderStateMan.SaveState (CONST_MYCOMPUTER, m_State);		
}


void CDriveViewer::OnClose () {
	SaveState (CONST_MYCOMPUTER);
}

CString & CDriveViewer::GetDriveInfoStr(const TCHAR *path) {

  CDriveArray drArray;
	getDrives(drArray);
			
	static CString info;
	info.Empty ();

	if (drArray.size() == 0) {
		MarkForUpdate();
	} else {
		for (int i = 0; i < static_cast<int>(drArray.size()); i++) {
		  if (drArray[i].m_nType == DRIVE_FIXED)
			if (PathIsSameRoot(path, drArray[i].m_Path)) {

			  TCHAR buf[128];
			  StrFormatByteSize64(drArray[i].m_FreeSpace, buf, 127);

			  if (*path && path[0] > 0) {
				info += path[0];
				info += TEXT(": ");
			  }

			  info += buf;
			  info += " free ";
			}
    }
  }
	
  return info;
}

// getters, setters

int CDriveViewer::GetCount () { 
	CGuard guard(m_lock);
	return static_cast<int>(m_Array.size ());
}


void CDriveViewer::loadState() {
  gFolderStateMan.LoadState(CONST_MYCOMPUTER, m_State);
  
  // make the name column always visibile
  m_State.m_Visible |= 1;

  // make the last column always visible
  m_State.m_Visible |= 1 << (HEADER_COLUMNS -1);
}
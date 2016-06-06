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
#include "explorerxp.h"
#include "RecBinViewer.h"
#include "globals.h"
#include "ShellContextMenu.h"
#include "Shell32.h"
#include "ThreadPool.h"

extern CThreadPool gPool;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CRecBinViewer gRecBinViewer;

#define MAX_COLUMN 4

//#define _countof(array) (sizeof(array)/sizeof(array[0]))

CRecBinViewer::CRecBinViewer()
:m_hWnd (NULL),
m_bNeedUpdate (false)
{
    m_pFolder2		= NULL;
	m_pRecycleBin	= NULL;

	ZeroMemory (&m_pidlDrives, sizeof (m_pidlDrives));
	ZeroMemory (&m_hNotifyDrives, sizeof (m_hNotifyDrives));
	FillHeader();
	
}

CRecBinViewer::~CRecBinViewer()
{

}

void CRecBinViewer::Fill (const TCHAR *root) 
{	
    UpdateList ();
	if (m_pGridCtrl)
	{		
		m_pGridCtrl->SetRowCount(m_List.size () +1);
		SetupGrid ();
	}	
}

const TCHAR * CRecBinViewer::GetTitle () 
{
    return _T("Recycle Bin");
}

void CRecBinViewer::GridCallBack (GV_DISPINFO *pDispInfo) 
{
	pDispInfo->item.nState |= GVIS_READONLY;

    if (pDispInfo->item.row == 0)
	{
		pDispInfo->item.strText = m_Headers[pDispInfo->item.col];
		//pDispInfo->item.lfFont.lfWeight  = FW_BOLD;
		return ;
	}

    int row = pDispInfo->item.row - 1;
    if (row >= static_cast<int>(m_List.size ()))
        return;

	if (pDispInfo->item.col == 0 )
	{
		if (m_List[row].m_PIDL)
		{
			SHFILEINFO		fi;

			ZeroMemory (&fi, sizeof (fi));				
			HRESULT hr = SHGetFileInfo ((LPCTSTR)m_List[row].m_PIDL, 0, &fi, sizeof (fi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_PIDL);

			if (SUCCEEDED (hr))			
				pDispInfo->item.iImage = fi.iIcon;			
		}
	}
		
	pDispInfo->item.crFgClr = GetSysColor (COLOR_BTNTEXT);

    if (pDispInfo->item.col < static_cast<int>(m_List[row].m_Ar.size()))
        pDispInfo->item.strText = m_List[row][pDispInfo->item.col];

	/*if (pDispInfo->item.col == m_State.m_nSortColumn)
		pDispInfo->item.crBkClr = RGB(247,247,247);  */

	if (pDispInfo->item.col == m_State.m_nSortColumn)
		pDispInfo->item.crBkClr = //RGB(247,247,247);             
		HLS_TRANSFORM (GetSysColor(COLOR_WINDOW),0, 5);

	if (pDispInfo->item.nState & GVIS_SELECTED)
		pDispInfo->item.crBkClr = GetSelectedColor();	
}


struct SortByString
{
	SortByString (int Col, bool bAscending) :
		m_bAscending (bAscending),
		m_iCol (Col	)		 
	{	
	}

	bool operator () (const CRecycleItem &f1, const CRecycleItem &f2)
	{
		if (m_bAscending)		
			return _tcsicmp (f1[m_iCol], f2[m_iCol]) <= 0;

		return _tcsicmp (f1[m_iCol], f2[m_iCol]) >= 0;		
	}

	int  m_iCol;
	bool m_bAscending;
};

struct SortByStringSize
{
	SortByStringSize (int Col, bool bAscending) :
	    m_iCol (Col	),		
		m_bAscending (bAscending)		 
	{	
	}

	bool operator () (const CRecycleItem &f1, const CRecycleItem &f2)
	{
		if (m_bAscending)
			if (f1[m_iCol].GetLength () > f2[m_iCol].GetLength ())
				return false;
			else
				if (f1[m_iCol].GetLength () < f2[m_iCol].GetLength ())
					return true;			
				else
					return _tcsicmp (f1[m_iCol], f2[m_iCol]) <= 0;

			if (f1[m_iCol].GetLength () > f2[m_iCol].GetLength ())
				return true;
			else
				if (f1[m_iCol].GetLength () < f2[m_iCol].GetLength ())
					return false;
				else
					return _tcsicmp (f1[m_iCol], f2[m_iCol]) >= 0;		
	}

	int  m_iCol;
	bool m_bAscending;
};

void CRecBinViewer::Sort () 
{	
	if (m_State.m_nSortColumn == 3)
		std::stable_sort (m_List.begin (), m_List.end (), SortByStringSize (m_State.m_nSortColumn, m_State.m_bAscending));	
	else
		if (m_State.m_nSortColumn <= MAX_COLUMN)
			std::stable_sort (m_List.begin (), m_List.end (), SortByString (m_State.m_nSortColumn, m_State.m_bAscending));	
}

bool CRecBinViewer::Sync (const TCHAR* folder , const TCHAR *name) 
{
    return false;
}

bool CRecBinViewer::CanChangeTo (int iRow) 
{
    return false;
}

CString  CRecBinViewer::GetPath (int iRow) 
{
	return m_List[iRow][0];
}

CString  CRecBinViewer::GetPath(int iRow, bool &bFolder) {
  bFolder = false;
  return m_List[iRow][0];
}

const TCHAR*  CRecBinViewer::GetName (int iRow) 
{	
	return m_List[iRow][0];    
}


bool CRecBinViewer::GetObjectSize (int iRow, ULONGLONG &size, ULONGLONG &sizeOnDisk)
{
    return false;
}


void CRecBinViewer::InitInterfaces (void)
{
	if (TRUE == GetFolder2 ())	
		HeaderFolder2 ();	
	else 
		if (TRUE == GetFolder ())			
				HeaderFolder ();
			
	if (gFolderStateMan.IsIn (CONST_RECYCLEBIN))
		gFolderStateMan.LoadState (CONST_RECYCLEBIN, m_State);
	else
		gFolderStateMan.SaveState (CONST_RECYCLEBIN, m_State);		
}

void CRecBinViewer::UpdateList (void)
{	
	RemovePIDL ();

    m_List.clear ();

	if (NULL != m_pFolder2)	
		FillFolder2 ();	
	else 
		if (NULL != m_pRecycleBin)	
			FillFolder ();	
}


void CRecBinViewer::GetName (STRRET str, CString &cs)
{
	//ZeroMemory (szPath, sizeof (szPath));
	switch (str.uType)
	{
		case STRRET_CSTR:			
			cs = str.cStr;
			break;
		case STRRET_OFFSET:
			break;
		case STRRET_WSTR:
			{				
				cs = str.pOleStr;
				CoTaskMemFree (str.pOleStr);				
			}
			break;
	}	
	
}

BOOL CRecBinViewer::GetFolder ()
{
	BOOL			bReturn			= FALSE;
	STRRET			strRet;
	LPSHELLFOLDER	pDesktop		= NULL;
	LPITEMIDLIST	pidlRecycleBin	= NULL;	
	CString			dspName;
		
	HRESULT hr = SHGetDesktopFolder(&pDesktop);
	
	hr = SHGetSpecialFolderLocation (m_hWnd, CSIDL_BITBUCKET, &pidlRecycleBin);
	if (NULL != m_pRecycleBin)
	{
		m_pRecycleBin->Release ();
		m_pRecycleBin = NULL;
	}
	hr = pDesktop->BindToObject(pidlRecycleBin, NULL, IID_IShellFolder, (LPVOID *)&m_pRecycleBin);
	if (SUCCEEDED (hr))
	{
		bReturn = TRUE;
	}

	if (S_OK == pDesktop->GetDisplayNameOf (pidlRecycleBin, SHGDN_NORMAL, &strRet))
	{
		GetName (strRet, dspName);
	}

	CoTaskMemFree (pidlRecycleBin);
	pDesktop->Release();
		
	return bReturn;
}

void CRecBinViewer::HeaderFolder ()
{	
	PSHELLDETAILS pDetails = NULL;
	 	
	HRESULT hr = m_pRecycleBin->CreateViewObject (m_hWnd, IID_IShellDetails, (LPVOID*)&pDetails);

	m_State.m_Visible = 0;

	if (SUCCEEDED (hr))
	{
		CString csTemp;
		SHELLDETAILS sd;
		int iSubItem = 0;
		
		while (SUCCEEDED (hr))
		{
			hr = pDetails->GetDetailsOf (NULL , iSubItem, &sd);
			if (SUCCEEDED (hr))
			{
				GetName (sd.str, csTemp);				
                m_Headers.push_back (csTemp);	
				m_State.m_Visible |= 1 << iSubItem;
                iSubItem++;
				if (iSubItem > MAX_COLUMN)
					break;				
			}
		}
	}
		
	CoTaskMemFree(pDetails);
	
}

void CRecBinViewer::FillFolder ()
{	
	LPENUMIDLIST	penumFiles		= NULL;
	LPITEMIDLIST	pidl			= NULL;
	PSHELLDETAILS	pDetails		= NULL;
	SHELLDETAILS	sd;
	int				iSubItem		= 0;
	HRESULT			hr				= S_OK;
	CString			csTemp;
	
	
	hr = m_pRecycleBin->CreateViewObject (m_hWnd, IID_IShellDetails, (LPVOID*)&pDetails);

	// Iterate through list
	m_pRecycleBin->EnumObjects(m_hWnd, SHCONTF_FOLDERS|SHCONTF_NONFOLDERS| SHCONTF_INCLUDEHIDDEN, &penumFiles);
	
    CStringAr ar;

	if (SUCCEEDED (hr))
	{
		while (penumFiles->Next(1, &pidl, NULL) != S_FALSE)
		{
            ar.clear ();		
			
			hr = S_OK;
			iSubItem = 0;
			
			while (SUCCEEDED (hr))
			{
				hr = pDetails->GetDetailsOf (pidl , iSubItem, &sd);
				if (SUCCEEDED (hr))
				{
					GetName (sd.str, csTemp);					
                    ar.push_back (csTemp);	
                    iSubItem++;

					if (iSubItem > MAX_COLUMN)
						break;
				}
			}
            m_List.push_back (CRecycleItem (ar, pidl));
		}
	}
		
	CoTaskMemFree (pDetails);
	
	if (NULL != penumFiles)
	{
		penumFiles->Release ();
		penumFiles = NULL;
	}
}

BOOL CRecBinViewer::GetFolder2 ()
{
	BOOL			bReturn			= FALSE;
	STRRET			strRet;
	LPSHELLFOLDER	pDesktop		= NULL;
	LPITEMIDLIST	pidlRecycleBin	= NULL;
	CString			dspName;

	
	if (NULL != m_pFolder2)
	{
		m_pFolder2->Release ();
		m_pFolder2 = NULL;
	}
	
	if ((SUCCEEDED (SHGetDesktopFolder(&pDesktop))) &&
		(SUCCEEDED (SHGetSpecialFolderLocation (m_hWnd, CSIDL_BITBUCKET, &pidlRecycleBin))))
	{
		if (SUCCEEDED (pDesktop->BindToObject(pidlRecycleBin, NULL, IID_IShellFolder2, (LPVOID *)&m_pFolder2)))
		{
			if (S_OK == pDesktop->GetDisplayNameOf (pidlRecycleBin, SHGDN_NORMAL, &strRet))			
				GetName (strRet, dspName);			
			bReturn = TRUE;
		}
	}
		
	CoTaskMemFree (pidlRecycleBin);
	
	if (NULL != pDesktop) pDesktop->Release();
	
	return bReturn;
}

void CRecBinViewer::HeaderFolder2 ()
{
	CString csTemp;
	HRESULT hr = S_OK;
	SHELLDETAILS sd;
	int iSubItem = 0;
	
	// We'are asking the object the list of available columns.
	// For each, we are adding them to the control in the right order.

	m_State.m_Visible = 0;

	while (SUCCEEDED (hr))
	{
		hr = m_pFolder2->GetDetailsOf (NULL , iSubItem, &sd);
		if (SUCCEEDED (hr))
		{
			GetName (sd.str, csTemp);			
			m_Headers.push_back (csTemp);
            m_State.m_Visible |= 1 << iSubItem;
			iSubItem++;
			if (iSubItem > MAX_COLUMN)
				break;
		}
	}
    
}

void CRecBinViewer::FillFolder2 ()
{	
	CString			csTemp;
	LPENUMIDLIST	penumFiles;
	LPITEMIDLIST	pidl = NULL;
	SHELLDETAILS	sd;
	int				iSubItem = 0;
				
	CStringAr ar;

	// Get the list of available objects
	HRESULT hr = m_pFolder2->EnumObjects(m_hWnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS| SHCONTF_INCLUDEHIDDEN, &penumFiles);
	if (SUCCEEDED (hr))
	{
		// Iterate through list
		while (penumFiles->Next(1, &pidl, NULL) != S_FALSE)
		{
            ar.clear ();
		//	iItem = m_List.InsertItem (iItem, EMPTYSTR);
		//	m_List.SetItemData (iItem, (DWORD)pidl);

			/*ZeroMemory (&fi, sizeof (fi));
			hr = SHGetFileInfo ((LPCTSTR)pidl, 0, &fi, sizeof (fi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_PIDL);

			if (SUCCEEDED (hr))
			{
				iIndex = fi.iIcon;
				m_List.SetItem (iItem, 0, LVIF_IMAGE, NULL, iIndex, 0, 0, 0);
			}*/

			// We iterate now in all the available columns.
			// Since it depends on the system, we "hope" that they are going to be as many 
			// and in the same order as when we have added the column's headers.

			hr = S_OK;
			iSubItem = 0;
			while (SUCCEEDED (hr))
			{
				hr = m_pFolder2->GetDetailsOf (pidl , iSubItem, &sd);
				if (SUCCEEDED (hr))
				{
					GetName (sd.str, csTemp);					
                    ar.push_back (csTemp);
					//m_List.SetItemText (iItem, iSubItem , szTemp);
					iSubItem ++;
					
					if (iSubItem > MAX_COLUMN)
						break;
				}
			}
			m_List.push_back (CRecycleItem (ar, pidl));
             //m_List.push_back (ar);
		}
	}

	if (NULL != penumFiles)
	{
		penumFiles->Release ();
		penumFiles = NULL;
	}	
}

LRESULT CRecBinViewer::OnShellNotify (WPARAM wParam, LPARAM lParam)
{
	LRESULT lReturn = 0;
	SHNOTIFYSTRUCT shns;
	TCHAR szBefore[MAX_PATH];
	TCHAR szAfter[MAX_PATH];

	
	memcpy((void *)&shns,(void *)wParam,sizeof(SHNOTIFYSTRUCT));

	SHGetPathFromIDList((struct _ITEMIDLIST *)shns.dwItem1, szBefore);
	SHGetPathFromIDList((struct _ITEMIDLIST *)shns.dwItem2, szAfter);

	switch (lParam)
	{
		case SHCNE_RENAMEITEM          : //0x00000001L
		case SHCNE_CREATE              : //0x00000002L
		case SHCNE_DELETE              : //0x00000004L
		case SHCNE_MKDIR               : //0x00000008L
		case SHCNE_RMDIR               : //0x00000010L
		case SHCNE_MEDIAINSERTED       : //0x00000020L
		case SHCNE_MEDIAREMOVED        : //0x00000040L
		case SHCNE_DRIVEREMOVED        : //0x00000080L
		case SHCNE_DRIVEADD            : //0x00000100L
		case SHCNE_NETSHARE            : //0x00000200L
		case SHCNE_NETUNSHARE          : //0x00000400L
		case SHCNE_ATTRIBUTES          : //0x00000800L
		case SHCNE_UPDATEDIR           : //0x00001000L
		case SHCNE_UPDATEITEM          : //0x00002000L
		case SHCNE_SERVERDISCONNECT    : //0x00004000L
		case SHCNE_UPDATEIMAGE         : //0x00008000L
		case SHCNE_DRIVEADDGUI         : //0x00010000L
		case SHCNE_RENAMEFOLDER        : //0x00020000L
		case SHCNE_FREESPACE           : //0x00040000L
			UpdateList ();
		break;

	default:
		TRACE (_T("EventID: %08x %d\n"), lParam, lParam);
		break;
	}
	return lReturn;
}

LRESULT CRecBinViewer::OnNotifyRBinDir (WPARAM wParam, LPARAM lParam)
{
	LRESULT lReturn = 0;
	SHNOTIFYSTRUCT shns;
	TCHAR szBefore[MAX_PATH];
	TCHAR szAfter[MAX_PATH];
	
	memcpy((void *)&shns,(void *)wParam,sizeof(SHNOTIFYSTRUCT));
	
	SHGetPathFromIDList((struct _ITEMIDLIST *)shns.dwItem1, szBefore);
	SHGetPathFromIDList((struct _ITEMIDLIST *)shns.dwItem2, szAfter);
	
	switch (lParam)
	{
		case SHCNE_RENAMEITEM          : //0x00000001L
		case SHCNE_CREATE              : //0x00000002L
		case SHCNE_DELETE              : //0x00000004L
		case SHCNE_MKDIR               : //0x00000008L
		case SHCNE_RMDIR               : //0x00000010L
		case SHCNE_MEDIAINSERTED       : //0x00000020L
		case SHCNE_MEDIAREMOVED        : //0x00000040L
		case SHCNE_DRIVEREMOVED        : //0x00000080L
		case SHCNE_DRIVEADD            : //0x00000100L
		case SHCNE_NETSHARE            : //0x00000200L
		case SHCNE_NETUNSHARE          : //0x00000400L
		case SHCNE_ATTRIBUTES          : //0x00000800L
		case SHCNE_UPDATEDIR           : //0x00001000L
		case SHCNE_UPDATEITEM          : //0x00002000L
		case SHCNE_SERVERDISCONNECT    : //0x00004000L
		case SHCNE_UPDATEIMAGE         : //0x00008000L
		case SHCNE_DRIVEADDGUI         : //0x00010000L
		case SHCNE_RENAMEFOLDER        : //0x00020000L
		case SHCNE_FREESPACE           : //0x00040000L
			UpdateList ();
		break;
		
	default:
		TRACE (_T("EventID: %08x %d"), lParam, lParam);		
		break;
	}
	
	return lReturn;
}

void CRecBinViewer::Install (HWND hWnd)
{	
    InitInterfaces ();
	m_hWnd = hWnd;
	InstallRBinNotify ();
	InstallShellNotify ();
	UpdateList ();
}

void CRecBinViewer::Remove  ()
{
	RemoveRBinNotify ();
	RemoveShellNotify ();

	if (NULL != m_pFolder2)
	{
		m_pFolder2->Release ();
	}

	if (NULL != m_pRecycleBin)
	{
		m_pRecycleBin->Release ();
	}

	m_pFolder2 = NULL;
	m_pRecycleBin = NULL;

}


void CRecBinViewer::InstallRBinNotify ()
{
	SHChangeNotifyEntry stPIDL;
	LPITEMIDLIST ppidl;
	
	SHGetSpecialFolderLocation(m_hWnd, CSIDL_BITBUCKET, &ppidl);
	
	stPIDL.pidl = ppidl;
	stPIDL.fRecursive = TRUE;

	m_hNotifyRBin = SHChangeNotifyRegister (m_hWnd, 
		SHCNF_ACCEPT_INTERRUPTS | SHCNF_ACCEPT_NON_INTERRUPTS, 
		SHCNE_ALLEVENTS, 
		WM_SHELLNOTIFY,				/* Message that would be sent by the Shell */
		1,
		&stPIDL);
	
	if(NULL == m_hNotifyRBin)
	{
		TRACE(_T("Change Register Failed for RecycleBin"));
	}
	
}

void CRecBinViewer::InstallShellNotify ()
{
	SHChangeNotifyEntry stPIDL;
	LPITEMIDLIST ppidl;
	
	int				iPos		= 0;
	TCHAR			szPath[MAX_PATH];		
	WIN32_FIND_DATA findData;
	HANDLE			hFindData	= INVALID_HANDLE_VALUE;
	HRESULT			hr			= S_OK;
	
	CDriveArray ar;

	GetDrives (ar);
	for (unsigned int i = 0; i < ar.size (); i++)
	{
		if (ar[i].m_nType != DRIVE_FIXED)	
			continue;
		
		SHQUERYRBINFO qrbi;
		ZeroMemory (&qrbi, sizeof (qrbi));
		qrbi.cbSize = sizeof (qrbi);
		hr = SHQueryRecycleBin (ar[i].m_Path, &qrbi);
		if (SUCCEEDED (hr))
		{
			ZeroMemory (&findData, sizeof (findData));
			wsprintf (szPath, _T("%sRecycler"), static_cast<LPCWSTR>(ar[i].m_Path));
			hFindData = FindFirstFile (szPath, &findData);
			if (INVALID_HANDLE_VALUE != hFindData)
			{
				ppidl = SHSimpleIDListFromPath (szPath);
				stPIDL.pidl = ppidl;
				m_pidlDrives[iPos] = ppidl;
				m_hNotifyDrives[iPos] = SHChangeNotifyRegister (m_hWnd, 
					SHCNF_ACCEPT_INTERRUPTS | SHCNF_ACCEPT_NON_INTERRUPTS, 
					SHCNE_RMDIR | SHCNE_RENAMEFOLDER | SHCNE_DELETE | SHCNE_RENAMEITEM, 
					WM_SHELLNOTIFYRBINDIR,
					1,
					&stPIDL);
				iPos ++;
				FindClose (hFindData);
			}
			else
			{
				ZeroMemory (&findData, sizeof (findData));
				wsprintf (szPath, _T("%sRecycled"), static_cast<LPCWSTR>(ar[i].m_Path));
				hFindData = FindFirstFile (szPath, &findData);
				if (INVALID_HANDLE_VALUE != hFindData)
				{
					ppidl = SHSimpleIDListFromPath (szPath);
					stPIDL.pidl = ppidl;
					m_pidlDrives[iPos] = ppidl;
					m_hNotifyDrives[iPos] = SHChangeNotifyRegister (m_hWnd, 
						SHCNF_ACCEPT_INTERRUPTS | SHCNF_ACCEPT_NON_INTERRUPTS, 
						SHCNE_RMDIR | SHCNE_RENAMEFOLDER | SHCNE_DELETE | SHCNE_RENAMEITEM, 
						WM_SHELLNOTIFYRBINDIR,
						1,
						&stPIDL);
					iPos ++;
					FindClose (hFindData);
				}
			}
		}			
	}
}

void CRecBinViewer::RemoveRBinNotify ()
{
	SHChangeNotifyDeregister(m_hNotifyRBin);
}

void CRecBinViewer::RemoveShellNotify ()
{
	int iPos = 0;	
	while (iPos < _countof (m_pidlDrives))
	{
		if (NULL != m_hNotifyDrives[iPos])
		{
			SHChangeNotifyDeregister(m_hNotifyDrives[iPos]);
		}
		if (NULL != m_pidlDrives[iPos])
		{
			CoTaskMemFree (m_pidlDrives[iPos]);
		}
		iPos ++;
	}

	ZeroMemory (&m_pidlDrives, sizeof (m_pidlDrives));
	ZeroMemory (&m_hNotifyDrives, sizeof (m_hNotifyDrives));
}

void CRecBinViewer::RemovePIDL ()
{	
	for (unsigned int iPos = 0 ; iPos < m_List.size () ; iPos ++)
	{
		LPITEMIDLIST pidl = m_List[iPos].m_PIDL;

		if (NULL != pidl)
		{
			CoTaskMemFree (pidl);
		}
	}	
}

BOOL CRecBinViewer::ExecCommand (int iItem, LPCTSTR lpszCommand)
{
	if (iItem >= static_cast<int>(m_List.size ()))
		return FALSE;

	BOOL bReturn = FALSE;
	LPITEMIDLIST	pidl = NULL;
	LPCONTEXTMENU pCtxMenu = NULL;
	HRESULT hr = S_OK;

	pidl = m_List[iItem].m_PIDL;
	
	if (NULL != m_pFolder2)	
		hr = m_pFolder2->GetUIObjectOf (m_hWnd, 1, (LPCITEMIDLIST *)&pidl, IID_IContextMenu, NULL, (LPVOID *)&pCtxMenu);
	
	else	
		hr = m_pRecycleBin->GetUIObjectOf (m_hWnd, 1, (LPCITEMIDLIST *)&pidl, IID_IContextMenu, NULL, (LPVOID *)&pCtxMenu);
	
	if (SUCCEEDED (hr))
		bReturn = ExecCommand (pCtxMenu, lpszCommand);
	
	pCtxMenu->Release();
	
	return bReturn;
}


void CRecBinViewer::UndeleteAll ()
{
	for (unsigned int i = 0; i< m_List.size (); i++)
		Undelete (i);
}


void CRecBinViewer::Undelete (int iPos)
{
	ExecCommand (iPos, _T("undelete"));
}

void CRecBinViewer::EmptyHelper ()
{
	SHEmptyRecycleBin (m_hWnd, NULL, 0);
}

void CRecBinViewer::Empty ()
{
	gPool.StartThread(RECYCLEBIN_THREAD,  this, &CRecBinViewer::EmptyHelper);
}

bool CRecBinViewer::OnDrop (COleDataObject *object, DROPEFFECT dwEffect , const TCHAR *pTo)
{
	CSelRowArray ar;

	CString source_folder;	
	CString dest_folder = pTo;

	if (!ReadHDropData (object, ar, source_folder))
		return false;

	UINT file_oper = FO_DELETE;
		
	return FileOperation (AfxGetMainWnd(), ar, file_oper, FOF_ALLOWUNDO  , dest_folder) != 0;	
}

void CRecBinViewer::CheckUpdate ()
{
	if (m_bNeedUpdate)
	{
		Fill (EMPTYSTR);
		m_bNeedUpdate = false;
	}
}

BOOL CRecBinViewer::ExecCommandOnSelection (LPCTSTR lpszCommand)
{
	if (!m_pGridCtrl)
		return false;

	int nItems = m_List.size();
	LPITEMIDLIST *pidlArray = (LPITEMIDLIST *) malloc ( (nItems) * sizeof (LPITEMIDLIST));
	int selCount = 0;
	for (POSITION pos = m_pGridCtrl->m_SelectedCellMap.GetStartPosition(); pos != NULL; )
    {
		DWORD key;
        CCellID cell;
        m_pGridCtrl->m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
		if (cell.col == 0)  
		{
			pidlArray[selCount] =  m_List[cell.row -1].m_PIDL;
			selCount++;
		}		
    }

	HRESULT hr = S_OK;
	LPCONTEXTMENU pCtxMenu = NULL;

	if (NULL != m_pFolder2)
		hr = m_pFolder2->GetUIObjectOf (m_hWnd, selCount, (LPCITEMIDLIST *)pidlArray, IID_IContextMenu, NULL, (LPVOID *)&pCtxMenu);	
	else
		hr = m_pRecycleBin->GetUIObjectOf (m_hWnd, selCount, (LPCITEMIDLIST *)pidlArray, IID_IContextMenu, NULL, (LPVOID *)&pCtxMenu);
	
	BOOL bReturn = FALSE;

	if (SUCCEEDED (hr))	
		bReturn = ExecCommand (pCtxMenu, lpszCommand);
	
	pCtxMenu->Release();
	free (pidlArray);

	return bReturn;
}

void CRecBinViewer::DeleteSelection () 
{
	ExecCommandOnSelection (_T("Delete"));
}


BOOL CRecBinViewer::ExecCommand (LPCONTEXTMENU pCtxMenu,  LPCTSTR lpszCommand)
{
	UINT uiID = UINT (-1);
	UINT uiCommand = 0;
	UINT uiMenuFirst = 1;
	UINT uiMenuLast = 0x00007FFF;
	HMENU hmenuCtx;
	int iMenuPos = 0;
	int iMenuMax = 0;
	TCHAR szMenuItem[MAX_PATH];	
	TCHAR verb[MAX_PATH] ;

	hmenuCtx = CreatePopupMenu();
	HRESULT hr = pCtxMenu->QueryContextMenu(hmenuCtx, 0, uiMenuFirst, uiMenuLast, CMF_NORMAL);

	iMenuMax = GetMenuItemCount(hmenuCtx);
	
	for (iMenuPos = 0 ; iMenuPos < iMenuMax; iMenuPos++)
	{
		GetMenuString(hmenuCtx, iMenuPos, szMenuItem, MAX_PATH, MF_BYPOSITION) ;
	
		uiID = GetMenuItemID(hmenuCtx, iMenuPos) ;
		
		if ((uiID == -1) || (uiID == 0))
		{
			
		}
		else
		{
			hr = pCtxMenu->GetCommandString(uiID - 1, GCS_VERB, NULL, (LPSTR)verb, MAX_PATH);
			if (FAILED (hr))
			{
				verb[0] = TCHAR ('\0') ;
			}
			else
			{
				if (0 == _tcsicmp (verb, lpszCommand))				
					uiCommand = uiID - 1;				
			}			
		}
	}
	
	if ((UINT)-1 != uiCommand)
	{
		CMINVOKECOMMANDINFO cmi;			
		ZeroMemory(&cmi, sizeof(CMINVOKECOMMANDINFO));
		cmi.cbSize			= sizeof(CMINVOKECOMMANDINFO);
		cmi.fMask			= CMIC_MASK_FLAG_NO_UI;
		cmi.hwnd			= m_hWnd;				
		cmi.lpVerb			= (LPSTR)MAKEINTRESOURCE (uiCommand);
		cmi.nShow			= SW_SHOWNORMAL;		
		hr = pCtxMenu->InvokeCommand(&cmi);			
		if (SUCCEEDED (hr))		
			return TRUE;		
	}

	return false;
}

CString CRecBinViewer::GetText (int row, int col)
{
	return m_List[row][col];
}

void CRecBinViewer::ContextMenu (CView* pView, CSelRowArray &ar, CPoint &pt) 
{
	CShellContextMenu scm;
	IShellFolder*psfRecycle = NULL;
	
	IShellFolder* psfDesktop = NULL;
	SHGetDesktopFolder (&psfDesktop);	
	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation (NULL, CSIDL_BITBUCKET, &pidl);
	
	psfDesktop->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID *)&psfRecycle);

	LPITEMIDLIST *pidlArray = (LPITEMIDLIST *) malloc (ar.size () * sizeof (LPITEMIDLIST));

	
	for (unsigned int i = 0 ; i < ar.size () ; i++)				
		pidlArray[i] = m_List[ar[i].m_nRow-1].m_PIDL;		
	
	scm.SetObjects (psfRecycle, pidlArray, ar.size ());
	free (pidlArray);
	psfDesktop->Release ();

	scm.ShowContextMenu (pView, pt);
	if (psfRecycle)
		psfRecycle->Release ();
	
}

void CRecBinViewer::Properties (CSelRowArray &ar)
{	
	CShellContextMenu scm;
	IShellFolder*psfRecycle = NULL;
	
	IShellFolder* psfDesktop = NULL;
	SHGetDesktopFolder (&psfDesktop);	
	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation (NULL, CSIDL_BITBUCKET, &pidl);
	
	psfDesktop->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID *)&psfRecycle);

	LPITEMIDLIST *pidlArray = (LPITEMIDLIST *) malloc (ar.size () * sizeof (LPITEMIDLIST));


	for (int i = 0 ; i < static_cast<int>(ar.size()) ; i++)				
		pidlArray[i] = m_List[ar[i].m_nRow-1].m_PIDL;		
	
	scm.SetObjects (psfRecycle, pidlArray, ar.size ());
	free (pidlArray);
	psfDesktop->Release ();

	scm.InvokeCommand (_T("properties"));	
	if (psfRecycle)
		psfRecycle->Release ();
}

void CRecBinViewer::FillHeader()
{
	m_State.m_Widths[0] = 200;
	m_State.m_Widths[1] = 120;
	m_State.m_Widths[2] = 120;
	m_State.m_Widths[3] = 140;
	m_State.m_Widths[4] = 120;		
}

void CRecBinViewer::OnClose ()
{
	SaveState (CONST_RECYCLEBIN);
}


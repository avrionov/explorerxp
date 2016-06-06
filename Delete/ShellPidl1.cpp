// ShellPidl1.cpp: implementation of the CShellPidl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "explorerxp.h"
#include "globals.h"
#include "ShellPidl1.h"
#include "PIDL.H"
#include "ShellContextMenu.h"
#include "UICoolMenu.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TPM_FLAGS     (TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD)

CShellPidl gShellPidl;

CShellPidl::CShellPidl()
{
	SHGetMalloc(&m_pMalloc);
    SHGetDesktopFolder(&m_psfDesktop);
	ZeroMemory(&m_EmptyPidl,sizeof(ITEMIDLIST));

}

CShellPidl::~CShellPidl()
{
	if (m_pMalloc)
		m_pMalloc->Release();
	if (m_psfDesktop)
		m_psfDesktop->Release();
}


IMalloc *CShellPidl::GetMalloc()
{
	return m_pMalloc;
}

LPSHELLFOLDER CShellPidl::GetDesktopFolder()
{
	return m_psfDesktop;
}


STDMETHODIMP CShellPidl::StrRetToStr(STRRET StrRet, LPTSTR* str, LPITEMIDLIST pidl)
{
	HRESULT hr = S_OK;
	int cch;
	LPTSTR strOffset;

	*str = NULL;  // Assume failure

	switch (StrRet.uType)
   {
		case STRRET_WSTR: 
			cch = WideCharToMultiByte(CP_ACP, 0, StrRet.pOleStr, -1, NULL, 0, NULL, NULL); 
			*str = (LPTSTR)m_pMalloc->Alloc(cch * sizeof(TCHAR)); 

			if (*str != NULL)
				WideCharToMultiByte(CP_ACP, 0, StrRet.pOleStr, -1, *str, cch, NULL, NULL); 
			else
				hr = E_FAIL;
			break;

		case STRRET_OFFSET: 
			strOffset = (((char *) pidl) + StrRet.uOffset);

			cch = strlen(strOffset) + 1; // NULL terminator
			*str = (LPTSTR)m_pMalloc->Alloc(cch * sizeof(TCHAR));

			if (*str != NULL)
				strcpy(*str, strOffset);
			else
				hr = E_FAIL;
			break;

		case STRRET_CSTR: 
			cch = strlen(StrRet.cStr) + 1; // NULL terminator
			*str = (LPTSTR)m_pMalloc->Alloc(cch * sizeof(TCHAR));

			if (*str != NULL)
				strcpy(*str, StrRet.cStr);
			else
				hr = E_FAIL;

			break;
	} 

	return hr;
}

STDMETHODIMP CShellPidl::SHPidlToPathEx(LPCITEMIDLIST pidl, CString &sPath, LPSHELLFOLDER pFolder, DWORD dwFlags)
{
	STRRET StrRetFilePath;
	LPTSTR pszFilePath = NULL;
	HRESULT hr=E_FAIL;

	if (pFolder == NULL)
		pFolder = GetDesktopFolder();

	if (pFolder == NULL)
		return E_FAIL;

	hr = pFolder->GetDisplayNameOf(pidl, dwFlags, &StrRetFilePath);
	if (SUCCEEDED(hr))
	{
		StrRetToStr(StrRetFilePath, &pszFilePath, (LPITEMIDLIST)pidl);
		sPath = pszFilePath;
	}
	if (pszFilePath)
		m_pMalloc->Free(pszFilePath);
	return hr;
}

bool CShellPidl::PopupTheMenu(HWND hwnd, LPSHELLFOLDER lpsfParent, LPITEMIDLIST  *plpi, UINT cidl, LPPOINT lppt)
{
	CMenu menu;
    menu.CreatePopupMenu();
	g_CoolMenuManager.Install(CWnd::FromHandle(hwnd));
	CString sPath;
	
	if (lpsfParent == NULL)
		lpsfParent = GetDesktopFolder();

	SHPidlToPathEx (*plpi,sPath,lpsfParent);
	CShellContextMenu shell_menu(hwnd,sPath,plpi,cidl,lpsfParent);
	shell_menu.SetMenu(&menu);
    int idCmd = menu.TrackPopupMenu(TPM_FLAGS, lppt->x, lppt->y, CWnd::FromHandle(hwnd));
	shell_menu.InvokeCommand(idCmd);
	g_CoolMenuManager.Uninstall();
	return true;

}


bool CShellPidl::PopupTheMenu (HWND hwnd, const char *parent, CSelRowArray &ar, LPPOINT lppt)
{

	if (ar.size () == 0)
		return false;

	LPITEMIDLIST *pidls=(LPITEMIDLIST*) GetMalloc()->Alloc(ar.size () * sizeof(LPITEMIDLIST));
	char pszPath[MAX_PATH];

	LPITEMIDLIST pilds_parent;
	SHPathToPidlEx (parent, &pilds_parent);
	SHGetPathFromIDList(pilds_parent, pszPath);

	IShellFolder *psfParent = NULL;

	HRESULT hres = GetDesktopFolder ()->BindToObject (pilds_parent, NULL, IID_IShellFolder, (LPVOID*)&psfParent);


	for (int i = 0; i < ar.size (); i++)
	{	
		   CString text;
		   char drive[_MAX_DRIVE];
		   char dir[_MAX_DIR];
		   char fname[_MAX_FNAME];
		   char ext[_MAX_EXT];

		   _splitpath( ar[i].m_Path, drive, dir, fname, ext );
			text = fname;
			text += ext;

			SHPathToPidlEx (text, &pidls[i], psfParent);
			SHGetPathFromIDList(pidls[i], pszPath);
	}
	
	

	//HRESULT hres = SHBindToParent (pidls[i], IID_IShellFolder, (void**)&psfParent, NULL);

	if (SUCCEEDED (hres))
	{
		PopupTheMenu(hwnd, psfParent,  pidls, ar.size (), lppt);
		psfParent->Release();

	}

	
	

	Free(pidls);
	return true;
}

STDMETHODIMP CShellPidl::SHPathToPidlEx(LPCTSTR szPath, LPITEMIDLIST* ppidl, LPSHELLFOLDER pFolder)
{
   OLECHAR wszPath[MAX_PATH] = {0};
   ULONG nCharsParsed = 0;
   LPSHELLFOLDER pShellFolder = NULL;
   BOOL bFreeOnExit = FALSE;
#ifdef UNICODE
   lstrcpy(wszPath,szPath);
#else
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPath, -1, wszPath, MAX_PATH);
#endif
   // Use the desktop's IShellFolder by default
   if(pFolder == NULL)
   {
      SHGetDesktopFolder(&pShellFolder);
      bFreeOnExit = TRUE;
   }
   else
      pShellFolder = pFolder;

   HRESULT hr = pShellFolder->ParseDisplayName(NULL, NULL, wszPath, &nCharsParsed, ppidl, NULL);

   if(bFreeOnExit)
      pShellFolder->Release();

   return hr;
}


void CShellPidl::FreePidl(LPITEMIDLIST pidl)
{
	if (m_pMalloc)
		m_pMalloc->Free(pidl);
}

void CShellPidl::Free(void *pv)
{
	if (m_pMalloc)
		m_pMalloc->Free(pv);
}
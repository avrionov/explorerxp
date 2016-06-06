// ShellPidl1.h: interface for the CShellPidl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLPIDL1_H__EC7CCC88_B6A6_4AE8_B819_DCD59D41506D__INCLUDED_)
#define AFX_SHELLPIDL1_H__EC7CCC88_B6A6_4AE8_B819_DCD59D41506D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShellPidl  
{
public:
	CShellPidl();
	virtual ~CShellPidl();

	IMalloc *CShellPidl::GetMalloc();
	LPSHELLFOLDER CShellPidl::GetDesktopFolder();
	STDMETHOD(StrRetToStr)(STRRET StrRet, LPTSTR* str, LPITEMIDLIST pidl);
	
	bool PopupTheMenu(HWND hwnd, LPSHELLFOLDER lpsfParent, LPITEMIDLIST  *plpi, UINT cidl, LPPOINT lppt);
	bool PopupTheMenu (HWND hwnd, const char *parent, CSelRowArray &ar, LPPOINT lppt);
	STDMETHOD(SHPathToPidlEx)(LPCTSTR szPath, LPITEMIDLIST* ppidl, LPSHELLFOLDER pFolder=NULL);
	STDMETHOD(SHPidlToPathEx)(LPCITEMIDLIST pidl, CString &sPath, LPSHELLFOLDER pFolder=NULL,DWORD dwFlags=SHGDN_FORPARSING);
	void FreePidl(LPITEMIDLIST pidl);
	void Free(void *pv);

private:
	ITEMIDLIST m_EmptyPidl;
	LPMALLOC m_pMalloc;
	LPSHELLFOLDER m_psfDesktop;

};


extern CShellPidl gShellPidl;

#endif // !defined(AFX_SHELLPIDL1_H__EC7CCC88_B6A6_4AE8_B819_DCD59D41506D__INCLUDED_)

// ResizableVersion.cpp: implementation of the CResizableVersion class.
//
/////////////////////////////////////////////////////////////////////////////
//
// This file is part of ResizableLib
// http://sourceforge.net/projects/resizablelib
//
// Copyright (C) 2000-2004 by Paolo Messina
// http://www.geocities.com/ppescher - mailto:ppescher@hotmail.com
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResizableVersion.h"

//////////////////////////////////////////////////////////////////////
// Static initializer object (with macros to hide in ClassView)

// static intializer must be called before user code
//#pragma warning(disable:4073)
//#pragma init_seg(lib)


//////////////////////////////////////////////////////////////////////
// Private implementation

// DLL Version support
#include <shlwapi.h>

static DLLVERSIONINFO g_dviCommCtrls;
// static OSVERSIONINFOEX g_osviWindows;

static void CheckCommCtrlsVersion()
{
	// Check Common Controls version
	ZeroMemory(&g_dviCommCtrls, sizeof(DLLVERSIONINFO));
	HMODULE hMod = ::LoadLibrary(_T("comctl32.dll"));
	if (hMod != NULL)
	{
		// Get the version function
		DLLGETVERSIONPROC pfnDllGetVersion;
		pfnDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hMod, "DllGetVersion");

		if (pfnDllGetVersion != NULL)
		{
			// Obtain version information
			g_dviCommCtrls.cbSize = sizeof(DLLVERSIONINFO);
			if (SUCCEEDED(pfnDllGetVersion(&g_dviCommCtrls)))
			{
				::FreeLibrary(hMod);
				return;
			}
		}

		::FreeLibrary(hMod);
	}

	// Set values for the worst case
	g_dviCommCtrls.dwMajorVersion = 4;
	g_dviCommCtrls.dwMinorVersion = 0;
	g_dviCommCtrls.dwBuildNumber = 0;
	g_dviCommCtrls.dwPlatformID = DLLVER_PLATFORM_WINDOWS;
}


//////////////////////////////////////////////////////////////////////
// Exported global symbols


// macro to convert version numbers to hex format


void InitRealVersions()
{
	CheckCommCtrlsVersion();
}

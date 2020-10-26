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
#pragma warning(disable:4073)
#pragma init_seg(lib)

#ifdef _UNDEFINED_
#define BEGIN_HIDDEN {
#define END_HIDDEN }
#else
#define BEGIN_HIDDEN
#define END_HIDDEN
#endif

BEGIN_HIDDEN
struct _VersionInitializer
{
	_VersionInitializer()
	{
		InitRealVersions();
	};
};
END_HIDDEN

// The one and only version-check object
static _VersionInitializer g_version;

//////////////////////////////////////////////////////////////////////
// Private implementation

// DLL Version support
#include <shlwapi.h>

static DLLVERSIONINFO g_dviCommCtrls;
static OSVERSIONINFOEX g_osviWindows;

static void CheckOsVersion()
{
	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	ZeroMemory(&g_osviWindows, sizeof(OSVERSIONINFOEX));
	g_osviWindows.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((LPOSVERSIONINFO)&g_osviWindows))
		return;
	
	// If that fails, try using the OSVERSIONINFO structure.
	g_osviWindows.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	if (GetVersionEx((LPOSVERSIONINFO)&g_osviWindows))
		return;

	// When all the above fails, set values for the worst case
	g_osviWindows.dwMajorVersion = 4;
	g_osviWindows.dwMinorVersion = 0;
	g_osviWindows.dwBuildNumber = 0;
	g_osviWindows.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
	g_osviWindows.szCSDVersion[0] = TEXT('\0');
}

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

DWORD realWINVER = 0;

#ifdef _WIN32_WINDOWS
DWORD real_WIN32_WINDOWS = 0;
#endif

#ifdef _WIN32_WINNT
DWORD real_WIN32_WINNT = 0;
#endif

#ifdef _WIN32_IE
DWORD real_WIN32_IE = 0;
#endif

// macro to convert version numbers to hex format
#define CNV_OS_VER(x) ((BYTE)(((BYTE)(x) / 10 * 16) | ((BYTE)(x) % 10)))

void InitRealVersions()
{
	CheckCommCtrlsVersion();
	CheckOsVersion();

	// set real version values

	realWINVER = MAKEWORD(CNV_OS_VER(g_osviWindows.dwMinorVersion),
		CNV_OS_VER(g_osviWindows.dwMajorVersion));

#ifdef _WIN32_WINDOWS
	if (g_osviWindows.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		real_WIN32_WINDOWS = realWINVER;
	else
		real_WIN32_WINDOWS = 0;
#endif

#ifdef _WIN32_WINNT
	if (g_osviWindows.dwPlatformId == VER_PLATFORM_WIN32_NT)
		real_WIN32_WINNT = realWINVER;
	else
		real_WIN32_WINNT = 0;
#endif

#ifdef _WIN32_IE
	switch (g_dviCommCtrls.dwMajorVersion)
	{
	case 4:
		switch (g_dviCommCtrls.dwMinorVersion)
		{
		case 70:
			real_WIN32_IE = 0x0300;
			break;
		case 71:
			real_WIN32_IE = 0x0400;
			break;
		case 72:
			real_WIN32_IE = 0x0401;
			break;
		default:
			real_WIN32_IE = 0x0200;
		}
		break;
	case 5:
		if (g_dviCommCtrls.dwMinorVersion > 80)
			real_WIN32_IE = 0x0501;
		else
			real_WIN32_IE = 0x0500;
		break;
	case 6:
		real_WIN32_IE = 0x0600;	// includes checks for 0x0560 (IE6)
		break;
	default:
		real_WIN32_IE = 0;
	}
#endif
}

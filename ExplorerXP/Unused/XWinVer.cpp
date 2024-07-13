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
#define XWINVER_CPP
#include "XWinVer.h"

// from winbase.h
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS      1
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT           2
#endif
#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE           3
#endif
// from winnt.h
#ifndef VER_NT_WORKSTATION
#define VER_NT_WORKSTATION              0x0000001
#endif
#ifndef VER_SUITE_PERSONAL
#define VER_SUITE_PERSONAL              0x00000200
#endif


///////////////////////////////////////////////////////////////////////////////
/*
    This table has been assembled from Usenet postings, personal
    observations, and reading other people's code.  Please feel
    free to add to it or correct it.


         dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
95             1              4               0             950
95 SP1         1              4               0        >950 && <=1080
95 OSR2        1              4             <10           >1080
98             1              4              10            1998
98 SP1         1              4              10       >1998 && <2183
98 SE          1              4              10          >=2183
ME             1              4              90            3000

NT 3.51        2              3              51            1057
NT 4           2              4               0            1381
2000           2              5               0            2195
XP             2              5               1            2600
2003           2              5               2            3790
Vista          2              6               0

CE 1.0         3              1               0
CE 2.0         3              2               0
CE 2.1         3              2               1
CE 3.0         3              3               0
*/

///////////////////////////////////////////////////////////////////////////////
//  per process data
BOOL CXWinVersion::m_bInitialized = FALSE;
CXWinVersion::OSVERSIONINFOEXX CXWinVersion::m_osinfo = { 0 };
DWORD CXWinVersion::m_dwVistaProductType = 0;

///////////////////////////////////////////////////////////////////////////////
// Init
void CXWinVersion::Init()
{
	TRACE(_T("in CXWinVersion::Init\n"));

	m_dwVistaProductType = 0;

	ZeroMemory(&m_osinfo, sizeof(m_osinfo));

	m_osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (GetVersionEx((LPOSVERSIONINFO) &m_osinfo))
	{
		m_bInitialized = TRUE;

		if ((m_osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
			(m_osinfo.dwMajorVersion >= 5))
		{
			// get extended version info for 2000 and later

			ZeroMemory(&m_osinfo, sizeof(m_osinfo));

			m_osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			GetVersionEx((LPOSVERSIONINFO) &m_osinfo);

			TRACE(_T("dwMajorVersion =%u\n"), m_osinfo.dwMajorVersion );
			TRACE(_T("dwMinorVersion =%u\n"), m_osinfo.dwMinorVersion );
			TRACE(_T("dwBuildNumber=%u\n"), m_osinfo.dwBuildNumber);
			TRACE(_T("suite mask=%u\n"), m_osinfo.wSuiteMask);
			TRACE(_T("product type=%u\n"), m_osinfo.wProductType);
			TRACE(_T("sp major=%u\n"), m_osinfo.wServicePackMajor);
			TRACE(_T("sp minor=%u\n"), m_osinfo.wServicePackMinor);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetWinVersionString
CString CXWinVersion::GetWinVersionString()
{
	CString strVersion = WUNKNOWNSTR;

	int nVersion = GetWinVersion();

	switch (nVersion)
	{
		default:
		case WUNKNOWN:								break;
		case W95:		strVersion = W95STR;		break;
		case W95SP1:	strVersion = W95SP1STR;		break;
		case W95OSR2:	strVersion = W95OSR2STR;	break;
		case W98:		strVersion = W98STR;		break;
		case W98SP1:	strVersion = W98SP1STR;		break;
		case W98SE:		strVersion = W98SESTR;		break;
		case WME:		strVersion = WMESTR;		break;
		case WNT351:	strVersion = WNT351STR;		break;
		case WNT4:		strVersion = WNT4STR;		break;
		case W2K:		strVersion = W2KSTR;		break;
		case WXP:		strVersion = WXPSTR;		break;
		case W2003:		strVersion = W2003STR;		break;
		case WVISTA:	strVersion = WVISTASTR;		break;
		case WCE:		strVersion = WCESTR;		break;
	}

	return strVersion;
}

///////////////////////////////////////////////////////////////////////////////
// GetWinVersion
int CXWinVersion::GetWinVersion()
{
	int nVersion = WUNKNOWN;

	DWORD dwPlatformId   = m_osinfo.dwPlatformId;
	DWORD dwMinorVersion = m_osinfo.dwMinorVersion;
	DWORD dwMajorVersion = m_osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = m_osinfo.dwBuildNumber & 0xFFFF;	// Win 9x needs this

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			nVersion = W95;
		}
		else if ((dwMinorVersion < 10) &&
				((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			nVersion = W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			nVersion = W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			nVersion = W98;
		}
		else if ((dwMinorVersion == 10) &&
				((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			nVersion = W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			nVersion = W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			nVersion = WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			nVersion = WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			nVersion = WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			nVersion = W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			nVersion = WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			nVersion = W2003;
		}
		else if ((dwMajorVersion == 6) && (dwMinorVersion == 0))
		{
			nVersion = WVISTA;
			GetVistaProductType();
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	{
		nVersion = WCE;
	}

	return nVersion;
}

///////////////////////////////////////////////////////////////////////////////
// GetServicePackNT - returns a valid service pack number only for NT platform
int CXWinVersion::GetServicePackNT()
{
	int nServicePack = 0;

	for (int i = 0;
		 (m_osinfo.szCSDVersion[i] != _T('\0')) &&
			 (i < (sizeof(m_osinfo.szCSDVersion)/sizeof(TCHAR)));
		 i++)
	{
		if (_istdigit(m_osinfo.szCSDVersion[i]))
		{
			nServicePack = _ttoi(&m_osinfo.szCSDVersion[i]);
			break;
		}
	}

	return nServicePack;
}

///////////////////////////////////////////////////////////////////////////////
// IsXP
BOOL CXWinVersion::IsXP()
{
	if (GetWinVersion() == WXP)
	{
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsXPHome
BOOL CXWinVersion::IsXPHome()
{
	if (GetWinVersion() == WXP)
	{
		if (m_osinfo.wSuiteMask & VER_SUITE_PERSONAL)
			return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsXPPro
BOOL CXWinVersion::IsXPPro()
{
	if (GetWinVersion() == WXP)
	{
		if ((m_osinfo.wProductType == VER_NT_WORKSTATION) && !IsXPHome())
			return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsXPSP2
BOOL CXWinVersion::IsXPSP2()
{
	if (GetWinVersion() == WXP)
	{
		if (GetServicePackNT() == 2)
			return TRUE;
	}

	return FALSE;
}

#ifndef SM_MEDIACENTER
#define SM_MEDIACENTER          87
#endif

///////////////////////////////////////////////////////////////////////////////
// IsMediaCenter
BOOL CXWinVersion::IsMediaCenter()
{
	if (GetSystemMetrics(SM_MEDIACENTER))
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsWin2003
BOOL CXWinVersion::IsWin2003()
{
	if ((m_osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) && 
		(m_osinfo.dwMajorVersion == 5) &&
		(m_osinfo.dwMinorVersion == 2))
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// GetVistaProductType
DWORD CXWinVersion::GetVistaProductType()
{
	if (m_dwVistaProductType == 0)
	{
		typedef BOOL (FAR PASCAL * lpfnGetProductInfo) (DWORD, DWORD, DWORD, DWORD, PDWORD);

		HMODULE hKernel32 = GetModuleHandle(_T("KERNEL32.DLL"));
		if (hKernel32)
		{
			lpfnGetProductInfo pGetProductInfo = (lpfnGetProductInfo) GetProcAddress(hKernel32, "GetProductInfo"); 

			if (pGetProductInfo)
				pGetProductInfo(6, 0, 0, 0, &m_dwVistaProductType);
		}  
	}

	return m_dwVistaProductType;
}

///////////////////////////////////////////////////////////////////////////////
// GetVistaProductString
CString CXWinVersion::GetVistaProductString()
{
	CString strProductType = _T("");

	switch (m_dwVistaProductType)
	{
		case PRODUCT_BUSINESS:						 strProductType = _T("Business Edition"); break;
		case PRODUCT_BUSINESS_N:					 strProductType = _T("Business Edition"); break;
		case PRODUCT_CLUSTER_SERVER:				 strProductType = _T("Cluster Server Edition"); break;
		case PRODUCT_DATACENTER_SERVER:				 strProductType = _T("Server Datacenter Edition (full installation)"); break;
		case PRODUCT_DATACENTER_SERVER_CORE:		 strProductType = _T("Server Datacenter Edition (core installation)"); break;
		case PRODUCT_ENTERPRISE:					 strProductType = _T("Enterprise Edition"); break;
		case PRODUCT_ENTERPRISE_N:					 strProductType = _T("Enterprise Edition"); break;
		case PRODUCT_ENTERPRISE_SERVER:				 strProductType = _T("Server Enterprise Edition (full installation)"); break;
		case PRODUCT_ENTERPRISE_SERVER_CORE:		 strProductType = _T("Server Enterprise Edition (core installation)"); break;
		case PRODUCT_ENTERPRISE_SERVER_IA64:		 strProductType = _T("Server Enterprise Edition for Itanium-based Systems"); break;
		case PRODUCT_HOME_BASIC:					 strProductType = _T("Home Basic Edition"); break;
		case PRODUCT_HOME_BASIC_N:					 strProductType = _T("Home Basic Edition"); break;
		case PRODUCT_HOME_PREMIUM:					 strProductType = _T("Home Premium Edition"); break;
		case PRODUCT_HOME_PREMIUM_N:				 strProductType = _T("Home Premium Edition"); break;
		case PRODUCT_HOME_SERVER:					 strProductType = _T("Home Server Edition"); break;
		case PRODUCT_SERVER_FOR_SMALLBUSINESS:		 strProductType = _T("Server for Small Business Edition"); break;
		case PRODUCT_SMALLBUSINESS_SERVER:			 strProductType = _T("Small Business Server"); break;
		case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:	 strProductType = _T("Small Business Server Premium Edition"); break;
		case PRODUCT_STANDARD_SERVER:				 strProductType = _T("Server Standard Edition (full installation)"); break;
		case PRODUCT_STANDARD_SERVER_CORE:			 strProductType = _T("Server Standard Edition (core installation)"); break;
		case PRODUCT_STARTER:						 strProductType = _T("Starter Edition"); break;
		case PRODUCT_STORAGE_ENTERPRISE_SERVER:		 strProductType = _T("Storage Server Enterprise Edition"); break;
		case PRODUCT_STORAGE_EXPRESS_SERVER:		 strProductType = _T("Storage Server Express Edition"); break;
		case PRODUCT_STORAGE_STANDARD_SERVER:		 strProductType = _T("Storage Server Standard Edition"); break;
		case PRODUCT_STORAGE_WORKGROUP_SERVER:		 strProductType = _T("Storage Server Workgroup Edition"); break;
		case PRODUCT_UNDEFINED:						 strProductType = _T("An unknown product"); break;
		case PRODUCT_ULTIMATE:						 strProductType = _T("Ultimate Edition"); break;
		case PRODUCT_ULTIMATE_N:					 strProductType = _T("Ultimate Edition"); break;
		case PRODUCT_WEB_SERVER:					 strProductType = _T("Web Server Edition"); break;

		default: break;
	}

	return strProductType;
}

///////////////////////////////////////////////////////////////////////////////
// IsVista
BOOL CXWinVersion::IsVista()
{
	if (GetWinVersion() == WVISTA)
	{
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsVistaHome
BOOL CXWinVersion::IsVistaHome()
{
	if (GetWinVersion() == WVISTA)
	{
		switch (m_dwVistaProductType)
		{
			case PRODUCT_HOME_BASIC:	
			case PRODUCT_HOME_BASIC_N:	
			case PRODUCT_HOME_PREMIUM:	
			case PRODUCT_HOME_PREMIUM_N:
			case PRODUCT_HOME_SERVER:	
				return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsVistaBusiness
BOOL CXWinVersion::IsVistaBusiness()
{
	if (GetWinVersion() == WVISTA)
	{
		switch (m_dwVistaProductType)
		{
			case PRODUCT_BUSINESS:	
			case PRODUCT_BUSINESS_N:
				return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsVistaEnterprise
BOOL CXWinVersion::IsVistaEnterprise()
{
	if (GetWinVersion() == WVISTA)
	{
		switch (m_dwVistaProductType)
		{
			case PRODUCT_ENTERPRISE:			
			case PRODUCT_ENTERPRISE_N:			
			case PRODUCT_ENTERPRISE_SERVER:		
			case PRODUCT_ENTERPRISE_SERVER_CORE:
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsVistaUltimate
BOOL CXWinVersion::IsVistaUltimate()
{
	if (GetWinVersion() == WVISTA)
	{
		switch (m_dwVistaProductType)
		{
			case PRODUCT_ULTIMATE:	
			case PRODUCT_ULTIMATE_N:
				return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsWin2KorLater
BOOL CXWinVersion::IsWin2KorLater()
{
	if ((m_osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) && 
		(m_osinfo.dwMajorVersion >= 5))
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsXPorLater
BOOL CXWinVersion::IsXPorLater()
{
	if ((m_osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) && 
		(((m_osinfo.dwMajorVersion == 5) && (m_osinfo.dwMinorVersion > 0)) || 
		(m_osinfo.dwMajorVersion > 5)))
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsWin95
BOOL CXWinVersion::IsWin95()
{
	if ((m_osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && 
		(m_osinfo.dwMajorVersion == 4) && 
		(m_osinfo.dwMinorVersion < 10))
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsWin98
BOOL CXWinVersion::IsWin98()
{
	if ((m_osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && 
		(m_osinfo.dwMajorVersion == 4) && 
		(m_osinfo.dwMinorVersion >= 10))
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// IsWinCE
BOOL CXWinVersion::IsWinCE()
{
	return (GetWinVersion() == WCE);
}


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

#ifndef XWINVER_H
#define XWINVER_H

///////////////////////////////////////////////////////////////////////////////
//
// version strings
//
#define WUNKNOWNSTR	_T("unknown Windows version")

#define W95STR		_T("Windows 95")
#define W95SP1STR	_T("Windows 95 SP1")
#define W95OSR2STR	_T("Windows 95 OSR2")
#define W98STR		_T("Windows 98")
#define W98SP1STR	_T("Windows 98 SP1")
#define W98SESTR	_T("Windows 98 SE")
#define WMESTR		_T("Windows ME")

#define WNT351STR	_T("Windows NT 3.51")
#define WNT4STR		_T("Windows NT 4")
#define W2KSTR		_T("Windows 2000")
#define WXPSTR		_T("Windows XP")
#define WVISTASTR	_T("Windows Vista")
#define W2003STR	_T("Windows Server 2003")

#define WCESTR		_T("Windows CE")


///////////////////////////////////////////////////////////////////////////////
//
// version integer values
//
#define WUNKNOWN	0

#define W9XFIRST	1
#define W95			1
#define W95SP1		2
#define W95OSR2		3
#define W98			4
#define W98SP1		5
#define W98SE		6
#define WME			7
#define W9XLAST		99

#define WNTFIRST	101
#define WNT351		101
#define WNT4		102
#define W2K			103
#define WXP			104
#define W2003		105
#define WVISTA		106
#define WNTLAST		199

#define WCEFIRST	201
#define WCE			201
#define WCELAST		299

// flags returned from GetVistaProductType()
#ifndef PRODUCT_BUSINESS
#define PRODUCT_BUSINESS						0x00000006 	// Business Edition
#define PRODUCT_BUSINESS_N						0x00000010 	// Business Edition
#define PRODUCT_CLUSTER_SERVER					0x00000012 	// Cluster Server Edition
#define PRODUCT_DATACENTER_SERVER				0x00000008 	// Server Datacenter Edition (full installation)
#define PRODUCT_DATACENTER_SERVER_CORE			0x0000000C 	// Server Datacenter Edition (core installation)
#define PRODUCT_ENTERPRISE						0x00000004 	// Enterprise Edition
#define PRODUCT_ENTERPRISE_SERVER				0x0000000A 	// Server Enterprise Edition (full installation)
#define PRODUCT_ENTERPRISE_SERVER_CORE			0x0000000E 	// Server Enterprise Edition (core installation)
#define PRODUCT_ENTERPRISE_SERVER_IA64			0x0000000F 	// Server Enterprise Edition for Itanium-based Systems
#define PRODUCT_HOME_BASIC						0x00000002 	// Home Basic Edition
#define PRODUCT_HOME_BASIC_N					0x00000005 	// Home Basic Edition
#define PRODUCT_HOME_PREMIUM					0x00000003 	// Home Premium Edition
#define PRODUCT_HOME_SERVER						0x00000013 	// Home Server Edition
#define PRODUCT_SERVER_FOR_SMALLBUSINESS		0x00000018 	// Server for Small Business Edition
#define PRODUCT_SMALLBUSINESS_SERVER			0x00000009 	// Small Business Server
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM	0x00000019 	// Small Business Server Premium Edition
#define PRODUCT_STANDARD_SERVER					0x00000007 	// Server Standard Edition (full installation)
#define PRODUCT_STANDARD_SERVER_CORE			0x0000000D 	// Server Standard Edition (core installation)
#define PRODUCT_STARTER							0x0000000B 	// Starter Edition
#define PRODUCT_STORAGE_ENTERPRISE_SERVER		0x00000017 	// Storage Server Enterprise Edition
#define PRODUCT_STORAGE_EXPRESS_SERVER			0x00000014 	// Storage Server Express Edition
#define PRODUCT_STORAGE_STANDARD_SERVER			0x00000015 	// Storage Server Standard Edition
#define PRODUCT_STORAGE_WORKGROUP_SERVER		0x00000016 	// Storage Server Workgroup Edition
#define PRODUCT_UNDEFINED						0x00000000 	// An unknown product
#define PRODUCT_ULTIMATE						0x00000001 	// Ultimate Edition
#define PRODUCT_WEB_SERVER						0x00000011 	// Web Server Edition
#endif

#define PRODUCT_HOME_PREMIUM_N					0x0000001A 	// Home Premium Edition
#define PRODUCT_ENTERPRISE_N					0x0000001B 	// Enterprise Edition
#define PRODUCT_ULTIMATE_N						0x0000001C 	// Ultimate Edition

///////////////////////////////////////////////////////////////////////////////
//
// CXWinVersion singleton class
//
class CXWinVersion
{
// Construction
public:
	CXWinVersion()
	{
		TRACE(_T("in CXWinVersion\n"));
		if (!m_bInitialized)
		{
			Init();
		}
		else
		{
			TRACE(_T("already initialized\n"));
		}
	}

// Attributes
public:
	int		GetMajorVersion()		{ return m_osinfo.dwMajorVersion; }
	int		GetMinorVersion()		{ return m_osinfo.dwMinorVersion; }
	int		GetBuildNumber()		{ return (m_osinfo.dwBuildNumber & 0xFFFF); }	// needed for 9x
	int		GetPlatformId()			{ return (m_osinfo.dwPlatformId); }
	int		GetServicePackNT();
	CString	GetServicePackString()	{ return m_osinfo.szCSDVersion; }
	CString GetVistaProductString();
	DWORD	GetVistaProductType();
	int		GetWinVersion();
	CString	GetWinVersionString();
	BOOL	IsWin95();
	BOOL	IsWin98();
	BOOL	IsWin2KorLater();
	BOOL	IsWin2003();
	BOOL	IsWinCE();
	BOOL	IsXP();
	BOOL	IsXPorLater();
	BOOL	IsXPHome();
	BOOL	IsXPPro();
	BOOL	IsXPSP2();
	BOOL	IsMediaCenter();
	BOOL	IsVista();
	BOOL	IsVistaHome();
	BOOL	IsVistaBusiness();
	BOOL	IsVistaEnterprise();
	BOOL	IsVistaUltimate();

// Implementation
private:
	void Init();

	struct OSVERSIONINFOEXX
	{  
		DWORD dwOSVersionInfoSize;  
		DWORD dwMajorVersion;  
		DWORD dwMinorVersion;  
		DWORD dwBuildNumber;  
		DWORD dwPlatformId;  
		TCHAR szCSDVersion[128];  
		WORD  wServicePackMajor;  
		WORD  wServicePackMinor;  
		WORD  wSuiteMask;  
		BYTE  wProductType;  
		BYTE  wReserved;
	};

	static BOOL m_bInitialized;
	static OSVERSIONINFOEXX m_osinfo;
	static DWORD m_dwVistaProductType;
};


#endif //XWINVER_H

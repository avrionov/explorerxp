/////////////////////////////////////////////////////////////////////////////
// GlobalData.h: global definitions
//
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 by Nikolay Denisov. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Please email bug reports, bug fixes, enhancements, requests and
// comments to: nick@actor.ru
/////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBALDATA_H__
#define __GLOBALDATA_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// Copied from the latest Platform SDK available.
// This should help if you don't have SDK installed on your PC.

/////////////////////////////////////////////////////////////////////////////
// COMCTL32 v5.80 specific:

#ifndef RBBS_USECHEVRON
#define RBBS_USECHEVRON     0x00000200
#define RBHT_CHEVRON        0x0008
#define RB_PUSHCHEVRON      (WM_USER + 43)
#define RBN_CHEVRONPUSHED   (RBN_FIRST - 10)

typedef struct tagNMREBARCHEVRON
{
    NMHDR hdr;
    UINT uBand;
    UINT wID;
    LPARAM lParam;
    RECT rc;
    LPARAM lParamNM;
} NMREBARCHEVRON, *LPNMREBARCHEVRON;

#endif  // !RBBS_USECHEVRON

#ifndef BTNS_WHOLEDROPDOWN
#define BTNS_WHOLEDROPDOWN      0x0080
#endif  // !BTNS_WHOLEDROPDOWN

#ifndef TBN_INITCUSTOMIZE
#define TBN_INITCUSTOMIZE       (TBN_FIRST - 23)
#define TBNRF_HIDEHELP          0x00000001
#endif  // !TBN_INITCUSTOMIZE

#ifndef TBN_SAVE
#define TBN_SAVE                (TBN_FIRST - 22)
#define TBN_RESTORE             (TBN_FIRST - 21)

typedef struct tagNMTBSAVE
{
    NMHDR hdr;
    DWORD* pData;
    DWORD* pCurrent;
    UINT cbData;
    int iItem;
    int cButtons;
    TBBUTTON tbButton;
} NMTBSAVE, *LPNMTBSAVE;

typedef struct tagNMTBRESTORE
{
    NMHDR hdr;
    DWORD* pData;
    DWORD* pCurrent;
    UINT cbData;
    int iItem;
    int cButtons;
    int cbBytesPerRecord;
    TBBUTTON tbButton;
} NMTBRESTORE, *LPNMTBRESTORE;

#endif  // !TBN_SAVE

/////////////////////////////////////////////////////////////////////////////
// COMCTL32 v5.81 specific:

#ifndef TBSTYLE_EX_MIXEDBUTTONS
#define TBSTYLE_EX_MIXEDBUTTONS         0x00000008
#define TBSTYLE_EX_HIDECLIPPEDBUTTONS   0x00000010
#endif  // !TBSTYLE_EX_MIXEDBUTTONS

#ifndef BTNS_SHOWTEXT
#define BTNS_SHOWTEXT       0x0040
#endif  // !BTNS_SHOWTEXT

#ifndef I_IMAGENONE
#define I_IMAGENONE         (-2)
#endif  // !I_IMAGENONE

/////////////////////////////////////////////////////////////////////////////
// W98 specific:

#ifndef AW_SLIDE
#define AW_SLIDE                0x00040000
#define AW_HOR_POSITIVE         0x00000001
#define AW_HOR_NEGATIVE         0x00000002
#define AW_VER_POSITIVE         0x00000004
#define AW_VER_NEGATIVE         0x00000008
#endif  // !AW_SLIDE

#ifndef TPM_HORPOSANIMATION
#define TPM_HORPOSANIMATION     0x0400L
#define TPM_HORNEGANIMATION     0x0800L
#define TPM_VERPOSANIMATION     0x1000L
#define TPM_VERNEGANIMATION     0x2000L
#endif  // !TPM_HORPOSANIMATION

#ifndef SPI_GETMENUANIMATION
#define SPI_GETMENUANIMATION    0x1002
#define SPI_GETMENUUNDERLINES   0x100A
#endif  // !SPI_GETMENUANIMATION

#if ( WINVER < 0x0500 )

#define MIIM_STRING         0x00000040
#define MIIM_BITMAP         0x00000080
#define MIIM_FTYPE          0x00000100

#define HBMMENU_SYSTEM      ((HBITMAP)  1)
#define HBMMENU_CALLBACK    ((HBITMAP) -1)

struct MENUITEMINFO_WIN50 : MENUITEMINFO
{
    HBITMAP  hbmpItem;
};

#define MIM_STYLE       0x00000010
#define MNS_NOCHECK     0x80000000
#define MNS_CHECKORBMP  0x04000000

struct MENUINFO_WIN50
{
    DWORD   cbSize;
    DWORD   fMask;
    DWORD   dwStyle;
    UINT    cyMax;
    HBRUSH  hbrBack;
    DWORD   dwContextHelpID;
    ULONG   dwMenuData;
};

BOOL GetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi );
BOOL SetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi );
BOOL AnimateWindowWin50( HWND hwnd, DWORD dwTime, DWORD dwFlags );

#else

#define MENUINFO_WIN50      MENUINFO
#define MENUITEMINFO_WIN50  MENUITEMINFO

#define GetMenuInfoWin50    GetMenuInfo
#define SetMenuInfoWin50    SetMenuInfo
#define AnimateWindowWin50  AnimateWindow

#endif  // WINVER < 0x0500

/////////////////////////////////////////////////////////////////////////////
// W2K specific:

#ifndef DT_HIDEPREFIX
#define DT_HIDEPREFIX           0x00100000
#endif  // !DT_HIDEPREFIX

#ifndef AW_BLEND
#define AW_BLEND                0x00080000
#endif  // !AW_BLEND

#ifndef TPM_NOANIMATION
#define TPM_NOANIMATION         0x4000L
#endif  // !TPM_NOANIMATION

#ifndef SPI_GETMENUFADE
#define SPI_GETMENUFADE         0x1012
#define SPI_GETUIEFFECTS        0x103E
#endif  // !SPI_GETMENUFADE

/////////////////////////////////////////////////////////////////////////////
// Custom ROPs

#define ROP_PSDPxax 0x00B8074A

/////////////////////////////////////////////////////////////////////////////
// Private messages

#define WM_MB_SHOWPOPUPMENU     ( WM_USER + 100 )
#define WM_TB_ENDMODALLOOP      ( WM_USER + 101 )
#define WM_REBAR_CHILDSIZE      ( WM_USER + 102 )
#define WM_REBAR_CHEVRONPUSHED  ( WM_USER + 103 )
#define WM_REBAR_CONTEXTMENU    ( WM_USER + 104 )

/////////////////////////////////////////////////////////////////////////////
// Custom maps

typedef CMap< UINT, UINT, int, int > CMapUIntToInt;
typedef CMap< int, int, int, int >   CMapIntToInt;

/////////////////////////////////////////////////////////////////////////////
// Inline functions

#if ( WINVER < 0x0500 )
inline FARPROC GetProcAddress( LPCTSTR lpModuleName, LPCSTR lpProcName )
{
    HMODULE hModule = ::GetModuleHandle( lpModuleName );
    ASSERT( hModule != 0 );

    return ::GetProcAddress( hModule, lpProcName );
}

inline BOOL GetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi )
{
    typedef BOOL ( WINAPI* GETMENUINFO )( HMENU hmenu, const MENUINFO_WIN50* lpcmi );
    GETMENUINFO pfGetMenuInfo = ( GETMENUINFO )::GetProcAddress( _T("USER32"), "GetMenuInfo");
    ASSERT( pfGetMenuInfo != 0 );

    return pfGetMenuInfo( hmenu, lpcmi );
}

inline BOOL SetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi )
{
    typedef BOOL ( WINAPI* SETMENUINFO )( HMENU hmenu, const MENUINFO_WIN50* lpcmi );
    SETMENUINFO pfSetMenuInfo = ( SETMENUINFO )::GetProcAddress( _T("USER32"), "SetMenuInfo");
    ASSERT( pfSetMenuInfo != 0 );

    return pfSetMenuInfo( hmenu, lpcmi );
}

inline BOOL AnimateWindowWin50( HWND hwnd, DWORD dwTime, DWORD dwFlags )
{
    typedef BOOL ( WINAPI* ANIMATEWINDOW )( HWND hwnd, DWORD dwTime, DWORD dwFlags );
    ANIMATEWINDOW pfAnimateWindow = ( ANIMATEWINDOW )::GetProcAddress( _T("USER32"), "AnimateWindow");
    ASSERT( pfAnimateWindow != 0 );

    return pfAnimateWindow( hwnd, dwTime, dwFlags );
}
#endif  // WINVER < 0x0500

/////////////////////////////////////////////////////////////////////////////
#endif  // !__GLOBALDATA_H__

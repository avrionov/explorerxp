/* Copyright 2002-2020 Nikolay Avrionov. All Rights Reserved.
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
#include "ShortcutManager.h"
#include "wclassdefines.h"
#include "winclasses.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// edit shortcuts
DWORD EDIT_SHRTCUTS[] = {
  MAKELONG('C', HOTKEYF_CONTROL),                    // copy
  MAKELONG('V', HOTKEYF_CONTROL),                    // paste
  MAKELONG('X', HOTKEYF_CONTROL),                    // cut
  MAKELONG('Z', HOTKEYF_CONTROL),                    // undo
  MAKELONG('\t', HOTKEYF_CONTROL),                   // tab
  MAKELONG(VK_LEFT, HOTKEYF_CONTROL | HOTKEYF_EXT),  // left one word
  MAKELONG(VK_RIGHT, HOTKEYF_CONTROL | HOTKEYF_EXT), // right one word
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShortcutManager::CShortcutManager(BOOL bAutoSendCmds)
    : m_bAutoSendCmds(bAutoSendCmds), m_wInvalidComb(0),
      m_wFallbackModifiers(0) {}

CShortcutManager::~CShortcutManager() {}

BOOL CShortcutManager::Initialize(WORD wInvalidComb, WORD wFallbackModifiers) {
  if (wInvalidComb && !IsHooked()) {
    CWnd *pWnd = AfxGetMainWnd();

    if (pWnd && HookWindow(*pWnd)) {
      m_wInvalidComb = wInvalidComb;
      m_wFallbackModifiers = wFallbackModifiers;
      LoadSettings();

      return TRUE;
    }
  }

  return FALSE;
}

void CShortcutManager::PreDetachWindow() { SaveSettings(); }

void CShortcutManager::SetShortcut(UINT nCmdID, WORD wVirtKeyCode,
                                   WORD wModifiers) {
  UINT nOtherCmdID = 0;
  DWORD dwShortcut = MAKELONG(wVirtKeyCode, wModifiers);

  // if the shortcut == 0 then remove the existing shortcut associated with
  // nCmdID
  if (!dwShortcut) {
    m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

    m_mapShortcut2ID.RemoveKey(dwShortcut);
    m_mapID2Shortcut.RemoveKey(nCmdID);

    return;
  }
  // check for existing cmds using this shortcut to remove
  else if (m_mapShortcut2ID.Lookup(dwShortcut, nOtherCmdID)) {
    m_mapShortcut2ID.RemoveKey(dwShortcut);
    m_mapID2Shortcut.RemoveKey(nOtherCmdID);
  }

  // then simple add
  AddShortcut(nCmdID, wVirtKeyCode, wModifiers);
}

void CShortcutManager::SetShortcut(UINT nCmdID, DWORD dwShortcut) {
  SetShortcut(nCmdID, LOWORD(dwShortcut), HIWORD(dwShortcut));
}

BOOL CShortcutManager::AddShortcut(UINT nCmdID, DWORD dwShortcut) {
  return AddShortcut(nCmdID, LOWORD(dwShortcut), HIWORD(dwShortcut));
}

BOOL CShortcutManager::AddShortcut(UINT nCmdID, WORD wVirtKeyCode,
                                   WORD wModifiers) {
  // test for invalid modifiers
  if (ValidateModifiers(wModifiers, wVirtKeyCode) != wModifiers)
    return FALSE;

  // check for existing cmds using this shortcut
  DWORD dwShortcut = MAKELONG(wVirtKeyCode, wModifiers);

  if (!nCmdID || !dwShortcut)
    return FALSE;

  UINT nOtherCmdID = 0;

  if (m_mapShortcut2ID.Lookup(dwShortcut, nOtherCmdID) && nOtherCmdID)
    return FALSE;

  // check for existing shortcut on this cmd that we'll need to clean up
  DWORD dwOtherShortcut = 0;

  if (m_mapID2Shortcut.Lookup(nCmdID, dwOtherShortcut))
    m_mapShortcut2ID.RemoveKey(dwOtherShortcut);

  m_mapShortcut2ID[dwShortcut] = nCmdID;
  m_mapID2Shortcut[nCmdID] = dwShortcut;

  return TRUE;
}

WORD CShortcutManager::ValidateModifiers(WORD wModifiers,
                                         WORD wVirtKeyCode) const {
  if (!m_wInvalidComb) // optimization
    return wModifiers;

  // check for our special modifiers first
  if ((m_wInvalidComb & HKCOMB_EXFKEYS) &&
      (wVirtKeyCode >= VK_F1 && wVirtKeyCode <= VK_F24))
    return wModifiers;

  // test for invalid combinations
  BOOL bCtrl = (wModifiers & HOTKEYF_CONTROL);
  BOOL bShift = (wModifiers & HOTKEYF_SHIFT);
  BOOL bAlt = (wModifiers & HOTKEYF_ALT);
  BOOL bExtended = (wModifiers & HOTKEYF_EXT);

  BOOL bFail = ((m_wInvalidComb & HKCOMB_NONE) && !bCtrl && !bShift && !bAlt);

  bFail |= ((m_wInvalidComb & HKCOMB_S) && !bCtrl && bShift && !bAlt);
  bFail |= ((m_wInvalidComb & HKCOMB_C) && bCtrl && !bShift && !bAlt);
  bFail |= ((m_wInvalidComb & HKCOMB_A) && !bCtrl && !bShift && bAlt);
  bFail |= ((m_wInvalidComb & HKCOMB_SC) && bCtrl && bShift && !bAlt);
  bFail |= ((m_wInvalidComb & HKCOMB_SA) && !bCtrl && bShift && bAlt);
  bFail |= ((m_wInvalidComb & HKCOMB_CA) && bCtrl && !bShift && bAlt);
  bFail |= ((m_wInvalidComb & HKCOMB_SCA) && bCtrl && bShift && bAlt);

  if (bFail)
    return (m_wFallbackModifiers | (bExtended ? HOTKEYF_EXT : 0));

  // else ok
  return wModifiers;
}

DWORD CShortcutManager::GetShortcut(WORD wVirtKeyCode, BOOL bExtended) const {
  BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
  BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
  BOOL bAlt = (GetKeyState(VK_MENU) & 0x8000);

  WORD wModifiers = (bCtrl ? HOTKEYF_CONTROL : 0) |
                    (bShift ? HOTKEYF_SHIFT : 0) | (bAlt ? HOTKEYF_ALT : 0) |
                    (bExtended ? HOTKEYF_EXT : 0);

  return MAKELONG(wVirtKeyCode, wModifiers);
}

UINT CShortcutManager::ProcessMessage(MSG *pMsg) const {
  // only process accelerators if we are enabled and visible
  if (!IsWindowEnabled() || !IsWindowVisible())
    return FALSE;

  switch (pMsg->message) {
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN: // usually <alt> + ...
  {
    switch (pMsg->wParam) {
    case VK_CONTROL:
    case VK_SHIFT:
    case VK_MENU:
    case VK_NUMLOCK:
    case VK_SCROLL:
    case VK_CAPITAL:
      return FALSE;

    // don't handle return/cancel keys
    case VK_RETURN:
    case VK_CANCEL:
      return FALSE;

    // shortcut keys
    default: {
      // don't process messages destined for hotkey controls!
      if (CWinClasses::IsClass(pMsg->hwnd, WC_HOTKEY))
        return FALSE;

      // get DWORD shortcut
      BOOL bExtKey = (pMsg->lParam & 0x01000000);
      DWORD dwShortcut = GetShortcut((WORD)pMsg->wParam, bExtKey);

      // look it up
      UINT nCmdID = 0;

      if (!m_mapShortcut2ID.Lookup(dwShortcut, nCmdID) || !nCmdID)
        return FALSE;

      // check if HKCOMB_EDITCTRLS is set and a edit has the focus
      // and the shortcut clashes
      if (m_wInvalidComb & HKCOMB_EDITCTRLS) {
        CString sClass = CWinClasses::GetClass(pMsg->hwnd);

        if (CWinClasses::IsClass(sClass, WC_EDIT) ||
            CWinClasses::IsClass(sClass, WC_RICHEDIT) ||
            CWinClasses::IsClass(sClass, WC_RICHEDIT20)) {
          // 1. check does not clash with edit shortcuts
          int nShortcut = sizeof(EDIT_SHRTCUTS) / sizeof(DWORD);

          while (nShortcut--) {
            if (dwShortcut == EDIT_SHRTCUTS[nShortcut])
              return FALSE;
          }

          //							WORD wVirtKeyCode =
          //LOWORD(dwShortcut);
          WORD wModifiers = HIWORD(dwShortcut);

          // 2. else must have <ctrl> or <alt>
          if (!(wModifiers & (HOTKEYF_ALT | HOTKEYF_CONTROL)))
            return FALSE;
        }
      }

      // return command ID
      if (m_bAutoSendCmds)
        SendMessage(WM_COMMAND, nCmdID);

      return nCmdID;
    }
    }
  }
  }

  return FALSE;
}

LRESULT CShortcutManager::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp,
                                     LPARAM lp) {
  switch (msg) {
  case WM_INITMENUPOPUP: {
    // default processing so all text changes
    // are complete before we have a go
    LRESULT lr = Default();
    PrepareMenuItems(CMenu::FromHandle((HMENU)wp));
    return lr;
  } break;
  }
  return CSubclassWnd::WindowProc(hRealWnd, msg, wp, lp);
}

void CShortcutManager::PrepareMenuItems(CMenu *pMenu) const {
  if (!pMenu || !pMenu->GetSafeHmenu())
    return;

  // we iterate all the menu items
  // if we find a match we get the menu text and add the shortcut
  // first removing any existing one
  int nItem = pMenu->GetMenuItemCount();

  while (nItem--) {
    UINT nCmdID = pMenu->GetMenuItemID(nItem);
    DWORD dwShortcut = GetShortcut(nCmdID);

    if (dwShortcut) {
      CString sShortcut = GetShortcutText(dwShortcut);

      if (!sShortcut.IsEmpty()) {
        CString sMenuText;
        pMenu->GetMenuString(nItem, sMenuText, MF_BYPOSITION);

        // look for '\t' indicating existing hint
        int nTab = sMenuText.Find('\t');

        // remove it
        if (nTab >= 0)
          sMenuText = sMenuText.Left(nTab + 1);
        else
          sMenuText += '\t';

        // add new hint
        sMenuText += sShortcut;

        // maintain state
        UINT nState = pMenu->GetMenuState(nItem, MF_BYPOSITION);

        pMenu->ModifyMenu(nItem, MF_BYPOSITION | nState, nCmdID, sMenuText);
      }
    }
  }
}

DWORD CShortcutManager::GetShortcut(UINT nCmdID) const {
  DWORD dwShortcut = 0;

  m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

  return dwShortcut;
}

CString CShortcutManager::GetShortcutTextByCmd(UINT nCmdID) {
  return GetShortcutText(GetShortcut(nCmdID));
}

CString CShortcutManager::GetShortcutText(DWORD dwShortcut) {
  CString sText;

  WORD wVirtKeyCode = LOWORD(dwShortcut);
  WORD wModifiers = HIWORD(dwShortcut);

  if (wModifiers & HOTKEYF_CONTROL) {
    sText += GetKeyName(VK_CONTROL);
    sText += "+";
  }

  if (wModifiers & HOTKEYF_SHIFT) {
    sText += GetKeyName(VK_SHIFT);
    sText += "+";
  }

  if (wModifiers & HOTKEYF_ALT) {
    sText += GetKeyName(VK_MENU);
    sText += "+";
  }

  CString sKey = GetKeyName(wVirtKeyCode, (wModifiers & HOTKEYF_EXT));

  if (!sKey.IsEmpty())
    sText += sKey;
  else
    sText.Empty();

  return sText;
}

/*
CString CShortcutManager::GetKeyName(WORD wVirtKeyCode, BOOL bExtended)
{
        const int KEYNAMELEN = 64;
        static TCHAR szKeyName[64];

        WORD wScanCode = MapVirtualKey(wVirtKeyCode, 0);

        // build lParam to send to GetKeyNameText
        LPARAM lParam = (wScanCode * 0x00010000);

        if (bExtended)
                lParam += 0x01000000;

        GetKeyNameText(lParam, szKeyName, KEYNAMELEN);

        return CString(szKeyName);
}*/

CString CShortcutManager::GetKeyName(UINT nVK, BOOL bExtended) {
  CString str;

  UINT nScanCode = MapVirtualKeyEx(nVK, 0, GetKeyboardLayout(0));
  switch (nVK) {
  // Keys which are "extended" (except for Return which is Numeric Enter as
  // extended)
  case VK_INSERT:
  case VK_DELETE:
  case VK_HOME:
  case VK_END:
  case VK_NEXT:  // Page down
  case VK_PRIOR: // Page up
  case VK_LEFT:
  case VK_RIGHT:
  case VK_UP:
  case VK_DOWN:
    nScanCode |= 0x100; // Add extended bit
  }

  // GetKeyNameText() expects the scan code to be on the same format as
  // WM_KEYDOWN
  // Hence the left shift
  BOOL bResult = GetKeyNameText(nScanCode << 16, str.GetBuffer(20), 19);
  str.ReleaseBuffer();
  return str;
}

void CShortcutManager::LoadSettings() {
  int nItem =
      AfxGetApp()->GetProfileInt(_T("KeyboardShortcuts"), _T("NumItems"), 0);

  while (nItem--) {
    CString sKey;
    sKey.Format(_T("KeyboardShortcuts\\Item%02d"), nItem);

    UINT nCmdID = (UINT)AfxGetApp()->GetProfileInt(sKey, _T("CmdID"), 0);
    DWORD dwShortcut =
        (DWORD)AfxGetApp()->GetProfileInt(sKey, _T("Shortcut"), 0);

    if (nCmdID && dwShortcut)
      AddShortcut(nCmdID, dwShortcut);
  }
}

void CShortcutManager::SaveSettings() {
  AfxGetApp()->WriteProfileInt(_T("KeyboardShortcuts"), _T("NumItems"),
                               m_mapID2Shortcut.GetCount());

  POSITION pos = m_mapID2Shortcut.GetStartPosition();
  int nItem = 0;

  while (pos) {
    UINT nCmdID = 0;
    DWORD dwShortcut = 0;

    m_mapID2Shortcut.GetNextAssoc(pos, nCmdID, dwShortcut);

    if (nCmdID && dwShortcut) {
      CString sKey;
      sKey.Format(_T("KeyboardShortcuts\\Item%02d"), nItem);

      AfxGetApp()->WriteProfileInt(sKey, _T("CmdID"), nCmdID);
      AfxGetApp()->WriteProfileInt(sKey, _T("Shortcut"), dwShortcut);

      nItem++;
    }
  }
}

BOOL CShortcutManager::LoadAccel(UINT idAccel) {
  HACCEL hAccel =
      ::LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(idAccel));
  if (!hAccel)
    return FALSE;

  int nAccel = ::CopyAcceleratorTable(hAccel, NULL, 0);

  LPACCEL lpAccel = (LPACCEL)LocalAlloc(LPTR, (nAccel) * sizeof(ACCEL));

  if (!lpAccel)
    return false;

  ::CopyAcceleratorTable(hAccel, lpAccel, nAccel);

  //	bool bRet = false;
  for (int i = 0; i < nAccel; i++) {
    // Iterate through the accelerator table and
    // add all accels matching dwIDCommand id
    //		if (lpAccel[i].cmd == dwIDCommand)
    //		{
    //			if (AddAccel (lpAccel[i].fVirt, dwIDCommand, lpAccel[i].key,
    //szCommand, bLocked))
    //				bRet = true;
    //		}

    BOOL bCtrl = (lpAccel[i].fVirt & FCONTROL);
    BOOL bShift = (lpAccel[i].fVirt & FSHIFT);
    BOOL bAlt = (lpAccel[i].fVirt & FALT);
    // BOOL bExtended = (lpAccel[i].fVirt & FVIRTKEY);

    DWORD key = lpAccel[i].key;

    if ((!lpAccel[i].fVirt & FVIRTKEY)) {
      key = MapVirtualKey(lpAccel[i].key, 0);
    }
    WORD wScanCode = MapVirtualKey(lpAccel[i].key, 0);
    WORD wModifiers = (bCtrl ? HOTKEYF_CONTROL : 0) |
                      (bShift ? HOTKEYF_SHIFT : 0) |
                      (bAlt ? HOTKEYF_ALT : 0) /*|
			(bExtended ? HOTKEYF_EXT : 0)*/;

    DWORD dwOtherShortcut = 0;
    if (!m_mapID2Shortcut.Lookup(lpAccel[i].cmd, dwOtherShortcut))
      AddShortcut(lpAccel[i].cmd, key, wModifiers);
  }

  ::LocalFree(lpAccel);

  return TRUE;
}

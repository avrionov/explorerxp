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
#include "hotkeyctrlex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHotKeyCtrlEx

CHotKeyCtrlEx::CHotKeyCtrlEx() : m_wInvalidComb(0), m_wModifiers(0)
{
}

CHotKeyCtrlEx::~CHotKeyCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CHotKeyCtrlEx, CHotKeyCtrl)
	//{{AFX_MSG_MAP(CHotKeyCtrlEx)
	//}}AFX_MSG_MAP
	ON_MESSAGE(HKM_SETRULES, OnSetRules)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHotKeyCtrlEx message handlers

LRESULT CHotKeyCtrlEx::OnSetRules(WPARAM wParam, LPARAM lParam)
{
	m_wInvalidComb = wParam;
	m_wModifiers = LOWORD(lParam);

	return 0L;
}

BOOL CHotKeyCtrlEx::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == *this)
	{
		UINT nChar = pMsg->wParam;

		switch (nChar)
		{
		case VK_DELETE:
		case VK_BACK:
		case VK_SPACE:
		case VK_TAB:
			{
				UINT nFlags = HIWORD(pMsg->lParam);

				BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
				BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
				BOOL bAlt = (GetKeyState(VK_MENU) & 0x8000);
				BOOL bExtended = (nFlags & 0x100);

				WORD wModifiers = (bCtrl ? HOTKEYF_CONTROL : 0) |
									(bShift ? HOTKEYF_SHIFT : 0) |
									(bAlt ? HOTKEYF_ALT : 0) |
									(bExtended ? HOTKEYF_EXT : 0);

				// some keys not to process by themselves
				if (!bCtrl && !bShift && !bAlt)
				{
					switch (nChar)
					{
					case VK_BACK:
						return CHotKeyCtrl::PreTranslateMessage(pMsg);

					case VK_TAB:
						{
							CWnd* pNext = GetParent()->GetNextDlgTabItem(this);

							if (pNext != this)
							{
								pNext->SetFocus();
								return TRUE; // eat the message
							}
						}
						break;
					}
				}

				// check for breaking rules
				BOOL bFail = ((m_wInvalidComb & HKCOMB_NONE) && !bCtrl && !bShift && !bAlt);

				bFail |= ((m_wInvalidComb & HKCOMB_S) && !bCtrl && bShift && !bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_C) && bCtrl && !bShift && !bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_A) && !bCtrl && !bShift && bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_SC) && bCtrl && bShift && !bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_SA) && !bCtrl && bShift && bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_CA) && bCtrl && !bShift && bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_SCA) && bCtrl && bShift && bAlt);

				if (bFail)
					wModifiers = m_wModifiers | (bExtended ? HOTKEYF_EXT : 0);

				SetHotKey(nChar, wModifiers);

				// send parent notification manually
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_CHANGE), (LPARAM)GetSafeHwnd());

				return TRUE;
			}
		}
	}
	
	return CHotKeyCtrl::PreTranslateMessage(pMsg);
}

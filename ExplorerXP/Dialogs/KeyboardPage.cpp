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
#include "ExplorerXP.h"
#include "KeyboardPage.h"
#include "ShortcutManager.h"
#include ".\keyboardpage.h"



// CKeyboardPage dialog

extern CShortcutManager gShortcutManager;

IMPLEMENT_DYNAMIC(CKeyboardPage, CPropertyPage)
CKeyboardPage::CKeyboardPage()
	: CPropertyPage(CKeyboardPage::IDD)
{
}

CKeyboardPage::~CKeyboardPage()
{
}

void CKeyboardPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMMANDS, m_tcCommands);
	DDX_Control(pDX, IDC_CURHOTKEY, m_hkCur);	
	DDX_Control(pDX, IDC_NEWHOTKEY, m_hkNew);
	DDX_Text(pDX, IDC_INUSE, m_sOtherCmdID);
}


BEGIN_MESSAGE_MAP(CKeyboardPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_COMMANDS, OnSelchangedShortcuts)
	ON_BN_CLICKED(IDC_ASSIGNSHORTCUT, OnAssignshortcut)	
	ON_EN_CHANGE(IDC_NEWHOTKEY, OnChangeShortcut)
END_MESSAGE_MAP()


// CKeyboardPage message handlers

BOOL CKeyboardPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	m_tcCommands.SetTabMode();

	int m_nMenuID = IDR_EXPLORTYPE;
//	if (m_nMenuID && m_pShortcutMgr)
	{
		CMenu menu;
		HTREEITEM htiFirst = NULL;

		if (menu.LoadMenu(m_nMenuID))
		{
			for (int nPos = 0; nPos < (int)menu.GetMenuItemCount(); nPos++)
			{
				HTREEITEM hti = AddMenuItem(NULL, &menu, nPos);

				if (!htiFirst)
					htiFirst = hti;

				m_tcCommands.Expand(hti, TVE_EXPAND);
			}
		}

		if (htiFirst)
			m_tcCommands.EnsureVisible(htiFirst);
	}

	m_hkNew.SetRules(0, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HTREEITEM CKeyboardPage::AddMenuItem(HTREEITEM htiParent, const CMenu* pMenu, int nPos)
	{
		CString sItem;
		pMenu->GetMenuString(nPos, sItem, MF_BYPOSITION);

		// remove '&'
		sItem.Replace(_T("&&"), _T("~~~"));
		sItem.Replace(_T("&"), EMPTYSTR);
		sItem.Replace(_T("~~~"), _T("&"));

		// remove everything after '\t'
		int nTab = sItem.Find(_T('\t'));

		if (nTab >= 0)
			sItem = sItem.Left(nTab);

		//sItem.Replace(__T("\t"), _T("   "));
		if (!sItem.IsEmpty())
		{
			// don't add disabled items if req
//			if (m_bIgnoreGrayedItems && (pMenu->GetMenuState(nPos, MF_BYPOSITION) & MF_GRAYED))
//				return NULL;
			HTREEITEM hti = NULL;
			
			UINT nCmdID = pMenu->GetMenuItemID(nPos);

			if (nCmdID == (UINT)-1) // submenu
				{
					CMenu* pSubMenu = pMenu->GetSubMenu(nPos);
					
					hti = m_tcCommands.InsertItem(sItem, htiParent ? htiParent : TVI_ROOT);
					m_tcCommands.SetItemColor(hti, GetSysColor (COLOR_WINDOWTEXT));

					if (pSubMenu)
					{
						for (int nSubPos = 0; nSubPos < (int)pSubMenu->GetMenuItemCount(); nSubPos++)
							m_tcCommands.Expand(AddMenuItem(hti, pSubMenu, nSubPos), TVE_EXPAND);
						m_tcCommands.SetItemState(hti,  TVIS_BOLD, TVIS_BOLD);
					}
				}
				else
				{
					DWORD dwShortcut = gShortcutManager.GetShortcut(nCmdID);

					if (dwShortcut)
					{
						CString csShort = gShortcutManager.GetShortcutText(dwShortcut);
						if (!csShort.IsEmpty())
						{
							sItem += _T('\t');
							sItem += csShort;
						}					
					}

					hti = m_tcCommands.InsertItem(sItem, htiParent ? htiParent : TVI_ROOT);
					m_tcCommands.SetItemColor(hti, GetSysColor (COLOR_WINDOWTEXT));

					m_tcCommands.SetItemData(hti, nCmdID);
				
					if (dwShortcut)
					{
						m_mapID2Shortcut[nCmdID] = dwShortcut;
						m_mapShortcut2HTI[dwShortcut] = hti;	
					}
				}
			return hti;			
		}
		return NULL;
	}

void CKeyboardPage::OnSelchangedShortcuts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	UINT nCmdID = (UINT)pNMTreeView->itemNew.lParam;
	DWORD dwShortcut = 0;

	m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

	WORD wVKeyCode = LOWORD(dwShortcut);
	WORD wModifiers = HIWORD(dwShortcut);

	m_hkCur.SetHotKey(wVKeyCode, wModifiers);
	m_hkNew.SetHotKey(wVKeyCode, wModifiers);

	BOOL bCanHaveShortcut = !m_tcCommands.ItemHasChildren(pNMTreeView->itemNew.hItem);

	m_hkNew.EnableWindow(bCanHaveShortcut);
	GetDlgItem(IDC_ASSIGNSHORTCUT)->EnableWindow(bCanHaveShortcut);
	GetDlgItem(IDC_CURLABEL)->EnableWindow(bCanHaveShortcut);
	GetDlgItem(IDC_NEWLABEL)->EnableWindow(bCanHaveShortcut);

	m_sOtherCmdID.Empty();
	UpdateData(FALSE);
	*pResult = 0;
}



void CKeyboardPage::OnAssignshortcut() 
	{
		HTREEITEM htiSel = m_tcCommands.GetSelectedItem();

		if (!htiSel)
			return;

		UINT nCmdID = static_cast<UINT>(m_tcCommands.GetItemData(htiSel));

		if (nCmdID)
		{
			// remove any shortcut currently assigned to nCmdID
			DWORD dwPrevSC = 0;

			if (m_mapID2Shortcut.Lookup(nCmdID, dwPrevSC))
				m_mapShortcut2HTI.RemoveKey(dwPrevSC);

			// if anyone has this shortcut we must remove their mapping first
			WORD wVKeyCode = 0, wModifiers = 0;
			m_hkNew.GetHotKey(wVKeyCode, wModifiers);

			DWORD dwShortcut = MAKELONG(wVKeyCode, wModifiers);

			HTREEITEM htiOther = NULL;

			if (m_mapShortcut2HTI.Lookup(dwShortcut, htiOther) && htiOther)
			{
				UINT nOtherCmdID = static_cast<UINT>(m_tcCommands.GetItemData(htiOther));

				if (nOtherCmdID)
					m_mapID2Shortcut.RemoveKey(nOtherCmdID);

				CString otherString = m_tcCommands.GetItemText (htiOther);

				int nTab = otherString.Find(_T('\t'));

				if (nTab >= 0)
					otherString = otherString.Left(nTab);
				m_tcCommands.SetItemText (htiOther, otherString);
			}

			// update maps
			m_mapID2Shortcut[nCmdID] = dwShortcut;
			m_mapShortcut2HTI[dwShortcut] = htiSel;

			m_hkCur.SetHotKey(LOWORD(dwShortcut), HIWORD(dwShortcut));
			m_sOtherCmdID.Empty();

			CString sItem = m_tcCommands.GetItemText(htiSel);

			int nTab = sItem.Find(_T('\t'));

			if (nTab >= 0)
				sItem = sItem.Left(nTab);

			
			CString csShort = gShortcutManager.GetShortcutText(dwShortcut);
			if (!csShort.IsEmpty())
			{
				sItem += _T('\t');
				sItem += csShort;
			}

			m_tcCommands.SetItemText(htiSel, sItem);
			m_tcCommands.Invalidate();

//			m_tcCommands.RecalcGutter();
//			m_tcCommands.RedrawGutter();

			UpdateData(FALSE);
		}
	}

void CKeyboardPage::OnChangeShortcut()
{
	   HTREEITEM htiSel = m_tcCommands.GetSelectedItem();

		if (!htiSel)
			return;

		UINT nCmdID = static_cast<UINT>(m_tcCommands.GetItemData(htiSel));

		WORD wVKeyCode = 0, wModifiers = 0;
		m_hkNew.GetHotKey(wVKeyCode, wModifiers);

		// validate modifiers but only if a 'main' key has been pressed
		if (wVKeyCode)
		{
			WORD wValidModifiers = gShortcutManager.ValidateModifiers(wModifiers, wVKeyCode);

			if (wValidModifiers != wModifiers)
			{
				wModifiers = wValidModifiers;
				m_hkNew.SetHotKey(wVKeyCode, wModifiers);
			}
		}

		DWORD dwShortcut = MAKELONG(wVKeyCode, wModifiers);

		// if anyone has this shortcut we show who it is
		HTREEITEM htiOther = NULL;
		m_mapShortcut2HTI.Lookup(dwShortcut, htiOther);

		if (htiOther && m_tcCommands.GetItemData(htiOther) != nCmdID)
			m_sOtherCmdID.Format(_T("Currently assigned to:\n\n'%s'"),  (LPCTSTR)m_tcCommands.GetItemText(htiOther));
		else
			m_sOtherCmdID.Empty();

		UpdateData(FALSE);
}
void CKeyboardPage::OnOK()
{
	// copy all the changes to m_pShortcutMgr
	POSITION pos = m_mapID2Shortcut.GetStartPosition();

	while (pos)
	{
		UINT nCmdID = 0;
		DWORD dwShortcut = 0;

		m_mapID2Shortcut.GetNextAssoc(pos, nCmdID, dwShortcut);
		ASSERT (nCmdID);

		gShortcutManager.SetShortcut(nCmdID, dwShortcut);
	}
	

	CPropertyPage::OnOK();
}

void CKeyboardPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertyPage::OnCancel();
}

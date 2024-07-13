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
#include "MainToolbar.h"

#include "debug_new.h"

CMainToolBar::CMainToolBar() {}

void CMainToolBar::Init() {
  static TBBUTTONEX
  tbButtons[] = { { { 0,               ID_BACK,
                      TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
                      0,               0 },
                    true },
                  { { 1,               ID_FORWARD,
                      TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
                      0,               0 },
                    true },
                  { { 2, ID_UP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }, true },
                  { { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 }, true },
                  //{ {  3, ID_SEARCH,			TBSTATE_ENABLED,
                  //TBSTYLE_BUTTON, 0, 0 }, true },
                  { { 4, ID_FOLDERTREE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 18, ID_FAVS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 19, ID_NEWFOLDER, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 5, ID_COPYTO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 6, ID_MOVETO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 7, ID_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 16,             ID_COMMANDS_CLEAN, TBSTATE_ENABLED,
                      TBSTYLE_BUTTON, 0,                 0 },
                    true },
                  { { 20,              ID_COMMANDS_FINDDUPLICATES,
                      TBSTATE_ENABLED, TBSTYLE_BUTTON,
                      0,               0 },
                    true },
                  { { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 }, true },
                  { { 9,               ID_MYCOMPUTER,
                      TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
                      0,               0 },
                    true },
                  { { 17, ID_DESKTOP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 10,              ID_RECYCLEBIN,
                      TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
                      0,               0 },
                    true },
                  { { 15, ID_MYDOCUMENTS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 21, ID_TEMPFOLDER, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
                    true },
                  { { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 }, true },
                  { { 12,             ID_WINDOW_CASCADE, TBSTATE_ENABLED,
                      TBSTYLE_BUTTON, 0,                 0 },
                    true },
                  { { 13,             ID_WINDOW_TILE_HORZ, TBSTATE_ENABLED,
                      TBSTYLE_BUTTON, 0,                   0 },
                    true },
                  { { 14,             ID_WINDOW_TILE_VERT, TBSTATE_ENABLED,
                      TBSTYLE_BUTTON, 0,                   0 },
                    true }, };

  // ID_IMAGE_TOOLBAR_TGA
  SetBitmaps(isWindowsXPorLater() ? ID_IMAGE_TOOLBAR_TGA : IDR_BUTTONBAR,
             0, 0,
             isWindowsXPorLater() ? ID_IMAGE_TOOLBAR_TGA : IDR_BUTTONBAR,
             0, 0, ioLargeIcons, RGB(255, 255, 255));

  SetButtons(sizeof(tbButtons) / sizeof(tbButtons[0]), tbButtons,
             toTextOnRight);

  m_DropTarget.Register(this);
}

bool CMainToolBar::HasButtonText(UINT nID) {
  return false;

  //switch (nID) {
	 // // case ID_BACK:
	 // case ID_SEARCH:
	 // case ID_FOLDERTREE:
	 // case ID_FAVS:
		//return true;

	 // default:
		//return false;
  //}
}

bool CMainToolBar::PassThroughTip(UINT nID) {
  switch (nID) {
  case ID_BACK:
  case ID_FORWARD:
    return true;

  default:
    return false;
  }
}

DROPEFFECT CMainToolBar::OnDragEnter(COleDataObject *pDataObject,
                                     DWORD dwKeyState, CPoint point) {
  m_DropFiles.clear();
  m_LastDropFolder.Empty();
  TCHAR path[2094];
  SHGetSpecialFolderPath(m_hWnd, path, CSIDL_PERSONAL, FALSE);
  m_LastDropFolder = path;
  //	m_LastDropFolder.MakeLower ();
  SureBackSlash(m_LastDropFolder);
  if (ReadHDropData(pDataObject, m_DropFiles, m_DropFolder)) {
    int cmd_id = HitTestButton(point);
    if (cmd_id == ID_RECYCLEBIN)
      return DROPEFFECT_MOVE;

    if (cmd_id == ID_MYDOCUMENTS) {
      return CalcDropEffect(m_DropFiles, m_LastDropFolder, m_DropFolder,
                            dwKeyState);
    }
  }

  return DROPEFFECT_NONE;
}

DROPEFFECT CMainToolBar::OnDragOver(COleDataObject *pDataObject,
                                    DWORD dwKeyState, CPoint point) {
  DROPEFFECT dwEffect = DROPEFFECT_NONE;

  int cmd_id = HitTestButton(point);
  if (cmd_id == ID_RECYCLEBIN)
    dwEffect = DROPEFFECT_MOVE;

  if (cmd_id == ID_MYDOCUMENTS) {
    dwEffect =
        CalcDropEffect(m_DropFiles, m_LastDropFolder, m_DropFolder, dwKeyState);
  }

  return dwEffect;
}

void CMainToolBar::OnDragLeave() {
  m_LastDropFolder.Empty();
  m_DropFiles.clear();
}

BOOL CMainToolBar::OnDrop(COleDataObject *pDataObject, DROPEFFECT dropEffect,
                          CPoint point) {
  int cmd_id = HitTestButton(point);
  if (cmd_id == ID_RECYCLEBIN) {
    FileOperation(this, m_DropFiles, FO_DELETE, FOF_ALLOWUNDO, NULL);
    return TRUE;
  }

  if (cmd_id == ID_MYDOCUMENTS) {
    UINT file_oper = FO_COPY;

    switch (dropEffect) {
    case DROPEFFECT_COPY:
      file_oper = FO_COPY;
      break;

    case DROPEFFECT_MOVE:
      file_oper = FO_MOVE;
      break;
    case DROPEFFECT_LINK:
      return CreateShortcuts(m_DropFiles, m_LastDropFolder);

    default:
      TRACE(_T("Invalide file operation %d\n"), dropEffect);
      return false;
    }

    FileOperation(this, m_DropFiles, file_oper, FOF_ALLOWUNDO,
                  m_LastDropFolder);
    return TRUE;
  }

  return false;
}

int CMainToolBar::HitTestButton(CPoint &pt) {
  int index = GetToolBarCtrl().HitTest(&pt);
  if (index >= 0)
    return GetItemID(index);

  return index;
}

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

#ifndef __MAINBARDROPTARGET_H__
#define __MAINBARDROPTARGET_H__

#pragma once

class CMainToolBar;


class CMainBarDropTarget : public COleDropTarget {
public:
  CMainBarDropTarget();
  virtual ~CMainBarDropTarget();

  // Attributes
public:
  CMainToolBar *m_pToolbar;
  BOOL m_bRegistered;

  // Operations
public:
  BOOL Register(CMainToolBar *pTree);
  virtual void Revoke();

  BOOL OnDrop(CWnd *pWnd, COleDataObject *pDataObject, DROPEFFECT dropEffect,
              CPoint point);
  DROPEFFECT OnDragEnter(CWnd *pWnd, COleDataObject *pDataObject,
                         DWORD dwKeyState, CPoint point);
  void OnDragLeave(CWnd *pWnd);
  DROPEFFECT OnDragOver(CWnd *pWnd, COleDataObject *pDataObject,
                        DWORD dwKeyState, CPoint point);
  DROPEFFECT OnDragScroll(CWnd *pWnd, DWORD dwKeyState, CPoint point);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGridDropTarget)
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CGridDropTarget)
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

#endif // __MAINBARDROPTARGET_H__

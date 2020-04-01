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

#pragma once

class CPathEdit : public CEdit {
  // Construction
public:
  CPathEdit();

  // Attributes
public:
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPathEdit)
public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CPathEdit();

  // Generated message map functions
protected:
  //{{AFX_MSG(CPathEdit)
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg void OnSetFocus(CWnd *pOldWnd);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnChange();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

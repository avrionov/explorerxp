/* Copyright 2002-2016 Nikolay Avrionov. All Rights Reserved.
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

// COptionsDialog dialog

#define baseOptions CPropertyPage

class COptionsDialog : public baseOptions {
  DECLARE_DYNAMIC(COptionsDialog)
public:
  COptionsDialog(); // standard constructor
  virtual ~COptionsDialog();

  // Dialog Data
  enum {
    IDD = IDD_OPTIONSSIMPLE
  };

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
  BOOL m_bVert;
  BOOL m_bHorz;
  afx_msg void OnBnClickedOk();
  BOOL m_bAtStartNone;
  BOOL m_bHiddenFiles;
  virtual BOOL OnApply();
  virtual void OnOK();
  void GetFromDlg();
  afx_msg void OnBnClickedHiddenFiles();
  afx_msg void OnBnClickedHorizontal();
  afx_msg void OnBnClickedVertical();
  afx_msg void OnBnClickedAtstartnone();
};

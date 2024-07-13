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
#include "ExplorerXPDoc.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPDoc

IMPLEMENT_DYNCREATE(CExplorerXPDoc, CDocument)

BEGIN_MESSAGE_MAP(CExplorerXPDoc, CDocument)
//{{AFX_MSG_MAP(CExplorerXPDoc)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPDoc construction/destruction

CExplorerXPDoc::CExplorerXPDoc() { m_bCanClose = TRUE; }

CExplorerXPDoc::~CExplorerXPDoc() {}

BOOL CExplorerXPDoc::OnNewDocument() {
  m_Path = CONST_MYCOMPUTER;
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPDoc serialization

void CExplorerXPDoc::Serialize(CArchive &ar) {}

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPDoc diagnostics

#ifdef _DEBUG
void CExplorerXPDoc::AssertValid() const { CDocument::AssertValid(); }

void CExplorerXPDoc::Dump(CDumpContext &dc) const { CDocument::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExplorerXPDoc commands

BOOL CExplorerXPDoc::OnOpenDocument(LPCTSTR lpszPathName) {
  m_Path = lpszPathName;
  return TRUE;
}

void CExplorerXPDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) {
  m_strPathName = lpszPathName;

  m_bEmbedded = FALSE;

  SetTitle(lpszPathName);

  ASSERT_VALID(this);
}
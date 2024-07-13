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


#ifndef __EXPLORERXPDOC_H__
#define __EXPLORERXPDOC_H__

#pragma once

class CExplorerXPDoc : public CDocument
{
protected: // create from serialization only
	CExplorerXPDoc();
	DECLARE_DYNCREATE(CExplorerXPDoc)

// Attributes
public:

// Operations
public:
    CString & GetPath () { return m_Path;}
	void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	void CanClose (BOOL bCan) { m_bCanClose = bCan;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExplorerXPDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//virtual BOOL CanCloseFrame(CFrameWnd* pFrame ) { return m_bCanClose; }
	virtual void OnCloseDocument () { if (m_bCanClose) return CDocument::OnCloseDocument ();}
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExplorerXPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CString m_Path;
	BOOL    m_bCanClose;

// Generated message map functions
protected:
	//{{AFX_MSG(CExplorerXPDoc)	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 


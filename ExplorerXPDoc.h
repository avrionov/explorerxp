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

#endif // #endif // _MSC_VER > 1000


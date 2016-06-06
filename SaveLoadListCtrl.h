#ifndef __SAVELOAD_LISTCTRL_H__
#define __SAVELOAD_LISTCTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveLoadListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaveLoadListCtrl window

class CSaveLoadListCtrl : public CListCtrl
{
// Construction
public:
	CSaveLoadListCtrl(const char *csSection);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveLoadListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSaveLoadListCtrl();
	void Load ();
	void Save ();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSaveLoadListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	std::vector <CString> m_Paths;
	CString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__SAVELOAD_LISTCTRL_H__

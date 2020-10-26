//////////////////////////////////////////////////////////////////////
// 
// This utility written and copyright by Michael Dunn (mdunn at inreach
// dot com).  You may freely use and redistribute this source code and
// binary as long as this notice is retained.
//
// Contact me if you have any questions, comments, or bug reports. Get
// the latest updates at http://home.inreach.com/mdunn/code/
//
//////////////////////////////////////////////////////////////////////
// 
// Revision history:
//  Feb 28, 2000: Version 1.0: First release.
//
//  June 5, 2000: Version 1.1: Fixed (un)registration so the DLL works
//      on NT/2000.
//
//  Oct 28, 2001: Version 1.1.1: Added 4 default wildcards, *.ncb, *.aps,
//      *.bsc, *.sbr.
//
//////////////////////////////////////////////////////////////////////

// OptionsDlg.h : header file
//

#if !defined(AFX_OPTIONSDLG_H__D74CECA2_218F_4C67_9025_A68A88438A92__INCLUDED_)
#define AFX_OPTIONSDLG_H__D74CECA2_218F_4C67_9025_A68A88438A92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg(CWnd* pParent, CStringList& listPatterns );   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS };
	CListCtrl	m_list;
	BOOL	m_bRecycle;
	//}}AFX_DATA

    CStringList m_lsPatterns;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
    afx_msg void OnKickIdle();
    afx_msg void OnUpdateEdit(CCmdUI*);
    afx_msg void OnUpdateDelete(CCmdUI*);
    afx_msg void OnUpdateOK(CCmdUI*);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__D74CECA2_218F_4C67_9025_A68A88438A92__INCLUDED_)

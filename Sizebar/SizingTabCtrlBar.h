//This file is part of the Build system for Warcaster.
//Copyright Tremor Entertainment, 2002.
////File:
//  SizingTabCtrlBar.h
//Author
//  Nikolay Avrionov
//Targets:
//  SYSTEM Win32


#ifndef __SIZINGTABCTRLBAR_H__
#define __SIZINGTABCTRLBAR_H__


#pragma once

#include "sizecbar.h"

typedef struct
{
	CWnd *pWnd;
	TCHAR szLabel[32];
}TCB_ITEM;

/////////////////////////////////////////////////////////////////////////////
// Fenster CSizingTabCtrlBar 

class CSizingTabCtrlBar : public CSizingControlBar
{
// Konstruktion
public:
	CSizingTabCtrlBar();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	void SetActiveView(int nNewTab);
	void SetActiveView(CRuntimeClass *pViewClass);
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSizingTabCtrlBar)
	//}}AFX_VIRTUAL

// Implementierung
public:
	CView* GetActiveView();
	CView* GetView(int nView);
	CView* GetView(CRuntimeClass *pViewClass);
	BOOL AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext = NULL);
	int GetActiveViewIndex  ();
	void RemoveView(int nView);
	BOOL IsHorz() const;
	CTabCtrl m_tabctrl;
	virtual ~CSizingTabCtrlBar();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	int m_nActiveTab;
	CView* m_pActiveView;
	CList <TCB_ITEM *,TCB_ITEM *> m_views;
	CImageList m_images;
	//{{AFX_MSG(CSizingTabCtrlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // __SIZINGTABCTRLBAR_H__

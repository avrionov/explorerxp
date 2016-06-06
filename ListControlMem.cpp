// ListControlMem.cpp : implementation file
//

#include "stdafx.h"
#include "ExplorerXP.h"
#include "ListControlMem.h"
#include ".\listcontrolmem.h"


// CListControlMem

IMPLEMENT_DYNAMIC(CListControlMem, CListCtrl)
CListControlMem::CListControlMem()
{
}

CListControlMem::~CListControlMem()
{
}


BEGIN_MESSAGE_MAP(CListControlMem, CListCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CListControlMem message handlers


void CListControlMem::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// Paint to a memory device context to reduce screen flicker.
	CMemDC memDC(&dc, &m_rectClient);

	

		// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );es
}

BOOL CListControlMem::OnEraseBkgnd(CDC* pDC)
{
	UNUSED_ALWAYS(pDC);
	
	return TRUE;
}

void CListControlMem::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	GetClientRect(m_rectClient);

	CHeaderCtrl* pHC;
	pHC = GetHeaderCtrl();
	if (pHC != NULL)
	{
		CRect rectHeader;
		pHC->GetItemRect( 0, &rectHeader );
		m_rectClient.top += rectHeader.bottom;
	}}

/////////////////////////////////////////////////////////////////////////////
//
// This file is part of ResizableLib
// http://sourceforge.net/projects/resizablelib
//
// Copyright (C) 2000-2004 by Paolo Messina
// http://www.geocities.com/ppescher - mailto:ppescher@hotmail.com
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

/*!
 *  @file
 *  @brief Interface for the CResizableMinMax class.
 */

#if !defined(AFX_RESIZABLEMINMAX_H__INCLUDED_)
#define AFX_RESIZABLEMINMAX_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*! @addtogroup CoreComponents
 *  @{
 */

//! @brief brief_description
/*!
 *  long_description
 */
class CResizableMinMax
{
// Attributes
private:
	// flags
	BOOL m_bUseMaxTrack;
	BOOL m_bUseMinTrack;
	BOOL m_bUseMaxRect;

	POINT m_ptMinTrackSize;		// min tracking size
	POINT m_ptMaxTrackSize;		// max tracking size
	POINT m_ptMaxPos;			// maximized position
	POINT m_ptMaxSize;			// maximized size

public:
	CResizableMinMax();
	virtual ~CResizableMinMax();

protected:
	void MinMaxInfo(LPMINMAXINFO lpMMI);
	void ChainMinMaxInfo(LPMINMAXINFO lpMMI, CWnd* pParentFrame, CWnd* pWnd);

	void ChainMinMaxInfo(LPMINMAXINFO lpMMI, HWND hWndChild, CSize sizeExtra);
	
	void ChainMinMaxInfo(LPMINMAXINFO lpMMI, CWnd* pParentWnd, UINT nID, CSize sizeExtra)
	{
		ChainMinMaxInfo(lpMMI,
			::GetDlgItem(pParentWnd->GetSafeHwnd(), nID), sizeExtra);
	}

	void ChainMinMaxInfoCB(LPMINMAXINFO lpMMI, HWND hWndChild);
	virtual BOOL CalcSizeExtra(HWND hWndChild, CSize sizeChild, CSize& sizeExtra);

	void SetMaximizedRect(const CRect& rc);		// set window rect when maximized
	void ResetMaximizedRect();					// reset to default maximized rect
	void SetMinTrackSize(const CSize& size);	// set minimum tracking size
	void ResetMinTrackSize();					// reset to default minimum tracking size
	void SetMaxTrackSize(const CSize& size);	// set maximum tracking size
	void ResetMaxTrackSize();					// reset to default maximum tracking size
};

// @}
#endif // !defined(AFX_RESIZABLEMINMAX_H__INCLUDED_)

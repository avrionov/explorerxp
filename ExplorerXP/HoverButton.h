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

#pragma once

class CHoverButton : public CBitmapButton
{
	// Construction
public:
	CHoverButton();
	void SetToolTipText(CString* spText, BOOL bActivate = TRUE);
	void SetToolTipText(int nId, BOOL bActivate = TRUE);
	
		virtual BOOL PreTranslateMessage(MSG* pMsg);
// Attributes
protected:
	void ActivateTooltip(BOOL bActivate = TRUE);
	BOOL m_bHover;						// indicates if mouse is over the button
	CSize m_ButtonSize;					// width and height of the button
	CBitmap mybitmap;
	BOOL m_bTracking;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoverButton)
	protected:

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadBitmap(UINT bitmapid);
	virtual ~CHoverButton();

	// Generated message map functions
protected:
	CToolTipCtrl m_ToolTip;
	void InitToolTip();
	//{{AFX_MSG(CHoverButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
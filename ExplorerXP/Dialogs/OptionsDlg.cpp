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

// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "AddEditPatternDlg.h"
#include <afxpriv.h>    // for WM_KICKIDLE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg ( CWnd* pParent, CStringList& listPatterns )
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_bRecycle = FALSE;
	//}}AFX_DATA_INIT

    m_lsPatterns.AddTail ( &listPatterns );
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Check(pDX, IDC_RECYCLE_FILES, m_bRecycle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList)
	//}}AFX_MSG_MAP
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
    ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateEdit)
    ON_UPDATE_COMMAND_UI(IDC_DELETE, OnUpdateDelete)
    ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

CString  sPattern;
POSITION pos;
int      nIndex = 0;

    m_list.InsertColumn ( 0, _T("") );

    for ( pos = m_lsPatterns.GetHeadPosition(); NULL != pos; )
        {
        sPattern = m_lsPatterns.GetNext ( pos );
        m_list.InsertItem ( nIndex++, sPattern );
        }

    m_list.SetColumnWidth ( 0, LVSCW_AUTOSIZE_USEHEADER );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnOK() 
{
    if ( m_list.GetItemCount() == 0 )
        {
        AfxMessageBox ( _T("Please enter at least one wildcard."), 
                        MB_ICONERROR );

        GotoDlgCtrl ( GetDlgItem ( IDC_ADD ));
        return;
        }

    m_lsPatterns.RemoveAll();

int nIndex, nMaxIndex = m_list.GetItemCount() - 1;

    for ( nIndex = 0; nIndex <= nMaxIndex; nIndex++ )
        {
        m_lsPatterns.AddTail ( m_list.GetItemText ( nIndex, 0 ));
        }

    CDialog::OnOK();
}

void COptionsDlg::OnAdd() 
{
CAddEditPatternDlg dlg ( TRUE, this );	
int nIndex;

    if ( IDOK == dlg.DoModal() )
        {
        nIndex = m_list.GetItemCount();
        m_list.InsertItem ( nIndex, dlg.m_sPattern );
        m_list.EnsureVisible ( nIndex, FALSE );
        m_list.SetItemState ( nIndex, LVIS_SELECTED, LVIS_SELECTED );
        }
}

void COptionsDlg::OnEdit() 
{
CAddEditPatternDlg dlg ( FALSE, this );	
int nSelIndex = m_list.GetNextItem ( -1, LVNI_SELECTED );

    ASSERT ( -1 != nSelIndex );

    dlg.m_sPattern = m_list.GetItemText ( nSelIndex, 0 );

    if ( IDOK == dlg.DoModal() )
        {
        m_list.SetItemText ( nSelIndex, 0, dlg.m_sPattern );
        m_list.EnsureVisible ( nSelIndex, FALSE );
        }
}

void COptionsDlg::OnDelete() 
{
int nSelItem = m_list.GetNextItem ( -1, LVIS_SELECTED );

    if ( -1 != nSelItem )
        {
        m_list.DeleteItem ( nSelItem );
        }
}

void COptionsDlg::OnKickIdle()
{
    UpdateDialogControls ( this, FALSE );
}

void COptionsDlg::OnUpdateDelete ( CCmdUI* pCmdUI )
{
    pCmdUI->Enable ( NULL != m_list.GetFirstSelectedItemPosition() );
}

void COptionsDlg::OnUpdateEdit ( CCmdUI* pCmdUI )
{
    pCmdUI->Enable ( NULL != m_list.GetFirstSelectedItemPosition() );
}

void COptionsDlg::OnUpdateOK ( CCmdUI* pCmdUI )
{
    pCmdUI->Enable ( m_list.GetItemCount() > 0 );
}

void COptionsDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if ( -1 != m_list.GetNextItem ( -1, LVNI_SELECTED ))
        OnEdit();

    *pResult = 0;   // return value is ignored
}

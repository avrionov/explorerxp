// RenReNumberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExplorerXP.h"
#include "RenReNumberDlg.h"


// CRenReNumberDlg dialog

IMPLEMENT_DYNCREATE(CRenReNumberDlg, CDHtmlDialog)

CRenReNumberDlg::CRenReNumberDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CRenReNumberDlg::IDD, CRenReNumberDlg::IDH, pParent)
{
}

CRenReNumberDlg::~CRenReNumberDlg()
{
}

void CRenReNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CRenReNumberDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CRenReNumberDlg, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CRenReNumberDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CRenReNumberDlg message handlers

HRESULT CRenReNumberDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT CRenReNumberDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

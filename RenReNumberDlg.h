#pragma once


// CRenReNumberDlg dialog

class CRenReNumberDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CRenReNumberDlg)

public:
	CRenReNumberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRenReNumberDlg();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_REN_COUNT, IDH = IDR_HTML_RENRENUMBERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};

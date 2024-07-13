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


#include "stdafx.h"
#include "explorerxp.h"
#include "SplitDlg.h"
#include "SplitPath.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitDlg dialog
void GetFullPathName (const TCHAR * file, CString &path)
{
	CSplitPath split_path(file);
	path = split_path.GetDrive ();
	path += split_path.GetDir ();
}

CSplitDlg::CSplitDlg(const TCHAR *file, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSplitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplitDlg)	
	m_OutputDir = EMPTYSTR;
	//}}AFX_DATA_INIT
	if (file)
		m_InputFile = file;

	m_nOutputtype = 1;
	m_nFiles = 10;
	m_nFileSize = 1024;
	m_bIsWorking = false;
}


void CSplitDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplitDlg)
	DDX_Control(pDX, IDC_PROGRESS_SPLIT, m_Progress);
	DDX_Control(pDX, IDC_MULTIPLAYER, m_Multi);
	DDX_Text(pDX, IDC_EDIT_INPUTFILE, m_InputFile);
	DDX_Text(pDX, IDC_EDIT_OUTPUTDIR, m_OutputDir);
	DDX_Text(pDX, IDC_EDIT_FILESIZE, m_nFileSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplitDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSplitDlg)
	ON_BN_CLICKED(IDC_BTN_SELINPUTFILE, OnBtnSelinputfile)
	ON_BN_CLICKED(IDC_BTN_SELOUTPUTDIR, OnBtnSeloutputdir)
	ON_BN_CLICKED(IDC_BTN_STARTSPLIT, OnBtnStartsplit)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FS_UPDATESTATUS, OnUpdateStatus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitDlg message handlers

BOOL CSplitDlg::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	CRect rc;
	GetWindowRect (rc);
	CSize sz;
	sz.cy  = rc.Height ();
	sz.cx = ::GetSystemMetrics (SM_CXFULLSCREEN);
	SetMaxTrackSize (sz);

	AddAnchor (IDC_STATIC_INPUT_SEP, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_STATIC_INPUT, MIDDLE_CENTER);
	AddAnchor (IDC_EDIT_INPUTFILE, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_BTN_SELINPUTFILE,TOP_RIGHT);

	AddAnchor (IDC_STATIC_OUTPUT_SEP, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_STATIC_OUTPUT, MIDDLE_CENTER);
	AddAnchor (IDC_EDIT_OUTPUTDIR, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_BTN_SELOUTPUTDIR,TOP_RIGHT);
		
	AddAnchor (IDC_EDIT_FILESIZE, TOP_LEFT, TOP_RIGHT);
	
	AddAnchor (IDC_PROGRESS_SPLIT, TOP_LEFT, TOP_RIGHT);
	AddAnchor (IDC_STATIC_SEP, TOP_LEFT, TOP_RIGHT);

	AddAnchor (IDC_MULTIPLAYER,TOP_RIGHT);

	m_Multi.AddString (_T("Bytes"));
	m_Multi.AddString (_T("Kbytes"));
	m_Multi.AddString (_T("Mbytes"));

	m_Multi.SetCurSel (1);
	
	EnableSaveRestore (_T("SplitDlg")); 
	GetFullPathName (m_InputFile, m_OutputDir);
	UpdateData (FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSplitDlg::OnBtnSelinputfile() 
{
	CFileDialog dlg(TRUE);
	if(dlg.DoModal()==IDOK)
	{
		UpdateData(TRUE);
		m_InputFile = dlg.GetPathName();
		if(m_OutputDir.IsEmpty())		
			GetFullPathName (m_InputFile, m_OutputDir);		
		UpdateData(FALSE);
	}
}

void CSplitDlg::OnBtnSeloutputdir() 
{	
	UpdateData(TRUE);
	BROWSEINFO bi;
	TCHAR pszBuffer[MAX_PATH];
	LPITEMIDLIST pidl;
	// Get help on BROWSEINFO struct - it's got all the bit settings.
	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = pszBuffer;
	bi.lpszTitle = _T("Select Path For Output Files");
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	// This next call issues the dialog box.
	if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
	{
		if (::SHGetPathFromIDList(pidl, pszBuffer))
		{ 
		// At this point pszBuffer contains the selected path */.
			m_OutputDir = pszBuffer;
			UpdateData(FALSE);
		}
		// Free the PIDL allocated by SHBrowseForFolder.
		CoTaskMemFree(pidl);
	}
	
}


void CSplitDlg::EnableAllControls(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_INPUTFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_OUTPUTDIR)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_SELINPUTFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_SELOUTPUTDIR)->EnableWindow(bEnable);
	GetDlgItem(IDCANCEL)->EnableWindow(bEnable);
	//GetDlgItem(IDC_RDO_NFILES    )->EnableWindow(bEnable);
//	GetDlgItem(IDC_RDO_FILESIZE  )->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_FILESIZE)->EnableWindow(m_nOutputtype!=0 && bEnable);
//	GetDlgItem(IDC_EDIT_NFILES)->EnableWindow(m_nOutputtype==0 && bEnable);
}

void CSplitDlg::OnBtnStartsplit() 
{		
	if(!m_bIsWorking)
	{
		if(UpdateData(TRUE))
		{
			//validate
			if(m_InputFile.IsEmpty())
				return;

			int multi = m_Multi.GetCurSel ();
			int mlt = (int)pow ((double)1024,  multi);

			int nSplitVal = m_nOutputtype==0 ? m_nFiles:(m_nFileSize * mlt);

			if(nSplitVal==0)
				return;
			
			EnableAllControls(FALSE);
			GetDlgItem(IDC_BTN_STARTSPLIT)->SetWindowText(_T("Cancel"));
			m_pFS = (CFileSplit*)(AfxBeginThread(RUNTIME_CLASS(CFileSplit)));
			m_pFS->m_pMainWnd = this;//AfxGetMainWnd();
			m_pFS->m_InputFileName = m_InputFile;
			m_pFS->m_OutputDirName = m_OutputDir;
			m_pFS->m_nSplitMode    = m_nOutputtype;
			m_pFS->m_nSplitVal	   = nSplitVal;
			
			m_pFS->PostThreadMessage(WM_FS_START,0,0);
			m_bIsWorking = true;
		}
	}
	else
	{
		//Cancel...
		m_pFS->PostThreadMessage(WM_FS_CANCEL,0,0);
		EnableAllControls(TRUE);
	}
}


LRESULT CSplitDlg::OnUpdateStatus(WPARAM wP, LPARAM lP)
{
	if(lP==0)
		m_Progress.SetPos((int)wP);
	if(lP==-1)
	{
		m_Progress.SetPos(0);
		MessageBox(_T("Error splitting file, please check filenames"), _T("Error"),MB_ICONERROR);
		EnableAllControls(TRUE);
		GetDlgItem(IDC_BTN_STARTSPLIT)->SetWindowText(_T("Start"));		
		m_bIsWorking = false;

	}
	if(lP==1)
	{
		m_Progress.SetPos(100);
		EnableAllControls(TRUE);
		GetDlgItem(IDC_BTN_STARTSPLIT)->SetWindowText(_T("Start"));		
		m_bIsWorking = false;
	}
	return 0;
}

/* Copyright 2002-2020 Nikolay Avrionov. All Rights Reserved.
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
#include "MergeDlg.h"
#include "SplitPath.h"
#include "BrowseForFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, 
   LPARAM lParamSort)
{
	if(lParam1<lParam2)
		return -1;
	if(lParam1>lParam2)
		return 1;
	return 0;
}

CMergeDlg::CMergeDlg(const TCHAR *initial_name, CWnd* pParent /*=NULL*/)
	:CResizableDialog(CMergeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMergeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_InitialName = initial_name;
	m_bIsWorking = false;
}


void CMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeDlg)
	DDX_Control(pDX, IDC_BTN_SELDIR, m_DirSel);
	DDX_Control(pDX, IDC_BTN_FILEADD, m_FileAdd);
	DDX_Control(pDX, IDC_BTN_FILEDEL, m_FileDel);
	DDX_Control(pDX, IDC_BTN_FILEDOWN, m_FileDown);
	DDX_Control(pDX, IDC_BTN_FILEUP, m_FileUp);
	DDX_Control(pDX, IDC_PROGRESS_MERGE, m_Progress);
	//DDX_Control(pDX, IDC_BTN_SELFILES, m_SelFiles);
	DDX_Control(pDX, IDC_LIST_FILES, m_FileList);
	DDX_Text(pDX, IDC_EDIT_OUTFILE, m_OutFile);
	//}}AFX_DATA_MAP	
}


BEGIN_MESSAGE_MAP(CMergeDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CMergeDlg)
	ON_BN_CLICKED(IDC_BTN_SELFILES, OnBtnSelfiles)
	ON_BN_CLICKED(IDC_BTN_STARTMERGE, OnBtnStartmerge)
	ON_BN_CLICKED(IDC_BTN_FILEUP, OnBtnFileup)
	ON_BN_CLICKED(IDC_BTN_FILEDOWN, OnBtnFiledown)
	ON_BN_CLICKED(IDC_BTN_FILEDEL, OnBtnFiledel)
	ON_BN_CLICKED(IDC_BTN_FILEADD, OnBtnFileadd)
	ON_BN_CLICKED(IDC_BTN_SELDIR, OnBtnSeldir)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FM_UPDATESTATUS, OnUpdateStatus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDlg message handlers

BOOL CMergeDlg::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
//	m_SelFiles.SetFlat (FALSE);
//	m_SelFiles.SetIcon(IDI_ICON_FOPEN);

	/*m_DirSel.SetIcon( AfxGetApp()->LoadIcon(IDI_ICON_FADD));	
	//m_DirSel.SetFlat (FALSE);

	m_FileUp.SetIcon( AfxGetApp()->LoadIcon(IDI_ICON_FUP));	
	//m_FileUp.SetFlat (FALSE);

	m_FileDown.SetIcon( AfxGetApp()->LoadIcon(IDI_ICON_FDOWN));	
	//m_FileDown.SetFlat (FALSE);

	m_FileDel.SetIcon( AfxGetApp()->LoadIcon(IDI_ICON_FDEL));	
	//m_FileDel.SetFlat (FALSE);

	m_FileAdd.SetIcon( AfxGetApp()->LoadIcon(IDI_ICON_FADD));
	//m_FileAdd.SetFlat (FALSE);*/

	//m_FileList.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	
	AddAnchor (IDCANCEL, BOTTOM_LEFT);
	AddAnchor (IDC_BTN_STARTMERGE, BOTTOM_LEFT);
	AddAnchor (IDC_EDIT_OUTFILE, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDC_PROGRESS_MERGE, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor (IDC_OUTPUT_STATIC, BOTTOM_LEFT);

	AddAnchor (IDC_BTN_SELDIR, BOTTOM_RIGHT);

	AddAnchor (IDC_BTN_FILEDEL, TOP_RIGHT);
	AddAnchor (IDC_BTN_FILEDOWN, TOP_RIGHT);
	AddAnchor (IDC_BTN_FILEUP, TOP_RIGHT);
	AddAnchor (IDC_BTN_FILEADD, TOP_RIGHT);
//	AddAnchor (IDC_BTN_SELFILES, TOP_RIGHT);

	AddAnchor (IDC_LIST_FILES, TOP_LEFT, BOTTOM_RIGHT);
	
	EnableSaveRestore (_T("MergeDlg")); 

	CRect rc;
	m_FileList.GetClientRect (rc);
	m_FileList.InsertColumn(0, _T("Files"), LVCFMT_LEFT, rc.Width());
	m_FileList.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	FillWith (m_InitialName);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool IsNumber (CString &str)
{
	for (int i = 0; i < str.GetLength (); i++)
		if (!isdigit (str[i]))
			return false;

	return true;
}

void CMergeDlg::FillWith (const TCHAR *name)
{
	
	CSplitPath path (name);
	CString pattern = path.GetDrive ();
	pattern += path.GetDir ();
	pattern += path.GetFName ();
	m_OutFile =  pattern;
	pattern += ".*";
	
	
	CSplitPath path2 (m_OutFile);
	m_OutDirName = path2.GetDrive ();
	m_OutDirName += path2.GetDir ();
	m_OutFileName = path2.GetFName ();
	m_OutFileName += path2.GetExt ();

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	
	hFind = FindFirstFile(pattern, &FindFileData);
	int count = 0;

	 if (hFind == INVALID_HANDLE_VALUE)
		 return;
	
	BOOL bNext = TRUE;

	while (bNext)
	{		
		bool bDir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==FILE_ATTRIBUTE_DIRECTORY;
		if (!bDir)
		{
				CString strFileName = m_OutDirName;
				strFileName+=FindFileData.cFileName;
				
				CString strExt = strFileName.Mid(strFileName.ReverseFind('.')+1);
				if (IsNumber (strExt))
				{
					m_FileList.InsertItem(0,strFileName);
					int nExtNo = _ttoi(strExt);
					m_FileList.SetItemData(0,nExtNo);
					count ++;
				}				
		}
		bNext = FindNextFile (hFind, &FindFileData);
	}		
	
	if (count)
	{
		m_FileList.SetColumnWidth(0,LVSCW_AUTOSIZE);
		m_FileList.SortItems(CompareFunc,0);
		UpdateData (FALSE);
	}	
	FindClose(hFind);	
}

void CMergeDlg::EnableAllControls(bool bEnable)
{
//	GetDlgItem(IDC_BTN_SELFILES  )->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_OUTFILE  )->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_FILEUP    )->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_FILEDOWN  )->EnableWindow(bEnable);
	GetDlgItem(IDC_LIST_FILES    )->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_FILEDEL   )->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_FILEADD   )->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_SELDIR    )->EnableWindow(bEnable);
	GetDlgItem(IDCANCEL			 )->EnableWindow(bEnable);
}


void CMergeDlg::OnBtnFileadd() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, NULL, this);

	dlg.m_ofn.lpstrFile = new TCHAR[4097];
	*dlg.m_ofn.lpstrFile = 0;
	dlg.m_ofn.nMaxFile = 4096;
	dlg.m_ofn.lpstrInitialDir = m_OutDirName;
	
	if(dlg.DoModal()==IDOK)
	{
		POSITION pos = dlg.GetStartPosition();
		CString strFileName;
		while(pos)
		{
			strFileName = dlg.GetNextPathName(pos);
			CString strExt = strFileName.Mid(strFileName.ReverseFind('.')+1);
			m_FileList.InsertItem(0,strFileName);
			int nExtNo = _ttoi(strExt);
			m_FileList.SetItemData(0,nExtNo);

		}
		m_FileList.SetColumnWidth(0,LVSCW_AUTOSIZE);
		m_FileList.SortItems(CompareFunc,0);
	}
	delete dlg.m_ofn.lpstrFile;
	
}


void CMergeDlg::OnBtnSeldir() 
{
	CString sz;
	CBrowseForFolder bf;	
	bf.strTitle = _T("Select Path For Output Files");
	bf.strStartupDir = m_OutDirName;
	if (bf.GetFolder(sz))
	{
		m_OutDirName = sz;
		m_OutFileName=m_OutFile.Mid(m_OutFile.ReverseFind('\\'));
		m_OutFile = m_OutDirName+m_OutFileName;
		UpdateData(FALSE);
	}
}


void CMergeDlg::OnBtnSelfiles() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, NULL, this);
	dlg.m_ofn.lpstrFile = new TCHAR[4097];
	*dlg.m_ofn.lpstrFile = 0;

	dlg.m_ofn.nMaxFile = 4096;
	if(dlg.DoModal()==IDOK)
	{
		POSITION pos = dlg.GetStartPosition();
		m_FileList.DeleteAllItems();
		CString strFileName;
		while(pos)
		{
			strFileName = dlg.GetNextPathName(pos);
			CString strExt = strFileName.Mid(strFileName.ReverseFind('.')+1);
			m_FileList.InsertItem(0,strFileName);
			int nExtNo = _ttoi(strExt);
			m_FileList.SetItemData(0,nExtNo);

		}
		m_FileList.SetColumnWidth(0,LVSCW_AUTOSIZE);
		m_FileList.SortItems(CompareFunc,0);
		if(m_OutFile.IsEmpty())
			m_OutFile =  strFileName.Left(strFileName.ReverseFind('.'));
		UpdateData(FALSE);
	}
	delete []dlg.m_ofn.lpstrFile;
}

void CMergeDlg::OnBtnStartmerge() 
{
	if(!m_bIsWorking)
	{
		if(UpdateData(TRUE))
		{
			int nCount = m_FileList.GetItemCount();
			if(nCount==0)
				return;
			if(m_OutFile.IsEmpty())
				return;
			GetDlgItem(IDC_BTN_STARTMERGE)->SetWindowText(_T("Cancel"));

			EnableAllControls(false);

			CStringArray arr;
			for(int i=0;i<nCount;++i)
			{
				arr.Add(m_FileList.GetItemText(i,0));
			}
			m_pFM = (CFileMerge*)AfxBeginThread(RUNTIME_CLASS(CFileMerge));
			m_pFM->m_pMainWnd=this;
			m_pFM->m_InFiles.Copy(arr);
			m_pFM->m_OutFile = m_OutFile;
			m_pFM->PostThreadMessage(WM_FM_START,0,0);
			m_bIsWorking = true;

		}
	}
	else
	{
		m_pFM->PostThreadMessage(WM_FM_CANCEL,0,0);
	}
	
}

void CMergeDlg::OnBtnFiledel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	int nDeleted = 0;
	CUIntArray arr;

	while(pos)
	{
		int nSel = m_FileList.GetNextSelectedItem(pos);
		//m_FileList.DeleteItem(nSel-nDeleted);
		arr.Add(nSel);
		
	}
	for(int i=0;i<arr.GetSize();++i)
	{
		m_FileList.DeleteItem(arr.GetAt(i)-nDeleted);
		nDeleted++;
	}

	m_FileList.SortItems(CompareFunc,0);
}


void CMergeDlg::OnBtnFileup() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	std::vector <int> sels;
	while(pos)
	{
		int nSel = m_FileList.GetNextSelectedItem(pos);
		if(nSel>0)
		{
			m_FileList.SetItemData(nSel-1, m_FileList.GetItemData(nSel-1)+ 1);
			m_FileList.SetItemData(nSel, m_FileList.GetItemData(nSel)-1);
			sels.push_back (nSel -1);
		}
		m_FileList.SortItems(CompareFunc,0);

	}
	m_FileList.SortItems(CompareFunc,0);

//	for (int i = 0; i < sels.size (); i++)
//		m_FileList.SetItemState(sels[i], LVIS_SELECTED, LVIS_SELECTED);	
}

void CMergeDlg::OnBtnFiledown() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	int nSel;
	int nDec = 1;
	while(pos)
	{
		nSel = m_FileList.GetNextSelectedItem(pos);
		if(nSel<m_FileList.GetItemCount()-1)
		{
			if(!( LVIS_SELECTED == m_FileList.GetItemState(nSel+1,LVIS_SELECTED) ))
			{
				m_FileList.SetItemData(nSel+1, m_FileList.GetItemData(nSel+1) - nDec);
				nDec=1;
			}
			else
				nDec++;

			m_FileList.SetItemData(nSel, m_FileList.GetItemData(nSel)+1);
		}
		//m_FileList.SortItems(CompareFunc,0);
	}
	/*if(nSel<m_FileList.GetItemCount()-1)
		m_FileList.SetItemData(nSel+1,m_FileList.GetItemData(nSel+1) - nDec);
	*/
	m_FileList.SortItems(CompareFunc,0);	
}



LRESULT CMergeDlg::OnUpdateStatus(WPARAM wP, LPARAM lP)
{
	if(lP==0)
		m_Progress.SetPos((int)wP);

	if(lP==-1)
	{
		m_Progress.SetPos(0);
		MessageBox(_T("Error merging files, please check filenames"), _T("Error"),MB_ICONERROR);
		EnableAllControls(true);
		GetDlgItem(IDC_BTN_STARTMERGE)->SetWindowText(_T("Merge"));		
		m_bIsWorking = false;

	}
	if(lP==1)
	{
		m_Progress.SetPos(100);
		EnableAllControls(true);
		GetDlgItem(IDC_BTN_STARTMERGE)->SetWindowText(_T("Merge"));
		//GetDlgItem(IDC_ME_STATIC)->SetWindowText("Done");
		m_bIsWorking = false;
	}
	return 0;	
}

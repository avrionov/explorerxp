/* Copyright 2002-2016 Nikolay Avrionov. All Rights Reserved.
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
#include "ExplorerXP.h"
#include "FileSplit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileSplit

IMPLEMENT_DYNCREATE(CFileSplit, CWinThread)

CFileSplit::CFileSplit()
{
	m_pMainWnd=NULL;
	pInFile = NULL;
	pOutFile = NULL;
}

CFileSplit::~CFileSplit()
{
	if(NULL!=pInFile)
	{
		delete pInFile;
	}
	if(NULL!=pOutFile)
	{
		delete pOutFile;
	}
}

BOOL CFileSplit::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	pInFile = NULL;
	m_nFileNameIndex = 1;
	pOutFile = NULL;
	m_dStatus=0;
	return TRUE;
}

int CFileSplit::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	if(NULL!=pInFile)
	{
		delete pInFile;
		pInFile=NULL;
	}
	if(NULL!=pOutFile)
	{
		delete pOutFile;
		pOutFile=NULL;
	}
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CFileSplit, CWinThread)
	//{{AFX_MSG_MAP(CFileSplit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(WM_FS_START,OnSplitStart)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileSplit message handlers
/*
int howTo: Indicates how to split.. 0 - Split into fixed number of files ( contained in splitVal)
									1 - Split into fixed size files ( size of each file is in splitVal)
  */
bool CFileSplit::CreateSplitFiles(CString inputFile, CString outputDir, int howTo, int splitVal)
{
	try
	{
		if(!start())
		{
			if(m_pMainWnd)
				m_pMainWnd->SendMessage(WM_FS_UPDATESTATUS,0,-1);
			return false;
		}

		CFileStatus rStatus;
		pInFile->GetStatus(rStatus);

		ULONGLONG nSize = splitVal;

		if (0 == howTo)
		{
			//make fixed no: of files i.e splitVal contains no: of parts to generate
			nSize = rStatus.m_size / splitVal + 1;
		}

		ULONGLONG nBytesRead;

		//calculate how many bytes to read for each cycle. This will be BYTES_PER_READ_MAX unless 
		//the total size of each output file comes out to less than that...

		UINT nBytesPerRead = (nSize > BYTES_PER_READ_MAX) ? BYTES_PER_READ_MAX : (UINT)nSize;

		//total bytes read ( this will reset for each output file )
		ULONGLONG nBytesTot = 0;
		
		do
		{
			nBytesRead = pInFile->Read(pBuf,nBytesPerRead);

			nBytesTot += nBytesRead;
			//pOutFile->Write(pBuf,nBytesRead);
			if(nBytesTot>=nSize)
			{
				pOutFile->Write(pBuf, static_cast <UINT> (nSize - (nBytesTot - nBytesRead)) );
				if(!changeOutputFile())
				{
					if(m_pMainWnd)
						m_pMainWnd->SendMessage(WM_FS_UPDATESTATUS,100,1);
					return false;
				}
				pOutFile->Write(pBuf +  static_cast<UINT> (nSize - (nBytesTot-nBytesRead)), static_cast <UINT>(nBytesTot-nSize));
				UpdateStatus(GetStatus()+100* (double)(nBytesTot)/(double)rStatus.m_size);
				nBytesTot = nBytesTot-nSize;
			}
			else
				pOutFile->Write(pBuf, static_cast <UINT>(nBytesRead));

			/*Check For Cancel..*/
			MSG msg;
			if(PeekMessage(&msg,(HWND)-1, WM_FS_CANCEL, WM_FS_CANCEL,PM_NOREMOVE))
				break;

		}
		while( nBytesRead == nBytesPerRead);

		//Close final output file;
		pOutFile->Close();
		if(m_pMainWnd)
			m_pMainWnd->SendMessage(WM_FS_UPDATESTATUS,100,1);

		return true;
	}
	catch(CFileException *e)
	{
		e->Delete();
		return false;
	}	
}

bool CFileSplit::start()
{
	m_nFileNameIndex = 1;

	if(NULL!=pInFile)
	{
		delete pInFile;
	}
	pInFile = new CStdioFile();
	if(!pInFile->Open(m_InputFileName, CFile::modeRead | CFile::typeBinary))
		return false;
	
	if(NULL!=pOutFile)
	{
		delete pOutFile;
	}
	pOutFile = new CStdioFile();
	if(!pOutFile->Open(m_OutputDirName + "\\" + makeNewFileName(), CFile::modeCreate  | CFile::typeBinary| CFile::modeWrite ))
		return false;
	return true;
}

CString CFileSplit::makeNewFileName()
{
	CString strName;
	strName.Format(_T("%s.%03d"),  static_cast<LPCWSTR>(pInFile->GetFileName()), m_nFileNameIndex);
	m_nFileNameIndex++;
	return strName;
}

bool CFileSplit::changeOutputFile()
{
	//close the current output file and open new one
	pOutFile->Close();
	if(!pOutFile->Open(m_OutputDirName + "\\" + makeNewFileName(),CFile::modeCreate  | CFile::typeBinary | CFile::modeWrite))
		return false;
	return true;
}


double CFileSplit::GetStatus()
{
	return m_dStatus;
}
/*
Function updates the status, sends a message to the main window indicating the progress of the 
split
*/
void CFileSplit::UpdateStatus(double dPerc)
{
	if(m_pMainWnd && IsWindow(m_pMainWnd->m_hWnd))
	{
		m_pMainWnd->SendMessage(WM_FS_UPDATESTATUS,(WPARAM)dPerc,0);
	}
	m_dStatus = dPerc;
}


/////////////////////////////////////////////////////////////////////////////
// 
/* 
	Message Handler for Starting split
*/
void CFileSplit::OnSplitStart(WPARAM wP, LPARAM lP)
{
	CreateSplitFiles(m_InputFileName, m_OutputDirName, m_nSplitMode, m_nSplitVal);
	::PostQuitMessage(0);
}

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
#include "FileMerge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileMerge

IMPLEMENT_DYNCREATE(CFileMerge, CWinThread)

CFileMerge::CFileMerge()
{
	pInFile = NULL;
	pOutFile = NULL;
	m_dStatus = 0.0;
}

CFileMerge::~CFileMerge()
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

BOOL CFileMerge::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	pInFile = NULL;
	pOutFile = NULL;
	return TRUE;
}

int CFileMerge::ExitInstance()
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
//handler for thread message is added below
BEGIN_MESSAGE_MAP(CFileMerge, CWinThread)
	//{{AFX_MSG_MAP(CFileMerge)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(WM_FM_START,OnMergeStart)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileMerge message handlers

//function to start off the merging process..
bool CFileMerge::start()
{
	m_OutFile = m_InFiles[0].Left(m_InFiles[0].ReverseFind('.')+1);

	if(NULL!=pInFile)
	{
		delete pInFile;
		pInFile = NULL;
	}
	if(NULL!=pOutFile)
	{
		delete pOutFile;
		pOutFile=NULL;
	}
	return true;
}

//function that actually creates the joined file. This function can be called 
//directly or it will be invoked when the thread receives the WM_FM_START 
//message
bool CFileMerge::CreateJoinFile(CStringArray &inFiles, CString outFile)
{
	try
	{
		//outFile = inFiles[0].Left(m_InFiles[0].ReverseFind('.')+1);
		bool bCancel = false;
		if(NULL!=pInFile)
		{
			delete pInFile;
		}
		if(NULL!=pOutFile)
		{
			delete pOutFile;
		}
		pOutFile = new CStdioFile();
		if(!pOutFile->Open(outFile, CFile::modeCreate  | CFile::typeBinary | CFile::modeWrite))
		{
			if(m_pMainWnd)
				m_pMainWnd->SendMessage(WM_FM_UPDATESTATUS,0,-1);
			return false;
		}


		pInFile  = new CStdioFile();

		for(int i=0;i<inFiles.GetSize();++i)
		{
			if(!pInFile->Open(inFiles[i],CFile::modeRead | CFile::typeBinary))
			{
				if(m_pMainWnd)
					m_pMainWnd->SendMessage(WM_FM_UPDATESTATUS,0,-1);
				return false;
			}
			int nBytesRead = 0;
			do
			{
				nBytesRead = pInFile->Read(pBuf,BYTES_PER_READ);
				pOutFile->Write(pBuf,nBytesRead);
				/*Check For Cancel..*/
				MSG msg;
				if(PeekMessage(&msg,(HWND)-1,WM_FM_CANCEL,WM_FM_CANCEL,PM_NOREMOVE))
				{
					bCancel = true;
					break;
				}

			}
			while(nBytesRead==BYTES_PER_READ);
			UpdateStatus(100*(double)i/(double)inFiles.GetSize());
			pInFile->Close();
			if(bCancel)
				break;
			
		}
		pOutFile->Close();
		if(m_pMainWnd)
			m_pMainWnd->SendMessage(WM_FM_UPDATESTATUS,100,1);

	}
	catch(CFileException *e)
	{
		e->Delete();    
		return false;
	}
	
	return true;
}


//function to update the invoking thread of the status of the 
//merging process.
void CFileMerge::UpdateStatus(double dPerc)
{
	if(IsWindow(m_pMainWnd->m_hWnd))
	{
		m_pMainWnd->SendMessage(WM_FM_UPDATESTATUS,(WPARAM)dPerc,0);
		//::SendMessage(m_pMainWnd->m_hWnd, WM_FM_UPDATESTATUS, (WPARAM)dPerc,0);
	}
	m_dStatus = dPerc;
}

double CFileMerge::GetStatus()
{
	return m_dStatus;
}

/* 
	Message Handler for Starting Merge
*/
void CFileMerge::OnMergeStart(WPARAM wP, LPARAM lP)
{
	CreateJoinFile(m_InFiles, m_OutFile);
	::PostQuitMessage(0);
}

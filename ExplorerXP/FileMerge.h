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


#ifndef __FILEMERGE_H__
#define __FILEMERGE_H__

#pragma once

#define BYTES_PER_READ 65536

//messages used by the thread
#define WM_FM_UPDATESTATUS (WM_APP+5)
#define WM_FM_START		   (WM_APP+6)
#define WM_FM_CANCEL	   (WM_APP+7)

/////////////////////////////////////////////////////////////////////////////
// CFileMerge thread
// This Class Handles File Merging
class CFileMerge : public CWinThread
{
	DECLARE_DYNCREATE(CFileMerge)
protected:
	CFileMerge();           // protected constructor used by dynamic creation

// Attributes
public:
	CStringArray m_InFiles;
	CString		 m_OutFile;
protected:
	CFile *pInFile, *pOutFile;
	char		pBuf[BYTES_PER_READ];
	double m_dStatus;

// Operations
public:
	bool CreateJoinFile(CStringArray& inFiles, CString outFile);
	double GetStatus();
	void UpdateStatus(double dPerc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileMerge)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool start();
	virtual ~CFileMerge();

	// Generated message map functions
	//{{AFX_MSG(CFileMerge)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnMergeStart(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __FILEMERGE_H__

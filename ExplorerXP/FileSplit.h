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


#ifndef __FILESPLIT_H__
#define __FILESPLIT_H__

#pragma once

#define BYTES_PER_READ_MAX 65536

//messages used by the thread
#define WM_FS_UPDATESTATUS (WM_APP+1)
#define WM_FS_START		   (WM_APP+2)
#define WM_FS_CANCEL	   (WM_APP+3)


/////////////////////////////////////////////////////////////////////////////
// CFileSplit thread
// This class handles file splitting
class CFileSplit : public CWinThread
{
	DECLARE_DYNCREATE(CFileSplit)
protected:
	CFileSplit();           // protected constructor used by dynamic creation

// Attributes
public:
	double GetStatus();
	void UpdateStatus(double dPerc);

protected:
	bool changeOutputFile();
	CString makeNewFileName();
	bool start();
	CStdioFile *pInFile;
	CStdioFile *pOutFile;
	int m_nFileNameIndex;
	char pBuf[BYTES_PER_READ_MAX];
	double m_dStatus;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileSplit)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool CreateSplitFiles(CString inputFile, CString outputDir,int howTo, int splitVal);
	CString m_OutputDirName;
	CString m_InputFileName;
	int m_nSplitMode;
	int m_nSplitVal;
	virtual ~CFileSplit();

	// Generated message map functions
	//{{AFX_MSG(CFileSplit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnSplitStart(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __FILESPLIT_H__

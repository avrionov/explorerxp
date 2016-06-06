// ==========================================================================
// 					Class Specification : COXFileChanger
// ==========================================================================
// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
//
// Version: 9.3
// //////////////////////////////////////////////////////////////////////////
// Properties:
//		NO		 Abstract class (does not have any objects)
//		NO		 Derived from CObject
//
//		NO		 Is a CWnd.                     
//		NO		 Two stage creation (constructor & Create())
//		NO		 Has a message map
//		NO		 Needs a resource (template)
//
//		NO		 Persistent objects (saveable on disk)      
//		YES		 Uses exceptions
//
// Description: COXFileChanger is a class that organizes search/replace of a 
//				file's contents. So the class can search the contents of a 
//				file for a certain part and (optionally) replace it by 
//				another part. The class can operate both on text (ansi, non-
//				unicode) files and binary files.
// Algorithm:	Boyer-Moore
//				Original publication : R.S. Boyer, J.S.Moore, "A fast string searching
//				 algorithm", Comm. ACM, 20, 10 (Oct 77), p. 762-772.
//				Other source : "Algorithms in C++", Robert Sedgewick, Princeton University,
//					Addison-Wesley, 1992, pp 286-289.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __OXFILECHANGER_H__
#define __OXFILECHANGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "OXDllExt.h"
//#include "OXMainRes.h"


/////////////////////////////////////////////////////////////////////////////


class OX_CLASS_DECL COXFileChanger : public CObject
{
DECLARE_DYNAMIC(COXFileChanger);
// Data members -------------------------------------------------------------
public:
	class OX_CLASS_DECL COXFileChangerData
		{
		public:
		CFile* m_pFile;
		// --- A CFile pointer to the input file (default) or output file (WM_OX_FILE_POST_REPLACE)
		LONG m_nPos;
		// --- The absolute position (zero-based) of the first character of 
		//     the found or replaced part in the input file
		BOOL& m_bContinue;
		// --- TRUE by default if bAll was TRUE, otherwise FALSE by default. 
		//     This value may be changed from within the notification handler

		COXFileChangerData(CFile* pFile, LONG nPos, BOOL& bContinue)
			:
			m_pFile(pFile),
			m_nPos(nPos),
			m_bContinue(bContinue)
			{
			}
		};

	static LONG m_nBufferSize;
	// ... Maximum search/replace length (64K). It can be increased for larger searching block.
	// Theoretical upper limit is the biggest int (31 bits), yet limited by
	// run-time memory footprint

protected:
	HWND m_hNotifyWnd;
	UINT m_nFlag;
	CFile* m_pFileIn;
	CFile* m_pFileOut;
	CFile m_fileIn;
	CFile m_fileOut;
	CString m_sInputFileName;
	CString m_sOutputFileName;
	CByteArray* m_pSearch;
	CByteArray* m_pSearchOppositeCase;
	CByteArray* m_pReplace;

	static const int m_nFlagUseCFile;
	static const int m_nFlagOverwrite;
	static const int m_nFlagReplace;
	static const int m_nFlagText;
	static const int m_nFlagAll;
	static const int m_nFlagMatchCase;
	static const int m_nFlagWholeWord;
	static LPCTSTR m_sTempFilePrefix;

private:

// Member functions ---------------------------------------------------------
public:
	COXFileChanger();
	// --- In      :
	// --- Out     : 
	// --- Returns :
	// --- Effect  : Constructor of object

	virtual ~COXFileChanger();
	// --- In      :
	// --- Out     : 
	// --- Returns :
	// --- Effect  : Destructor of object

	void UseFiles(LPCTSTR pszInputFileName, LPCTSTR pszOutputFileName = NULL);
	// --- In      : pszInputFileName, pathname of the input file (file to be 
	//					searched in)
	//				 pszOutputFileName, pathname of the output file (when 
	//					Replace() will be called); if NULL, the input file 
	//					will be overwritten with the Replace() result
	// --- Out     : 
	// --- Returns :
	// --- Effect  : provides the input and output files to the class
	//				 NOTE: the files will not be opened only until Search() or 
	//					Replace() is called
	
	void UseFiles(CFile* pInputFile, CFile* pOutputFile = NULL);
	// --- In      : pInputFile, a CFile pointer to the input file (file to 
	//					be searched in)
	//				 pOutputFile, a CFile pointer to the output file (when 
	//					Replace() will be called); cannot be NULL if Replace()
	//					will be called.
	//				 NOTE: these two pointers cannot be the same. The objects
	//					will not be opened or closed by the COXFileChanger, so
	//					they should have already been opened.
	// --- Out     : 
	// --- Returns :
	// --- Effect  : provides the input and output files to the class
	
	void SetNotificationWnd(CWnd* pNotifyWnd);
	//	--- In:	pNotifyWnd : CWnd to which notification messages are sent
	//	--- Out:
	//	--- Returns:	
	//	---	Effect:	This window will receive the OnFound, OnPreReplace and OnPostReplace
	//              messages. This can be used to visiually inform the user of the progress
	//				(WM_OX_FILE_MATCH_FOUND, WM_OX_FILE_PRE_REPLACE, WM_OX_FILE_POST_REPLACE)

	UINT Search (LPCTSTR pszText, BOOL bAll = FALSE, BOOL bMatchCase = FALSE, 
		BOOL bWholeWordOnly = FALSE);
	UINT Search (const CByteArray& binary, BOOL bAll = FALSE);
	// --- In      : pszText, the text to search for in the input file
	//				 binary, the binary to search for in the input file
	//				 bAll, specifies whether all occurrences should be found 
	//					(TRUE) or only the first one (FALSE).
	//				 bMatchCase, specifies whether the search is case 
	//					sensitive (TRUE) or not (FALSE)
	//				 bWholeWordOnly, if TRUE, specifies that the characters 
	//					proceding and trailing the found word should not be 
	//					alphanumeric character (a-z, A-Z, 0-9); if FALSE, any 
	//					character may surround the found word.
	// --- Out     : 
	// --- Returns : the number of times the text (binary) was found
	// --- Effect  : search text or binary contents in the input file
	//				 NOTE: in the case of CFile pointer is supplied in 
	//					UseFiles(), the file pointers will not be set to the
	//					beginning after search
	//				 NOTE: OnFound() can be derived to handle an event 
	//					when a match is found
		
	UINT Replace(LPCTSTR pszText, LPCTSTR pszReplaceText, BOOL bAll = FALSE, 
		BOOL bMatchCase = FALSE, BOOL bWholeWordOnly = FALSE);
	UINT Replace(const CByteArray& binary, const CByteArray& replaceBinary, 
		BOOL bAll = FALSE);
	// --- In      : pszText, the text to be replaced in the input file
	//				 pszReplaceText, the text to change to in the output file
	//				 binary, the binary to be replaced in the input file
	//				 replaceBinary, the binary to change to in the output file
	//				 bAll, specifies whether all occurrences should be 
	//					replaced (TRUE) or only the first one (FALSE).
	//				 bMatchCase, specifies whether the search is case 
	//					sensitive (TRUE) or not (FALSE)
	//				 bWholeWordOnly, if TRUE, specifies that the characters 
	//					proceding and trailing the word to be replaced should
	//					not be alphanumeric character (a-z, A-Z, 0-9); 
	//					if FALSE, any character may surround it.
	// --- Out     : 
	// --- Returns : occurence of the replacement
	// --- Effect  : search text or binary contents in the input file, and 
	//				 replace it with another text or binary in the output
	//				 file
	//				 NOTE: in the case of CFile pointer is supplied in 
	//					UseFiles(), the file pointers will not be set to the
	//					beginning after search
	//				 NOTE: OnPreReplace() and OnPostReplace() can be derived
	//					to handle the events before or after the replacement
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : AssertValid performs a validity check on this object 
	//				by checking its internal state. 
	//				In the Debug version of the library, AssertValid may assert and 
	//				thus terminate the program.
	
	virtual void Dump(CDumpContext& dc) const;
	// --- In  : dc : The diagnostic dump context for dumping, usually afxDump.
	// --- Out : 
	// --- Returns :
	// --- Effect : Dumps the contents of the object to a CDumpContext object. 
	//				It provides diagnostic services for yourself and 
	//				 other users of your class. 
	//				Note  The Dump function does not print a newline character
	//				 at the end of its output. 
#endif

protected:
	virtual BOOL OnFound (CFile* pInputFile, LONG nInPos, BOOL& bContinue);
	virtual BOOL OnPreReplace (CFile* pInputFile, LONG nInPos, BOOL& bContinue);
	virtual void OnPostReplace (CFile* pOutputFile, LONG nOutPos);

	void SetFlag(UINT nOXFCFLAG, BOOL bValue = TRUE);
	BOOL GetFlag(UINT nOXFCFLAG);
	UINT Run();
	void OpenFiles();
	void CloseFiles();

	static void CopyTextToByteArray(LPCTSTR pszText, CByteArray* pBuffer);
	static void ReverseCase(CString& sText);
	static CString GetUniqueTempName();

private:
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __OXFILECHANGER_H__

// end of OXFileChanger.h
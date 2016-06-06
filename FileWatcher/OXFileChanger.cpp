// =============================================================================
// 							Class Implementation : COXFileChanger
// =============================================================================
//

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXFileChanger.h"

#include "UTBStrOp.h"
#include "UTB64Bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXFileChanger, CObject)

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

LONG COXFileChanger::m_nBufferSize = 0xFFFF;

const int COXFileChanger::m_nFlagUseCFile	=	0x0001;
// --- 00000001 1: UseFiles(CFile*); 0: UseFiles(pszFileName)
const int COXFileChanger::m_nFlagOverwrite =	0x0002;
// --- 00000010 1: Overwrite; 0: Output to another file
const int COXFileChanger::m_nFlagReplace =		0x0004;
// --- 00000100 1: Replace(); 0: Search()
const int COXFileChanger::m_nFlagText =			0x0008;
// --- 00001000 1: Text; 0: Binary
const int COXFileChanger::m_nFlagAll =			0x0010;
// --- 00010000 bAll ? 1 : 0
const int COXFileChanger::m_nFlagMatchCase =	0x0020;
// --- 00100000 bMatchCase	  ? 1 : 0
const int COXFileChanger::m_nFlagWholeWord =	0x0040;
// --- 01000000 bWholeWordOnly ? 1 : 0

TCHAR szTempFilePrefix[] = _T("~fc");
LPCTSTR COXFileChanger::m_sTempFilePrefix = szTempFilePrefix;
// ---- Prefix used to create temporary file

// Data members -------------------------------------------------------------
// protected:
	//	UINT m_nFlag;
	// --- Internal status flags

	//	CFile* m_pFileIn;
	// --- Actual input file

	//	CFile* m_pFileOut;
	// --- Actual output file

	//	CFile m_fileIn;
	// --- Used when file name is supplied

	//	CFile m_fileOut;
	// --- Used when file name is supplied

	//	CString m_sInputFileName;
	// --- Supplied input file name

	//	CString m_sOutputFileName;
	// --- Supplied output file name

	//	CByteArray* m_pSearch;
	// --- Search bytes

	//  CByteArray* m_pSearchOppositeCase;
	// --- Search bytes with opposite case

	//	CByteArray* m_pReplace;
	// --- Replace bytes

// Member functions ---------------------------------------------------------
COXFileChanger::COXFileChanger()
	:
	m_hNotifyWnd(NULL),
	m_nFlag(0),
	m_pFileIn(NULL),
	m_pFileOut(NULL),
	m_fileIn(),
	m_fileOut(),
	m_sInputFileName(),
	m_sOutputFileName(),
	m_pSearch(NULL),
	m_pSearchOppositeCase(NULL),
	m_pReplace(NULL)
	{
	}

COXFileChanger::~COXFileChanger()
	{
	}

void COXFileChanger::UseFiles(LPCTSTR pszInputFileName, 
							  LPCTSTR pszOutputFileName /* = NULL */)
	{
	ASSERT(pszInputFileName);

	m_pFileIn = &m_fileIn;
	m_pFileOut = &m_fileOut;
	m_sInputFileName = pszInputFileName;	
	ASSERT(!m_sInputFileName.IsEmpty());
	m_sOutputFileName = pszOutputFileName;
	SetFlag(m_nFlagUseCFile, FALSE);
	SetFlag(m_nFlagOverwrite, (pszOutputFileName == NULL ||
		m_sInputFileName.CompareNoCase(m_sOutputFileName) == 0));
	}

void COXFileChanger::UseFiles(CFile* pInputFile, CFile* pOutputFile /* = NULL */)
	{
	ASSERT((pInputFile != NULL) && (pInputFile != pOutputFile));

	m_pFileIn = pInputFile;
	m_pFileOut = pOutputFile;
	m_sInputFileName.Empty();
	m_sOutputFileName.Empty();
	SetFlag(m_nFlagUseCFile);
	SetFlag(m_nFlagOverwrite, FALSE);
	}

void COXFileChanger::SetNotificationWnd(CWnd* pNotifyWnd)
	{
	m_hNotifyWnd = pNotifyWnd->GetSafeHwnd();
	}

UINT COXFileChanger::Search (LPCTSTR pszText, 
							 BOOL bAll /* = FALSE */, BOOL bMatchCase /* = FALSE */, 
							 BOOL bWholeWordOnly /* = FALSE */)
	{
	ASSERT(m_pFileIn != NULL);

	CByteArray searchText, searchTextRevCase;
	CopyTextToByteArray(pszText, m_pSearch = &searchText);
	if (!bMatchCase)
		{
		CString sText = pszText;
		ReverseCase(sText);
		CopyTextToByteArray(sText, m_pSearchOppositeCase = &searchTextRevCase);
		}
	SetFlag(m_nFlagReplace, FALSE);
	SetFlag(m_nFlagText);
	SetFlag(m_nFlagAll, bAll);
	SetFlag(m_nFlagMatchCase, bMatchCase);
	SetFlag(m_nFlagWholeWord, bWholeWordOnly);
	return Run();
	}

UINT COXFileChanger::Search (const CByteArray& binary, BOOL bAll /* = FALSE */)
	{
	ASSERT(m_pFileIn != NULL);

	m_pSearch = (CByteArray*)&binary;
	SetFlag(m_nFlagReplace, FALSE);
	SetFlag(m_nFlagText, FALSE);
	SetFlag(m_nFlagAll, bAll);
	return Run();
	}

UINT COXFileChanger::Replace(LPCTSTR pszText, LPCTSTR pszReplaceText, 
							 BOOL bAll /* = FALSE */, BOOL bMatchCase /* = FALSE */,
							 BOOL bWholeWordOnly /* = FALSE */)
	{
	ASSERT((m_pFileIn != NULL) && (m_pFileOut != NULL));

	CByteArray searchText, searchTextRevCase, replaceText;
	CopyTextToByteArray(pszText, m_pSearch = &searchText);
	if (!bMatchCase)
		{
		CString sText = pszText;
		ReverseCase(sText);
		CopyTextToByteArray(sText, m_pSearchOppositeCase = &searchTextRevCase);
		}
	CopyTextToByteArray(pszReplaceText, m_pReplace = &replaceText);
	SetFlag(m_nFlagReplace);
	SetFlag(m_nFlagText);
	SetFlag(m_nFlagAll, bAll);
	SetFlag(m_nFlagMatchCase, bMatchCase);
	SetFlag(m_nFlagWholeWord, bWholeWordOnly);
	return Run();
	}

UINT COXFileChanger::Replace(const CByteArray& binary, const CByteArray& replaceBinary, 
							 BOOL bAll /* = FALSE */)
	{
	ASSERT((m_pFileIn != NULL) && (m_pFileOut != NULL));

	m_pSearch = (CByteArray*)&binary;
	m_pReplace = (CByteArray*)&replaceBinary;
	SetFlag(m_nFlagReplace);
	SetFlag(m_nFlagText, FALSE);
	SetFlag(m_nFlagAll, bAll);
	return Run();
	}

#ifdef _DEBUG
void COXFileChanger::AssertValid() const
	{
	CObject::AssertValid();
	}

void COXFileChanger::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);

	dc << "\nm_nBufferSize=" << m_nBufferSize;
	dc << "\nm_hNotifyWnd=" << m_hNotifyWnd;

	dc << "\nm_nFlag=" << m_nFlag;
	dc << "\nm_pFileIn=" << m_pFileIn;
	dc << "\nm_pFileOut=" << m_pFileOut;
	dc << "\nm_fileIn=" << m_fileIn;
	dc << "\nm_fileOut=" << m_fileOut;
	dc << "\nm_sInputFileName=" << m_sInputFileName;
	dc << "\nm_sOutputFileName=" << m_sOutputFileName;

	dc << "\nm_pSearch=" << m_pSearch;
	dc << "\nm_pSearchOppositeCase=" << m_pSearchOppositeCase;
	dc << "\nm_pReplace=" << m_pReplace;

	dc << "\nm_nFlagUseCFile=" << m_nFlagUseCFile;
	dc << "\nm_nFlagOverwrite=" << m_nFlagOverwrite;
	dc << "\nm_nFlagReplace=" << m_nFlagReplace;
	dc << "\nm_nFlagText=" << m_nFlagText;
	dc << "\nm_nFlagAll=" << m_nFlagAll;
	dc << "\nm_nFlagMatchCase=" << m_nFlagMatchCase;
	dc << "\nm_nFlagWholeWord=" << m_nFlagWholeWord;
	dc << "\nm_sTempFilePrefix=" << m_sTempFilePrefix;

	dc << "\n";
	}
#endif //_DEBUG

// protected:
BOOL COXFileChanger::OnFound (CFile* pInputFile, LONG nInPos, BOOL& bContinue)
	// --- In      : pInputFile, a CFile pointer to the input file
	//				 nInPos, the absolute position (zero-based) of the first
	//					character of the found part in the input file
	// --- Out     : bContinue, TRUE by default if bAll was TRUE, otherwise
	//					FALSE by default. It may be changed within the 
	//					function to specify whether to continue searching
	// --- Returns : whether this occurrence should be ignored (FALSE) or not
	//					(TRUE, default)
	// --- Effect  : This function is called when an occurrence has been found
	//					during the Search().
	//				 NOTE: The current position of the file specified by 
	//					pInputFile may be changed within this function. It 
	//					will be automatically reset to the last search 
	//					position after the function returns.
	{
	BOOL bResult = TRUE;
	if (m_hNotifyWnd != NULL)
		{
		COXFileChangerData fileChangerData(pInputFile, nInPos, bContinue);
		bResult = (BOOL)::SendMessage(m_hNotifyWnd, WM_OX_FILE_MATCH_FOUND, (WPARAM)&fileChangerData, NULL);
		}

	return bResult;
	}

BOOL COXFileChanger::OnPreReplace (CFile* pInputFile, LONG nInPos, BOOL& bContinue)
	// --- In      : pInputFile, a CFile pointer to the input file
	//				 nInPos, the absolute position (zero-based) of the first
	//					character of the found part in the input file
	// --- Out     : bContinue, TRUE by default if bAll was TRUE, otherwise
	//					FALSE by default. It may be changed within the 
	//					function to specify whether to continue searching after
	//					the current replacement is processed
	// --- Returns : whether this occurrence should be ignored (FALSE) thus 
	//					not replaced, or not (TRUE, default)
	// --- Effect  : This function is called when an occurrence has been found
	//					during the Replace().
	//				 NOTE: The current position of the file specified by 
	//					pInputFile may be changed within this function. It 
	//					will be automatically reset to the last search 
	//					position after the function returns.
	{
	BOOL bResult = TRUE;
	if (m_hNotifyWnd != NULL)
		{
		COXFileChangerData fileChangerData(pInputFile, nInPos, bContinue);
		bResult = (BOOL)::SendMessage(m_hNotifyWnd, WM_OX_FILE_PRE_REPLACE, (WPARAM)&fileChangerData, NULL);
		}

	return bResult;
	}

void COXFileChanger::OnPostReplace (CFile* pOutputFile, LONG nOutPos)
	// --- In      : pOutputFile, a CFile pointer to the output file
	//				 nOutPos, the absolute position (zero-based) of the first
	//					character of the replaced part in the output file.
	// --- Out     : 
	// --- Returns :
	// --- Effect  : This function is called after the replacement has been
	//					taken place. The replacing contents has already been
	//					written to file when this is called.
	//				 NOTE: The current position of the file specified by 
	//					pOutputFile may be changed within this function. It
	//					will be automatically reset after the function returns.
	//				 NOTE: If a previous OnPreReplace() returns FALSE, 
	//					OnPostReplace will not be called. In this situation
	//					the search may still continue depending on the value
	//					of the out parameter bContinue in OnPreReplace().
	{
	if (m_hNotifyWnd != NULL)
		{
		BOOL bDummy = FALSE;
		COXFileChangerData fileChangerData(pOutputFile, nOutPos, bDummy);
		::SendMessage(m_hNotifyWnd, WM_OX_FILE_POST_REPLACE, (WPARAM)&fileChangerData, NULL);
		}
	}

void COXFileChanger::SetFlag(UINT nOXFCFLAG, BOOL bValue /* = TRUE */)
	// --- In      : nOXFCFLAG, specify which flag to set
	//				 bValue, specify the value to set
	// --- Out     : 
	// --- Returns :
	// --- Effect  : set a flag to 0 or 1
	{
	if (bValue)
		m_nFlag |= nOXFCFLAG;
	else
		m_nFlag &= ~nOXFCFLAG;
	}

BOOL COXFileChanger::GetFlag(UINT nOXFCFLAG)
	// --- In      : nOXFCFLAG, specify which flag to get
	// --- Out     : 
	// --- Returns :
	// --- Effect  : retreive flag status
	{
	return m_nFlag & nOXFCFLAG;
	}

UINT COXFileChanger::Run()
	// --- In      :
	// --- Out     : 
	// --- Returns : occurences of found match or replacement
	// --- Effect  : perform search/replace
	{
	int  M = PtrToInt(m_pSearch->GetSize());
	// ... search text/binary length
	int	 M_1 = M - 1;
	// ... array max subscript
	int	 MM_1 = M + M_1;
	// ... i shift value when found
	int	 n = 0;
	// ... actual bytes read from the file at one time
	int	 i = 0;
	// ... scanning position pointer of file buffer
	int	 j = 0;
	// ... scanning position pointer of search text/binary
	int	 w = 0;
	// ... write position pointer in the loaded file buffer
	int	 t = 0;
	// ... temp value
	int	 skip[255];					
	// ... skip array for each byte value

	LONG lPos0 = 0;
	// ... file position of the current buffer[0]
	LONG nCurInPos = 0;
	// ... loading position of the input file
	LONG nInPos = 0;
	// ... match found position of the input file
	LONG nCurOutPos = 0;
	// ... writing position of the output file
	LONG nOutPos = 0;
	// ... replace text/binary position of the output file
	LONG nChPos = 0;
	// ... position of prev/next char for whole word match
	LONG nChPosMax = 0;
	// ... maximum file position of prev/next char for whole word match
	UINT nResult = 0;
	// ... match occurence
	BYTE* pBuffer = NULL;	
	// ... file buffer

	BOOL bEOF = FALSE;
	// ... end of file
	BOOL bContinue = TRUE;
	// ... default bContinue value
	BOOL bReplace = GetFlag(m_nFlagReplace);
	// ... Replace() (TRUE) or Search() (FALSE)
	BOOL bText = GetFlag(m_nFlagText);
	// ... whether text search
	BOOL bMatchCase = (!bText || GetFlag(m_nFlagMatchCase));
	// ... whether case-insensitive
	BOOL bWholeWord = (bText && GetFlag(m_nFlagWholeWord));
	// ... whether whole word
	char ch = '\0';
	// ... prev/next char for whole word match
	CString sSafeStack;
	// Increases the stack size
	
	if (M == 0)
		{
		TRACE0("COXFileChanger::Run(): aborted: zero length search text/binary encountered.\r\n");
		return 0;
		}

	if (m_nBufferSize < M)
		{
		TRACE0("COXFileChanger::Run(): aborted: length of search text/binary exceeds upper limit.\r\n");
		return 0;
		}

	// Initialize skip array
	for (j = 0; j < 256; j++) 
		skip[j] = M;
	for (j = 0, t = M; j < M; j++) 
		skip[m_pSearch->GetAt(j)] = --t;
	if (!bMatchCase) 
		for (j = 0, t = M; j < M; j++) 
			skip[m_pSearchOppositeCase->GetAt(j)] = --t;
		
	try
		{
		// Alllocate the buffer
		pBuffer = new BYTE[m_nBufferSize];

		OpenFiles();
		nChPosMax = (LONG) m_pFileIn->GetLength() - 1;
		while (!bEOF && (n = m_pFileIn->Read(pBuffer, m_nBufferSize)) > 0)
			{
			bEOF = (n < m_nBufferSize);
			if (!bContinue && bReplace)
				{ 
				// Cycle until all remaining contents are copied
				m_pFileOut->Write(pBuffer, n);
				continue;
				}

			if (!bContinue || n < M) 
				break;
			i = - M; w = 0;
			while (bContinue)
				{
				// search one occurence
				for (i += MM_1, j = M_1; 0 <= j && i < n; i--, j--)
					{
					while (pBuffer[i] != m_pSearch->GetAt(j) && (bMatchCase || 
						   pBuffer[i] != m_pSearchOppositeCase->GetAt(j)))
						{
						t = skip[pBuffer[i]];
						i += __max(M - j, t);
						if (n <= i)	
							{
							i++; 
							break;
							}
						j = M_1;
						}
					}
				// out of buffer boundary
				if (n <= i)
					{
					if (bReplace) 
						m_pFileOut->Write(pBuffer + w, (bEOF ? n : i - M_1) - w);
					if (!bEOF) 
						lPos0 = (LONG) m_pFileIn->Seek(i - M_1 - n, CFile::current);
					break;
					}

				i++;
				nCurInPos = (LONG) m_pFileIn->GetPosition();
				nInPos = lPos0 + i;
				
				// test wholeword match
				if (bWholeWord)
					{
					nChPos = i - 1;
					for (t = 0; t < 2; t++)
						{
						// look in buffer first
						if (0 <= nChPos && nChPos < n)
							ch = *(pBuffer + nChPos);
						else
							{ // look in the file
							nChPos += lPos0;
							if (0 <= nChPos && nChPos <= nChPosMax)
								{
								m_pFileIn->Seek(nChPos, CFile::begin);
								m_pFileIn->Read(&ch, 1);
								}
							else 
								// beginning or the end of the file
								ch = ' ';
							}
						if (isalnum(ch)) 
							break;
						nChPos = i + M;
						}
					if (t < 2)
						{
						m_pFileIn->Seek(nCurInPos, CFile::begin);
						i -= M_1;
						continue;
						}
					}

				// found one match
				bContinue = GetFlag(m_nFlagAll);
				m_pFileIn->Seek(nInPos, CFile::begin);
				if (bReplace)
					{
					if (OnPreReplace(m_pFileIn, nInPos, bContinue))
						{
						nResult++;
						m_pFileOut->Write(pBuffer + w, i - w);
						nOutPos = (LONG) m_pFileOut->GetPosition();
						m_pFileOut->Write(m_pReplace->GetData(), PtrToUint(m_pReplace->GetSize()));
						w = i + M;
						nCurOutPos = (LONG) m_pFileOut->GetPosition();
						m_pFileOut->Seek(nOutPos, CFile::begin);
						OnPostReplace(m_pFileOut, nOutPos);
						m_pFileOut->Seek(nCurOutPos, CFile::begin);
						}
					if (!bContinue)	
						m_pFileOut->Write(pBuffer + w, n - w);
					}
				else
					{
					if (OnFound(m_pFileIn, nInPos, bContinue))
						nResult++;
					}
				m_pFileIn->Seek(nCurInPos, CFile::begin);
				}
			}
		CloseFiles();
		}
	catch (...)
		{
		if (!GetFlag(m_nFlagUseCFile))
			{
			CString sTempFileName = m_pFileOut->GetFilePath();
			m_pFileIn->Abort();
			m_pFileOut->Abort();
			if (bReplace && GetFlag(m_nFlagOverwrite) && !sTempFileName.IsEmpty())
				::DeleteFile(sTempFileName);
			}
		m_pFileIn = NULL;
		m_pFileOut = NULL;
		delete[] pBuffer;
		pBuffer = NULL;
		throw;
		}

	// Clean up
	delete[] pBuffer;
	pBuffer = NULL;
	m_pFileIn = NULL;
	m_pFileOut = NULL;
	return nResult;
	}

void COXFileChanger::OpenFiles()
	// --- In      :
	// --- Out     : 
	// --- Returns :
	// --- Effect  : prepare m_pFileIn and m_pFileOut
	{
	if (!GetFlag(m_nFlagUseCFile))
		{
		m_pFileIn->Open(m_sInputFileName, CFile::modeRead | CFile::shareDenyWrite);
		if (GetFlag(m_nFlagReplace))
			{
			CString sOutputFileName = (GetFlag(m_nFlagOverwrite) ? 
				GetUniqueTempName() : m_sOutputFileName);
			ASSERT(!sOutputFileName.IsEmpty());
			m_pFileOut->Open(sOutputFileName, CFile::modeReadWrite | CFile::modeCreate);
			}
		}

	m_pFileIn->SeekToBegin();
	}

void COXFileChanger::CloseFiles()
	// --- In      :
	// --- Out     : 
	// --- Returns :
	// --- Effect  : close m_pFileIn and m_pFileOut
	{
	if (!GetFlag(m_nFlagUseCFile))
		{
		m_pFileIn->Close();
		if (GetFlag(m_nFlagReplace))
			{
			CString sTempFileName = m_pFileOut->GetFilePath();
			m_pFileOut->Close();
			if (GetFlag(m_nFlagOverwrite) && (!::DeleteFile(m_sInputFileName) || 
				!::MoveFile(sTempFileName, m_sInputFileName)))
				CFileException::ThrowOsError((LONG)::GetLastError(), m_sInputFileName);
			}
		}
	}

void COXFileChanger::CopyTextToByteArray(LPCTSTR pszText, CByteArray* pBuffer)
	// --- In      : pszText, the text to copy
	//				 pBuffer, the byte array to store result
	// --- Out     : 
	// --- Returns :
	// --- Effect  : copy a string (ANSI or UNICODE) into a byte array
	//				 Unicode characters are converted to ansi characters
	{
#ifdef _UNICODE
	size_t nLen = 0;

	UTBStr::wcstombs(&nLen, NULL, 0, pszText, 0);
	if (nLen < 0)
	{
		TRACE0("COXFileChanger::CopyTextToByteArray : Conversion of Unicode to Ansi failed, using empty string\n");
		nLen = 0;
	}
	pBuffer->SetSize(nLen);
	char* pByte = (LPSTR)pBuffer->GetData();
	// ... Convert Unicode to ansi
	size_t t;
	UTBStr::wcstombs(&t, pByte, nLen, pszText, nLen);
#else
	int nLen = _tcslen(pszText);
	pBuffer->SetSize(nLen);
	char* pByte = (LPSTR)pBuffer->GetData();
	// ... Just copy Ansi sting
	::CopyMemory(pByte, pszText, nLen);
#endif // _UNICODE
	}

void COXFileChanger::ReverseCase(CString& sText)
	// --- In      : sText, the text to reverse case
	// --- Out     : 
	// --- Returns : a string with reversed case for each character
	// --- Effect  : reverse every character's case in a string
	{
	TCHAR ch;
	for (int i = 0; i < sText.GetLength(); i++)
		{
		ch = sText[i];
		sText.SetAt(i, (_istlower(ch) ? (TCHAR)_totupper(ch) : (TCHAR)_totlower(ch)));
		}
	}

CString COXFileChanger::GetUniqueTempName()
	{
	TCHAR szTempPath[MAX_PATH];
	TCHAR szTempFile[MAX_PATH];
	VERIFY(::GetTempPath(MAX_PATH, szTempPath));
	VERIFY(::GetTempFileName(szTempPath, m_sTempFilePrefix, 0, szTempFile));
	return CString(szTempFile);
	}

// end of OXFileChanger.cpp
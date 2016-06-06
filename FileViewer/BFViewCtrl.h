/*
 
 Name : CBFViewCtrl ( BigFile View Controller )
 File : BFViewerCtrl.h

 Author    : Mathias Svensson  ( ms@result42.com )

 Copyright : Mathias Svensson , 2004

 Version   : v1.1

 Rights of Use : You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) when your product is released in binary form. 
				 You are allowed to modify the source code in any way you want except you cannot modify the copyright details at the top of each module. 
                 If you want to distribute source code with your application, then you are only allowed to distribute versions released by the author.

 Disclaimer : There is no warranty. So use this code at your own risk. 

 Restriction : Since this controller is almost an application in it self, 
               so you are not allowed to use this controller to create a 
			   program where this controller is the main purpose of the 
			   application (eg a text file view program). 
			   A modified and highly improved version of it can be used 
			   in freeware application as long as the author is mention in 
			   the credits in the program (About dialog) and is informed about it. 
			   All other kind of software is allowed to use it as they like.




 BUGS / Limits
 ----
 *  None Unicode Build is not able to view Unicode files correctly


*/

#if !defined(AFX_ZTEXTVIEWER_H__C1058F7C_880B_46E0_BE14_EDDEFF4C4765__INCLUDED_)
#define AFX_ZTEXTVIEWER_H__C1058F7C_880B_46E0_BE14_EDDEFF4C4765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _LPCBYTE_DEFINED
#define _LPCBYTE_DEFINED
typedef const BYTE *LPCBYTE;
#endif

#include "MemMap2.h"
#include "AutoFont.h"

#define MAX_CLIPBOARD_SIZE  10*1024*1024   // 15 MB

#define MAX_TMPSTRING_SIZE   2048

#define MAXLINES 400	// Max Lines to "cache"

#define TEXTTYPE_AUTO		0
#define TEXTTYPE_ASCII		1
#define TEXTTYPE_UNICODE	2
#define TEXTTYPE_UNICODEBE	3
#define TEXTTYPE_UTF8		4
#define TEXTTYPE_BINARY		5
#define TEXTTYPE_HEX		6


// Parent Notify Message

#define BFVN_SELECTION_CHANGED	 2 // Selected is changed
#define BFVN_OPEN                3 // A File/buffert has been opened
#define BFVN_RELOADED            4 // FileSize can be updated. 

/// forward decleration
class CDataHandler;


class CDataBuffert
{
public:
	CDataBuffert()
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
        m_pBuffert = NULL;
		m_dwMapSize = si.dwAllocationGranularity * 4; //64k*4 = 256k
		m_nBuffertOffset = 0;
		m_dwBuffertSize = 0;
		m_nPreByte = 0;
		m_strFilename = _T("");
	}
	~CDataBuffert()
	{
		Close();
	}
	DWORD GetMappingChunkSize()
	{
		return m_dwMapSize; 
	}

	LPCBYTE  GetBuffer()		// return pointer to buffer
	{
		if( m_nBuffertOffset == 0 )
			return m_pBuffert + m_nPreByte;
		else
			return m_pBuffert;
	}

	LPCBYTE GetBufferEnd( int nByteSize = 1 )
	{
		if( m_dwBuffertSize == 0 )
			return m_pBuffert;
		return (m_pBuffert + m_dwBuffertSize) - nByteSize;
	}

	DWORD    GetBufferSize()    // return size of current buffer chunk
	{
		return m_dwBuffertSize;
	}

	__int64  GetDataLength()   // Get Total Data Length
	{
		return m_nTotalSize;
	}

	__int64  GetDataOffset()   // return the offset off for Buffert to the real buffert.
	{
		return m_nBuffertOffset;
	}

	__int64 GetPos( LPCBYTE pData )
	{
		if( pData >= m_pBuffert && pData <= (m_pBuffert+m_dwBuffertSize ) )
		{
			return (pData - m_pBuffert)+1;
		}
		return -1;
	}

	__int64 GetAbsolutePos( LPCBYTE pData )
	{
		__int64 nPos = GetPos( pData );
		if( nPos == -1 )
			return -1;

        nPos += m_nBuffertOffset;
		return nPos;
	}

	// Is nPos within the range of the map area
	BOOL IsInRange( INT64 nPos )
	{
		if( nPos >= m_nBuffertOffset && nPos <= (m_nBuffertOffset + m_dwBuffertSize) )
			return TRUE;

		return FALSE;
	}

	LPCBYTE GetPointerToPos( INT64 nPos)
	{
		if( nPos < m_nBuffertOffset )
			return NULL;

		if( nPos > (m_nBuffertOffset + m_dwBuffertSize) )
			return NULL;

		__int64 nRelativePosition = (nPos-1) - m_nBuffertOffset;
		return m_pBuffert + nRelativePosition;
	}

	BOOL IsEOB( LPCBYTE pData )
	{
		__int64 nPos = GetAbsolutePos( pData );
		if( nPos == -1 )
			return -1;

		if( nPos >= m_dwBuffertSize )
			return TRUE;
		return FALSE;

	}
	BOOL IsEOF( LPCBYTE pData )
	{
		__int64 nPos = GetAbsolutePos( pData );
		if( nPos == -1 )
			return -1;

		if( nPos == m_nTotalSize )
			return TRUE;
		return FALSE;	
			
	}
	BOOL ReMapLast()
	{
		// if filesize is less then m_dwMapSize then map whole file
		if( m_nTotalSize < m_dwMapSize )
			return ReMap( 0 );

		__int64 nOffset = m_nTotalSize - (m_dwMapSize/2);

		return ReMap( nOffset );
	}
	BOOL ReMap( INT64 nOffset )
	{
		if( m_Memfile.IsOpen() )
		{
			m_pBuffert =(LPCBYTE) m_Memfile.Remap( nOffset , m_dwMapSize );
			if( m_pBuffert )
			{
				m_nBuffertOffset = m_Memfile.GetOffset();
				m_dwBuffertSize = m_Memfile.GetFileMapLength();
				return TRUE;
			}
		}
		return FALSE;
	}
	void SetPrefixSize( USHORT nBytes )
	{
		m_nPreByte = nBytes;
	}

	BOOL     OpenFile( LPCTSTR strFilename );
	void     Close()
	{
		if( m_Memfile.IsOpen() )
		{
			m_Memfile.Close();
			m_Memfile.UnMap();
			m_dwBuffertSize = 0;
			m_nBuffertOffset = 0;
			m_nTotalSize = 0;
		}
		m_strFilename = _T("");
	}
	CString GetFileName() { return m_strFilename; }
	BOOL Reload( INT64 nOffset );

	BOOL IsFileChanged()
	{
		HANDLE hFile = m_Memfile.GetFileHandle();
		DWORD dwFileSizeHigh=0;
		DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);

		if (dwFileSizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR ) 
			return FALSE;

		ULARGE_INTEGER ul;
		ul.LowPart = dwFileSizeLow;
		ul.HighPart = dwFileSizeHigh;
        if( (__int64)ul.QuadPart > GetDataLength() )
			return TRUE;

		return FALSE;

	}

protected:
	CString      m_strFilename;

	DWORD		 m_dwMapSize;
	CMemMapFile2 m_Memfile;

	USHORT		m_nPreByte;	// bytes to skip from begining of buffert.. eeg. Unicode files have a Unicode tag at the begining

	LPCBYTE     m_pBuffert;
	DWORD       m_dwBuffertSize;
	__int64     m_nBuffertOffset;

	__int64     m_nTotalSize;

};

/////////////////////////////////////////////////////////////////////////////
// CZTextViewer window


/*

 dwLength does not need to be the same as pEnd - pStart, is can be less.
 like ASCII Parse the CRLF is not count in dwLength..  
 since we can't use pStart + dwLength to find out where to start to parse next line.
 we need to keep track of there this line ends. 
 
*/

#define LIF_FIRSTLINE		0x0001	// first line in file, not more data exists before this line
#define LIF_LASTLINE		0x0002	// last line in the file, no more data exists after this line

#define LIF_FIRSTLINE_CHUNK	0x0010  // First line in this chunk. if geting previuose line a remap is needed.
#define LIF_LASTLINE_CHUNK  0x0020
struct LineInfo
{
	LPCBYTE pStart;		// Pointer to the first byte in this line
	LPCBYTE pEnd;		// Pointer to the last byte in this line ( OR is it next byte after )
	DWORD  dwLength;    // length of this line in bytes 
	short  sFlags;
};



class CBFViewerCtrl : public CWnd
{
// Construction
public:	
			 CBFViewerCtrl();
	virtual ~CBFViewerCtrl();
		BOOL RegisterWindowClass();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZFileViewCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	
public:
	BOOL Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle );

	virtual BOOL OpenFile( CString &filename , short nMode = TEXTTYPE_AUTO , BOOL bRefresh = TRUE );

	CString GetFileName() { return m_strFilename; }

	CString GetViewModeAsString();
	int     GetViewMode()	{ return m_nViewMode;  	}

	// Change to a new View Mode
	BOOL    ChangeViewMode( short nNewMode );
	
	// is Autoreload active
	BOOL    IsAutoReload() { return m_bAutoReload; }


	// Set Font and Color
	void SetFont( CFont* pFont , BOOL bRedraw = TRUE );
	void SetFont( LPCTSTR strFontName , int nFontSize );
	void SetColor( DWORD crTextColor = -1 , DWORD crBGColor = -1 , DWORD crSelTextColor = -1 , DWORD crSelBGColor = -1 , BOOL bRedraw = TRUE );


	// Print.. 
	void    Print( CPrintDialog* pPrntDlg );

	// if nEnd == -1 then select until the end.
    BOOL    SetSelection( __int64 nStart , __int64 nEnd );

	// Get Total file size
	__int64 GetTotalSize() { return m_Buffert.GetDataLength(); }

	// Copy Selection to Clipboard or File..
	BOOL	DoCopy( BOOL bToFile = FALSE );

	// Get DataHandler
	CDataHandler* GetDataHandler() { return m_pDataHandler; }

    // if activated it will check if file is updated every (ms) and reload if needed.
	void  SetReloadChkTimer( DWORD ms );
	virtual void  ShowFontDlg();

	virtual void LineDown();
	virtual void LineUp();
	virtual void PageDown();
	virtual void PageUp();
	virtual void Top();
	virtual void Bottom();

// Implementation
protected:
	void CalcLineHeight();
		
	LRESULT SendNotifyToParent( int nMsg );
	void    UpdateScollbars();
	void    Draw(CDC *pDC);
	short   IdentifyFileType();
	BOOL    IsDataBinary( LPCBYTE pData , int iLength );
	BOOL    IsDataUTF8( LPCBYTE pData , int iLength );

	__forceinline BOOL IsSpace( int ch );
	__forceinline BOOL IsPrint( int ch );

	CDataHandler* CreateDataHandler( int nMode );
	
	BOOL PutIntoClipboardA(  const unsigned char* strText , DWORD len );
	BOOL PutIntoClipboardW( const WCHAR* strText , DWORD len );
	

	
private:
	CAutoFont	m_Font;
	BOOL		m_bMemDC;
	int			m_nFontSize;
	long		m_lRowHeight;
	
	int			m_nRowsPerWheelNotch;
	BOOL		m_bOnlyWheelScrollIfMouseInsideWindow;

	LPVOID		m_pFileData;
	CString		m_strFilename;
	short		m_nViewMode;
	
	BOOL		m_bTrackSelection;

	CDataBuffert m_Buffert;
	CDataHandler* m_pDataHandler;

	BOOL		 m_bAutoReload;

	CString      m_strTextSaveClip;

	// Generated message map functions
protected:
	//{{AFX_MSG(CZFileViewCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
protected:
	virtual void PreSubclassWindow();
};



//============================================================================

class CDataHandler
{
public:
	CDataHandler( CWnd *pWnd , CDataBuffert* pBuffert );

	void CalcLinesAPage()
	{
		if( m_pWnd )
		{
			CRect rcClient;
			m_pWnd->GetClientRect( rcClient );
			m_nLinesAPage = (rcClient.Height() / m_dwLineHeight )+1;
		}
		else
			m_nLinesAPage = 0;
	}
	void Set_LineHeight( DWORD dwLineHeight ) { m_dwLineHeight = dwLineHeight; CalcLinesAPage(); }
	__int64 GetCurrentTopPos()
	{
		if( m_nTopLine >= m_nStartIndex && m_nTopLine <= m_nEndIndex )
		{
			return m_pBuffert->GetAbsolutePos( m_LineCache[ m_nTopLine ].pStart );
		}
		return -1;			
	}
	BOOL Reload();

	void OnSize()
	{
		CalcLinesAPage();
		Prepare( NULL , m_nTopLine , m_nLinesAPage );

	}

	// virtual function - that MUST be overriden
	virtual void Draw( CDC *pDC , DWORD nCol ) = 0;
	virtual CString GetName() = 0;


	// Prepare..  makes sure that all the lines from nStartIndex to nLines are acceccable. will remap and parse data if necceccary
	// if pLineStart is NULL it will use nStartIndex to get the start position. else it will start with pLineStart
	//
	virtual int  Prepare(  LPCBYTE pLineStart , int nStartIndex , int nLines );
	// bBreak is just the so we dont get in an unlimited loop..
	virtual int  PrepareReverse(  LPCBYTE pLineStart , int nStartIndex , int nLines , BOOL bBreak = FALSE );

	// Data View Position
	virtual BOOL MoveToTop();
	virtual BOOL MoveToBottom();
	virtual BOOL MovePageUp();
	virtual BOOL MovePageDown();
	virtual BOOL MoveLineUp();
	virtual BOOL MoveLineDown();
	virtual BOOL MoveToScrollPos( DWORD pos );
	virtual BOOL GoToPos( __int64 nPos );

	virtual BOOL	SetScrollRange();

			BOOL	UpdateScrollInfo();
			DWORD	GetLinesInIndex() { return m_nEndIndex - m_nStartIndex; }// m_nLineCacheCount; }
			DWORD   GetLinesAPage() { return m_nLinesAPage; }
			void    StartTracking( CPoint & pt );
			void    StopTracking( CPoint & pt );
			void    Track( CPoint & pt );

			BOOL	lineHasSelection( LPCBYTE pLineStart , LPCBYTE pLineEnd );
			BOOL	hasSelection();
			void	SetFont( CAutoFont* pFont ) { m_pFont = pFont; }

			BOOL		SetSelection( __int64 nStart , __int64 nEnd );
			DWORD		GetSelection( LPCBYTE* pStart );
			BOOL		GetSelection( LPCTSTR strFilename );
			__int64		GetSelectionSize() { return m_nSelectionEnd - m_nSelectionStart; }
			BOOL		IsSelectionInBuffer()
			{
				if( m_pBuffert->IsInRange( m_nSelectionStart ) && m_pBuffert->IsInRange( m_nSelectionEnd) )
					return TRUE;
				return FALSE;
			}
			void		TempCleanup() { delete[] m_pTempBuffert; m_pTempBuffert=NULL; }

			//	Set Color, -1 == do not change
			void		SetColor( DWORD crTextColor = -1 , DWORD crBGColor = -1 , DWORD crSelTextColor = -1 , DWORD crSelBGColor = -1 );
			// 1 = TextColor , 2 = BGColor , 3 = SelTextColor , 4 = SelBGColor
			DWORD		GetColor( int nColor );

protected:

			LPCBYTE GetDataPostion( CPoint& pt );					// Find where pt is pointing to

	virtual LPCBYTE  GetHDataPosition( int rowidx , int x ) = 0;	// Find what col/char 'x' is pointing at
	virtual short	 ByteSize() = 0;								// How big is one char. ( eg in unicode 1 char is 2 bytes.. )
	virtual LPCBYTE	 FindLineStart( LPCBYTE pData ) = 0;			// Search backward to find the begining of the line

	virtual LPCBYTE  ParseLine( LPCBYTE pByteData , DWORD& dwLength ) = 0;  // Parse Line. and pByteData is the begining of the line, return the pointer to begining of line
	virtual LPCBYTE	 ParseLineReverse( LPCBYTE pData , DWORD& dwLength )=0; // Parse Line Reverse, pData is the END of the line. return the pointer to begining of line

    		int		 ParseData( LPCBYTE pData , DWORD nStartIndex , DWORD nLines );  // ParseData from pData , and insert at nStartIndex in Cache and parse nLines numbers of lines.
			BOOL	 ParseDataReverse( LPCBYTE pData , DWORD dwLines );              // same as above but reverse.

			// Remap Mempory... 
			LPCBYTE  ReMapNext( LPCBYTE pNewRemapPos , DWORD nRetIndex);  // returns Data pointer to the line where nRetIndex was pointing to.. return NULL if failed.
			LPCBYTE	 ReMapPrev( LPCBYTE pBottomPos );

protected:
	void    ClearCache();
	BOOL    RearrangeCache_Front( );	        // Move m_nTopLine pos to begining of the array
	BOOL    RearrangeCache_Back( int nCount );	// Move m_nTopLine pos 'nCount' numbers of step down the array
	DWORD	GetMaxCacheCount() { return MAXLINES; }
	int		FindLine( LPVOID pData );

	// Used for copy to Clipboard and file
	BOOL    CopyToBuffert( LPCBYTE pBuffert , LPCTSTR strFilename , DWORD nRWChunkSize  , __int64 nOffset , __int64 nSize );		
	BOOL    CopyToFile( LPCTSTR strToFilename ,LPCTSTR strFromFilename , DWORD nRWChunkSize  , __int64 nOffset , __int64 nSize );		

	CAutoFont*		m_pFont;

	DWORD           m_crTextColor;			// Font Color
	DWORD           m_crBGColor;			// Background Color

	DWORD			m_crSelectionTextColor; // Selection Text Color
	DWORD			m_crSelectionBGColor;   // Selection Background Color

	CWnd*			m_pWnd;
	CDataBuffert*	m_pBuffert;

	long		m_nLinesAPage;			    // How many lines we can show on one page
    
	LineInfo	m_LineCache[ MAXLINES ];	// Array with cached row info
	DWORD		m_nEndIndex;				// Lines cached
	DWORD       m_nStartIndex;				// Start index in the cache. if we started with looking at the file from the bottom.. 
											// The linecache will only be fill up in the end.. and startindex will be the index where linecache starts 
	DWORD		m_nTopLine;					// The topline to show. has this cache index value

	DWORD       m_dwVertScrollRes;			// Vertical Scroll Resolution, eg if 100, then the there is 100 step and every step is 1%. 1000,, every step is 0.1%
	
	DWORD	m_dwLineHeight;

	__int64 m_nSelectionStart;
	__int64 m_nSelectionEnd;

	LPCBYTE m_pTempBuffert;

	int     m_nLeftMargin;  // How may pixel from the edge to draw..  drawing directly beside edge looks not good.
	int     m_nSnap;		// use when selecting text. m_nSnap is a margin to snap the 

};

class CAsciiHandler : public CDataHandler
{
public:
	CAsciiHandler( CWnd *pWnd , CDataBuffert* pBuffert  ) : CDataHandler( pWnd , pBuffert )
	{
	}
	void Draw( CDC *pDC , DWORD nCol );

	CString GetName(){ return _T("ASCII"); }
protected:
	short ByteSize()
	{
		return sizeof(UCHAR);
	}

	virtual LPCBYTE GetHDataPosition( int rowidx ,  int x );

	virtual LPCBYTE  ParseLine( LPCBYTE pLineBegin , DWORD& dwLength );
	virtual LPCBYTE	 ParseLineReverse( LPCBYTE pData , DWORD& dwLength ); //, LPCBYTE* pLineEnd  );
	virtual LPCBYTE	 FindLineStart( LPCBYTE pData );

	WCHAR	m_strTempBuffer[MAX_TMPSTRING_SIZE];
	
};

class CUnicodeHandler : public CDataHandler
{
public:
	CUnicodeHandler( CWnd *pWnd , CDataBuffert* pBuffert ) : CDataHandler( pWnd , pBuffert )
	{
	}
	void Draw( CDC *pDC , DWORD nCol );
	CString GetName(){ return _T("Unicode"); }

protected:
	short ByteSize()
	{
		return sizeof(WCHAR);
	}
	LPCBYTE   GetHDataPosition( int rowidx ,  int x );

	LPCBYTE  ParseLine( LPCBYTE pLineBegin , DWORD& dwLength );
	LPCBYTE	 ParseLineReverse( LPCBYTE pData , DWORD& dwLength ); //, LPCBYTE* pLineEnd  );
	LPCBYTE	 FindLineStart( LPCBYTE pData );


	CHAR	m_strTempBuffer[MAX_TMPSTRING_SIZE];

};

class CBinaryHandler : public CDataHandler
{
public:
	CBinaryHandler( CWnd *pWnd, CDataBuffert* pBuffert ) : CDataHandler( pWnd , pBuffert)
	{
		m_LineWidth	= 80;
	}

	void Draw( CDC *pDC , DWORD nCol );

	CString GetName(){ return _T("Binary"); }
		
protected:
	short ByteSize()
	{
		return sizeof(BYTE);
	}
	LPCBYTE  GetHDataPosition( int rowidx ,  int x );
	LPCBYTE  ParseLine( LPCBYTE pLineBegin , DWORD& dwLength );
	LPCBYTE	 ParseLineReverse( LPCBYTE pData , DWORD& dwLength ); 
	LPCBYTE  FindLineStart( LPCBYTE pData );

	WCHAR	m_strTempBuffer[MAX_TMPSTRING_SIZE];
	short	m_LineWidth; // in chars; default 80

};





/*
class CUTF8Handler : public CDataHandler
{
public:
	void Draw( CDC *pDC , DWORD nCol );

protected:

};



class CHexHandler : public CBinaryHandler // Difference only on Drawing. I Think
{
public:
	void Draw( CDC *pDC , DWORD nCol );

		// Data View Position
	
};
*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZTEXTVIEWER_H__C1058F7C_880B_46E0_BE14_EDDEFF4C4765__INCLUDED_)

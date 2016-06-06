// RDragDropFiles.h: interface for the RDragDropFiles class.
//
/*
	Author:			Richard Chambers, rickcham@mindspring.com

	Copyright:		Richard Chambers, 2002

	Version:		1.0

	Rights of Use:	This source code may be used by anyone for any reason
					so long as the copyright information is maintained.

					There is no warranty, express or implied, as to fitness
					of purpose nor is there any other warranty, expressed
					or implied, as to correct functioning.

					This software depends on the existance of two functions
					in the Windows API as well as the correct function of
					those functions.  See the Requires section below.

	Requires:		RDragDropFiles.cpp containing the class declaration.
					DragQueryFile function from the Windows API.
					Client must provide a correct HDROP handle to the constructor.
					Client must initialize file drop using DragAcceptFiles.

	Assumptions:	If debug is turned on, the macro _DEBUG is defined and ASSERT
					is enabled.  If debug is turned off, the macro _DEBUG is
					undefined and ASSERT is disabled.

					The DragQueryFile Windows API function remains unchanged
					across Windows versions (Windows 95/98/ME/NT/2000).

	Edit History:

 */

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RDRAGDROPFILES_H__D7D93FAA_AB71_4356_BF4F_884F871493D0__INCLUDED_)
#define AFX_RDRAGDROPFILES_H__D7D93FAA_AB71_4356_BF4F_884F871493D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace rjc {
	class RDragDropFiles  
	{
	public:
		/*
			We force the user to provide an HDROP handle for the constructor.

			We overload the operator () to provide a method for checking if there
			are any more files in the list when iterating over the list using
			sNextFile.  You could also do a check on the IsEmpty method of CString
			after calling one of sFirstFile, sNextFile, or sCurrFile.

			The IsEmpty method is used to determine if there are any files in
			the file list.  It is NOT used to see if we have reached the end
			of the file list when iterating using the sNextFile method.

			The Reset method resets the file list position indicator to the
			beginning of the list.  If you do a Reset and then a sNextFile,
			you will get the first file in the list as if you had used sFirstFile.

			The Clear method is used drop the association between the object and
			the HDROP handle provided.  This means that if you use the Clear
			method, you are responsible for file drag and drop cleanup.
			You must explicitly use the DragFinish function for this cleanup
			rather than letting the RDragDropFiles object do it for you.
		 */
		RDragDropFiles(HDROP hDrop);
		virtual ~RDragDropFiles();
		UINT uCount (void);
		CString & sFirstFile (CString & sBuff);
		CString & sNextFile (CString & sBuff);
		CString & sCurrFile (CString & sBuff);
		boolean operator () (void);
		boolean IsEmpty (void);
		void    Reset (void);
		void    Clear (void);

	protected:
		// we will not allow the compiler to create a
		// copy constructor or an assignment constructor.
		// Each object should be unique due to hDropFiles
		// being unique.  The destructor for the object
		// will release the drop file resources so there
		// must only be one unique object per hDropFiles.
		RDragDropFiles (RDragDropFiles &dr);
		RDragDropFiles & operator = (RDragDropFiles &dr);

	protected:
		UINT nFiles;        // number of files from hDropInfo
		UINT iPosition;     // current position in list of files
		HDROP hDropInfo;    // copy of HDROP from OnDropFiles

	};
};

#ifndef _DEBUG

//  debug versions of these are in RDragDropFiles.cpp
//  comments for these functions are also in RDragDropFiles.cpp
inline boolean rjc::RDragDropFiles::operator () () { return (hDropInfo && (iPosition < nFiles)); }
inline boolean rjc::RDragDropFiles::IsEmpty () { return (!hDropInfo || (nFiles < 1));}
inline void    rjc::RDragDropFiles::Reset () {iPosition = 0;}
inline UINT  rjc::RDragDropFiles::uCount () { return nFiles; }
inline void  rjc::RDragDropFiles::Clear () { hDropInfo = 0; nFiles = 0; }
#endif

#endif // !defined(AFX_RDRAGDROPFILES_H__D7D93FAA_AB71_4356_BF4F_884F871493D0__INCLUDED_)

// RDragDropFiles.cpp: implementation of the RDragDropFiles class.
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

	Requires:		RDragDropFiles.h containing the class declaration.
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

/*
	Description:

	This file implements the file drag and drop functionality
	by encapsulating the DragQueryFile function call for the
	Windows file drag and drop.

	This is not the same as the Windows OLE drag and drop.

	In order to activate a window as a drop target, you must
	insert a call to DragAcceptFiles into your OnCreate
	handler of your window.

	Then catch the WM_DROPFILES message (OnDropFiles method of
	a CView class using Visual C++ Class Wizard) and use this
	class in the method.

	void CScreen01View::OnDropFiles(HDROP hDropInfo) 
	{
		rjc::RDragDropFiles myFiles (hDropInfo);
		CString buf;

		while (myFiles ()) {
			myFiles.sNextFile (buf);
			...  do something with buf .....
		}
	}

 */

#include "stdafx.h"
//#include "screen01.h"
#include "RDragDropFiles.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace rjc;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RDragDropFiles::RDragDropFiles(HDROP hDrop)
	:hDropInfo (hDrop), nFiles (0), iPosition (0)
{
	ASSERT (hDropInfo);

	if (hDropInfo) {
		nFiles = DragQueryFile (hDropInfo, 0xFFFFFFFF, NULL, 0);
	}

	ASSERT (nFiles > 0);
}

RDragDropFiles::~RDragDropFiles()
{
	//  Release the drag and drop resources
	//  We check that hDropInfo is valid before
	//  doing the DragFinish in case the client has
	//  called the Clear method in order to prevent
	//  the use of DragFinish when the object goes out
	//  of scope.
	//  Reference counting is left as an exercise.
	if (hDropInfo) {
		DragFinish(hDropInfo);
	}
}

/*
	In the following three methods for iterating over the list
	of files, we require the client to provide us a CString
	for the buffer.  We require the client's CString so that
	we don't have to do funky pointers and stuff.  The client
	can provide us an auto object on the stack which will be
	cleaned up when it goes out of scope.

	We will also set the CString to Empty if there is not a file
	name to put into it.  That way the client can depend on the
	value of the CString to be logically correct.

	The client can either call the constructor and then start
	iterating using sNextFile or the client can use sFirstFile
	or Reset to start at the beginning of the list after iterating
	some of it then continue with sNextFile.

	iPosition, which is the file position in the list of files,
	is a 1 index rather an a 0 index.  However, DragQueryFile
	uses a 0 index so we subtract 1 from iPosition.  We are
	using 1 index in our class as it simplifies the code when
	mixing sFirstFile, sNextFile, and Reset.

	sFirstFile forces iPosition to 1 for the first file in
	the list.  sCurrFile forces iPosition to 1 if it is 0 so
	as to ensure the correct functioning of sNextFile if it is
	used after a sCurrFile.

	We use DragQueryFile to find the length of the pathname
	we want to get, set our CString length accordingly, and
	then copy the pathname of the file into our CString.  The
	pathname is a standard C string with an end of string
	terminator at the end for which we also have to account.

	When we do the CString.ReleaseBuffer, CString will recalc
	the length information based on the end of string terminator.
 */


CString & RDragDropFiles::sFirstFile (CString & sBuff)
{
	ASSERT (hDropInfo);

	iPosition = 1;
	if (iPosition <= nFiles) {
		UINT nLen = DragQueryFile (hDropInfo, iPosition - 1, NULL, 0);
		TCHAR *pzsFN = sBuff.GetBufferSetLength (nLen+2);
		DragQueryFile (hDropInfo, iPosition - 1, pzsFN, nLen+2);
		sBuff.ReleaseBuffer ();
	}
	else {
		sBuff.Empty ();
	}
	return sBuff;
}

CString & RDragDropFiles::sNextFile (CString & sBuff)
{
	ASSERT (hDropInfo);
	ASSERT (iPosition < nFiles);
	ASSERT (iPosition >= 0);

	if (iPosition < nFiles) {
		iPosition++;
		UINT nLen = DragQueryFile (hDropInfo, iPosition - 1, NULL, 0);
		TCHAR *pzsFN = sBuff.GetBufferSetLength (nLen+2);
		DragQueryFile (hDropInfo, iPosition - 1, pzsFN, nLen+2);
		sBuff.ReleaseBuffer ();
	}
	else {
		sBuff.Empty ();
	}
	return sBuff;
}


CString & RDragDropFiles::sCurrFile (CString & sBuff)
{
	ASSERT (hDropInfo);
	ASSERT (iPosition <= nFiles);

	if (iPosition < 1)
		iPosition = 1;

	if (iPosition <= nFiles) {
		UINT nLen = DragQueryFile (hDropInfo, iPosition - 1, NULL, 0);
		TCHAR *pzsFN = sBuff.GetBufferSetLength (nLen+2);
		DragQueryFile (hDropInfo, iPosition - 1, pzsFN, nLen+2);
		sBuff.ReleaseBuffer ();
	}
	else {
		sBuff.Empty ();
	}
	return sBuff;
}

#ifdef _DEBUG
// non-debug versions of these are inline in RDragDropFiles.h

// Any changes made to functionality should be reflected in the
// in-line versions as well.

// Need to do retesting in both debug and non-debug configurations.

/*
	uCount - provide a count of the files in the drop list.
 */
UINT RDragDropFiles::uCount ()
{
	ASSERT (hDropInfo);

	if (hDropInfo) {
		return nFiles;
	}
	else {
		return 0;
	}
}

/*
	function operator - returns whether there are any more
	files to obtain from the list of dropped files.

	iPosition == 0 if after Reset or newly constructed.
	iPosition == 1 if after sFirstFile or sCurrFile or first sNextFile (iPosition was 0)

	We are using increment before fetch in sNextFile so iPosition == nFiles on
	exit from last legal sNextFile.  iPostion should be less than nFiles if
	the next call to sNextFile will return an item from the file list.
 */
boolean RDragDropFiles::operator () ()
{
	ASSERT (hDropInfo);

	return (hDropInfo && (iPosition < nFiles));
}

/*
	IsEmpty - return whether the list of files is empty.
		An empty list is either hDropInfo is not a good
		item or the number of files being dropped is less
		than 1 (in other words zero).

	IsEmpty tells us if there are any files in the list and not
	whether iPosition points to a legal list entry.
 */
boolean RDragDropFiles::IsEmpty ()
{
	return (!hDropInfo || (nFiles < 1));
}

/*
	Reset - reset the position so that we can iterate over
		the list from the beginning.  Set iPosition to 0
		allowing sNextFile to work correctly if called
		after doing a Reset.
 */
void    RDragDropFiles::Reset ()
{
	ASSERT (hDropInfo);

	iPosition = 0;

	ASSERT (iPosition < nFiles);
}

/*
	Clear - clear the object's stored data.
		The purpose of clear is to object's data before
		its destructor is called in those cases where
		the client wants to use the hDropIno for something
		else.

		The main reason to use this method is to prevent
		the call to DragFinish(hDropInfo); when the object's
		destructor is called as it goes out of scope.
 */
void RDragDropFiles::Clear ()
{
	ASSERT (hDropInfo);

	hDropInfo = 0;
	nFiles = 0;
}

#endif

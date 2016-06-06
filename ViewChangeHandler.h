// ViewChangeHandler.h: interface for the CViewChangeHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWCHANGEHANDLER_H__540639BB_C37B_4BEB_B55E_26653377ECB2__INCLUDED_)
#define AFX_VIEWCHANGEHANDLER_H__540639BB_C37B_4BEB_B55E_26653377ECB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DIRCHANGES\DirectoryChanges.h"

class CViewChangeHandler : public CDirectoryChangeHandler  
{
public:
	CViewChangeHandler();
	virtual ~CViewChangeHandler();

};

#endif // !defined(AFX_VIEWCHANGEHANDLER_H__540639BB_C37B_4BEB_B55E_26653377ECB2__INCLUDED_)

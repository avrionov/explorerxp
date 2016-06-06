// Timer.cpp: implementation of the CTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Timer.h"
//#include "Globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimer::CTimer (const char *name)
{

	m_Name = name;
	m_StartTime = CTime::GetCurrentTime ();
	TRACE ("%s...\n", m_Name);

}

CTimer::~CTimer()
{

	m_EndTime = CTime::GetCurrentTime ();
	CTimeSpan span = m_EndTime - m_StartTime;
	TRACE ("%s time %02d:%02d:%02d\n", m_Name, span.GetHours (), span.GetMinutes (), span.GetSeconds ());

}

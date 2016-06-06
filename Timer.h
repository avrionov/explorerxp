// Timer.h: interface for the CTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER_H__278386F4_13AA_4359_BDFA_89769630EB5E__INCLUDED_)
#define AFX_TIMER_H__278386F4_13AA_4359_BDFA_89769630EB5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTimer  
{
public:
	CTimer (const char *name);
	virtual ~CTimer ();

	CTime m_StartTime;
	CTime m_EndTime;
	CString m_Name;

};

#endif // !defined(AFX_TIMER_H__278386F4_13AA_4359_BDFA_89769630EB5E__INCLUDED_)

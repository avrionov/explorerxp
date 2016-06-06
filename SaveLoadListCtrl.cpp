// SaveLoadListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "explorerxp.h"
#include "SaveLoadListCtrl.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveLoadListCtrl

CSaveLoadListCtrl::CSaveLoadListCtrl(const char *csSection)
{
	m_Name = csSection;
}

CSaveLoadListCtrl::~CSaveLoadListCtrl()
{
}

void CSaveLoadListCtrl::Load() 
{
	CString key="Path0";

	CString path = AfxGetApp()->GetProfileString(m_Name, key);
	
	while (!path.IsEmpty() )	
	{
		m_Paths.push_back (path);
		key.Format (_T("Path%d"), (int)m_Paths.size ());
		path = AfxGetApp()->GetProfileString(m_Name, key);
	}
	
}
	
void CSaveLoadListCtrl::Save ()
{

	///Sort the files by ID.
	 std::sort( m_Paths.begin(), m_Paths.end() );

	//Now, find the elements in the files vector that share the same ID,
	//and erase the ones with older time.
	loop:
    if ( m_Paths.size () > 0)
	{
		for( unsigned int i = 0;  i < m_Paths.size () -1; i++)
		{
			if (m_Paths[i].CompareNoCase (m_Paths[i+1]) == 0)
			{				
				m_Paths.erase(m_Paths.begin () + i+1 );
				goto loop;
			}
		}
	}

	CString key;

	for (unsigned int i = 0; i < m_Paths.size (); i++)
	{
		key.Format (_T("Path%d"), i);
		AfxGetApp()->WriteProfileString(m_Name, key, m_Paths[i]);
	}

	key.Format (_T("Path%d"), (int)m_Paths.size ());
	AfxGetApp()->WriteProfileString(m_Name, key, EMPTYSTR);
}

BEGIN_MESSAGE_MAP(CSaveLoadListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSaveLoadListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveLoadListCtrl message handlers

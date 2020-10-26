#include "stdafx.h"
#include "FontMapper.h"
#include "..\Globals.h"

///*-
// FUNCTION NAME:  IsFontSame
// 
// DESCRIPTION: Are the fonts same?
// 
// 
// PARAMETERS:
//             *fnt1
//                TYPE:             LOGFONT
//                MODE:             in
//                MECHANISM:        by value
//                DESCRIPTION:
//             *fnt2
//                TYPE:             LOGFONT
//                MODE:             in
//                MECHANISM:        by value
//                DESCRIPTION:
// 
// RETURN VALUE: BOOL
// 
// NOTES:
// 
//+*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL IsFontSame(LOGFONT *fnt1, LOGFONT *fnt2)
{
    if (memcmp(fnt1, fnt2, sizeof(LOGFONT) - LF_FACESIZE * sizeof(TCHAR)) != 0)
        return FALSE;
    return _tcscmp(fnt1->lfFaceName, fnt2->lfFaceName) == 0;
}

//default constructor
CFontMapper::CFontMapper()
{
}

//default destructor
CFontMapper::~CFontMapper()
{
    CFontListIt start, end, it;

    start = m_List.begin() ;   
    end = m_List.end() ;       
    for(it = start; it != end; ++it)
        DeleteObject(it->hFont);
}

///*-
// FUNCTION NAME:  CFontMapper::CreateFont
// 
// DESCRIPTION: Creates font
// 
// 
// PARAMETERS:
//             *fntFont
//                TYPE:             LOGFONT
//                MODE:             in
//                MECHANISM:        by reference
//                DESCRIPTION:
// 
// RETURN VALUE: HFONT
// 
// NOTES:
// 
//+*/

HFONT CFontMapper::CreateFont(LOGFONT *fntFont)
{	
    CFontListIt start, end, it;
    FontInfo    info;

    start = m_List.begin() ;   
    end = m_List.end() ;       
    for(it = start; it != end; ++it)
    {
        if (IsFontSame(&(it->fntFont), fntFont))
        {
            it->nCounter++;
            return it->hFont;
        }
    }
    info.fntFont = *fntFont;
    info.hFont = ::CreateFontIndirect(fntFont);
    if (!info.hFont)
        return NULL;
    info.nCounter = 1;
    m_List.push_back(info);
    return info.hFont;
}

///*-
// FUNCTION NAME:  CFontMapper::DeleteFont
// 
// DESCRIPTION: Deletes font
// 
// 
// PARAMETERS:
//             hFont
//                TYPE:             HFONT
//                MODE:             in
//                MECHANISM:        by value
//                DESCRIPTION:
// 
// RETURN VALUE: void
// 
// NOTES:
// 
//+*/

void CFontMapper::DeleteFont(HFONT hFont)
{
    CFontListIt start, end, it;

    start = m_List.begin() ;   
    end = m_List.end() ;       
    for(it = start; it != end; ++it)
    {
        if (it->hFont == hFont)
        {
            it->nCounter--;
            if (it->nCounter == 0)
            {
                DeleteObject(it->hFont);
                m_List.erase(it);
            }
            return;
        }
    }
    DeleteObject(hFont);
}


CFontMapper  FontMapper;     


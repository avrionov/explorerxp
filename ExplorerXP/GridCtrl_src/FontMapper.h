#ifndef __FONTMAPPER_H
#define __FONTMAPPER_H


#include <list>

struct FontInfo{
    LOGFONT fntFont;
    int     nCounter;
    HFONT   hFont;
};

typedef std::list<FontInfo> CFontList;
typedef CFontList::iterator CFontListIt;

class CFontMapper {
public:
            CFontMapper ( ) ;
    virtual ~CFontMapper ( ) ;

    virtual HFONT   CreateFont ( LOGFONT *fntFont ) ;
    virtual void    DeleteFont ( HFONT hFont ) ;

protected:
    CFontList   m_List;
};

BOOL IsFontSame ( LOGFONT *fnt1, LOGFONT *fnt2 ) ;


extern CFontMapper   FontMapper;

#endif

#ifndef __FILEINFO_CACHE_H__
#define __FILEINFO_CACHE_H__

#pragma once

typedef std::map <std::string, SHFILEINFO, std::less <std::string> > CShellInfoMap;

typedef CShellInfoMap::iterator icon_it;
typedef CShellInfoMap::value_type iconmap_value;

class CFileInfoCache  
{
public:
	CFileInfoCache();
	virtual ~CFileInfoCache();

    int GetFileIcon (const char *file_name);
    const char * GetFileType (const char *file_name);
    CShellInfoMap IconMap;
};

#endif // __FILEINFO_CACHE_H__

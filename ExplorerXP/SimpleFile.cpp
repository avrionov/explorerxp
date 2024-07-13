/* Copyright 2002-2021 Nikolay Avrionov. All Rights Reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#include "stdafx.h"
#include "simplefile.h"

CSimpleFile::CSimpleFile(const char *fileName, const char *mode)
{
	m_FileName = fileName;
	m_Mode = mode;
	m_File = fopen (fileName, mode);
	m_buf = NULL;
	m_CurPos = NULL;
	m_Length = NULL;		
}

CSimpleFile::~CSimpleFile(void)
{
	if (m_File)
		fclose (m_File);

	if (m_buf)
		delete m_buf;
}


size_t CSimpleFile::write(const void *buffer, size_t size, size_t count) 
{
	if (!m_File)
		return 0;

	return fwrite (buffer, size, count, m_File);
}

size_t CSimpleFile::read(void *buffer, size_t size, size_t count)
{
	if (!m_File)
		return 0;

	return fread (buffer, size, count, m_File);
}

size_t CSimpleFile::size ()
{
	if (!m_File)
		return 0;

	fseek (m_File, 0, SEEK_END);
	size_t pos = ftell (m_File);
	fseek(m_File, 0, SEEK_SET);

	return pos;
}

void  CSimpleFile::read_all () 
{
	m_Length = size();
	
	m_buf = new char [ m_Length+3];
	
	read (m_buf, size(), 1);

	m_CurPos = 0;	
	m_buf[m_Length] = 0;
	m_buf[m_Length+1] = 0;
	m_buf[m_Length+2] = 0;
}

char *CSimpleFile::read_line (int &len) 
{
	if (!m_buf)
		return NULL;

	if (m_CurPos >= m_Length)
		return NULL;

	size_t i =m_CurPos;

	while (m_buf[i] != '\n' && 
		   m_buf[i] != '\r' &&
		   m_buf[i] != 0)
			i++;
	
	len = static_cast<int>(i - m_CurPos);

again:
	if (m_buf[i] == '\n')
	{
		m_buf[i] = 0;
		i++;
		goto again;
	}
	
	if (m_buf[i] == '\r')
	{
		m_buf[i] = 0;
		i++;
		goto again;
	}
	
	char *ret = m_buf + m_CurPos;
	m_CurPos = i;
	return ret;
}

void CSimpleFile::write_line (char *line)
{
	if (!line)
		return;

	size_t len = strlen (line);

	write (line, len, 1);
}
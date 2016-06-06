/* Copyright 2002-2016 Nikolay Avrionov. All Rights Reserved.
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


#pragma once

class CSimpleFile
{
public:
	CSimpleFile(const char *fileName, const char *mode);
	~CSimpleFile(void);
	size_t write(const void *buffer, size_t size, size_t count);
	size_t read(void *buffer, size_t size, size_t count);	
	bool isValid () { return m_File != 0;}
	size_t size () ;
	void read_all ();
	char *read_line (int &len);
	void write_line (char *line);

protected:
	FILE*	m_File;
	CString m_FileName;
	CString m_Mode;
	char*  m_buf;
	size_t  m_CurPos;
	size_t	m_Length;		
};

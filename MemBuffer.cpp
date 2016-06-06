/*
 * Copyright (c) 2005 Citrix Systems, Inc.
 * All Rights Reserved Worldwide.
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO EXPERTCITY 
 * AND CONSTITUTES A VALUABLE TRADE SECRET.  Any unauthorized use,
 * reproduction, modification, or disclosure of this program is 
 * strictly prohibited.  Any use of this program by an authorized 
 * licensee is strictly subject to the terms and conditions, 
 * including confidentiality obligations, set forth in the applicable
 * License and Co-Branding Agreement between Expertcity, Inc. and 
 * the licensee.
 *
 * $Header: $
 * $NoKeywords: $
 */


#include "stdafx.h"
#include "MemBuffer.h"

#define RETURN_ON_ERROR(expr)                                           \
{                                                                       \
	if ((expr) != ECError::eNoError) 									\
		return expr;													\
	                                                                    \
}

// default constructor - does not allocate any memory
CMemBuffer::CMemBuffer() throw()
{
    _bufferEnd = 0;
    _position = 0;
    _dirty = false;
    _memsize = sizeof(unsigned int);
    ::memset(_dummy, 0, _memsize*sizeof(TCHAR));
    _memblock = _dummy;
}


// COnstructor that allocates memory
CMemBuffer::CMemBuffer(unsigned int size) throw() 
{
    _bufferEnd = 0;
    _position = 0;
    _dirty = false;
    _memsize = sizeof(unsigned int);
    ::memset(_dummy, 0, _memsize*sizeof(TCHAR));
    _memblock = _dummy;
    CMemBuffer::setSize(size);
}

// virtual destructor
CMemBuffer::~CMemBuffer() throw()
{
    if (_memblock != _dummy) 
    {
        delete [] _memblock;
    }   
}

ECResult CMemBuffer::setSize(unsigned int size) throw()
{
    if (size < 4) 
    {
        size = 4;
    }
    if (size == _memsize) 
    {
        return ECError::eNoError;
    }
    TCHAR* memblocknew = new TCHAR[size];

    if (memblocknew) 
    {
        if (size > _memsize) 
        {
            if (_dirty)
                ::memcpy((byte*)memblocknew, (byte*)_memblock, _memsize*sizeof(TCHAR));
            ::memset((byte*)(memblocknew + _memsize), 0, (size - _memsize)*sizeof(TCHAR));
        }
        else 
        {
            if (_dirty)
                ::memcpy((byte*)memblocknew, (byte*)_memblock, size*sizeof(TCHAR));
        }

        if (_memblock != _dummy) 
        {
            delete [] _memblock;
        }
        _memblock = memblocknew;
        _memsize = size;
        return ECError::eNoError;
    }
    else 
    {
        // Keep original memory
        return ECError::eNoMemory;
    }
}

// Assignment operator
CMemBuffer& CMemBuffer::operator= (const CMemBuffer& source)
{
    if (getSize() < source.bufferEnd())
        setSize(source.bufferEnd());
    
    ::memcpy((byte*)getCharPtr(), (byte*)source.getCharPtr(), source.bufferEnd()*sizeof(TCHAR));
    _bufferEnd = source.bufferEnd();
    _position = _bufferEnd;
    
    return *this;
}


// Clear the entire buffer
ECResult CMemBuffer::clear() throw() 
{
    ::memset((byte*)getCharPtr(), 0, getByteSize());
    _position = 0;
    _bufferEnd = 0;
    _dirty = false;
    return ECError::eNoError;
}

// Resets the cursor and buffer end to index 0;
// This is less processing intensive then calling clear();
void CMemBuffer::reset() throw() 
{
    _position = 0;
    _bufferEnd = 0;
}

// read one character from the buffer
ECResult CMemBuffer::read(TCHAR& chr) throw()
{

    if (_position >= bufferEnd())
        return ECError::eEnd;

    chr = _memblock[_position];
    _position++;
    return ECError::eNoError;

}

// Push the read cursor back by one. 
// Succeeds only if the character being unread is
// same as character that was most recently read.
ECResult CMemBuffer::unread(const TCHAR& ch) throw()
{
    if (_position == 0)
        return ECError::ePreconditionFailed;
    if (_memblock[_position - 1] == ch)
    {
        _position--;
        return ECError::eNoError;
    }
    return ECError::eBadParameter;
}

// Position the cursor at index provided in the argument
ECResult CMemBuffer::setCursor(unsigned int index) throw() 
{
    if ( CMemBuffer::getSize() < index )
        return ECError::eNoAccess;

    _position = index;
    return ECError::eNoError;
}

// Write one char to index pointed by write cursor
ECResult CMemBuffer::write (const TCHAR& chr)  throw()
{
    
    if ( CMemBuffer::getSize() <= _position )
        RETURN_ON_ERROR(grow());
    
    
    _memblock[_position] = chr;
    _position++;
    if (_position > _bufferEnd)
        _bufferEnd = _position;

    _dirty = true;
    
    return ECError::eNoError;
}

// Write one char to index pointed by write cursor
ECResult CMemBuffer::write (const CString& str) throw()
{
    return write(str.c_str(), str.numCharsUsed());
}

// write buffer of size numbytes to location pointed by write cursor
ECResult CMemBuffer::write (const TCHAR* buf, unsigned int numchars) throw() 
{
    
    if (buf == NULL)
        return ECError::eNullPointer;
    
    while (CMemBuffer::getSize() - _position < numchars)
        RETURN_ON_ERROR(grow());
    
    
    ::memcpy((byte*)(getCharPtr() + _position), (byte*)buf, numchars*sizeof(TCHAR));
    _position += numchars;
    if (_position > _bufferEnd)
        _bufferEnd = _position;

    _dirty = true;
    
    return ECError::eNoError;
}

// write buffer of size numbytes to index pointed by write cursor
ECResult CMemBuffer::write (const CMemBuffer& buffer) throw() 
{
    return write(buffer.getCharPtr(), buffer.bufferEnd());
}


// Find the index of last occurrence of given character.
// If character is not found returns end of buffer
unsigned int CMemBuffer::findLast (TCHAR ch) const throw() 
{
    TCHAR* charstrm = getCharPtr();

    if (_bufferEnd > 0)
    {
        int match = _bufferEnd - 1;
        
        do 
        {
            if (charstrm[match] == ch)
                return match;
            match--;
        } 
        while ( match  >= 0 );
    }
    return _bufferEnd;
}


// Expand the size of buffer
ECResult CMemBuffer::grow () throw()  
{
    return setSize( 2*CMemBuffer::getSize() );
}


// Read from file pointed by filePath and fil the buffer with the file contents
ECResult CMemBuffer::readFile(const CString& filePath) throw()
{
    ECFile file;
    int64 filesize = 0;
    
    RETURN_ON_UNEXPECTED(file.open(filePath, ECFile::eRead, ECFile::eOpenExisting));
    RETURN_ON_UNEXPECTED(file.getSize(filesize));
    
    int32 bytestoread = (int32)filesize;
    bool isEOF = false;
    unsigned int bytesread = 0;
    unsigned int sumbytesread = 0;

    RETURN_ON_UNEXPECTED(setSize((unsigned int)filesize));
    RETURN_ON_UNEXPECTED(clear());

    do {
        RETURN_ON_UNEXPECTED (file.read((sumbytesread + (byte*)_memblock), bytestoread, &isEOF, &bytesread));
        sumbytesread += bytesread;
        bytestoread -= bytesread;
    }
    while (sumbytesread < filesize);
    _bufferEnd = sumbytesread;
    _position = sumbytesread;
    

    /*TCHAR ch;
    RETURN_ON_UNEXPECTED (file.read(&ch, sizeof(TCHAR), &isEOF, &bytesread));


    while (!isEOF)
    {
        append(ch);
        RETURN_ON_UNEXPECTED (file.read(&ch, sizeof(TCHAR), &isEOF, &bytesread));
    }*/

    return file.close();
}

// Write buffer contents to File specified by filepath
// If the file already exists then it is truncated and its contents are
// overwritten by the buffer contents
ECResult CMemBuffer::writeToFile (const CString& filePath) throw()
{
    ECFile file;
    RETURN_ON_UNEXPECTED(file.open(filePath, ECFile::eWrite, ECFile::eCreateAlways));

    
    setCursor(0);

    int32 bytestowrite = bufferEnd()*sizeof(TCHAR);
    unsigned int byteswritten = 0;
    unsigned int sum = 0;

    do
    {
        RETURN_ON_UNEXPECTED (file.write((sum + (byte*)_memblock), bytestowrite, &byteswritten));
        bytestowrite -= byteswritten;
        sum += byteswritten;
    }
    while (bytestowrite > 0);
    
    /*
    TCHAR ch;
    while (read(ch) != ECError::eEnd)
    {
        RETURN_ON_UNEXPECTED (file.write(&ch, sizeof(TCHAR), &byteswritten));
    }*/

    return file.close();
}

ECResult CMemBuffer::append (const TCHAR& ch) throw()
{
    RETURN_ON_ERROR(setCursor(bufferEnd()));
    return write(ch);
}

// Append functions
ECResult CMemBuffer::append (const TCHAR* buf, unsigned int numchars) throw()
{
    RETURN_ON_ERROR(setCursor(bufferEnd()));
    return write(buf, numchars);
}

ECResult CMemBuffer::append (const CString& str) throw()
{
    RETURN_ON_ERROR(setCursor(bufferEnd()));
    return write(str);
}

ECResult CMemBuffer::append (const CMemBuffer& buffer) throw()
{
    RETURN_ON_ERROR(setCursor(bufferEnd()));
    return write(buffer.getCharPtr(), buffer.bufferEnd());
}

//----CAREFUL-------MEMORY INTENSIVE OPERATIONS-------------
// Prepend one char to this buffer
ECResult CMemBuffer::prepend (const TCHAR& ch) throw()
{
    return prepend(&ch, 1);
}

// Prepend given buffer to this buffer
ECResult CMemBuffer::prepend (const TCHAR* buf, unsigned int numchars) throw()
{
    TCHAR* newbuffer = new TCHAR[_memsize + numchars];

    if (newbuffer)
    {
        ::memcpy((byte*)newbuffer, (byte*) buf, numchars*sizeof(TCHAR));
        ::memcpy((byte*)(newbuffer + numchars), (byte*)_memblock, getByteSize());
        if (_memblock != _dummy)
            delete [] _memblock;
        _memblock = newbuffer;
        _memsize += numchars;
        _bufferEnd += numchars;
        return ECError::eNoError;
    }
    else
    {
        return ECError::eNoMemory;
    }
}

// Prepend given string to this buffer
ECResult CMemBuffer::prepend (const CString& str) throw()
{
    return prepend(str.c_str(), str.numCharsUsed());
}

// Prepend given buffer to this buffer
ECResult CMemBuffer::prepend (const CMemBuffer& buffer) throw()
{
    return prepend(buffer.getCharPtr(), buffer.bufferEnd());
}

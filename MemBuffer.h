

#ifndef _CMEMBUFFER_H_
#define _CMEMBUFFER_H_

#define ECResult int

struct ECError {
	enum ECErrorCodes {
		eNoError = 1, ///< No error, but not the same as "success". 
		eSomeError,            ///< Some error, no further information.
		eNullPointer,          ///< A pointer had the value zero (NULL).
		eBadHandle,            ///< A handle (platform resource) was invalid.
		eNoMemory,             ///< Memory allocation failed.
		eBadParameter,         ///< Some parameter in a function call was invalid.
		eNoAccess,             ///< Caller did not have access permission.
		eNotInitialized,       ///< Something is not initialized properly
		eNotFound,             ///< Some specifed object was not found
		eExists,               ///< Some specifed object already exists (file, or other unique object)
		eIOError,              ///< Some I/O operation failed
		eTooMany,              ///< Too many of some object have been created or used.
		eBusy,                 ///< Some operation/device is busy and can not respond
		eEnd,                  ///< Some endpoint has been reached (i.e. end of file)
		eDiskFull,             ///< The disk is full
		eBadIndex,		       ///< An index value is outside legal range
		eBadFormat,            ///< Bad format, e.g. "abc" when expecting a number.

		eUnexpected,           ///< Unexepected error 
		ePreconditionFailed,   ///< Precondition failed error
		eVerifyFailed,         ///< Verify failed error

		eErrorCount            ///< The number of the next possible error code...
	};
};

class CMemBuffer
{

protected:
    unsigned int _bufferEnd; // End of buffer - indicates the number of bytes written
    unsigned int _position; // points to one after the last position written
    TCHAR* _memblock;
    unsigned int _memsize;
    TCHAR _dummy[sizeof(unsigned int)];
    bool _dirty;

public:
    // default constructor - does not allocate any memory
    CMemBuffer () throw();

    // COnstructor that allocates memory
    CMemBuffer (unsigned int size) throw();

    // virtual destructor
    virtual ~CMemBuffer () throw();

    // Get the pointer to the character array of the buffer
    TCHAR* getCharPtr () const throw() { return (TCHAR*) _memblock; }

    // Get the size of the buffer
    unsigned int getSize () const throw() { return _memsize; }
    
    // Set the size of buffer to that specified in the argument
    ECResult setSize (unsigned int size) throw();

    // Return index of next location from the end of buffer
    unsigned int bufferEnd () const throw() { return _bufferEnd; }

    // Assignement operator
    CMemBuffer& operator= (const CMemBuffer& source);


public:
    // Clear the entire buffer
    ECResult clear () throw();

    void reset() throw();
    
    // read one character from the buffer
    ECResult read (TCHAR& chr) throw();

    // Unread the character that was previously read
    ECResult unread (const TCHAR& ch) throw();

    // Write one character at location pointed by write cursor
    ECResult write (const TCHAR& chr) throw();

    // Write given string at location pointed by write cursor
    ECResult write(const CString& str) throw();

    // write buffer numchars at location pointed by write cursor
    //ECResult write(const TCHAR* buf) throw() ;

    // write buffer of size numchars at location pointed by write cursor
    ECResult write(const TCHAR* buf, unsigned int numchars) throw() ;

    // write given buffer location pointed by write cursor
    ECResult write(const CMemBuffer& buffer) throw();
 
    // Append one char to the end of the buffer
    ECResult append (const TCHAR& ch) throw();

    // Append given buffer to end of this buffer
    ECResult append (const TCHAR* buf, unsigned int numchars) throw();

    // Append given buffer to end of this buffer
    //ECResult append (const TCHAR* buf) throw();

    // Append given string to end of this buffer
    ECResult append(const CString& str) throw();

    // Append the given buffer to this buffer
    ECResult append(const CMemBuffer& buffer) throw();

    // Prepend one char to this buffer
    ECResult prepend (const TCHAR& ch) throw();

    // Prepend given buffer to this buffer
    ECResult prepend (const TCHAR* buf, unsigned int numchars) throw();

    // Prepend given string to this buffer
    ECResult prepend (const CString& str) throw();

    // Prepend the given buffer to this buffer
    ECResult prepend(const CMemBuffer& buffer) throw();

    // Write buffer to File
    ECResult writeToFile (const CString& filepath) throw();

    // Set the write cursor to given index
    ECResult setCursor (unsigned int index) throw();

    // Find the index of last occurrence of given character
    unsigned int findLast (TCHAR ch) const throw();

    // Read the contents of the file and populate the buffer
    ECResult readFile(const CString& filePath) throw();

    // get size in bytes
    unsigned int getByteSize() const throw() { return _memsize*sizeof(TCHAR);  }
private:
    ECResult grow();

    

    // Copy constructor
    CMemBuffer(const CMemBuffer& source);
};


#endif //_CMEMBUFFER_H_
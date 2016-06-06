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


#ifndef __FAST_ARRAY__
#define __FAST_ARRAY__

#pragma once

template  <class T> class fast_array
{
public:	
// iterator

	fast_array (int initial = 1024, int step = 256)
	{
		_step = step;
		_type_size = sizeof (T);
		_buf =(T*) malloc (_type_size * initial);
		_size = 0;
		_capasity = initial;
	}

	~fast_array ()
	{
		free (_buf);
	}

	inline void clear () 
	{
		_size = 0;
	}

	inline T & operator [] (int pos) 
	{
		return _buf[pos];
	}

	size_t size () { return _size;}

	inline T & last ()
	{
		return _buf[_size];
	}

	inline T & back ()
	{
		return _buf[_size-1];
	}
	

	inline void add ()
	{
		_size ++;
		if (_size >= _capasity)
		{
			_capasity += _step;
			_buf =(T*)realloc (_buf, _capasity* _type_size);
		}

	}
	T* begin () {
		return (T*)_buf;
	}
	
	T* end () {
		return (T*)(_buf) + _size;
	}
protected:
	T* _buf;
	size_t _size;
	size_t _type_size;
	size_t _step;
	size_t _capasity;	
};

#endif
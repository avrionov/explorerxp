#ifndef __QUICK_ALLOC__
#define __QUICK_ALLOC__

#include <assert.h>

template <class T>
class quick_alloc
{

	public:
	quick_alloc (int size)
		:_pos(0),
		_size(size),
		_mem(NULL)
		{
			assert (_size > 0);
			_mem = static_cast <T*> (operator new (_size * sizeof (T)));
			assert (_mem != NULL);
		};

	~quick_alloc ()
	{
		if (_mem)
			operator delete (_mem);
	};

	T *alloc () { return &_mem[_pos++];};

protected:
	T *_mem;
	int _pos;
	int _size;
};
#endif
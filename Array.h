// Array.h

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "Object.h"
#include <string.h> // memset

/*
==============================================================================

    Array  

    * simple interface for managing array length. good for situations where
      you will need to use an array memory several times, but can't 
      anticipate the exact size

==============================================================================
*/


template<typename type>
struct Array : public Object_t
{
    type * data;

    Array() : data(0), _size(0)
    {
        init();
    }
    
    void init( size_t sz =256 ) 
    {
        if ( sz > _size ) 
        {
            _size = sz;

            if ( data ) 
            {
                delete ( data );
            }

            data = (type *) new type[ _size ];
        }
    }

    // start, reset, init == all the same thing
    void reset( size_t sz =256 ) 
    {
        init(sz);
    }

    void start( size_t sz =256 ) 
    {
        reset(sz);
    }

    ~Array()
    {
        if ( data ) 
        {
            delete data;
        }
    }

    size_t size() { return _size; }

	void zero_out( void );

protected:

    size_t _size;                       // count of how many user added elts

};  

/*
====================
 Array::zero_out
====================
*/
template <typename type>
void Array<type>::zero_out( void ) 
{
	size_t byte_sz = sizeof( type ) * _size;

	if ( byte_sz > 0 ) 
    {
		memset( data, 0, byte_sz );
	}
}




#endif // __ARRAY_H__

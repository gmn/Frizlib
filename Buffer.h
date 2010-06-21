// Buffer.h

#ifndef __BUFFER_H__
#define __BUFFER_H__


#include "Object.h"

#include <string.h>


#define BUFFER_DEFAULT_ELEMENTS 1024


// method to delete pointer data is only conditionally included
// stupid that I have to do it this way
template<typename type>
void delete_contents( type * pStart, const size_t how_many ) ;


/*
==============================================================================

    Buffer      - (like an array, but with more features, ie. copy_in arrays)

    * mechanisms to add single-elt or array data
    * will grow dynamically.
    * late initialisation

==============================================================================
*/
template <typename type>
class Buffer : public Object_t
{

protected:

    type *                  free_p;        // pointer to next free elt
    size_t                  _byte_size;        // byte amount held

    typedef void (*deleteFunc_t)( type *, const size_t );
    deleteFunc_t deleteFunc;

public:

    type *                  data;        // master data pointer

    void init ( size_t sz =BUFFER_DEFAULT_ELEMENTS ) ;

    Buffer( void ) : free_p(0),_byte_size(0),deleteFunc(0),data(0) { }
    //Buffer( bool b ) : free_p(0),_byte_size(0),data(0) { setDeletable( b ); }
    Buffer( deleteFunc_t D ) : free_p(0),_byte_size(0),data(0) { setDeletable( D ); }

    ~Buffer( void ) 
    {
        destroy();
    }

    // add -- single elt 
    void add ( type t ) ;

    void push( type t ) { add ( t ); }

    // add -- multiple elt array
    void copy_in( const type *, size_t );

    // removes one, however doesn't return. not all templated types return well
    void pop();

    void zero_out( void ) ;

    void reset( void ) 
    {
        zero_out();
    }

    void release()
    {
        destroy();
    }

    void fast_reset( void ) { free_p = data; }

    // # of elts in use
    size_t length( void ) const { return (size_t)(free_p-data); }

    size_t size() const { return length(); }

    // # of bytes alloc'd internally
    size_t size_allocd( void ) const { return _byte_size; }

    void set_by_ref( Buffer<type> const& obj ) ;

    type operator[]( size_t index ) 
    {
        return data[ index ];
    }

    bool isstarted() { return data != 0; }

    void removeIndex( size_t index ) ;

    // also deletes *data and resets it 
    void destroy();

/*
    void setDeletable( bool tf ) { 
        deleteFunc = ( tf ) ? &(delete_contents<type>) : 0;
    }
*/
    void setDeletable( deleteFunc_t D ) { deleteFunc = D; }
};

//// Methods

template<typename type>
void Buffer<type>::init ( size_t sz ) 
{
    if ( data )
    {
        if ( sz > length() )
        {
            destroy();

            _byte_size = sz * sizeof(type);

            data = new type [ _byte_size ];

            free_p = data;
        }
        else 
        {
            if ( deleteFunc )
                deleteFunc( data, length() );

            zero_out();
        }
    }
    else
    {
        _byte_size = sz * sizeof(type);

        data = new type [ _byte_size ];

        free_p = data;
    }
}

template <typename type>
void Buffer<type>::add ( type t ) 
{
    if ( !data )
        init();

    if ( (length()+1)*sizeof(type) > _byte_size ) 
    {
        size_t newsz = _byte_size << 1;

        type *tmp = (type*) new type[ newsz ];

        memcpy( tmp, data, _byte_size );

        _byte_size = newsz;

        int free_p_ofst = free_p - data;

        delete ( data );

        data = tmp;

        free_p = data + free_p_ofst;
    }

    *free_p++ = t;
}

// copy in an array
template <typename type>
void Buffer<type>::copy_in( const type * new_array, size_t elts ) 
{
    size_t needed = ( length() + elts ) * sizeof(type);

    if ( !data )
    {
        init( elts );
    }

    if ( needed > _byte_size ) 
    {
        size_t newsz = _byte_size << 1;
        while ( newsz < needed ) { 
            newsz <<= 1; 
        } 

        type *tmp = (type*) new type[ newsz ];

        memcpy( tmp, data, _byte_size );

        _byte_size = newsz;

        int free_p_ofst = free_p - data;

        delete( data );

        data = tmp;

        free_p = data + free_p_ofst;
    }

    //memcpy( free_p, new_array, elts * sizeof(data) );

    for ( size_t i = 0; i < elts ; i++ ) 
    {
        free_p[ i ] = new_array[ i ];
    }

    free_p += elts;
}

// pops one off end, but does NOT return it; difficult with templates.
template <typename type>
inline void Buffer<type>::pop( void )
{
    if ( data && free_p > data )
    {
        if ( deleteFunc )
        {
            deleteFunc( free_p, 1 );
        }

        *free_p-- = 0;
    }
}


template <typename type>
inline void Buffer<type>::zero_out( void ) 
{
    if ( data ) 
    {
        memset( data, 0, _byte_size );
    }

    free_p = data;
}


template <typename type>
void Buffer<type>::set_by_ref( Buffer<type> const & obj ) 
{
    size_t elts = obj.length();

    init( elts );

    copy_in( obj.data, elts  );
}


template <typename type>
void Buffer<type>::removeIndex( size_t index ) 
{
    if ( !data || index >= length() || length() == 0 )
        return;

    type * src_p = data + index + 1;

    type * dest_p = data + index;

    size_t elts = length() - (src_p - data) ;

    memmove( dest_p, src_p, sizeof(type) * elts );

    --free_p;
}

template <typename type>
void Buffer<type>::destroy()
{
    if ( deleteFunc )
    {
        deleteFunc( data, length() );
    }

    if ( data )
    {
        delete data;
        data = 0;
        _byte_size = 0;
        free_p = data;
    }
}


// call delete on each element of an array of specified length

// templated function instead of class method because we can not be sure
//  which type the class will be instantiated with. this way we only call it
//  when we have pointer types, and setDeletable is called.
template<typename type>
void delete_contents( type * pStart, const size_t how_many ) 
{
    if ( !pStart )
        return;

    for ( size_t i = 0; i < how_many; i++ ) 
    {
        if ( pStart[i] )
        {
            delete pStart[i];

            pStart[i] = 0;
        }
#ifdef _DEBUG
        pStart[i] = (type) 0xcdcdcdcd;
#endif
    }
}





/*
==============================================================================

    function: buffer_delete

    takes a double-pointer to a templated Buffer type, and calls delete
    on all its members, deletes the data array, and then deletes the main 
    buffer and setting its pointer to NULL. 
==============================================================================
*/
template<typename type>
inline void buffer_delete( Buffer<type> ** buffer_pp ) 
{
    if ( !buffer_pp || !(*buffer_pp) )
        return;

    Buffer<type> & buffer = **buffer_pp;
    
    if ( !buffer.isstarted() )
        return;

    buffer.destroy();

    delete *buffer_pp;

    *buffer_pp = 0;
}


#endif // __LIB_BUFFER_H__


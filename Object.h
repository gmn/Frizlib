#ifndef __OBJECT_H__
#define __OBJECT_H__



#if 0

void * V_Malloc( size_t );
void V_Free( void * );
int  V_isStarted( void );

/*
====================
 Allocator_t
====================
*/
class Allocator_t {
public:
	static void * operator new( size_t size ) { return V_Malloc( size ); }
	static void operator delete( void *ptr ) { V_Free( ptr ); }
	static void * operator new[]( size_t bytes ) { return V_Malloc( bytes ); }
	static void operator delete[]( void *ptr ) { V_Free( ptr ); }
};
#endif


class Allocator_t 
{
};

class Object_t : public Allocator_t
{
};


#endif // __OBJECT_H__

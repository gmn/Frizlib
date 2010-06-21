

#include <stdio.h>

class s 
{
public:
    int i;
    float f;
    double d;
    virtual float method() { return f * i; }
    s() : i(1), f(2.f), d(3.0) {}
    ~s() {
        printf( "in destructor()\n" );
    }
};

#include "../Buffer.h"

int main() 
{
/*
    //having the class attach a delete method WONT work

    Buffer<s *> a( true );
    a.add( new s() );
    a.add( new s() );
*/

    // but, doing it ourselves with the exact same templated function does ?

    Buffer<s *> b;
    b.add( new s() );
    b.add( new s() );
    delete_contents<s *>( b.data, 2 );

    // we could attach it ourselves, but that sort of defeats the purpose.
    // I imagine that if you broke out the chain of command far enough, 
    //  for instance, having a class method() that calls and external function
    //  that calls a class, that calls a function, that attaches the delete
    //  function, that it may work, although it might be much easier to look up
    //  the C++ type info information.  I'm sure there's a way in to this 
    //  somewhere.

    printf( "\n" );

    Buffer<s *> c;
    c.setDeletable( &delete_contents<s *> );
    c.add( new s() );
    c.add( new s() );
    c.add( new s() );

    // the preferred invokation
    Buffer<s *> d ( &delete_contents<s *> );
    d.add( new s() );
    d.add( new s() );
}

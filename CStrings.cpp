// CStrings.cpp


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void safe_strncpy( char * str_to, const char * str_from, size_t size ) 
{
    if ( size < 2 )
        return;
    strncpy( str_to, str_from, size-1 );
    str_to[ size - 1 ] = '\0';
}


/*
================================================================

    Assert()

    helper Assert that is turned off for production code

================================================================
*/
void __nocall_Assert( int die_if_false, const char *exp, const char *file, int line )
{
    if ( !die_if_false ) 
    {
        fprintf( stderr, "Assert failed with expression: \"%s\" at: %s, line: %d \n", exp, file, line );
        fflush( stderr );

#ifdef _WIN
        // breaks in VS.NET debugger
        __asm {
            int 0x3;
        }
#endif 
        exit( -1 );
    }
}




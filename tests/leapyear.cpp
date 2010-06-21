
#include "../Date.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i = 1970;

    while ( i < 2100 )
    {
        // print all of them
        // printf( "%d %s\n", i, is_leap_year( i ) ? "yes" : "no" );

        if ( is_leap_year( i ) ) {
            printf( "%d, ", i );
            fflush( stdout );
        }

        ++i;
    }

    printf ( "\n\n" );
}

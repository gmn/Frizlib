
#include "../Date.h"

#include <stdio.h>
#include <stdlib.h>

char * test( long _utc_sec )
{
    static char _utc[6];
    char sign = ( _utc_sec < 0 ) ? '-' : '+' ;
    long hours = labs(_utc_sec) / 3600;
    long frac = labs(_utc_sec) % 3600;
    frac /= 60;
    sprintf( _utc, "%1c%02ld%02ld", sign, hours, frac );

    return _utc;
}

int main()
{
    Date date;

    printf( "%s\n", date.datetime() );

    printf( "%s\n", date.utc_code() );
    printf( "%ld\n", date.utc_sec() );

    printf( "%s\n", date.datetime_full() );

    printf( "%s\n", date.formatted() );

    printf( "%s %s\n", date.formatted_full(), date.dstString() );

    printf( "test UTCs: %s ", test( 49500 ) );
    printf( "%s \n", test( -30600 ) );


    // duplicate it, to test our ability to build correct unix time
    Date t( date.datetime(), date.utc_sec() ); 

    printf( "%s %s %s\n", t.formatted_full(), t.dstString(), t.utc_code() );

    printf( "date.unix() : %ld\n   t.unix() : %ld\n", date.unix(), t.unix() );
    printf( "diff = %ld\n", t.unix() - date.unix() );

    // works
    Date x( "2009-02-13 23:31:30", 0 ); // 1234567890 day
    Date y( "2001-09-09 01:46:40", 0 ); // 1000000000 day
    printf( "%ld\n%ld\n", x.unix(), y.unix() );
}

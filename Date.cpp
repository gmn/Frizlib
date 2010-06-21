// Date.cpp

//#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Date.h"
#include "CStrings.h"


static int mdays[ 12 ] = { 31, 28, 31, 30, 31, 30, 
                           31, 31, 30, 31, 30, 31 };

int leapyears[] = { 1972, 1976, 1980, 1984, 1988, 1992, 1996, 2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036, 2040, 2044, 2048, 2052, 2056, 2060, 2064, 2068, 2072, 2076, 2080, 2084, 2088, 2092, 2096 };

const size_t totalLeap = ( sizeof( leapyears ) / sizeof( leapyears[0] ) );


void TimeInterval::set( const char * buf )
{
    sscanf( buf, "%04d-%02d-%02d %02d:%02d:%02d", &year, &mon, &day, &hour, &min, &sec );
}

struct tm TimeInterval::struct_tm( void )
{
    struct tm s_tm;
    memset( &s_tm, 0, sizeof(struct tm) );
    s_tm.tm_year = year;
    s_tm.tm_mon = mon;
    s_tm.tm_mday = day;
    s_tm.tm_hour = hour;
    s_tm.tm_min = min;
    s_tm.tm_sec = sec;
    return s_tm;
}

// assumes UTC-0
time_t TimeInterval::toUnix( const char * buf )
{
    if ( buf )
        set( buf );

    time_t total = 0;

    // year
    total += ( year - 1970 ) * 31536000;

    // leap days
    int leap_days = howManyLeapFrom1970( year - 1 ); // start w/ last year 

    // in case we are in a leap year 
    if ( is_leap_year_any( year ) && mon > 2 )
        ++leap_days;

    total += leap_days * 86400;

    // mon
    int m = mon - 1;                    // start w/ last month
    while ( --m >= 0 )                  // decrement once to index array
    {
        total += mdays[ m ] * 86400;
    }
    
    // day
    total += (day-1) * 86400;

    // hour
    total += hour * 3600;
    // min
    total += min * 60;
    // sec
    total += sec;

    return total;
}


void TimeInterval::modify( int seconds )
{
/*
    int sign = ( seconds < 0 ) ? -1 : 1;
    int s = abs( seconds );
    
    int y = s / 31536000;
    s -= y * 31536000;
    int m = 
*/
}


//============================================================================= 

// _utc_sec must be already set 
void Date::calcUTC()
{
    // example conversions
    // 49500 --> +1345
    // -30600 --> -0830

    char sign = ( _utc_sec < 0 ) ? '-' : '+' ;
    long hours = labs(_utc_sec) / 3600;
    long frac = labs(_utc_sec) % 3600;
    frac /= 60;
    sprintf( _utc, "%1c%02ld%02ld", sign, hours, frac );
}

const char * Date::setNow() 
{
    struct tm p;
    _unix = time(0);
    localtime_r( &_unix, &p );

    // datetime
    sprintf( _datetime, "%04d-%02d-%02d %02d:%02d:%02d", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday + 1, p.tm_hour, p.tm_min, p.tm_sec );

    // utc
    _utc_sec = p.tm_gmtoff;
    strcpy( _utc_code, p.tm_zone );
    calcUTC();

    // dst
    dst = p.tm_isdst;

    // datetime full
    sprintf( _full, "%s %s", _datetime, _utc );

    return const_cast<const char *>( _datetime );
}



TimeInterval Date::getInterval()
{
    struct tm p;
    localtime_r( &_unix, &p );
    TimeInterval ti( &p );
    return ti;
}




const char * Date::formatted() const
{
    return const_cast<const char *>( _full ); 
}

const char * Date::formatted_full() const
{
    return const_cast<const char *>( _full ); 
}

const char * Date::dstString() const
{
    if ( dst ) 
        return "Daylight Savings Time";
    else
        return "";  
}

void Date::setFromUnix( time_t u ) 
{
    struct tm p;
    _unix = u;
    localtime_r( &_unix, &p );

    sprintf( _datetime, "%04d-%02d-%02d %02d:%02d:%02d", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday + 1, p.tm_hour, p.tm_min, p.tm_sec );

    // utc
    _utc_sec = p.tm_gmtoff;
    strcpy( _utc_code, p.tm_zone );
    calcUTC();

    // dst
    dst = p.tm_isdst;

    // datetime full
    sprintf( _full, "%s %s", _datetime, _utc );
}


void Date::setFromDatetime( const char *_dstr, long _utc_in )
{
    /* sanity check  */
    {
        Assert( _dstr != 0 );

        if ( !_dstr )
            return;

        Assert( _dstr[0] != 0 );
    }

    safe_strncpy( _datetime, _dstr, 20 );   // datetime
    _utc_sec = _utc_in;                     // utc_sec
    calcUTC();                              // utc: eg. "-0500"

    // unix
    TimeInterval ti_date( _dstr );

    time_t unix_wo_utc = ti_date.toUnix();

    // FIXME: this is a bug. I am having a difficult time understanding UTC
    if ( _utc_sec < 0 ) 
    {
        unix_wo_utc -= 86400;
    }
    _unix = unix_wo_utc - _utc_sec;         // for unix epoch to be correct 

    //ti_date.modify( -_utc_sec );
    //_unix = ti_date.toUnix();

    // utc code: eg. "CDT"
    struct tm s_tm;
    localtime_r( &_unix, &s_tm ); 
    strcpy( _utc_code, s_tm.tm_zone );

    // dst
    dst = s_tm.tm_isdst;

    // datetime full
    sprintf( _full, "%s %s", _datetime, _utc );
}

/*
    struct tm s_tm = t.struct_tm();
    s_tm.tm_gmtoff = _utc;
    // fill in other fields
    _unix = mktime( &s_tm );
    localtime_r( &_unix, &s_tm ); 
*/


void Date::setFromFields( long u, int y, int mon, int d, int h, int m, int s )
{
    
}

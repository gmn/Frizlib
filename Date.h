// Date.h
#ifndef __DATE_H__
#define __DATE_H__

#include <time.h>

#include "Object.h"


#if 0
           int tm_sec;     /* seconds (0 - 60) */
           int tm_min;     /* minutes (0 - 59) */
           int tm_hour;    /* hours (0 - 23) */
           int tm_mday;    /* day of month (1 - 31) */
           int tm_mon;     /* month of year (0 - 11) */
           int tm_year;    /* year - 1900 */
           int tm_wday;    /* day of week (Sunday = 0) */
           int tm_yday;    /* day of year (0 - 365) */
           int tm_isdst;   /* is summer time in effect? */
           char *tm_zone;  /* abbreviation of timezone name */
           long tm_gmtoff; /* offset from UTC in seconds */
#endif


/* 

Date are stored in the 20 char SQL datetime format and Unix-sec from 1970 format

Regular time representation given in TimeInterval.

*/

extern int leapyears[];
extern const size_t totalLeap;


inline bool is_leap_year_any( int year ) 
{
    if ( (year % 400) == 0 )
        return true;
    else if ( (year % 100) == 0 )
        return false;
    else if ( (year % 4) == 0 )
        return true;
    return false;
}

// checks a static list of leap years through 2099
inline bool is_leap_year( int year ) 
{
    for ( size_t i = 0; i < totalLeap; i++ )
    {
        if ( leapyears[ i ] == year )
            return true;
    }

    return false;
}

inline int howManyLeapFrom1970( int year )
{
    if ( year <= 1971 )
        return 0;

    int total = 0;

    do 
    {
        if ( year > 2099 )
        {
            if ( is_leap_year_any( year ) ) 
            {
                ++total;
            }
        }
        else
        {
            if ( is_leap_year( year ) )
            {
                ++total;
            }
        }
    }
    while ( --year >= 1970 );

    return total;
}

//=============================================================================
//
//  TimeInterval
//
//=============================================================================
class TimeInterval
{
public:

    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;

    TimeInterval() : year(0),mon(0),day(0),hour(0),min(0),sec(0) {}
    TimeInterval( struct tm * time ) { set( time ); }
    TimeInterval( const char * str ) { set( str ); }

    void set( struct tm * );
    void set( const char * );
    
    struct tm struct_tm( void ); // export a partially filled-in struct tm

    time_t toUnix( const char * =0 ); // convert to unix from sql-datetime

    void modify( int seconds );
};

inline void TimeInterval::set( struct tm * time ) 
{
    year = time->tm_year;
    mon = time->tm_mon;
    day = time->tm_mday;
    hour = time->tm_hour;
    min = time->tm_min;
    sec = time->tm_sec;
}


//=============================================================================
//
//  Date
//
//=============================================================================
class Date : public Object_t
{

protected:

    char _datetime[ 20 ];
    time_t _unix;

    long _utc_sec;
    char _utc[6];        // -0500
    char _utc_code[6];   // CST, CDT
    void calcUTC();

    int  dst;            // 1 or 0

    char _full[ 30 ];
    
    // lazy creation
    char *_fmt;
    char *_ffmt;

public:
    
    Date() : _fmt(0), _ffmt(0) { setNow(); }
    Date( const char *d, long u =0) : _fmt(0),_ffmt(0) { setFromDatetime(d, u);}
    ~Date() {};

    const char * setNow( void );
    TimeInterval getInterval( void );

    const char * datetime() const ;
    const char * datetime_full() const ;
    const char * formatted() const ;
    const char * formatted_full() const ;

    long         utc_sec() const { return _utc_sec; }
    const char * utc_code();
    int is_dst() const { return dst; }
    const char * dstString() const;

    void setFromUnix( time_t );
    void setFromDatetime( const char *, long );
    void setFromFields( long =0, int =0, int =0, int =0, int =0, int =0, int =0 );

    time_t unix() const { return _unix; }
};


inline const char * Date::utc_code() 
{
    return const_cast<const char *>( _utc_code );
}

inline const char * Date::datetime() const 
{     
    return const_cast<const char *>( _datetime ); 
}

inline const char * Date::datetime_full() const 
{
    return const_cast<const char *>( _full ); 
}



#endif /* ! __DATE_H__ */



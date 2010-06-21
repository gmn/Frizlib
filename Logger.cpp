// Logger.cpp

#include <stdio.h>
#include <math.h>

#include "Logger.h"

#include "CStrings.h"


#define PREFIX_SIZE 64

/********************************************************************
 *
 * logger
 *
 ********************************************************************/

Logger::Logger() 
{
    prefix[0] = 0;
}

Logger::Logger( const char *_prefix ) 
{
    memset( prefix, 0, PREFIX_SIZE );
    safe_strncpy( prefix, _prefix, PREFIX_SIZE );
}

Logger::~Logger() 
{
    std::vector<log_entry *>::iterator it = entries.begin();
    for ( ; it != entries.end(); it++ ) 
    {
        delete( *it );
    }
}

void Logger::setPrefix( const char * _prefix )
{
    memset( prefix, 0, PREFIX_SIZE );
    safe_strncpy( prefix, _prefix, PREFIX_SIZE );
}

void Logger::msg( const char *_msg ) 
{
    if ( !_msg ) 
        return;

    if ( prefix[0] )
        entries.push_back( new log_entry( prefix, _msg ) );
    else
        entries.push_back( new log_entry( _msg ) );
}

void Logger::msg( const char *_pref, const char *_msg ) 
{
    if ( !_msg || !_pref ) 
        return;

    safe_strncpy( prefix, _pref, PREFIX_SIZE );

    entries.push_back( new log_entry( prefix, _msg ) );
}

void Logger::print( int fd ) 
{
    
    std::vector<log_entry *>::iterator it = entries.begin();
    int num = 1;

    char fmt[30];
    float ent_sz = (float) entries.size();
    if ( ent_sz < 10.f ) ent_sz = 10.f;
    int tens = (int) log10f( ent_sz ); // implicit floor()
    if ( tens < 1 ) tens = 1;
    ++tens;

    for (  ; it != entries.end(); it++ ) 
    {
        if ( (*it)->pref[0] ) 
        {
            sprintf( fmt, "%%0%dd: %%s: %%s\n", tens );
            printf( fmt, num++, (*it)->pref, (*it)->msg );
        }
        else
        {
            sprintf( fmt, "%%0%dd: %%s\n", tens );
            printf( fmt, num++, (*it)->msg );
        }
    }
}

void Logger::write( const char *path ) 
{
    if ( !path ) {
        msg( "Logger error: no path supplied to write()" );
        return;
    }
    FILE *fp = fopen( path, "w" );
    if ( !fp ) {
        char buf[200];
        sprintf( buf, "Logger error: couldn't open \"%s\" for writing", path );
        msg( buf );
        return;
    }
    
    // FIXME: not finished
    fclose( fp );
}



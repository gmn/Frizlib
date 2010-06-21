// Logger.h
#ifndef __LOGGER__
#define __LOGGER__

#include <string.h>
#include <vector>

#include "CStrings.h"

#define PREFIX_SIZE 64

class Logger 
{
protected:
    char prefix[PREFIX_SIZE];
    
    struct log_entry 
    {
        char * msg;
        char pref[ PREFIX_SIZE ];
        size_t len;

        log_entry() : msg(0),len(0) { 
            pref[0] = '\0';
        }

        log_entry( const char *_msg ) 
        {
            len = strlen( _msg ) + 1;
            if ( len == 1 ) { len = 0; return; }
            msg = new char [ len ];
            safe_strncpy( msg, _msg, len );
        }   

        log_entry( const char *_pref, const char *_msg ) 
        {
            safe_strncpy( pref, _pref, PREFIX_SIZE );

            len = strlen( _msg ) + 1;
            if ( len == 1 ) { len = 0; return; }
            msg = new char [ len ];
            safe_strncpy( msg, _msg, len );
        }

        ~log_entry()
        {
            if ( msg ) 
                delete ( msg );
            msg = 0;
        }
    };

    std::vector<log_entry *> entries;

public:
    Logger();
    Logger( const char * );
    ~Logger();

    void msg ( const char * );
    void msg ( const char *, const char * );

    void print( int =0 );
    
    void write( const char *path );
    
    void setPrefix( const char * );
};

#endif /* __LOGGER__ */

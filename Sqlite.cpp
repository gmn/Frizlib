// Sqlite.cpp


#include "Sqlite.h"

#include "CStrings.h"



/********************************************************************
 *
 * DBValue
 *
 ********************************************************************/

DBValue::DBValue( int i ) : ival(i), name(0) 
{
    sval = new char[50];
    sprintf( sval, "%i", ival );
    fval = (float) ival;
}

DBValue::DBValue( float f ) : fval(f), name(0) 
{
    sval = new char[50];
    sprintf( sval, "%f", fval );
    ival = (int) fval;
}

DBValue::DBValue( const char * str ) : name(0) 
{
    size_t len = strlen( str ) + 1;
    sval = new char[ len ];
    memset( sval, 0, len );
    strcpy( sval, str );
    ival = atoi( str );
    fval = atof( str );
}

DBValue::DBValue( const char * _name, const char * str ) 
{
    size_t len = strlen( str ) + 1;
    sval = new char[ len ];
    memset( sval, 0, len );
    strcpy( sval, str );
    ival = atoi( str );
    fval = atof( str );

    if ( !name ) 
        return;

    len = strlen( _name ) + 1;
    name = new char[ len ];
    memset( name, 0, len );
    strcpy( name, _name );
}

DBValue::~DBValue() 
{
    if ( sval ) {
        delete( sval );
        sval = 0;
    }

    if ( name ) {
        delete( name );
        name = 0;
    }
}



/********************************************************************
 *
 * DBRow
 *
 ********************************************************************/

DBRow::~DBRow() 
{
    std::vector<DBValue *>::iterator it = values.begin();
    for ( ; it != values.end(); it++ ) 
    {
        delete ( *it );
    }
}

void DBRow::addVal( const char * str ) 
{
    DBValue * v = new DBValue( str );
    values.push_back( v );
}

DBValue * DBRow::getField( size_t f ) 
{
    if ( f >= values.size() )
        return 0;

    return values[f];
}



/********************************************************************
 *
 * DBResult
 *
 ********************************************************************/

DBResult::~DBResult() 
{
    std::vector<DBRow *>::iterator it = rows.begin();
    for ( ; it != rows.end(); it++ ) 
    {
        delete ( *it );
    }

    std::vector<char *>::iterator cit = col_names.begin();
    for ( ; cit != col_names.end(); cit++ ) 
    {
        delete ( *cit );
    }
}

DBRow * DBResult::getRow( size_t r )
{
    if ( 0 == rows.size() || r >= rows.size() ) 
        return 0;
    
    return rows[ r ];
}

bool DBResult::is_first_row( int in =-1 )
{
    if ( in == 1 || in == 0 )
    {
        first_row = (in == 1) ? true : false;
    }

    return first_row;
}

void DBResult::addColName ( const char * c_str )
{
    int len = strlen( c_str ) + 1;
    char * p = new char[ len ];
    memset( p, 0, len );
    safe_strncpy( p, c_str, len );
    
    col_names.push_back( p );
}

const char * DBResult::colName( size_t j ) 
{
    if ( j >= col_names.size() || col_names.size() == 0 )
        return 0;

    return const_cast<const char *>( col_names[j] );
}

const char * DBResult::getColNames()
{
    buffer.reset();

    size_t n = 0;

    std::vector<char *>::iterator cit = col_names.begin();
    for ( ; cit != col_names.end(); cit++ ) 
    {
        buffer.copy_in( *cit, strlen(*cit) );

        if ( n++ < col_names.size() - 1 )
            buffer.add( ' ' );
    }

    buffer.add( '\0' );

    return buffer.data;
}

/********************************************************************
 *
 * DBManager
 *
 ********************************************************************/

const unsigned int DBManager::max_name_sz;

DBManager::DBManager() : defDBName(DEFAULT_DB_NAME) 
{
    memset( db_name, 0, max_name_sz );
}

DBManager::~DBManager() 
{
    if ( 0 == results.size() )
        return;

    std::vector<DBResult *>::iterator it = results.begin();
    for ( ; it != results.end(); it++ ) 
    {
        delete ( *it );
    }
}

// passive name setup. if a name is already there, skip it
void DBManager::setup_db_name( const char * db_name_in ) 
{
    if ( db_name[0] != '\0' )
        return;

    if ( db_name_in ) 
    {
        safe_strncpy( db_name, db_name_in, max_name_sz );
    } 
    else 
    {
        strcpy( db_name, defDBName );
    } 
}

void DBManager::set_db( const char * db_name_in ) 
{
    if ( !db_name_in )
        return;

    safe_strncpy( db_name, db_name_in, max_name_sz );
}

void DBManager::reopen( void ) 
{
    close_db();

    if ( db_name[0] == 0 ) 
    {
        setup_db_name(0);
    }

    open_db();
}

DBResult * DBManager::newDBResult() 
{
    DBResult * r = new DBResult;
    results.push_back( r ); // save for clean up later
    return r;
}

//call this when you want. or it will do it automatically
void DBManager::clean_up() 
{
    if ( 0 == results.size() )
        return;

    std::vector<DBResult *>::iterator it = results.begin();
    for ( ; it != results.end(); it++ ) 
    {
        delete ( *it );
    }
    
    results.clear();
}



/*******************************************************************
 *
 * SqliteDBManager
 *
 *******************************************************************/

SqliteDBManager::SqliteDBManager() : db(0) , logger_p(0), mngr_name("SqliteDBManager")
{
}

SqliteDBManager::SqliteDBManager( Logger * L ) : db(0) , mngr_name("SqliteDBManager")
{
    setLogger( L );
}

SqliteDBManager::SqliteDBManager( const char *db_to_load ) : logger_p(0), mngr_name("SqliteDBManager")
{
    open_db( db_to_load );
}

SqliteDBManager::SqliteDBManager( const char *db_to_load, Logger *L ) : mngr_name("SqliteDBManager")
{
    open_db( db_to_load );
    setLogger( L );
}

SqliteDBManager::~SqliteDBManager() 
{
    if ( db ) 
        sqlite3_close( db );
}

void SqliteDBManager::setLogger( Logger *L )
{
    if ( !L ) {
        logger_p = 0;
        return;
    }

    L->setPrefix( mngr_name );
    logger_p = L;
}

void SqliteDBManager::setLoggerPrefix() 
{
    if ( logger_p )
        logger_p->setPrefix( mngr_name );
}

void SqliteDBManager::msg ( const char *_msg ) 
{
    if ( logger_p ) 
    {
        logger_p->msg( _msg );
    }
}

//
int SqliteDBManager::open_db( const char *db_name_in ) 
{
    char buf[1000];
    sprintf( buf, "opening: %s", db_name_in );
    msg( buf );

    setup_db_name( db_name_in );

    int l = strlen( db_name );

    char buf2[ l + 10 ];

    sprintf( buf2, "./%s", db_name );

    sqlite3_open( buf2 , &db );

    if (db == 0) 
    {
        sprintf( buf, "Could not open database: \"%s\"", db_name_in );
        msg( buf );
        return 0;
    }

    return 1;
}

//
void SqliteDBManager::close_db( void ) 
{
    if ( !db ) 
    {
        msg( "no db to close" );
        return;
    }

    msg( "closing db" );
    sqlite3_close( db );
    db = 0;
    db_name[0] = '\0';
}

int select_callback( void * p_result, int num_fields, char ** p_fields, char ** p_colnames ) 
{
	int i;

	DBResult * res = (DBResult *) p_result;

    DBRow * row = new DBRow;

    // get column names
    if ( res->is_first_row() ) 
    {
        for ( i = 0; i < num_fields; i++ ) 
        {
            res->addColName( p_colnames[ i ] );
        }

        res->is_first_row( false );
    }

    for ( i = 0; i < num_fields; i++ ) 
    {
        DBValue * v;

        if ( p_fields[i] ) 
        {
            if ( p_colnames[i] ) 
            {
                v = new DBValue( p_colnames[i], p_fields[i] ) ;
            }
            else
            {
                v = new DBValue( p_fields[i] ) ;
            }
        }
        else
        {
            if ( p_colnames[i] )            
            {
                v = new DBValue( p_colnames[i], "null" ) ;
            }
            else
            {
                v = new DBValue( "null" ) ;
            }
        }

        row->addVal( v );
    }

	res->addRow( row );
	
    return 0;
}

// SELECT
DBResult * SqliteDBManager::query( const char * str ) 
{
    if ( !str || !db )
        return 0;

    char *  errmsg;
    int     ret;

    DBResult * result = newDBResult();

    ret = sqlite3_exec ( db, str, select_callback, result, &errmsg );

    char buf[ 4096 ];
    if ( ret != SQLITE_OK ) 
    {
        sprintf( buf, "Error in select statement %s [%s].", str, errmsg );
        msg( buf );
    }
    else 
    {
        sprintf( buf, "query: \"%s\" [%d records returned]", str, (int) result->count() );
        msg( buf );
    }

    return result;
}


// UPDATE, INSERT, DELETE, ALTER
int SqliteDBManager::statement( const char * stmt ) {
    char *errmsg;
    int   ret;

    ret = sqlite3_exec( db, stmt, 0, 0, &errmsg );

    char buf[4096];
    if ( ret != SQLITE_OK ) {
        sprintf ( buf, "Error in statement: \"%s\" [%s].", stmt, errmsg );
        msg( buf );
        return 0;
    }

    last_changed_rows = sqlite3_changes( db );
        
    sprintf( buf, "statement: \"%s\" [%d rows changed]", stmt, last_changed_rows );
    msg( buf );

    return last_changed_rows;
}


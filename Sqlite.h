// Sqlite.h
#ifndef __SQLITE_WRAPPER__
#define __SQLITE_WRAPPER__


#include <string.h>
#include "sqlite3.h"
#include <vector>

#include "Buffer.h"
#include "Logger.h"

#define DEFAULT_DB_NAME "database.sqlite"



class DBValue
{
protected:
    int ival;
    float fval;
    char * sval;
    char * name;

public:

    DBValue() : ival(0),fval(0),sval(0),name(0) {}
    DBValue( int i );
    DBValue( float f );
    DBValue( const char * );

    // FIXME: stubs
    // same, with field name
    DBValue( const char *, int i ) {}
    DBValue( const char *, float f ) {}

    DBValue( const char *, const char * ) ;

    float getFloat() const { return fval; }
    int getInt() const { return ival; }
    char * getString() const { return sval; }
    char * getName() const { return name; }
    
    ~DBValue(); 
};




class DBRow
{
protected:

    std::vector<DBValue *> values;

public:

    DBRow() {}
    ~DBRow() ;

    void addVal( DBValue * v ) { values.push_back( v ); }
    void addVal( const char * );

    DBValue * getField( size_t ) ;

    size_t size() const { return values.size(); }
};



class DBResult
{
protected:

    std::vector<DBRow *> rows;
    
    std::vector<char *> col_names;
    
    Buffer<char> buffer;

    bool first_row;

public:

    DBResult() : first_row(1) {}
    ~DBResult();

    void addRow( DBRow * r ) { rows.push_back( r ); }

    size_t count() const { return rows.size(); }
    size_t size() const { return rows.size(); }

    DBRow * getRow( size_t );

    bool is_first_row( int );

    void addColName ( const char * );
    size_t columnCount() const { return col_names.size(); }
    const char * colName( size_t );
    const char * getColNames();
};



class DBManager 
{
protected:
    mutable const char * defDBName;
    static const unsigned int max_name_sz = 100;
    char db_name[ max_name_sz ];
    int last_changed_rows;

    void setup_db_name( const char * =0 ); // initialize with name or default
    void set_db( const char * =0 ); // choose or override the current db
    void reopen(); // if closed: open. if open: close and re-open

    std::vector<DBResult *> results; // for garbage collection

public:
    DBManager() ;
    virtual ~DBManager() ;

    virtual int open_db( const char * =0 ) = 0;
    virtual void close_db() = 0;

    // actual data
    virtual DBResult * query( const char * ) = 0;
    virtual int statement( const char * ) = 0;

    int rows_affected() { return last_changed_rows; } 

    DBResult * newDBResult(); // named constructor

    void clean_up(); // frees olds results memory
};



class SqliteDBManager : public DBManager
{
protected: 
    sqlite3* db;
    void msg (const char *) ;
    Logger *logger_p;
    const char * mngr_name;
    friend int select_callback ( void *, int , char **, char ** );


public:
    SqliteDBManager() ;
    SqliteDBManager( Logger * ) ;
    SqliteDBManager( const char * );
    SqliteDBManager( const char *, Logger * );

    ~SqliteDBManager() ;

    void setLogger( Logger * );

    int open_db( const char * =0 );
    void close_db();

    DBResult * query( const char * );
    int statement( const char * );

    void setLoggerPrefix() ;
};

#endif

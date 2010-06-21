// example usage of sqlite_wrapper 
// g++ -Wall -g sqlite_wrapper.cpp test.cpp -lsqlite3

#include "../Sqlite.h"

int main() 
{
    Logger debug("Main");

    debug.msg( "starting..." );

    SqliteDBManager DB ( &debug );

    // open
    DB.open_db( "../../drinkOMeter.sqlite" );

    DBResult *res = DB.query( "select * from DRINK limit 2" ); 

    debug.setPrefix( "Main" );

    char buf[4096];
    sprintf( buf, "columns: \"%s\"", res->getColNames() );
    debug.msg( buf );

    DB.setLoggerPrefix();

    for ( size_t i = 0 ; i < res->count(); i++ ) 
    {
        DBRow *row = res->getRow(i);

        sprintf( buf, "================= row %u - %u fields ================",(unsigned)(i+1), (unsigned)row->size() );
        debug.msg( "Main", buf );
        DB.setLoggerPrefix();

        for ( size_t j = 0; j < row->size(); j++ ) 
        {
            DBValue *val = row->getField( j );

            sprintf( buf, "name: %s\nfloat: %f\nint: %i\nstring: \"%s\"\n",
                    val->getName(),
                    val->getFloat(),
                    val->getInt(),
                    val->getString() );
            debug.msg( buf );
        }
    }
    
    // these 3 are optional, but here to demonstrate the interface
    DB.setLoggerPrefix(); 
    DB.close_db();
    DB.clean_up(); //call this when you want. or it will do it automatically

    debug.msg( "Main", "finishing up..." );
    debug.print();
}


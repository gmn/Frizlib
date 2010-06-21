// example of most minimal use of sqlite_wrapper 

#include "../Sqlite.h"

int main() 
{
    SqliteDBManager DB ( "../../drinkOMeter.sqlite" );

    DBResult *res = DB.query( "select * from DRINK limit 2" ); 

    for ( size_t i = 0 ; i < res->count(); i++ ) 
    {
        DBRow *row = res->getRow(i);

        printf( "================= ROW %u =================\n", (unsigned)i+1 );

        for ( size_t j = 0; j < row->size(); j++ ) 
        {
            DBValue *val = row->getField( j );

            printf( "%u: name: %s, float: %f, int: %i, string: \"%s\"\n",
                    (unsigned) j+1,
                    val->getName(),
                    val->getFloat(),
                    val->getInt(),
                    val->getString() );
        }
    }
}


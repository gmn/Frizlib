CFLAGS=-Wall -g
SQL=sqlite
MIN=sqlite-minimal
BUF=buffer
DATE=date

all:
	g++ ${CFLAGS} ${SQL}.cpp ../CStrings.cpp ../Logger.cpp ../Sqlite.cpp -lsqlite3 -o ${SQL}
	g++ ${CFLAGS} ${MIN}.cpp ../CStrings.cpp ../Logger.cpp ../Sqlite.cpp -lsqlite3 -o ${MIN}
	g++ ${CFLAGS} ${BUF}.cpp ../CStrings.cpp -lsqlite3 -o ${BUF}
	g++ ${CFLAGS} ${DATE}.cpp ../CStrings.cpp ../Date.cpp  -o ${DATE}


clean:
	rm -rf ${SQL} ${MIN} ${BUF}
	rm -rf ${SQL}.dSYM ${MIN}.dSYM ${BUF}.dSYM
	rm -rf ${DATE} ${DATE}.dSYM
	rm -rf a.out a.out.dSYM
    

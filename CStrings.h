// CStrings.h
#ifndef __CSTRINGS__
#define __CSTRINGS__


void safe_strncpy( char * str_to, const char * str_from, size_t size );


/******************************************************************************
 *
 *      Assert()
 *
 *****************************************************************************/
void __nocall_Assert( int, const char *, const char *, int );
#ifdef _DEBUG
#define Assert( v ) __nocall_Assert( ((v)), #v, __FILE__, __LINE__ )
#else
#define Assert( v )
#endif




#endif /* !__CSTRINGS__ */

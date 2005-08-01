#ifndef PSQ_TYPES_H
#define PSQ_TYPES_H

#include "iodbc.h"
#include "isqlext.h"
typedef RETCODE SQLRETURN;
typedef HENV    SQLHENV;
typedef HDBC    SQLHDBC;
typedef HSTMT   SQLHSTMT;
typedef PTR     SQLPOINTER;

/*    somehow wasn't defined for postgres.....*/
#define SQL_SQLSTATE_SIZE     6

#endif  /* PSQ_TYPES_H */

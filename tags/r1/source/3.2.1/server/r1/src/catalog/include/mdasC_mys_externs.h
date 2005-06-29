/**********
From MySQL manual:
Prior to MySQL 4.0.14, the target table of the INSERT statement cannot appear in the FROM clause of the SELECT part of the query. This limitation is lifted in 4.0.14. 
Thus this driver needs 4.0.14 or higher to process insert-select statements on same table.
**********/

#include "mysql.h"
#include "errmsg.h"
#include "mdasC_db2_externs.h"
#include "mdasGlobalsExtern.h"
#include "srbErrno.h" 
#include "elogExtern.h"

#include <sys/types.h>
#include <unistd.h>

#define SQL_SUCCESS 0
#define SQL_ERROR -1
#define SQLCHAR unsigned char
#define SQLUSMALLINT unsigned short int
#define SQLSMALLINT short int
#define SQLINTEGER long int
#define SQLUINTEGER unsigned long int
#define SQLHENV long int
#define SQLHDBC long int

#define ER_TABLE_NOT_LOCKED 1100

#define TRUE 1
#define FALSE 0

#define HOST "srbbrick10.sdsc.edu"
#define PORT "3306"
#define LOGPATH "/data/charliec"

#define MYSQL_DEBUG_FILE_NAME "/data/charliec/mysql-srb.out"

#define RETURN_SQL 0 == mysql_errno(&mysql) ? SQL_SUCCESS : SQL_ERROR; 

FILE *LOGMESSFILE;
char logmess[HUGH_HUGE_STRING];

MYSQL mysql;

char debug_file_path[1024];
char spec_buf[128];
char block[1024][1024];

char lock_string[4096];
char* lock_elements[256];
int lock_elements_idx;

typedef struct result_info
{
	long int inhstmt;
	struct result_info* next;
	SQLSMALLINT num_of_cols;
    	SQLCHAR** data;
        SQLINTEGER* data_size;
} result_info;

result_info* result_info_head;

void log_debug(char *message, unsigned int status);
int lt_push(char* table);
int lt_push_alias(char* alias, char* table);
void lt_pop();
void lt_clear();
void lt_remove(char* table);
int lt_lock();
int lt_unlock();
int srb_query(SQLCHAR *query);
void init_result_info();
void clear_result_info();
void push_result_info(long int inhstmt, SQLCHAR* data[], SQLINTEGER data_size[], SQLSMALLINT num_of_cols);
int get_result_info(long int inhstmt, SQLCHAR*** data, SQLINTEGER** data_size, SQLSMALLINT *num_of_cols);
void pop_result_info(long int inhstmt);
void filterQuery(char *query);
int open_env_db2(long int *inhenv);
int disconnect_db2(long int inhenv, long int inhdbc, SQLUSMALLINT transstate);
int exec_no_res_w_prelock(SQLCHAR *query, int start);
int exec_no_res_lock(SQLCHAR *query);
int exec_no_res_sql_db2(long int inhenv, long int inhdbc, SQLCHAR *query);
int exec_sql_db2(long int inhenv, long int inhdbc, long int *inhstmt, SQLCHAR *query);
int done_with_result_db2(long int inhstmt);
int done_with_default_result_db2(long int inhstmt, SQLCHAR *data[], SQLINTEGER *data_size, SQLSMALLINT num_of_cols);
int bind_default_row_db2(long int inhstmt, SQLCHAR *data[], SQLINTEGER data_size[], SQLSMALLINT *num_of_cols);
int get_row_db2(long int inhstmt);
int get_no_of_columns_db2(long int inhstmt, SQLSMALLINT *num_of_cols);
int get_column_info_db2(long int inhstmt, SQLSMALLINT  column_number, SQLINTEGER *displaysize, SQLCHAR colname[], SQLSMALLINT *coltype, SQLSMALLINT buf_size, SQLSMALLINT *colnamelen, SQLSMALLINT *nullable, SQLUINTEGER *precision, SQLSMALLINT *scale);
int close_db2_interaction_internal(SQLHENV henv, SQLHDBC hdbc, int transaction_end_code);
int open_db2_interaction_internal(SQLHENV *henv, SQLHDBC *hdbc);
int commit_db2_interaction_internal(SQLHENV henv, SQLHDBC hdbc, int  transaction_end_code);

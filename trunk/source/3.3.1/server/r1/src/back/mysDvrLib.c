/*
From MySQL manual:
Prior to MySQL 4.0.14, the target table of the INSERT statement cannot appear
in the FROM clause of the SELECT part of the query. This limitation is lifted
in 4.0.14. Thus this driver needs 4.0.14 or higher to process insert-select
statements on same table.
*/

#include "mdasC_mys_externs.h"
#include "mysDvrLib.h"
/***************************************************************************
  NAME   : mys_log_debug
  PURPOSE: Logging message in LOGMESS file handle
****************************************************************************/
void mys_log_debug(char *message, unsigned int status)
{
	time_t *mmmmclk, mmmmcl;
	mmmmcl = time(NULL);

	#ifdef DEBUGON
	/*LOGMESSFILE = fopen(DB2LOGFILE,"a");*/
	LOGMESSFILE = fopen(debug_file_path,"a");
	fprintf(LOGMESSFILE,"%24.24s =>%li %s\n",ctime(&mmmmcl),status, message);
	fclose(LOGMESSFILE);
	#endif
}

/*********
LOCK TABLES support functions: MySQL does not support multiple LOCK TABLE
calls; All tables you wish to lock must be specified in the same LOCK TABLE
call. A call to LOCK TABLE will automatically unlock whatever was previously
locked. Hence, the mys_lt_ functions simulate a stack to keep track of
previously locked tables. The sql command stored in lock_string can be used
to lock all the required tables at any time.
*********/
int mys_lt_push(char* table)
{
        int len = strlen(table);

		#ifdef DEBUGON
		mys_log_debug("mys_lt_push - current lock status:", 0);
		mys_log_debug(lock_string, 0);
		mys_log_debug("pushing onto stack:", 0);
		mys_log_debug(table, 0);
		#endif

        if(lock_elements_idx)
        {
				if(strstr(lock_string, table))
				{
					#ifdef DEBUGON
					mys_log_debug("item is already on stack.", 0);
					#endif
					return FALSE;
				}

                sprintf(lock_elements[lock_elements_idx], ", %s WRITE;", table);
                len += 8; 
                lock_elements[++lock_elements_idx] = lock_elements[lock_elements_idx-1] + len;		
        }else
        {
                sprintf(lock_string, "LOCK TABLES %s WRITE;", table);
                len += 18; 
				lock_elements[0] = &lock_string[0];
                lock_elements[1] = &lock_string[len];
				lock_elements_idx = 1;
        }
		return TRUE;
}

int mys_lt_push_alias(char* alias, char* table)
{
        int len = strlen(table) + strlen(alias);

		#ifdef DEBUGON
		mys_log_debug("mys_lt_push_alias - current lock status:", 0);
		mys_log_debug(lock_string, 0);
		mys_log_debug("pushing onto stack:", 0);
		mys_log_debug(alias, 0);
		#endif

        if(lock_elements_idx)
        {
				if(strstr(lock_string, alias))
				{
					#ifdef DEBUGON
					mys_log_debug("item is already on stack.", 0);
					#endif
					return FALSE;
				}

                sprintf(lock_elements[lock_elements_idx], ", %s AS %s WRITE;", table, alias);
                len += 12;
                lock_elements[++lock_elements_idx] = lock_elements[lock_elements_idx-1] + len;		
        }else
        {
                sprintf(lock_string, "LOCK TABLES %s AS %s WRITE;", table, alias);
                len += 22;
				lock_elements[0] = &lock_string[0];
                lock_elements[1] = &lock_string[len];
				lock_elements_idx = 1;
        }
		return TRUE;
}

void mys_lt_pop()
{
	#ifdef DEBUGON
	mys_log_debug("mys_lt_pop - current lock status:", 0);
	mys_log_debug(lock_string, 0);
	mys_log_debug("popping off stack:", 0);
	mys_log_debug(lock_elements[lock_elements_idx-1], 0);
	#endif

	if(--lock_elements_idx)
	{
		lock_elements[lock_elements_idx][0] = ';';
		lock_elements[lock_elements_idx][1] = '\0';
	}else
	{
		lock_elements[0][0] = '\0';
	}
}

void mys_lt_clear()
{
	int status1, status2;
	char buf[2048];

	#ifdef DEBUGON
	mys_log_debug("mys_lt_clear - current lock status:", 0);
	mys_log_debug(lock_string, 0);
	#endif

	if(lock_elements_idx)
	{
		lock_elements_idx = 0;
		lock_elements[0][0] = '\0';
		status1 = mysql_query(&mysql, "UNLOCK TABLES;");

		#ifdef DEBUGON
		if(status1 != 0)
		{
			status2 = mysql_errno(&mysql);
			sprintf(buf, "unlock tables failed: [%d][%d], %s", status1, status2, mysql_error(&mysql));
			mys_log_debug(buf, 0);
		}
		#endif
	}
}

void mys_lt_remove(char* table)
{
        int i,j;
        int len;
        char *one, *two;
        char *ptr;

		#ifdef DEBUGON
		mys_log_debug("mys_lt_remove - current lock status:", 0);
		mys_log_debug(lock_string, 0);
		mys_log_debug("removing from stack:", 0);
		mys_log_debug(table, 0);
		#endif

        len = strlen(table);

        for(i = lock_elements_idx-1; i > 0; i--)
                if(0 == strncmp(table, lock_elements[i]+2, len)
				&& lock_elements[i][len+2] == ' ')
                        break;

        if(i > 0)
        {
                one = lock_elements[i];
                two = lock_elements[i+1];

                for(j = i+1; j <= lock_elements_idx; j++)
                {
                        ptr = lock_elements[j+1];
                        ptr -= (len+8);
                        lock_elements[j] = ptr;
                }

                len = strlen(one);

                for(j = 0; j < len; j++, one++, two++)
                        *one = *two;

                lock_elements_idx--;
        }else if(i == 0)
        {
                if(0 == strncmp(table, lock_elements[0]+12, len)
				&& lock_elements[i][len+12] == ' ')
                {
                        one = lock_elements[0]+12;
                        two = lock_elements[1]+2;
                        for(j = 1; j <= lock_elements_idx; j++)
                        {
                                ptr = lock_elements[j+1];
                                ptr -= (len+8);
                                lock_elements[j] = ptr;
                        }

                        len = strlen(one);
                        for(j = 0; j < len; j++, one++, two++)
                                *one = *two;
                        lock_elements_idx--;
                        if(lock_elements_idx == 0)
                                lock_elements[0][0] = '\0';
                }else
				{
						#ifdef DEBUGON
						mys_log_debug("table is not in lock stack:", 0);
						#endif
				}
        }
}

int mys_lt_lock()
{
	int status1, status2;
	char buf[1024];

	#ifdef DEBUGON
	mys_log_debug("mys_lt_lock - current lock status:", 0);
	mys_log_debug(lock_string, 0);
	#endif

	status1 = mysql_query(&mysql, lock_string);

	if(status1 != 0)
	{
		#ifdef DEBUGON
		status2 = mysql_errno(&mysql);
		sprintf(buf, "failure: [%d][%d], %s", status1, status2, mysql_error(&mysql));
		mys_log_debug(buf, 0);
		#endif
		return FALSE;
	}

	return TRUE;
}

int mys_lt_unlock()
{
	int status1, status2;
	char buf[1024];

	#ifdef DEBUGON
	mys_log_debug("mys_lt_unlock - current lock status:", 0);
	mys_log_debug(lock_string, 0);
	#endif

	mys_lt_pop();

	#ifdef DEBUGON
	mys_log_debug("current lock status:", 0);
	mys_log_debug(lock_string, 0);
	#endif

	if(lock_string[0] != '\0')
	{
		mys_log_debug("relocking tables", 0);
		status1 = mysql_query(&mysql, lock_string);
	}
	else
	{
		mys_log_debug("unlock all tables", 0);
		status1 = mysql_query(&mysql, "UNLOCK TABLES;");
	}

	if(status1 != 0)
	{
		#ifdef DEBUGON
		status2 = mysql_errno(&mysql);
		sprintf(buf, "failure: [%d][%d], %s", status1, status2, mysql_error(&mysql));
		mys_log_debug(buf, 0);
		#endif
		return FALSE;
	}

	return TRUE;
}

int mys_srb_query(SQLCHAR *query)
{
	int status1, status2;
	char buf[1024];

	#ifdef DEBUGON
	mys_log_debug("mys_srb_query - current lock status:", 0);
	mys_log_debug(lock_string, 0);
	#endif

	status1 = mysql_query(&mysql, query);

	if(status1 != 0)
	{
		#ifdef DEBUGON
		status2 = mysql_errno(&mysql);
		sprintf(buf, "failed: [%d][%d], %s", status1, status2, mysql_error(&mysql));
		mys_log_debug(buf, 0);
		#endif
		return FALSE;
	}

	return TRUE;
}

/**********
RESULT INFO support functions. After a SQL command is executed, SRB calls
the bind function to assign space to store the results of the command.
The get row function is then called to write data into that space. MySQL has
no method for assigning space to store results in; space is allocated
automatically for results when the MySQL get function is called in 'get row'.
The data must be stored in the location returned in the prior bind call and
thus some of that information must be preserved. The resumys_lt_info functions
perform this functionality.
**********/

void init_result_info_mys()
{
	result_info_mys_head = NULL;
	lock_elements_idx = 0;
}

void clear_result_info_mys()
{
	result_info *res, *next;
	res = result_info_mys_head;

	while(res != NULL)
	{
		next = res->next;
		free(res);
		res = next;
	}

	result_info_mys_head = NULL;
	lock_elements_idx = 0;

	return;
}

void push_result_info_mys(long int inhstmt, SQLCHAR* data[], SQLINTEGER data_size[], SQLSMALLINT num_of_cols)
{
	result_info* res;

	res = (result_info*)malloc(sizeof(result_info));

	res->inhstmt = inhstmt;

	res->data = data;
	res->data_size = data_size;

	res->num_of_cols = num_of_cols;
	res->next = result_info_mys_head;
	result_info_mys_head = res;
}

int get_result_info_mys(long int inhstmt, SQLCHAR*** data, SQLINTEGER** data_size, SQLSMALLINT *num_of_cols)
{
	result_info* res = result_info_mys_head;

	while(res != NULL)
	{
		if(res->inhstmt == inhstmt)
		{
			*data = res->data;
			*data_size = res->data_size;
			*num_of_cols = res->num_of_cols;
			return TRUE;
		}
		res = res->next;
	}

	return FALSE;
}

void pop_result_info_mys(long int inhstmt)
{
	result_info *res, *prev;

	prev = NULL;
	res = result_info_mys_head;

	while(res != NULL)
	{
		if(res->inhstmt == inhstmt)
		{
			if(prev)
				prev->next = res->next;
			free(res);

			return;
		}
		prev = res;
		res = res->next;
	}

	return;
}

/**********
FILTER QUERY: SQL commands passed to the driver from SRB are Oracle-centric
and sometimes need massaging to work correctly on other systems.
**********/

void filterQuerymys(char *query)
{
	char *tmpPtr, *tmpPtr1, *tmpPtr2;
	char buf[4096];

	#ifdef DEBUGON
	mys_log_debug("filterQuerymys", 0);
	sprintf(buf, "query: %s\n", (char*)query);
	mys_log_debug(buf, 0);
	#endif

	/*standard code for converting oracle-centric sql to generic sql.*/
	if((tmpPtr = strstr(query,"/*+")) != NULL
	&& (tmpPtr2 = strstr(query,"*/")) != NULL
	&& tmpPtr < tmpPtr2)
	{
		*tmpPtr  = '\0';
		strcat(query," ");
		strcat(query, (char *) (tmpPtr2 + 2));
	}

	/*mysql-specific conversions*/

	tmpPtr = NULL;
	tmpPtr2 = NULL;

	/*LOCK TABLE table_name WRITE is the MYSQL equiv. of exclusive mode*/
	if(0 == strncmp("LOCK TABLE ", query, 11) || 0 == strncmp("lock table ", query, 11))
		if((tmpPtr = strstr(query, " in exclusive mode")) != NULL)
	{
		sprintf(tmpPtr, " WRITE");
	}

	/*mysql does not like (select x union select y) order by z - instead likes
	(select x)union(select y) order by z*/
	if(0 == strncmp("(SELECT ", query, 8) || 0 == strncmp("(select ", query, 8))
        {
                if((tmpPtr = strstr(query, "union")) != NULL)
                {
					sprintf(buf, "%s", (tmpPtr+5));
					sprintf(tmpPtr, ") union (%s", buf);
                }
	}

	/*mysql doesn't like select ' '*/
	tmpPtr = strstr(query, "'  '||");
	if(tmpPtr)
		sprintf(tmpPtr, "%s", &tmpPtr[6]); /*dangerous?*/	

	tmpPtr = strstr(query, " ESCAPE '\\' ");
	if(tmpPtr)
	{
		for(i = strlen(tmpPtr); i >= 0; i--)
			tmpPtr[i+1] = tmpPtr[i];
		
		sprintf(tmpPtr, " ESCAPE '\\\\' ");
	}

	tmpPtr = strstr(query, " substr(");
        if(tmpPtr)
        {
                for(i = strlen(tmpPtr); i >= 0; i--)
                        tmpPtr[i+3] = tmpPtr[i];
                                                                                                                 
                strncpy(tmpPtr, " SUBSTRING(", 11);
        }

	#ifdef DEBUGON
	sprintf(buf, "filtered query: %s\n", (char*)query);
	mys_log_debug(buf, 0);
	#endif
}

/***************************************************************************
  NAME   : open_env_mys
  PURPOSE: Opening an execution environment in db2 
****************************************************************************/ 
int open_env_mys(long int *inhenv)
{

	unsigned int status;

	lock_elements_idx = 0;


	init_result_info_mys();

	if(mysql_init(&mysql))
	{
		#ifdef DEBUGON
		status = mysql_errno(&mysql);
		mys_log_debug("open_env_mys: success", status);
		#endif
		*inhenv = (long int)(&mysql);
		return SQL_SUCCESS;
	}else
	{
		#ifdef DEBUGON
		status = mysql_errno(&mysql);
		mys_log_debug("open_env_mys: failure", status);
		#endif
		*inhenv = 0;
		return SQL_ERROR;
	}
}

/***************************************************************************
  NAME   : close_env_mys
  PURPOSE: Closing an open execution environment in db2 
****************************************************************************/
int close_env_mys(long int inhenv)
{
	#ifdef DEBUGON
	mys_log_debug("close_env_mys", 0);
	#endif
	clear_result_info_mys();
	lock_elements_idx = 0;

	return SQL_SUCCESS;
}

/***************************************************************************
  NAME   : connect_mys
  PURPOSE: Opening a connection to db2 server
****************************************************************************/
int connect_mys(long int inhenv, long int *inhdbc, SQLCHAR *databasename, SQLCHAR *user, SQLCHAR *password)
{
        *inhdbc = 0;

        lock_elements_idx = 0;

        #ifdef DEBUGON
        mys_log_debug("connect_mys", 0);
        #endif

	sprintf(spec_buf, "%s.", databasename);

        /*both db and connection info are stored in mysql; hence, no inhdbc.*/
        if(mysql_real_connect(&mysql, HOST, user, password, databasename, 0, PORT, 0))
        {
                #ifdef DEBUGON
                sprintf(logmess, "connect: Database = %s, User = %s", databasename,user);
                mys_log_debug(logmess, mysql_errno(&mysql));
                #endif

                return SQL_SUCCESS;
        }

        #ifdef DEBUGON
        sprintf(logmess, "connect failed: Database = %s, User = %s", databasename,user);
        mys_log_debug(logmess, mysql_errno(&mysql));
        #endif

        return RETURN_SQL;
}
/***************************************************************************
  NAME   : disconnect_mys
  PURPOSE: Closing an open connection  in db2 

****************************************************************************/
int disconnect_mys(long int inhenv, long int inhdbc, SQLUSMALLINT transstate)
{
	#ifdef DEBUGON
	mys_log_debug("disconnect_mys", 0);
	#endif

	mysql_close(&mysql);
	lock_elements_idx = 0;

	return SQL_SUCCESS;
}

/*exec_no_res_w_prelock As mysql does not have transaction-safe tables that
  we can use, all inserts, updates, and deletes must lock the affected table
  first. This function is called by exec_no_res_sql_mys to handle no-result
  SQL statements that require a lock wrap (INSERT, UPDATE, DELETE). */
int exec_no_res_w_prelock(SQLCHAR *query, int start)
{
	char buf[4096];
	int i, qstatus1, pstatus1;

	#ifdef DEBUGON
	mys_log_debug("exec_no_res_w_prelock", 0);
	#endif

	/* get table name, push it on the stack, and relock */
	sprintf(buf, "%s", query);
	for(i = start; i < strlen(buf); i++)
		if(buf[i] == ' ')
		{
			buf[i] = '\0';
			break;
		}
		
	pstatus1 = mys_lt_push(&buf[start]);

	if(mys_lt_lock() == FALSE)
		return SQL_ERROR;

	qstatus1 = mys_srb_query(query);

	if(pstatus1 == TRUE)
		if(mys_lt_unlock() == FALSE)
			return SQL_ERROR;

	if(qstatus1 == FALSE)
		return SQL_ERROR;

	return SQL_SUCCESS;
}

/*exec_no_res_lock this function is called my exec_no_res_sql_mys to handle
a lock table request. It wasn't entirely necessary to put this functionality
in a separate function, but it makes debugging cleaner.*/
int exec_no_res_lock(SQLCHAR *query)
{
	char buf[4096];
	int i, lstatus1, lstatus2, qstatus1, qstatus2, pstatus1;

	#ifdef DEBUGON
	mys_log_debug("exec_no_res_lock", 0);
	#endif

	/* get table name, push it on the stack, and relock */
	sprintf(buf, "%s", query);
	for(i = 11; i < strlen(buf); i++)
		if(buf[i] == ' ')
		{
			buf[i] = '\0';
			break;
		}
		
	if(mys_lt_push(&buf[11]) == TRUE)
		if(mys_lt_lock() == FALSE)
			return SQL_ERROR;

	return SQL_SUCCESS;

}

/***************************************************************************
  NAME   : exec_no_res_sql_mys
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
****************************************************************************/
int exec_no_res_sql_mys(long int inhenv, long int inhdbc, SQLCHAR *query)
{
	int status, status2;
	char buf[4096];

	#ifdef DEBUGON
	mys_log_debug("exec_no_res_sql_mys", 0);
	#endif

	filterQuerymys(query);

	if(0 == strncmp("insert into ", query, 12))
		return exec_no_res_w_prelock(query, 12);
	else if(0 == strncmp("update ", query, 7))
		return exec_no_res_w_prelock(query, 7);
	else if(0 == strncmp("delete from ", query, 12))
		return exec_no_res_w_prelock(query, 12);
	else if(0 == strncmp("lock table ", query, 11)) 
		return exec_no_res_lock(query);
	else
		return mys_srb_query(query);

	return SQL_ERROR;
}


/***************************************************************************
  NAME   : exec_sql_mys
  PURPOSE: Execute a SQL query which mave have  resulting table
****************************************************************************/
int exec_sql_mys(long int inhenv, long int inhdbc, long int *inhstmt,
		SQLCHAR *query)
{
		char buf2[1024];


	int status, lstatus1, lstatus2;
	MYSQL_RES *result = NULL;
	int extras_count;

	char buf[1024];
	char *ptr, *token;
	char* pbuf[12];
	int i, count = 0;



	#ifdef DEBUGON
	mys_log_debug("exec_sql_mys", 0);
	#endif
	
	*inhstmt = 0;

	filterQuerymys(query);

	extras_count = 0;

	while(TRUE)
	{
		if(mys_srb_query(query) == TRUE)
		{
			result = mysql_store_result(&mysql);
			/*whatever we pushed onto the stack to make the
			query work, we should pop off.*/
			if(extras_count)
			{
				while(extras_count)
				{
					mys_lt_pop();
					extras_count--;
				}

				if(mys_lt_unlock() == FALSE)
					return SQL_ERROR;
			}

			

			if(NULL == result)
				return RETURN_SQL;

			*inhstmt = (long int)result;
	
			return SQL_SUCCESS;
		}

		status = mysql_errno(&mysql);

		if(status == ER_TABLE_NOT_LOCKED)
		{
			#ifdef DEBUGON
			sprintf(buf, "%s", mysql_error(&mysql));
			mys_log_debug(buf, 0);
			mys_log_debug("trying mysql_query again...", 0);
			#endif
			sprintf(buf, "%s", query);
			ptr = strstr(buf, " where ");
			ptr[0] = '\0';
			ptr = strstr(buf, " from ");
			token = strtok(ptr+6, " ,");
			pbuf[count++] = token;

			while(token != 0)
			{
				mys_log_debug(token, 0);
				token = strtok(NULL, " ,");
				pbuf[count++] = token;
			}

			sprintf(buf2, "count = %d", count);
			mys_log_debug(buf2, 0);


			for(i = 0; i < count-1; i++)
			{
				sprintf(buf2, "i = %d", i);
				mys_log_debug(buf2, 0);
				if(0 == strncmp(spec_buf, pbuf[i], strlen(spec_buf)))
				{
					sprintf(buf2, "push: %s", pbuf[i]);
					mys_log_debug(buf2, 0);
					if(mys_lt_push(pbuf[i]))
						extras_count++;
				}
				else
				{
					sprintf(buf2, "push: %s of %s", pbuf[i], pbuf[i-1]);
					mys_log_debug(buf2, 0);
					if(mys_lt_push_alias(pbuf[i], pbuf[i-1]))
						extras_count++;
				}
			}

			mys_lt_lock();
		}else
		{
			return SQL_ERROR;
		}
	}

	return SQL_ERROR;	/*should not reach here*/
}

/***************************************************************************
  NAME   : done_with_result_db
  PURPOSE: Close a stament environment opened by exec_sql_mys() but which
           used user allocated result structure
****************************************************************************/
int done_with_result_mys(long int inhstmt)
{  
	#ifdef DEBUGON
	mys_log_debug("done_with_result_mys", 0);
	#endif

	MYSQL_RES *result = (MYSQL_RES*)inhstmt;
	mysql_free_result(result);
	return SQL_SUCCESS;
}

/***************************************************************************
  NAME   : done_with_default_result_mys
  PURPOSE: Close a stament environment opened by exec_sql_mys() which used
           result structure build by bind_default_row_mys()
****************************************************************************/
int done_with_default_result_mys(long int inhstmt, SQLCHAR *data[],
			SQLINTEGER *data_size, SQLSMALLINT num_of_cols)
{  
	#ifdef DEBUGON
	mys_log_debug("done_with_default_result_mys", 0);
	#endif
	mysql_free_result((MYSQL_RES*)inhstmt);

	return SQL_SUCCESS;
}

/***************************************************************************
  NAME   : bind_default_row_mys
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
****************************************************************************/
int bind_default_row_mys(long int inhstmt, SQLCHAR *data[],
			SQLINTEGER data_size[], SQLSMALLINT *num_of_cols)
{
	unsigned long* lengths;
	char* szptr;
	unsigned int i, colcount, field_count;
	MYSQL_FIELD_OFFSET prev;
	MYSQL_FIELD *field;
	MYSQL_ROW row;
	MYSQL_RES *result = (MYSQL_RES*)inhstmt;
	
	#ifdef DEBUGON
	mys_log_debug("bind_default_row_mys", 0);
	#endif

	prev = mysql_field_seek(result, 0);

	field_count = mysql_num_fields(result);

	for(i = 0; i < field_count; i++)
	{
		mysql_field_seek(result, (MYSQL_FIELD_OFFSET)i);
		field = mysql_fetch_field(result);
		data_size[i] = field->max_length + 1;
		data[i] = block[i];
	}

	mysql_field_seek(result, prev);

	*num_of_cols = field_count;

	push_result_info_mys(inhstmt, data, data_size, *num_of_cols);

	return SQL_SUCCESS;
}



/***************************************************************************
  NAME   : get_row_mys
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_mys() calls. To fetch
           multiple row use get_rows_mys()
****************************************************************************/
int get_row_mys(long int inhstmt)
{
	unsigned long* lengths;
	unsigned int i, len;
	SQLSMALLINT num_of_cols;
	MYSQL_ROW row;
	SQLCHAR** data;
	SQLINTEGER* data_size;
	MYSQL_RES *result = (MYSQL_RES*)inhstmt;

	#ifdef DEBUGON
	mys_log_debug("get_row_mys", 0);
	#endif

	if(FALSE == get_result_info_mys(inhstmt, &data, &data_size, &num_of_cols))
		return SQL_ERROR;

	row = mysql_fetch_row(result);

	if(row == NULL)
		return MCAT_NO_DATA_FOUND;

	lengths = mysql_fetch_lengths(result);

	if(lengths == NULL)
		return SQL_ERROR;	/*shouldn't have null lengths unless fetchrow
							wasn't called or there are no more rows; both
							of which should cause a return before this. */

	for(i = 0; i < num_of_cols; i++)
	{
		if(row[i])
			len = strlen(row[i])+1;
		else
			len = 1;

		if(*(data_size+i) > len)
			len = *(data_size+i);

		sprintf(*(data+i), "%s", row[i]);
	}

	return SQL_SUCCESS;

}

/***************************************************************************
  NAME   : get_no_of_columns_mys
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_mys statement. 
****************************************************************************/
int get_no_of_columns_mys(long int inhstmt, SQLSMALLINT *num_of_cols)
{
	#ifdef DEBUGON
	mys_log_debug("get_no_of_columns_mys", 0);
	#endif
        MYSQL_RES *result = (MYSQL_RES*)inhstmt;
	*num_of_cols = mysql_num_fields(result);
	return SQL_SUCCESS;
}

/***************************************************************************
  NAME   : get_column_info_mys
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
****************************************************************************/
int get_column_info_mys(long int inhstmt, SQLSMALLINT  column_number,
			SQLINTEGER *displaysize, SQLCHAR colname[],
			SQLSMALLINT *coltype, SQLSMALLINT buf_size,
                        SQLSMALLINT *colnamelen, SQLSMALLINT *nullable,
			SQLUINTEGER *precision, SQLSMALLINT *scale)
{
	MYSQL_FIELD_OFFSET prev;
	MYSQL_FIELD *field;

	MYSQL_RES *result = (MYSQL_RES*)inhstmt;

	#ifdef DEBUGON
	mys_log_debug("get_column_info_mys", 0);
	#endif

	prev = mysql_field_seek(result, (MYSQL_FIELD_OFFSET)column_number);
	field = mysql_fetch_field(result);
	*colnamelen = strlen(field->name);
	if(*colnamelen > buf_size)
		*colnamelen = buf_size;

	memcpy(colname, field->name, *colnamelen); /*no sprintf in case of bin string (nulls)*/
	/*SRB is not currently using coltype info - no need and no generic type to map to.*/
	*coltype = field->type;
	*displaysize = field->length;
	*scale = 0;
	*precision = field->decimals;
	*nullable = 0;

	mysql_field_seek(result, prev);
}

int close_mys_interaction_internal(SQLHENV henv, SQLHDBC hdbc, int transaction_end_code)
{
	int i; 
	MDAS_DB2_SERVER_HNDL *tmp_srvh;
	#ifdef DEBUGON
	mys_log_debug("close_mys_interaction_internal", 0);
	#endif
	/* transaction_end_code = 1 ABORT TRANSACTION */
	/* transaction_end_code = 0 COMMIT TRANSACTION */
	i = disconnect_mys( henv, hdbc, transaction_end_code); 
  	if (i != 0)  
		return(MCAT_DISCONNECT_ERROR);
	i = close_env_mys(henv);
	if(i != 0)  
		return(MCAT_CLOSE_ENV_ERROR);
	tmp_srvh = (MDAS_DB2_SERVER_HNDL *)serverhndl[0].conn; 
	free (tmp_srvh);

	return MDAS_SUCCESS;
}

int open_mys_interaction_internal(SQLHENV *henv, SQLHDBC *hdbc)
{
	MDAS_DB2_SERVER_HNDL *tmp_srvh;
	int i;

	pid_t pid;
	pid = getpid();
	sprintf(debug_file_path, "%s.%d", MYSQL_DEBUG_FILE_NAME, pid);

	#ifdef DEBUGON
	mys_log_debug("open_mys_interaction_internal", 0);
	#endif
	i= initMdasEnviron();
	#ifdef DEBUGON
	mys_log_debug("mysql open mys", i);
	#endif
	if(i != 0) 
		return (INIT_ENVIRON_ERROR);

	sprintf(db2instancebuffer,"DB2INSTANCE=%s",DB2INSTANCE);
	i = putenv(db2instancebuffer);
	#ifdef DEBUGON
	mys_log_debug(" mysql instancebuff", i);
	#endif
	if(i != 0) 
		return (UNIX_PUTENV_ERROR);

	i =  open_env_mys(henv);
	#ifdef DEBUGON
	mys_log_debug(" openenv", i);
	#endif
	if(i != 0)
		return(MCAT_OPEN_ENV_ERROR);

	/*server it appears from ora_internal.c actually represents database name, instead of db host*/
	i = connect_mys(*henv, hdbc, (unsigned char*)server, (unsigned char*)user, (unsigned char*)pass); 
	#ifdef DEBUGON
	mys_log_debug(" connect", i);
	#endif
	if(i != 0)
	{
	#ifdef DEBUGON
	mys_log_debug(" failed for some reason", i);
	#endif
		disconnect_mys(*henv, *hdbc, 1); 
		return(MCAT_OPEN_ENV_ERROR);
	}

	serverhndl[0].open = 1;
	strcpy(serverhndl[0].sname,cattype);
	tmp_srvh = (MDAS_DB2_SERVER_HNDL *) malloc(sizeof(MDAS_DB2_SERVER_HNDL));
	if(tmp_srvh == NULL) 
  		return(MEMORY_ALLOCATION_ERROR);
	strcpy(tmp_srvh->user, user);
	strcpy(tmp_srvh->server,catser);
	tmp_srvh->valid = 1;
	tmp_srvh->henv  = *henv;
	tmp_srvh->hdbc  = *hdbc;
	serverhndl[0].conn = tmp_srvh;

	return MDAS_SUCCESS;
}


int commit_mys_interaction_internal(SQLHENV henv, SQLHDBC hdbc, int  transaction_end_code)
{
	/*We are currently using ISAM tables, which don't support
	transactions. Therefore, not much here.*/

	#ifdef DEBUGON
	mys_log_debug ("commit mys interaction internal", 0);
	#endif

	/*assume we need to clear the table locks after a commit*/
	mys_lt_clear();

	return MDAS_SUCCESS;
}


/*********************  added  to the original mdasC_nys_intenal.c 
     after changing db2->psg and filterQuery to filetrQuerypsg  and changing names of init, lt and result
    routines  *****************/


/***************************************************************************
  NAME   : getDataTypeFromMap_mysTab
  PURPOSE: Getting Column DataType Name from Data Type Index given by Informix
****************************************************************************/ 
void
getDataTypeFromMap_mysTab(char *typeName, short int   typeId)
{
  int jj;
        for (jj = 0; jj < MAXDATATYPELISTSIZE ; jj++) {
          if (mysDataTypeCode[jj] == typeId) {
          strcpy(typeName, mysDataTypeName[jj]);
          break;
        }
      }
      if (jj == MAXDATATYPELISTSIZE) 
        strcpy(typeName, mysDataTypeName[0]);
}


int
getColTypeFromColNamemysTab(long int  henv, long int  hdbc,
		      char *colType, char *colName, char *inTableName)
{
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;
  char sqlQuery[HUGE_STRING];
  int i,j;
  SQLHSTMT hstmt;
  SQLINTEGER   data_size[4];
  unsigned char       *data[4];
  short int coltypenum;


  tmpPtr2 = inTableName;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  tmpPtr3 = inTableName + strlen(inTableName) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(tmpPtr3 +1) = '\0';
  tmpPtr4 = colName ;
  while (isspace(*tmpPtr4)) tmpPtr4++;
  tmpPtr3 = colName + strlen(colName) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(tmpPtr3 +1) = '\0';

  if ((tmpPtr =  strstr(tmpPtr2,".")) != NULL) {
    *tmpPtr = '\0';
    sprintf(sqlQuery,"select coltype from systables,syscolumns where COLNAME = '%s' and TABNAME = '%s' and OWNER = '%s' and A.tabid = B.tabid",    
	    tmpPtr4, tmpPtr+1,tmpPtr2 );
    *tmpPtr = '.';
  }
  else {
    sprintf(sqlQuery,"select coltype from systables A,syscolumns B where COLNAME = '%s' and TABNAME = '%s' and A.tabid = B.tabid",    
	    tmpPtr4, tmpPtr2 );
  }
  upper(sqlQuery);
  i = exec_sql_mysTab( (SQLHENV) henv, (SQLHDBC) hdbc, &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_mysTab( hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_mysTab( hstmt, data, data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_mysTab(hstmt);
  if (i == 0)       {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    coltypenum = atoi(data[0]);
    if (coltypenum >255) 
	coltypenum -= 256;
    getDataTypeFromMap_mysTab(colType, coltypenum);
	/*strcpy(colType, (char *) data[0]);*/
  }
  else
    {
      i = done_with_default_result_mysTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_mysTab(hstmt,data,data_size, num_of_cols);

  return(MDAS_SUCCESS);
}

int
getColumnInfoFormysTable(long int  henv, long int  hdbc,
		      char *inTableName,
		      int *numOfColumns,
		      char *colName[],
		      char colType[MAX_TABLE_COLS][SMALL_TOKEN],
		      long int displaysize[],
		      short int nullable[],
		      unsigned long int precision[],
		      short int   scale[])
{
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;
  char sqlQuery[HUGE_STRING];
  short int tmpcoltype,tmpint1, tmpint2;
  int i,j, jj;
  SQLHSTMT   hstmt;
  SQLINTEGER   data_size[10];
  unsigned char       *data[10];

  sprintf(sqlQuery,"select first 1 * from %s",inTableName);
  upper(sqlQuery);

  i = exec_sql_mysTab( (SQLHENV) henv, (SQLHENV) hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_mysTab(hstmt, &num_of_cols);
  if (i != 0) return(i);

  *numOfColumns = num_of_cols;
  for (i=1; i < num_of_cols; i++) {
	  
    j = get_column_info_mysTab(hstmt, i+1, (SQLINTEGER) &displaysize[i], colName[i], &tmpcoltype, SMALL_TOKEN , &tmpint2, &nullable[i], (SQLUINTEGER) &precision[i], &scale[i]);

    getDataTypeFromMap_mysTab(colType[i],tmpcoltype);
  }
/*

  tmpPtr2 = inTableName;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  tmpPtr3 = inTableName + strlen(inTableName) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(tmpPtr3 +1) = '\0';

  if ((tmpPtr =  strstr(tmpPtr2,".")) != NULL) {
    *tmpPtr = '\0';
    sprintf(sqlQuery,"select distinct COLNAME, COLTYPE, COLLENGTH, TYPESCHEMA, \
 SCALE, NULLS, COLNO  from SYSCAT.COLUMNS where TABNAME = '%s' \
 and TABSCHEMA = '%s' order by COLNO ASC",
	    tmpPtr+1,tmpPtr2 );
    *tmpPtr = '.';
  }
  else {
    sprintf(sqlQuery,"select distinct COLNAME, TYPENAME, LENGTH, TYPESCHEMA, \
 SCALE, NULLS, COLNO  from SYSCAT.COLUMNS where TABNAME = '%s' \
 order by COLNO ASC", 
	     tmpPtr2 );
  }
  upper(sqlQuery);
  i = exec_sql_mysTab(henv, hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_mysTab(hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_mysTab(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_mysTab(hstmt);
  if (i == 0)       {
    jj = 0;
    while (i == 0) {
      for (i = 0 ; i < num_of_cols; i++) {
	truncate_end_blanks(data_size[i], data[i]);
      }
      colName[jj] = strdup(data[0]);
      if (!strcmp(data[1],"SYSIBM"))
	strcpy(colType[jj], (char *) data[1]);
      else 
	sprintf(colType[jj], "%s.%s", data[3], data[1]);
      displaysize[jj] = atoi(data[2]);
      *//** precision[jj] = atoi(data[3]); **/
/*      precision[jj] = 0; 
      scale[jj] = atoi(data[4]);
      if (!strcmp(data[5], "Y"))
	nullable[jj] = 1;
      else
	nullable[jj] = 0;
      jj++;
      i = get_row_mysTab(hstmt);
    }
    *numOfColumns = jj;
  }
  else
    {
      i = done_with_default_result_mysTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_mysTab(hstmt,data,data_size, num_of_cols);
*/
  return(MDAS_SUCCESS);
}
  



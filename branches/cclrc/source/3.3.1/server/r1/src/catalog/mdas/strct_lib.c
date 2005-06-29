/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasC_db2_externs.h"
#include "mdasGlobalsExtern.h"

int
print_result(mdasC_sql_result_struct *resultinfo);
int
print_info(mdasC_sql_query_struct queryinfo);

int server_handle_count = 0;
int info_struct_count = 0;
int result_struct_count = 0;


/***************************************************************************
  NAME   : truncate_end_blanks
  PURPOSE: Truncates lagging blanks in a string
****************************************************************************/
void truncate_end_blanks(int data_size, unsigned char data[])
{
  int i;

  if (data == NULL)
        return;
  data_size = strlen((char *) data);      /**** NEW  ***/
  for (i = data_size-1; i > -1 ; i--){
    if (data[i] != ' ') {data[i+1] = '\0'; return;}
  }
  data[i+1] = '\0'; return;
}


int get_next_free_server_handle()
{
   server_handle_count++;
   if (server_handle_count == MAX_SERVERS) return (-1);
   return (server_handle_count - 1);
}
int  get_next_free_info()
{
   info_struct_count++;
   if (info_struct_count == MAX_INFO_STRUCTS) return (-1);
   return (info_struct_count - 1);
}

int  get_next_free_result()
{
   result_struct_count++;
   if (result_struct_count == MAX_INFO_STRUCTS) return (-1);
   return (result_struct_count - 1);
}

/* stripName - tab_name may be in the form of schema_name.tab_name. Strip out 
 * the schema name if it exists.
 */

char *stripName (char *tab_name) 
{
    char *tmpStr;
    

    if (tab_name == NULL)
	return NULL;

    tmpStr = tab_name;

    while (*tmpStr != '\0') {
	if (*tmpStr == '.') 
	    return (tmpStr + 1);
	tmpStr++;
    }
    return (tab_name);
}

char *get_from_result_struct(mdasC_sql_result_struct *result, 
			     char *tab_name, char *att_name)
{
  int i;
  char *tmp_tab_name1, *tmp_tab_name2;

  if (tab_name == NULL || att_name == NULL)
    return NULL;

  
  /* tab_name may be in the form of schema_name.tab_name. Strip out the schema
   * name if it exists.
   */

  tmp_tab_name1 = stripName (tab_name);

  for (i = 0; i <  result->result_count ; i++){
    tmp_tab_name2 = stripName (result->sqlresult[i].tab_name);
    if(!strcmp(tmp_tab_name2, tmp_tab_name1) &&
       !strcmp(result->sqlresult[i].att_name,att_name))
      {return ((char *) result->sqlresult[i].values);}
  }
  return (NULL);
}


int
print_result(mdasC_sql_result_struct *resultinfo)
{
  int i;

  for (i = 0; i <  resultinfo->result_count ; i++){
     printf("  %s.%s=%s\n",
	    resultinfo->sqlresult[i].tab_name,
	    resultinfo->sqlresult[i].att_name,
	    resultinfo->sqlresult[i].values);
  }

  return 0;
}

int
print_info(mdasC_sql_query_struct queryinfo)
{
  int i;

  printf("select\n");
  for (i = 0; i <  queryinfo.select_count ; i++){
     printf("  %s.%s\n",
	    queryinfo.sqlselect[i].tab_name,
	    queryinfo.sqlselect[i].att_name);
  }
  printf("where\n");
  for (i = 0; i <  queryinfo.condition_count ; i++){
     printf("  %s.%s=%s\n",
	    queryinfo.sqlwhere[i].tab_name,
	    queryinfo.sqlwhere[i].att_name,
	    queryinfo.sqlwhere[i].att_value);
  }
  return 0;

}

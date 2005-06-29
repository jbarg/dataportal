/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/****************************************************************************
** Source File Name = table_names_to_sql.c
**
**
** PURPOSE: 
**     Contains mid-level library functions to 
**     convert list of table-names to sql commands
**     using foreign key relationships to perform
**     joins.
**
** CALLABLE FUNCTIONS:
**     initialize_graph
**     convert_to_sql
**     print_sql
**
** INTERNAL FUNCTIONS USED:
**     initialize_flags_in_graph
**     find_steiner_tree_in_graph
**     transfer_in_nodes_to_out
**     make_sql
**
****************************************************************************/
/****************************************************************************
                             INCLUDES & EXTERNS
****************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "elogExtern.h"
#include "mdasC_db2_externs.h"
extern char MDASSCHEMENAME[];
extern int  distinct_flag;
extern char specialOrderByClause[];
extern int  fkrelgraph_initialized;
/****************************************************************************
                                FILE-GLOBALS
****************************************************************************/
 
typedef struct {
   char name1[200];
   char name2[200];
   char att1[120];
   char att2[300];
   int  fg1;
   int  fg2;
} mdas_metadata_graph_entry;
 
#define MAX_NO_OF_MDAS_METADATA_GRAPH_PATHS 600
#define MAX_NO_OF_TABLES 200


mdas_metadata_graph_entry 
             mdas_metadata_graph_path[MAX_NO_OF_MDAS_METADATA_GRAPH_PATHS];
char mdas_metadata_graph_in_name[MAX_NO_OF_TABLES][100];
char mdas_metadata_graph_out_name[MAX_NO_OF_TABLES][100];
char tabname1[100];
char tabname2[100];

int  mdas_metadata_graph_maxpaths;
int  mdas_metadata_graph_known = 0;
int  mdas_metadata_graph_given = 0;
char *sel_clause;
char *oth_cons;
/****************************************************************************
                                  FUNCTIONS                                  
****************************************************************************/
/***************************************************************************
  NAME   : print_graph
  PURPOSE: Print the  mdas metadata graph
****************************************************************************/

print_graph(FILE *fd)
{
   int i;
   for (i = 0; i < mdas_metadata_graph_maxpaths ; i++){
     fprintf(fd, "%s %s %s %s %3d,%3d\n",
	     mdas_metadata_graph_path[i].name1, 
	     mdas_metadata_graph_path[i].name2,
	     mdas_metadata_graph_path[i].att1, 
	     mdas_metadata_graph_path[i].att2, 
	     mdas_metadata_graph_path[i].fg1, 
	     mdas_metadata_graph_path[i].fg2);
   }
}
/***************************************************************************
  NAME   : initialize_graph
  PURPOSE: Initialize the mdas metadata graph structure from a file that
           contains the foreign key relationships among tables
****************************************************************************/
 
int
initialize_graph(char *file_name)
{
  int i;
  FILE *fd;
  int status;
  char tmpStr[300];
  fd = fopen(file_name,"r");
  if (fd == NULL)      {
    elog (NOTICE, "initialize_graph:Error in Opening File:'%s'\n",file_name);
    status = unixErrConv (errno);
    elog (NOTICE, "initialize_graph:UNIX open error. errorCode = %d", status);
    return (status);
  }

 

  for (i = 0; i < MAX_NO_OF_MDAS_METADATA_GRAPH_PATHS ; i++){
    if (fscanf(fd, "%s %s %s %s %[^\n]",
	       tabname1,
	       mdas_metadata_graph_path[i].att1, 
	       tabname2,
	       mdas_metadata_graph_path[i].att2, tmpStr) == EOF)
      break;
    if (strcmp(tmpStr,"A")) {
	strcat(mdas_metadata_graph_path[i].att2," ");
	strcat(mdas_metadata_graph_path[i].att2,tmpStr);
    }
    sprintf(mdas_metadata_graph_path[i].name1,"%s%s",MDASSCHEMENAME,tabname1);
    sprintf(mdas_metadata_graph_path[i].name2,"%s%s",MDASSCHEMENAME,tabname2);
  }

  mdas_metadata_graph_maxpaths = i;
  fclose(fd);
  fkrelgraph_initialized = 1;
  return(0);
}

/***************************************************************************
  NAME   : initialize_flags_in_graph
  PURPOSE: Initialize the flags in the mdas metadata graph structure.
           Called by convert_to_sql() before processing input
****************************************************************************/
initialize_flags_in_graph()
{
  int i;

  for (i = 0; i < mdas_metadata_graph_maxpaths ; i++){
    mdas_metadata_graph_path[i].fg1 = 0;
    mdas_metadata_graph_path[i].fg2 = 0;
  }
}


/***************************************************************************
  NAME   : get_next
  PURPOSE: Gets next ,-separated string from a parentheses enclosed list
****************************************************************************/
char* get_next(char *str, char nstr[])
{
  char ch;
  int i = 0;
  if (*str == '(' || *str ==',') str++;
  if (*str == ')') return (NULL);
  while(*str != ',' &&  *str !=  ')'){
    nstr[i] = *str;
    str++; i++;
  }
  nstr[i] = '\0';
  return (str);
}

   
  

/***************************************************************************
  NAME   : make_sql
  PURPOSE: Convert the path found by convert_to_sql() into ANSI SQL query
****************************************************************************/

make_sql(char *qs,char *other_conditions,char *select_clause)
{
   int i;
   int f =0;
   char temp[10000];
   char *str1;
   char *str2;
   char nstr1[10000];
   char nstr2[10000];

   if(distinct_flag)
     {
       if (select_clause == NULL || !strcmp(select_clause,""))
	 { sprintf (qs, "SELECT  DISTINCT *  "); }
       else { sprintf (qs, "SELECT DISTINCT %s  ", select_clause);}
     }
   else
     {
       if (select_clause == NULL || !strcmp(select_clause,""))
	 { sprintf (qs, "SELECT *  "); }
       else { sprintf (qs, "SELECT %s  ", select_clause);}
     }
   sprintf(temp, "FROM   ");
   strcat(qs,temp);
   for (i = 0; i < mdas_metadata_graph_known ; i++){
      if (f == 0)   f = 1;
      else 	    { sprintf(temp,", ");strcat(qs,temp);}
      sprintf(temp, "%s ",mdas_metadata_graph_out_name[i]);
      strcat(qs,temp);
   }

   if (other_conditions == NULL || !strcmp(other_conditions,""))
        { f = 0;}
   else { f = 1; sprintf(temp, " WHERE  %s ",other_conditions); 
          strcat(qs,temp);
        }
   for (i = 0; i < mdas_metadata_graph_maxpaths ; i++){
     if (mdas_metadata_graph_path[i].fg2 == 1){
       if (strchr(mdas_metadata_graph_path[i].att1,'(') == NULL){
	 if (f == 0) {f = 1; sprintf(temp," WHERE "); strcat(qs,temp);}
	 else 	     {sprintf(temp, " AND "); strcat(qs,temp);}
	 sprintf(temp, " %s.%s = %s.%s ",mdas_metadata_graph_path[i].name1, 
	       mdas_metadata_graph_path[i].att1,
	       mdas_metadata_graph_path[i].name2, 
	       mdas_metadata_graph_path[i].att2);
	 strcat(qs,temp);
       }
       else
       {
	 str1 = mdas_metadata_graph_path[i].att1;
	 str2 = mdas_metadata_graph_path[i].att2;
	 while( (str1 = get_next(str1, nstr1)) != NULL )
	   {
	     str2 = get_next( str2,nstr2);
	     if (f == 0) 
	       {f = 1; sprintf(temp," WHERE "); strcat(qs,temp);}
	     else 	     
	       {sprintf(temp, " AND "); strcat(qs,temp);}
	     sprintf(temp, " %s.%s = %s.%s ",
		     mdas_metadata_graph_path[i].name1, nstr1,
		     mdas_metadata_graph_path[i].name2, nstr2);
	     strcat(qs,temp);
	   }
       }
	 
     }
   }
}
		
/***************************************************************************
  NAME   : print_sql
  PURPOSE: Print the recent SQL query string generated

****************************************************************************/

print_sql(FILE *fd)
{
 char query_string[1000];

 make_sql(query_string,NULL,NULL);
 fprintf(fd, "%s \n",query_string);
}

		
/***************************************************************************
  NAME   : find_steiner_tree_in_graph
  PURPOSE: Find a steiner path that connects a given node (table_name) in
           the metada graph structure to other nodes already known.
	   This forms a steiner tree in steps. Called by convert_to_sql()
****************************************************************************/

int  isnotlast(char* key, char* key2)
{
  int i, j;

  for (j = 0; j < mdas_metadata_graph_given ; j++)
    {
      if (strcmp(mdas_metadata_graph_in_name[j],key))
	{

	  if (strcmp(mdas_metadata_graph_in_name[j],key2))
	    {
	     for ( i = 0; i < mdas_metadata_graph_known ; i++)
	       {
		 if(!strcmp(mdas_metadata_graph_in_name[j],
			   mdas_metadata_graph_out_name[i])) 
		   break;
	       }
	     if (i == mdas_metadata_graph_known) return (1);
	    }
	}
    }
  return (0);
}

int in_known_graph(char *key)
{
  int j;
  for (j = 0; j < mdas_metadata_graph_known ; j++){
	  if (!strcmp(key, 
		      mdas_metadata_graph_out_name[j]))
	    { return(1); }
  }
return (0);
}

int in_select_list(char *key)
{
  return ((int) strstr(sel_clause,key) + (int) strstr(oth_cons,key));

}
int find_steiner_tree_in_graph(char *key)
{
  int i;
  int j;
  int k;

  for (i = 0; i < mdas_metadata_graph_maxpaths ; i++){

   if (mdas_metadata_graph_path[i].fg1 != 1)
   {

    if (!strcmp(mdas_metadata_graph_path[i].name1,key))
      {

	for (j = 0; j < mdas_metadata_graph_known ; j++){
	  if (!strcmp(mdas_metadata_graph_path[i].name2, 
		      mdas_metadata_graph_out_name[j]))
	    { mdas_metadata_graph_path[i].fg1 = 1;
	      mdas_metadata_graph_path[i].fg2 = 1;
	      return(1);
	    }
	}
	/*	for (j = 0; j < mdas_metadata_graph_given ; j++){
	  if (!strcmp(mdas_metadata_graph_path[i].name2, 
		      mdas_metadata_graph_in_name[j]))
	    { mdas_metadata_graph_path[i].fg1 = 1;
	      mdas_metadata_graph_path[i].fg2 = 1;
	      if(isnotlast(key,mdas_metadata_graph_path[i].name2))
	      find_steiner_tree_in_graph(mdas_metadata_graph_path[i].name2);
	      return(1);
	    }
	}
	*/
	mdas_metadata_graph_path[i].fg1 = 1;
	if(strstr(mdas_metadata_graph_path[i].name2,"MDAS_TD")  != 0 )
	  {if (in_select_list(mdas_metadata_graph_path[i].name2) > 0) 
	    {
	      if (in_known_graph(mdas_metadata_graph_path[i].name2) > 0)
		{
		  mdas_metadata_graph_path[i].fg2 = 1;
		  return(1);
		}
	    }
	  }
	else {
	k = find_steiner_tree_in_graph(mdas_metadata_graph_path[i].name2);
	if (k == 1) {
	  mdas_metadata_graph_path[i].fg2 = 1;
	  return(1);
	}
	}
      }
      
    if (!strcmp(mdas_metadata_graph_path[i].name2,key))
      {

	for (j = 0; j < mdas_metadata_graph_known ; j++){
	  if (!strcmp(mdas_metadata_graph_path[i].name1, 
		      mdas_metadata_graph_out_name[j]))
	    {mdas_metadata_graph_path[i].fg1 = 1;
	      mdas_metadata_graph_path[i].fg2 = 1;
	      return(1);
	    }
	}
	/*
	for (j = 0; j < mdas_metadata_graph_given ; j++){
	  if (!strcmp(mdas_metadata_graph_path[i].name1, 
		      mdas_metadata_graph_in_name[j]))
	    { mdas_metadata_graph_path[i].fg1 = 1;
	      mdas_metadata_graph_path[i].fg2 = 1;
	      if(isnotlast(key,mdas_metadata_graph_path[i].name1))
	      find_steiner_tree_in_graph(mdas_metadata_graph_path[i].name1);
	      return(1);
	    }
	}
	*/
	mdas_metadata_graph_path[i].fg1 = 1;
	if(strstr(mdas_metadata_graph_path[i].name1,"MDAS_TD")  != 0 )
	  {
	    if (in_select_list(mdas_metadata_graph_path[i].name1) > 0) 
	      {
		if (in_known_graph(mdas_metadata_graph_path[i].name1) > 0)
		  {
		    mdas_metadata_graph_path[i].fg2 = 1;
		    return(1);
		  }
	      }
	  }
	else {
	k = find_steiner_tree_in_graph(mdas_metadata_graph_path[i].name1);
	if (k == 1) {
	  mdas_metadata_graph_path[i].fg2 = 1;
	  return(1);
	}
	}
      }
   }
  }
  return(0);    
}

/***************************************************************************
  NAME   : transfer_in_nodes_to_out
  PURPOSE: Marks any steiner nodes found in previous path by
           find_steiner_tree_in_graph() if they belong to input set.
	   Caled by convert_to_sql()
****************************************************************************/

void transfer_in_nodes_to_out(int k)
{
  int i;
  int j;

  for (i = 0; i < mdas_metadata_graph_maxpaths ; i++){
    mdas_metadata_graph_path[i].fg1 = 0;
    if (mdas_metadata_graph_path[i].fg2 == 1)
      {
	for (j =0; j < mdas_metadata_graph_known ; j++)
	  {
	    if(!strcmp(mdas_metadata_graph_path[i].name1, 
		       mdas_metadata_graph_out_name[j]))
	           break;
	  }
	if (j == mdas_metadata_graph_known) {
	  strcpy(mdas_metadata_graph_out_name[mdas_metadata_graph_known],
		 mdas_metadata_graph_path[i].name1);
	  mdas_metadata_graph_known++;
	}
	for (j =0; j < mdas_metadata_graph_known ; j++)
	  {
	    if(!strcmp(mdas_metadata_graph_path[i].name2,
		       mdas_metadata_graph_out_name[j]))
	           break;
	  }
	if (j == mdas_metadata_graph_known) {
	  strcpy(mdas_metadata_graph_out_name[mdas_metadata_graph_known],
		 mdas_metadata_graph_path[i].name2);
	  mdas_metadata_graph_known++;
	}
      }
  }
  for (i = k+1; i < mdas_metadata_graph_given ; i++){
    for  (j =0; j < mdas_metadata_graph_known ; j++)
	  {
	    if(!strcmp(mdas_metadata_graph_in_name[i], 
		       mdas_metadata_graph_out_name[j]))
	      {
		strcpy(mdas_metadata_graph_in_name[i],""); 
		break;
	      }
	  }
  }
  
}

/***************************************************************************
  NAME   : convert_to_sql
  PURPOSE: Converts a list of table names into an ANSI SQL query
****************************************************************************/

/*int rem_excess_td()
{
  int i,j;
  

}
 
clean_up_query()
{
  int i,j;

  i= rem_excess_td();
  if (i = 0) return;
  i= rem_excess_condition;
  j= rem_excess_from;
  if (i+j !=0)
    {
      i= rem_excess_condition;
      j= rem_excess_from;
    }
}
*/

convert_to_sql(int n, char table_names[][MAX_TOKEN], char *other_conditions,
		     char *select_clause, char *query_string)
{
 int i,j;
 char tmpStr[MAX_TOKEN];

  for (j = 0; j < MAX_NO_OF_TABLES ; j++){
    strcpy(mdas_metadata_graph_in_name[j],"");
    strcpy(mdas_metadata_graph_out_name[j],"");
  }
  initialize_flags_in_graph();
  for (j = 0; j < n ; j++){
    strcpy(mdas_metadata_graph_in_name[j],table_names[j]);
  }
  mdas_metadata_graph_given = n;
  /*  strcpy(mdas_metadata_graph_out_name[0],mdas_metadata_graph_in_name[0]);*/
  sel_clause = select_clause;
  oth_cons = other_conditions;
  /*  mdas_metadata_graph_known = 1; */
  mdas_metadata_graph_known = 0; 
  j = 0;
  /*  for (j = 1; j < mdas_metadata_graph_given ; j++){ */
      for (j = 0; j < mdas_metadata_graph_given ; j++){
      if(strcmp(mdas_metadata_graph_in_name[j] , ""))
	{
	  find_steiner_tree_in_graph(mdas_metadata_graph_in_name[j]);

	  strcpy(mdas_metadata_graph_out_name[mdas_metadata_graph_known],
		 mdas_metadata_graph_in_name[j]);
	  mdas_metadata_graph_known++;
          transfer_in_nodes_to_out(j);
	}
  }
  /* clean_up_query();*/
 make_sql(query_string, other_conditions, select_clause);

 /**** added for ordering by data_name for Sls -l ****/
 if (strlen(specialOrderByClause) == 0) {
   sprintf(tmpStr,"%s%s.%s", MDASSCHEMENAME, "MDAS_AD_REPL","offset");
   if (strstr(select_clause,tmpStr)) {
     strcat(query_string, " ORDER BY ");
     strcat(query_string,tmpStr);
   }
   else {
     sprintf(tmpStr,"%s%s.%s", MDASSCHEMENAME, "MDAS_AD_REPL","data_name");
     if (strstr(select_clause,tmpStr)) {
       strcat(query_string, " ORDER BY ");
       strcat(query_string,tmpStr);
     }
   }
 }
 else {
   strcat(query_string, " ");
   strcat(query_string,specialOrderByClause);
   strcpy(specialOrderByClause,"");
 }
 /**** added for ordering by data_name for Sls -l ****/

}



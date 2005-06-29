/*   nt_extra.c 
 *   Bing Zhu, Jan 25, 2001
 */
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include "srbType.h"


void bzero(void *s, size_t n)
{
  memset(s,0,n);
}

int strncasecmp(const char *s1, const char *s2, int n)
{
	return _strnicmp(s1,s2,n);
#if 0
  int t1,t2,t;
  char *ss1,*ss2;
  int i,result;

  if((s1==NULL)&&(s2==NULL))
    return 0;
  else if(s1 == NULL)
    return -1;
  else if(s2 == NULL)
    return 1;

  t1 = strlen(s1);
  t2 = strlen(s2);

  t = t1;
  if(t > t2)
    t = t2;

  if(t > n)
    t = n;

  ss1 = (char *)calloc(t+1,sizeof(char));
  ss2 = (char *)calloc(t+1,sizeof(char));

  strncpy(ss1,s1,t);
  ss1[t] = '\0';
  strncpy(ss2,s2,t);
  ss2[t] = '\0';

  for(i=0;i<t;i++)
  {
     if(islower(ss1[i]))
       toupper(ss1[i]);

     if(islower(ss2[i]))
       toupper(ss2[i]);
  }

  result = strcmp(ss1,ss2);
  free(ss1);
  free(ss2);

  return result;
#endif
}

int strcasecmp(const char *s1, const char *s2)
{
	return _stricmp(s1,s2);
}

void srandom(unsigned int seed)
{
  srand(seed);
}

int random(void)
{
  return rand();
}

/* The 'endptr' and 'base' are ignored. */
srb_long_t strtoll(const char *nptr, char **endptr, int base )
{
	return _atoi64(nptr);
}
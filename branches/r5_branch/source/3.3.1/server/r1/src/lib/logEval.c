/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "srb.h"
#include "dbTableDefines.h"
#ifdef _WIN32
#include "regex.h"
#endif

#ifdef PORTNAME_solaris
#include <ctype.h>
#endif

#include <math.h>

/***
#ifdef PORTNAME_aix
 #define INIT                    register char *sp=instring;
 #define GETC()                  (*sp++)
 #define PEEKC()                 (*sp)
 #define UNGETC(c)               (--sp)
 #define RETURN(c)               return(c)

 #define ERROR(c)                regerr
 #include <regexp.h>
#else
 #include <regexpr.h>
#endif 
***/


#if defined(PORTNAME_linux)
 #define INIT                    register char *sp=instring;
 #define GETC()                  (*sp++)
 #define PEEKC()                 (*sp)
 #define UNGETC(c)               (--sp)
 #define RETURN(c)               return((char *)c)
 #define ERROR(c)                return((char *)-c)
#include <regexp.h>

#else

#if defined(PORTNAME_alpha) /* True64 Alpha PSC port */ 

#define _THREAD_SAFE
/* or #define  _REENTRANT  */

#include <regex.h> 
 
int fake_tru64_getc();

#define INIT        register char *sp=instring;
#define GETC        fake_tru64_getc
#define PEEKC       (*sp)
#define UNGETC(c)   (--sp)
#define RETURN(c)   return;
#define ERROR(c)    regerror

#include <regexp.h>
/* regexp_data.loc1_r, regexp_data.loc2_r, regexp_data.locs */

#else 

#define INIT                    register char *sp=instring;
#define GETC()                  (*sp++)
#define PEEKC()                 (*sp)
#define UNGETC(c)               (--sp)
#define RETURN(c)               return;
#define ERROR                regerror
#ifndef _WIN32
#include <regexp.h>
#endif

#endif

#endif

#if !defined(PORTNAME_osx)
#ifdef _WIN32
char *loc1,*loc2;
#else

#if !defined(PORTNAME_alpha) 
extern char *loc1, *loc2, *locs;
#endif

#endif
#endif
extern int nbra, regerrno, reglength;
extern char *braslist[], *braelist[];



/**** STRING EVALUATION ****/
#define MAX_SIZE_OF_COMPILED_REG_EXP HUGH_HUGE_STRING
#define MAX_FUNC_ARGS 50

#define isperiod(c)  ( c == '.')
#define isunaryminus(c) ( c == '-')
#define isparen(c)  ( c == '(' || c == ')' || c == '[' || c == ']'  \
                    || c == '{' || c == '}')
#define isdelim(c)  (  c == '+' || c == '-' || c == '*' || c == '/' \
                    || c == '^' ||c == ','  \
		    || c == '=' || c == '&' || c == '|' || c == '!'  \
		    || c == '>' || c == '<' || c == '%'  || c == '?' )
#define isstrescape(c) (c == '\\')
#define isfirstcompchar(c) (c == '=' || c == '!' || c == '>' || c == '<' || c == '?' )
#define isfirstarithchar(c) (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' )
#define isfirststringchar(c) (c == '.' || c == '|' )
#define isfirstfuncchar(c) ( c == '$' )
#define isfuncescape(c) (c == '\\')

#define LOG 0   /* logical token */
#define NUM 1   /* numeric token */
#define STR 2   /* string  token */
#define PUN 3   /* punctuation   */
#define OPR 4   /* operator      */
#define FUN 5   /* function      */
#define BAD -1  /* bad token     */
#define EVAL_FAIL     0
#define EVAL_SUCCESS  1

char *level1();
char *level2();
char *level3();


/******** File Globals ********/
unsigned char*  inParseString;         
unsigned char*  parsedToken;            
int             tokenType = -1;
int             prevTokenType = -1;
char            errorMessage[MAX_TOKEN];




char *mycompile(char *inregExp, char *str1, char *str2)
{
#if defined(PORTNAME_osx)
  regexp *tmpStr;

  tmpStr = regcomp (inregExp);
  return ((char *) tmpStr);

#elif defined(_WIN32)
  struct re_pattern_buffer *buf;

  if(inregExp == NULL)
    return NULL;

  buf = (struct re_pattern_buffer *)calloc(1,sizeof(struct re_pattern_buffer));
  buf->buffer = malloc (MAX_SIZE_OF_COMPILED_REG_EXP);

  if(re_compile_pattern(inregExp,strlen(inregExp), buf) != 0)
  {
     free(buf->buffer);
     buf->buffer = NULL;
     free(buf);
  }

  return (char *)buf;

#else   /* PORTNAME_osx */
  char *tmpStr, *tmpStr1;

  tmpStr = malloc (MAX_SIZE_OF_COMPILED_REG_EXP);

#if defined(PORTNAME_alpha) /* True64 Alpha PSC port */ 

 /*
  char *compile_r(
		  char *instring,
		  char *expbuf,
		  char *endbuf,
		  int eof,
		  struct regexp_data *regexp_data );

  int advance_r(
		char *string,
		char *expbuf,
		struct regexp_data *regexp_data );

  int step_r(
	     char *string,
	     char *expbuf,
	     struct regexp_data *regexp_data );
  */

  tmpStr1 = compile_r (inregExp,tmpStr, 
		       &tmpStr[MAX_SIZE_OF_COMPILED_REG_EXP], '\0', NULL); /* the NULL is a stupid Tue64 NOT working fake */

#else
  tmpStr1 = compile (inregExp,tmpStr, 
		     &tmpStr[MAX_SIZE_OF_COMPILED_REG_EXP], '\0');
#endif

  if ((int) tmpStr1 >  0)  return (tmpStr);
  free(tmpStr);
  return (NULL);
#endif  /* PORTNAME_osx */

}
/*****
trimsides(char *str)
{   
  
  char *tmpPtr, *tmpPtr2, *tmpPtr3;

  tmpPtr2 = str;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  tmpPtr3 = str + strlen(str) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(++tmpPtr3) = '\0';
  memmove(str,tmpPtr2, (int) (tmpPtr3-tmpPtr2 + 1));
  return (0);
}

char *
skiptochar(char *inStr, char c)
{
  int i;
  int inQuote;
  int inDblQuote;
  inQuote = 0;
  inDblQuote = 0;

  while (*inStr != c) {
    if (inQuote) {
      if (*inStr == '\'' && *(inStr+1) !=  '\'') inQuote = 0;
      else if (*inStr == '\'' && *(inStr+1) ==  '\'') inStr++;
    }
    else if (inDblQuote) {
      if (*inStr == '\"' && *(inStr+1) !=  '\"') inDblQuote = 0;
      else if (*inStr == '\"' && *(inStr+1) ==  '\"') inStr++;
    }
    else {
      if (*inStr == '\'') inQuote = 1;
      else if (*inStr == '\"') inDblQuote = 1;
      else if (*inStr == '\0')
	return(NULL);
    }
    inStr++;
  }
  return(inStr);
}
****/
char *
skiptochar(char *inStr, char c)
{
  int inQuote = 0;
  int inDblQuote = 0;
  int inParen = 0;
  int inBracket = 0;
  int inBrace = 0;

  while (*inStr != c) {
    if (inQuote) {
      if (*inStr == '\'' && *(inStr+1) !=  '\'') inQuote = 0;
      else if (*inStr == '\'' && *(inStr+1) ==  '\'') inStr++;
    }
    else if (inDblQuote) {
      if (*inStr == '\"' && *(inStr+1) !=  '\"') inDblQuote = 0;
      else if (*inStr == '\"' && *(inStr+1) ==  '\"') inStr++;
    }
    else {
      if (*inStr == '\'') inQuote = 1;
      else if (*inStr == '\"') inDblQuote = 1;
      else if (*inStr == '(') inParen++;
      else if (*inStr == ')') inParen--;
      else if (*inStr == '[') inBracket++;
      else if (*inStr == ']') inBracket--;
      else if (*inStr == '{') inBrace++;
      else if (*inStr == '}') inBrace--;
      else if (*inStr == '\0')
	return(NULL);
    }
    inStr++;
    if (inParen < 0 || inBracket < 0 || inBrace < 0 )
      return(NULL);
  }
    if (inParen != 0 || inBracket != 0 || inBrace != 0 )
      return(NULL);
  return(inStr);
}

char *evaluateFunctionInEvaluable(char *fnName, char *fnArgVal[],
			    int fnArgTyp[], int fnArgCnt)
{
  char *r;
  char tmpStr[MAX_TOKEN];
  double f,g,h;

  if (!strcmp("convtoint",fnName)){
    sprintf(tmpStr ,"%i", atoi(fnArgVal[0]));
    r =  strdup(tmpStr);
    return(r);
  }
  else if (!strcmp("pow",fnName)){
    f = atof(fnArgVal[0]);
    h = atof(fnArgVal[1]);
    g = pow(f,h);
    /**
    i = atoi(fnArgVal[1]);
    g = 1.0;
    for (j = 0 ; j < i ; j++)
      g *= f;
    **/
    sprintf(tmpStr ,"%f",g);
    r =  strdup(tmpStr);
    return(r);
  }
  return(NULL);
}


int stringParse()
{
   unsigned char* t;

   t = parsedToken;
   prevTokenType = tokenType;
   while( isspace( *inParseString ) )
     inParseString++;
   if( isparen( *inParseString ) )  {
     *t++ = *inParseString++;
     tokenType = PUN;
   }
   else if( isdelim( *inParseString ) )  {
     *t++ = *inParseString++;
     while (isdelim( *inParseString ) )
       *t++ = *inParseString++;
     tokenType = OPR;
   }
   else if( isdigit( *inParseString ) || isunaryminus( *inParseString ) ||
	     (isperiod( *inParseString ) && isdigit( *(inParseString+1)))) {
     while( isunaryminus( *inParseString ) || 
	    isdigit( *inParseString ) || isperiod( *inParseString ) )
       *t++ = *inParseString++;
     tokenType = NUM;
   }
   else if(  *inParseString == '\'' )  {
     inParseString++;
     while( *inParseString != '\''  ) {
       if (isstrescape(*inParseString ) )  inParseString++;
       *t++ = *inParseString++;
     }
     inParseString++;
     tokenType = STR;
   }
   else if(  *inParseString == '\"' )  {
     inParseString++;
     while( *inParseString != '\"'  ) {
       if (isstrescape(*inParseString ) )  inParseString++;
       *t++ = *inParseString++;
     }
     inParseString++;
     tokenType = STR;
   }
   else if( isfirstfuncchar( *inParseString ) )  {
     inParseString++;
     while (*inParseString != '(') 
       *t++ = *inParseString++;
     inParseString++;
     tokenType = FUN;
   }
   else if( *inParseString )   {
     /** skip until space */
     while( !isspace( *inParseString ) )
       inParseString++;
     tokenType = BAD;

   }
   *t = 0;
   while( isspace( *inParseString ) )
     inParseString++;
   return (0);
}

char * level1()
{
  char *r, *t, o;
  int i,j;
   r  = level2();
   if (r == NULL) return(r);
   while ( (((o = *parsedToken) == '&') && (*(parsedToken+1) == '&')) ||
	  (((o = *parsedToken) == '|') && (*(parsedToken+1) == '|'))  )   {
     i = atoi(r);
     if  (strcmp(r,"0") && strcmp(r,"1"))  return(r);
     stringParse();
     t = level2();
     if (t == NULL ||(strcmp(t,"0") && strcmp(t,"1")) ) {
       free(r);
       return(t);
     }
     j = atoi(t);
     free(t);
     tokenType = LOG;
     if( o == '&' ) {
       if (i <= 0 || j <= 0) strcpy(r,"0");
       else          strcpy(r,"1");
     }
     else {
       if (i > 0 || j > 0) strcpy(r,"1");
       else         strcpy(r,"0");
     }
   }
   return(r);
}

char* level2()
{
  char *r, *s, *t, *u, oo[10], *regexpbuf;
  int i;
  int tempTokenType;
  double ii,jj,tt;

   r  = level3();
   if (r == NULL) return(r);
   while (isfirstcompchar(*parsedToken))  {
     strcpy(oo, (char *) parsedToken);
     tempTokenType = prevTokenType;
     stringParse();
     t = level3();
     if (t == NULL ) {
	free(r);
	return(t);
      }
     tokenType = LOG;
     if (prevTokenType == NUM && tempTokenType == NUM) {
       ii = (double) atof(r);
       jj = (double) atof(t);
       free(r);
       free(t);
       if (ii == jj) {
	 if (!strcmp(oo,"==") || !strcmp(oo,">=") || !strcmp(oo,"<="))
	   r = (char *) strdup("1");
	 else 
	   r = (char *)strdup("0");
       }
       else if (ii > jj) {
	 if (!strcmp(oo,">") || !strcmp(oo,">=") || !strcmp(oo,"!="))
	   r = (char *) strdup("1");
	 else 
	   r = (char *)strdup("0");
       }
       else if (ii < jj) {
	 if (!strcmp(oo,"<") || !strcmp(oo,">=") || !strcmp(oo,"!="))
	   r = (char *)strdup("1");
	 else 
	   r = (char *)strdup("0");
       }
     }
     else if (!strcmp(oo,"?")) {
        /* Use matchCompRegExp instead of step because of PORTNAME_osx */
        char *startPtr, *endPtr;

       if((regexpbuf = mycompile(t, NULL, NULL)) != NULL) {
         if (matchCompRegExp( regexpbuf, r, &startPtr, &endPtr) == MDAS_SUCCESS)
	   s = (char *)strdup("1");
	 else
	   s = (char *)strdup("0");
	 free(regexpbuf);
	 free(r);
	 free(t);
	 r = s;	 
       }
       /***
       if (strstr(r,t) == NULL) i = 0;
       else i = 1;
       free(r);
       free(t);
       if (i == 0) 
	 r = (char *)strdup("0");
       else
	 r = (char *)strdup("1");
       ***/
     }
     else  {
       i = strcmp(r,t);
       free(r);
       free(t);
       if (i == 0) {
	 if (!strcmp(oo,"==") || !strcmp(oo,">=") || !strcmp(oo,"<="))
	   r = (char *)strdup("1");
	 else 
	   r = (char *)strdup("0");
       }
       else if (i > 0) {
	 if (!strcmp(oo,">") || !strcmp(oo,">=") || !strcmp(oo,"!="))
	   r = (char *)strdup("1");
	 else 
	   r = (char *)strdup("0");
       }
       else if (i < 0) {
	 if (!strcmp(oo,"<") || !strcmp(oo,"<=") || !strcmp(oo,"!="))
	   r = (char *)strdup("1");
	 else 
	   r = (char *)strdup("0");
       }
     }
    }
    while (isfirststringchar(*parsedToken))  {
     strcpy(oo, (char *) parsedToken);
     tempTokenType = prevTokenType;
     stringParse();
     t = level3();
     if (t == NULL ) {
	free(r);
	return(t);
      }
     tokenType = STR;
     if (!strcmp(oo,".")) {
       u =  malloc(strlen(r) + strlen(t) + 1);
       sprintf(u,"%s%s",r,t);
       free(r);
       free(t);
       r = u;
     }
     else if (!strcmp(oo,"|h")) { /* head string upto t length */
       jj = atof(t);
       free(t);
       if (jj < strlen(r))
	 *(r + (int) jj)  = '\0';
     }
     else if (!strcmp(oo,"|t")) { /* tail starting at t */
       jj = atof(t);
       free(t);
       if (jj < strlen(r))
	 memmove(r,r+ (int)jj, strlen(r) - jj+1);
       else 
	 *r = '\0';
     }
     
    }
    while (isfirstarithchar(*parsedToken))  {
     strcpy(oo, (char *) parsedToken);
     tempTokenType = prevTokenType;
     stringParse();
     t = level3();
     if (t == NULL ) {
	free(r);
	return(t);
      }
     tokenType = NUM;
     ii = atof(r);
     jj = atof(t);
     free(r);
     free(t);
     if (!strcmp(oo,"+"))
       tt = ii + jj;
     else if (!strcmp(oo,"-"))
       tt = ii - jj;
     else if (!strcmp(oo,"/"))
       tt = ii / jj;
     else if (!strcmp(oo,"*"))
       tt = ii * jj;
     else if (!strcmp(oo,"%"))
       tt = ((int) ii) % ((int) jj);
      r =  (char *) malloc(MAX_TOKEN);
      sprintf(r,"%f",tt);
    }
    return(r);
}

char* level3()
{
   char *r;
   char *fnName;
   char *fnArgVal[MAX_FUNC_ARGS];
   int  fnArgTyp[MAX_FUNC_ARGS];
   int fnArgCnt;
   int tempTokenType;

   if( *parsedToken == '(' ) {
     stringParse();
     r = level1();
     tempTokenType =  prevTokenType;
     if( *parsedToken != ')' )  return(NULL);
     prevTokenType = tempTokenType;
     stringParse();
     return(r);
   }
   else if( *parsedToken == '[' ) {
     stringParse();
     r = level1();
     tempTokenType =  prevTokenType;     
     if( *parsedToken != ']' )  return(NULL);
     prevTokenType = tempTokenType;
     stringParse();
     return(r);
   }
   else if( *parsedToken == '{' ) {
     stringParse();
     r = level1();
     tempTokenType =  prevTokenType;
     if( *parsedToken != '}' )  return(NULL);
     prevTokenType = tempTokenType;
     stringParse();
     return(r);
   }
   else if ( tokenType == FUN) {
     fnArgCnt = 0;
     fnName = strdup((char *) parsedToken);
     tempTokenType =  prevTokenType;
     stringParse();
     r = level1();
     while ( *parsedToken == ',' ) {
       fnArgVal[fnArgCnt] =  r;
       fnArgTyp[fnArgCnt++] = prevTokenType;
       stringParse();
       r = level1();
     }
     
     if (*parsedToken !=  ')') return(NULL);
     fnArgVal[fnArgCnt] =  r;
     fnArgTyp[fnArgCnt++] = prevTokenType;     
     r = evaluateFunctionInEvaluable(fnName,fnArgVal,fnArgTyp,fnArgCnt); 
     prevTokenType = tempTokenType;
     stringParse();
     return(r);
   }
   else {
     r = (char *)strdup((char *) parsedToken );
     stringParse();
     return(r);
   }
}



int
compileRegExp( char **compiledRE,  char *inregExp)
{
  char *regexpbuf;

  regexpbuf = mycompile(inregExp, NULL, NULL);
  if (regexpbuf == NULL) {
    elog (NOTICE, "compileRegExp: Error  in compiling Regular Expression:%s",  inregExp);
    return (RE_EVALUATION_ERROR);
  }
  *compiledRE = regexpbuf;
  return(MDAS_SUCCESS);
}

int
matchCompRegExp( char *compiledRE, char *inStr, 
	     char **startPtr, char **endPtr)
{
#if defined(PORTNAME_osx)
  const regexp *myregexp;

  myregexp = (const regexp *) compiledRE;
  if (regexec (myregexp, inStr) == 0) {
    return(RE_MATCH_NOT_FOUND);
  } else {
    *startPtr = myregexp->startp[0];;
    *endPtr   = myregexp->endp[0];;
    return(MDAS_SUCCESS);
  }

#elif defined(_WIN32)
  struct re_pattern_buffer *buf;
  struct re_registers *regs;
  int t1,t2;

  buf = (struct re_pattern_buffer *)compiledRE;

  regs = (struct re_registers *)calloc(1,sizeof(struct re_registers));
  if(re_match(buf, inStr, strlen(inStr),0, regs) < 0)
  {
     free(regs);
     return RE_MATCH_NOT_FOUND;
  }

  t1 = regs->start[0];
  t2 = regs->end[0];

  *startPtr = &(inStr[t1]);
  *endPtr = &(inStr[t2]);

  free(regs);

  return MDAS_SUCCESS;
  

#else   /* PORTNAME_osx */

#if defined(PORTNAME_alpha) /* True64 Alpha PSC port */ 
  /*
  char *compile_r(
		  char *instring,
		  char *expbuf,
		  char *endbuf,
		  int eof,
		  struct regexp_data *regexp_data );

  int advance_r(
		char *string,
		char *expbuf,
		struct regexp_data *regexp_data );

  int step_r(
	     char *string,
	     char *expbuf,
	     struct regexp_data *regexp_data );
  */

  if (step_r(inStr, compiledRE, NULL) == 0) 
     /* the NULL is a stupid Tue64 NOT working fake */
    return(RE_MATCH_NOT_FOUND); 

  { 
    struct regexp_data *__regexp_data; 
 
    *startPtr = _LOC1; 
    *endPtr   = _LOC2; 
  }
 
#else
  if (step(inStr, compiledRE) == 0)
    return(RE_MATCH_NOT_FOUND);
  *startPtr = loc1;
  *endPtr   = loc2;
#endif

  return(MDAS_SUCCESS);
#endif  /* PORTNAME_osx */
}



char *
evaluateArithmeticStringExpression( char* e)
{
   char *r, *strPtr;
   inParseString = (unsigned char *) strdup(e);
   strPtr = (char *) inParseString;
   parsedToken = (unsigned char *) malloc(MAX_TOKEN);
   tokenType = -1;
   stringParse();
   r = level1();
   if (parsedToken[0] != '\0') {
     free(strPtr);
     free(parsedToken);
     sprintf(errorMessage,"%i",LOGICAL_EXPRESSION_EVALUATION_ERROR);
     return(errorMessage);
   }
   free(strPtr);
   free(parsedToken);
   /*   j = 0;
   for (i = 0; i < strlen(r); i++) {
     if (isdigit(r[i])) continue;
     else if (isperiod(r[i])) { j = 1; continue;}
     else break;
   }
   if (i <  strlen(r)) return(r);
   if (j == 0)  return(r);
   dd = (double) atof(r);
   if (dd == (int)dd/1) sprintf(r,"%i",(int)dd/1); */
   return(r);
}
int
evaluateLogicalExpression( char* e)
{
   char *r, *strPtr;
   int i;
   inParseString = (unsigned char *) strdup(e);
   strPtr = (char *) inParseString;
   parsedToken = (unsigned char *) malloc(MAX_TOKEN);
   tokenType = -1;
   stringParse();
   r = level1();
   if (parsedToken[0] != '\0') {
     free(strPtr);
     free(parsedToken);
     return(LOGICAL_EXPRESSION_EVALUATION_ERROR);
   }
   free(strPtr);
   free(parsedToken);
   if (r == NULL || (strcmp(r,"0") && strcmp(r,"1"))) {
     if (r != NULL) free(r);
     return(LOGICAL_EXPRESSION_EVALUATION_ERROR);
   }
   else {
     i =  atoi(r);
     free(r);
     if (i > 0) return (1);
     else return (0);
   }
}

int
evaluableCond(char *condition)
{
  if (strstr(condition,"$$$") != NULL) return EVAL_FAIL;
  if (strstr(condition,"+++") != NULL) return EVAL_FAIL;
  if (strstr(condition,"###") != NULL) return EVAL_FAIL;
  if (strstr(condition,"@@@") != NULL) return EVAL_FAIL;
  if (strstr(condition,"!!!") != NULL) return EVAL_FAIL;
  if (strstr(condition,"%%%") != NULL) return EVAL_FAIL;

  return (EVAL_SUCCESS);

  
}

int
evaluateForCondition(char *condition, 
		     char IVarName[][VAR_NAME_SIZE], 
		     char IVarVal[][VAR_VAL_SIZE], 
		     int *InIVarCnt )
{
  int i,res;
  char *tmpPtr;
  char *tmpPtr2;
  char *tmpPtr3;
  char fillStr[HUGE_STRING];
  char *cond1;
  char *cond2;
  char *cond3;
  char  cond2s[HUGE_STRING];
  char  cond3s[HUGE_STRING];
  char JVarVal[MAX_VAR_COUNT][VAR_NAME_SIZE];
  char *evalPtr;
  int IVarCnt;
  int lastItem;
  *InIVarCnt = 0;

  IVarCnt =  0;
  if ((tmpPtr2 = strchr(condition,';')) ==NULL)
      return (EXPRESSION_SYNTAX_ERROR);
  *tmpPtr2 = '\0';
  cond1 = condition;
  if ((tmpPtr = strchr(tmpPtr2 + 1,';')) ==NULL)
      return (EXPRESSION_SYNTAX_ERROR);
  *tmpPtr = '\0';
  cond2 = tmpPtr2 + 1;
  strcpy(cond2s,tmpPtr2 + 1);
  cond3 = tmpPtr + 1;
  strcpy(cond3s,tmpPtr + 1);

  lastItem = 1;
  while (((tmpPtr3 = skiptochar(cond1,',')) != NULL) || (lastItem ==1)) {
    if (tmpPtr3 != NULL)
      *tmpPtr3 = '\0';
    else
      lastItem = 0;
    if ((tmpPtr2  = skiptochar(cond1,'=')) == NULL)
      return(EXPRESSION_SYNTAX_ERROR);
    *tmpPtr2 = '\0';
    tmpPtr2++;
    trimsides(cond1);
    trimsides(tmpPtr2);
    evalPtr = evaluateArithmeticStringExpression(tmpPtr2);
    strcpy(IVarName[IVarCnt] , cond1);
    strcpy(IVarVal[IVarCnt] , evalPtr);
    strcpy(JVarVal[IVarCnt] , evalPtr);
    IVarCnt++;
    free(evalPtr);
    if (tmpPtr3 != NULL)
      cond1 = tmpPtr3+1;
  }
  fillInIVarValue(cond2,fillStr,IVarName, IVarVal,IVarCnt);
  res = evaluateLogicalExpression(fillStr);
  if (res != 1) {*InIVarCnt = IVarCnt; return(res);}
  lastItem = 1;
  while (((tmpPtr3 = skiptochar(cond3,',')) != NULL) || (lastItem ==1)) {
    if (tmpPtr3 != NULL)
      *tmpPtr3 = '\0';
    else
      lastItem = 0;
    if ((tmpPtr2  = skiptochar(cond3,'=')) == NULL)
      return(EXPRESSION_SYNTAX_ERROR);
    *tmpPtr2 = '\0';
    tmpPtr2++;
    trimsides(cond3);
    trimsides(tmpPtr2);
    fillInIVarValue(tmpPtr2,fillStr,IVarName, IVarVal,IVarCnt);
    evalPtr = evaluateArithmeticStringExpression(fillStr);
    for (i = 0; i < IVarCnt ; i++) {
      if (!strcmp(IVarName[i], cond3)) {
	strcpy(JVarVal[i] , evalPtr);
	break;
      }
    }
    if (i == IVarCnt) {
      strcpy(IVarName[IVarCnt] , cond3);
      strcpy(IVarVal[IVarCnt] , evalPtr);
      strcpy(JVarVal[IVarCnt] , evalPtr);
      IVarCnt++;
    }
    free(evalPtr);
     if (tmpPtr3 != NULL)
       cond3 = tmpPtr3+1;
  }
  condition[0] ='\0';
  for (i = 0; i < IVarCnt ; i++) {
    sprintf(&condition[strlen(condition)],"%s=%s",
	    IVarName[i], JVarVal[i]);
  }
  sprintf(&condition[strlen(condition)],";%s;%s",cond2s,cond3s);
  *InIVarCnt = IVarCnt; 
  return(res);
}

#if defined(PORTNAME_alpha) /* True64 Alpha PSC port */ 

/* fake INIT */
char *sp;

int fake_tru64_getc() {
  extern char *sp;

  return((*sp++));
}

#endif

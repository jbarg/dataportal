%{
#include <stdio.h>
#include <string.h> 
#include "y.tab.h"

int Lineno;

%}

/* Definition  */

quotestr "\""([^"]|\\\")*"\""      
mnenomic [A-Z]+[A-Z0-9_-]*
digits [\-]?[0-9]+ 
comma	","
ldelim "{"
rdelim "}"
comment "#"[^\n]*


/* Rules */
%%
{quotestr}		{
				char *p;
				int i;
				char buf[10000];

				strcpy(buf, yytext);
				
				/* Slashify embedded newlines */
				for (i = 0, p = yytext; *p != '\0'; p++, i++)
					{
					if ( *p == '\n' )
						{
						*(yylval.string + i) = '\\';
						i++;
						*(yylval.string + i) = '\n';
						}
					else
						*(yylval.string + i) = *p;
					}
				*(yylval.string + i ) = '\0';

				return(QUOTESTR);
				}

{mnenomic}		{
				strcpy(yylval.string, yytext);
				return(MNENOMIC);
				}

{comma}			{
				return(S_COMMA);
				}

{ldelim}		{
				return(S_LDELIM);
				}

{rdelim}		{
				return (S_RDELIM);
				}

{digits}  		{
				strcpy(yylval.string, yytext);
				return(ERRID);
				}

{comment}   {} /* Eat comments. */
[ \t]+		{} /* Stuff outside quotes or whitespace is not needed. */
[\n]		{  
			Lineno++;
			}

.			{}

%%


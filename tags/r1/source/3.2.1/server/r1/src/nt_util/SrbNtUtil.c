/*******************************************************************
 SrbNtUtil.c
 *******************************************************************/
#include <string.h>
#include "SrbNtUtil.h"
#include "misc_util.h"
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include "stdlib.h"
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>

int nRunInConsole=0;

/* The function is used to convert unix path delimiter,slash, to
   windows path delimiter, back slash. 
 */
static void StrChangeChar(char* str,char from, char to)
{
	int n,i;

	if(str == NULL)
		return;

	n = strlen(str);

	for(i=0;i<n;i++)
	{
		if(str[i] == from)
			str[i] = to;
	}
}

void SrbNtPathBackSlash(char *str)
{
	StrChangeChar(str,'/','\\');
}

void SrbNtPathForwardSlash(char *str)
{
	StrChangeChar(str,'\\','/');
}

FILE *SrbNt_fopen(const char *filename, const char *mode)
{
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);

	return fopen(filename,mode);
}

void SrbPathToNtPath(char *ntpath,const char *srbpath)
{
	char buff[1024];

	if(StrLen(srbpath) <= 3)
	{
		strcpy(buff,srbpath);
	}
	else if((srbpath[0] == '/')&&(srbpath[2] == ':'))  /* the path has form /D:/tete/... */
	{
		strcpy(buff,&(srbpath[1]));
	}
	else if((srbpath[0] == '\\')&&(srbpath[2] == ':'))
	{
		strcpy(buff,&(srbpath[1]));
	}
	else
	{
		strcpy(buff,srbpath);
	}

	SrbNtPathBackSlash(buff);
	strcpy(ntpath,buff);
}

int SrbNtFileOpen(const char *filename,int oflag, int istextfile)
{
	int New_Oflag;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);

	if(istextfile)
		New_Oflag = oflag;
	else
		New_Oflag = _O_BINARY | oflag;

	if(New_Oflag & _O_CREAT)
	{
		return _open(ntfp,New_Oflag,_S_IREAD|_S_IWRITE);
	}

	return _open(ntfp,New_Oflag);
}

int SrbNtFileBinaryOpen(const char *filename,int oflag)
{
	int New_Oflag;
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);

	New_Oflag = _O_BINARY | oflag;
	return _open(ntfp,New_Oflag);
}

int SrbNtFileBinaryCreate(const char *filename)
{
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);

	return _open(ntfp,_O_RDWR|_O_CREAT|_O_EXCL|_O_BINARY, _S_IREAD|_S_IWRITE);
}

int SrbNtUnlinkFile(char *filename)
{
	char ntfp[1024];

	SrbPathToNtPath(ntfp,filename);

	return _unlink(ntfp);
}

void SrbNtCheckExecMode(int aargc,char **aargv)
{
	int i;

	nRunInConsole = 0;
	for(i=0;i<aargc;i++)
	{
		if(strcmp(aargv[i],"console") == 0)
		{
			nRunInConsole = 1;
			return;
		}
	}
}

int SrbNtRunInConsoleMode()
{
	return nRunInConsole;
}

void NtEmergencyMessage(char *msg)
{
   int fd;
   fd = open("c:\\ntsrblog.txt",_O_CREAT|_O_APPEND|_O_WRONLY, _S_IREAD | _S_IWRITE);

   if(fd < 0)
     return;

   _write(fd,msg,strlen(msg));
   _close(fd);
}

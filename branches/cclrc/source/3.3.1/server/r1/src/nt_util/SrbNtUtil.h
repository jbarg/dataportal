/*******************************************************************
 SrbNtUtil.h
 *******************************************************************/
#ifndef _SrbNtUtil_h__
#define _SrbNtUtil_h__

#ifdef _WIN32

#include <stdio.h>

#define SRB_SERVICE_NAME	"SRBServer"
#define SRB_NT_LOG_FILE     "srbLog"

/* constant/data structure defs */

/*  function prototypes */
#if defined(__cplusplus)
extern "C"
{
#endif

void SrbNtPathBackSlash(char *str);
void SrbNtPathForwardSlash(char *str);
FILE *SrbNt_fopen(const char *filename, const char *mode);
int SrbNtFileOpen(const char *filename,int oflag, int istextfile);
int SrbNtFileBinaryOpen(const char *filename,int oflag);
int SrbNtFileBinaryCreate(const char *filename);
int SrbNtGetServerExecPathFromRegistry(char *pExecPath);
int SrbNtFindServerExec(char *pFileNameWP);
int SrbNtRunService(int aargc,char **aargv);
int SrbNtRunInConsoleMode();
void SrbPathToNtPath(char *ntpath,const char *srbpath);
void SrbNtCheckExecMode(int aargc,char **aargv);
void NtEmergencyMessage(char *msg);

#if defined(__cplusplus)
}
#endif

#endif /* _WIN32 */

#endif /* _SrbNtUtil_h__ */


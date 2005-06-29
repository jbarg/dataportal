/* misc_util.h */
#ifndef __misc_util_h__
#define __misc_util_h__

#include <stdlib.h>
#include <stdio.h>


#if defined(__cplusplus)
extern "C"
{
#endif

int StrLen(const char* str);
void FreeStrArray(int nCnt,char** pStrs);
FILE *fp_fileopen(char *filename,char *mode);
char *get_home_env();
int stat_file(const char *filename,struct stat *stat_p);
#ifdef _WIN32
int mk_dir(char *dir,int mode);
#else
int mk_dir(char *dir,mode_t mode);
#endif

#if defined(__cplusplus)
}
#endif

#endif
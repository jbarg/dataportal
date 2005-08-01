/* strlib.c */
#include "misc_util.h"
#include <string.h>
#ifdef _WIN32
#include "SrbNtUtil.h"
#endif
#include <sys/stat.h>
#include <direct.h>

int StrLen(const char* str)
{
	if(str == NULL)
		return 0;

	return strlen(str);
}

void FreeStrArray(int nCnt,char** pStrs)
{
	int i;

	if((nCnt <= 0) || (pStrs == NULL))
		return;

	for(i=0;i<nCnt;i++)
	{
		if(pStrs[i])
		{
			free(pStrs[i]);
			pStrs[i] = NULL;
		}
	}

	free(pStrs);
}

FILE *fp_fileopen(char *filename,char *mode)
{
	FILE *fp;
#ifdef _WIN32
	fp = SrbNt_fopen(filename,mode);
#else
	fp = fopen(filename,mode);
#endif

	return fp;
}

char *get_home_env()
{
#ifdef _WIN32
	return getenv("HOMEDRIVE");
#else
	return getenv ("HOME");
#endif
}

int stat_file(const char *filename,struct stat *stat_p)
{
#ifdef _WIN32
	char ntfp[1024];
	SrbPathToNtPath(ntfp,filename);
	return stat(ntfp,stat_p);
#else
	return stat(filename,stat_p);
#endif
}

#ifdef _WIN32
int mk_dir(char *dir,int mode)
#else
int mk_dir(char *dir,mode_t mode)
#endif
{
#ifdef _WIN32
	char ntfp[1024];
	SrbPathToNtPath(ntfp,dir);
	return _mkdir(ntfp);
#else
	return mkdir(dir,mode);
#endif
}
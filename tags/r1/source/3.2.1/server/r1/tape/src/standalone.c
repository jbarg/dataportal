#include "srbTapeMiscExtern.h"

tapeLibConfig_t *TapeLibConfigHead = NULL;

char * getSrbDataDir();
#ifdef foo
int
srbThreadSleep (int sec, int microSec);

int
srbThreadSleep (int sec, int microSec)
{
    struct timeval sleepTime;

    sleepTime.tv_sec = sec;
    sleepTime.tv_usec = microSec;

    select (0, NULL, NULL, NULL, &sleepTime);

    return 0;
}
#endif

/* getSrbDataDir - get the "data" directory (where configuration files are
 * kept). It will use the path defined in the srbData env variable first.
 * If it is not defined, use the constant string SRB_DATADIR which is
 * defined in the Makefile.
 *
 * Input - None
 *
 * Output - The "data" directory path
 */

char *
getSrbDataDir()
{
    char *p;

    if ((p = (char *) getenv("srbData")) != (char *) NULL) {
        return (p);
    }
    return (SRB_DATADIR);       /* SRB_DATADIR defined in the Makefile */
}


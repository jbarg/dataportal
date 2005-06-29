/* srbTapeMisc.h
 */

#ifndef SRB_TAPE_MISC_H
#define SRB_TAPE_MISC_H

#include "srb.h"
#include "srbTapeMiscExtern.h"

extern tapeLibConfig_t *TapeLibConfigHead;

tapeMediaChar_t TapeMediaCharTable[] = {
    {T_3480, "T_3480", 32*1024,  200*1024, GENERIC_TAPE_DRV},
    {T_3590, "T_3590", 256*1024, 20*1024*1024, GENERIC_TAPE_DRV}
};

int NumTapeMediaChar =
    (sizeof(TapeMediaCharTable) / sizeof(struct tapeMediaChar));

typedef struct cartConfig {
    char tapeNum[TAPE_TYPE_LEN];
    int clientSock;     /* The socket to use to reply client's mount req */
    int clientPid;      /* The Pid of the client proccess */

} cartConfig_t;

tapeDrvConfig_t *TapeDrvConfig;
int NumTapeDrv = 0;

extern char *getSrbDataDir();

#endif	/* SRB_TAPE_MISC_H */


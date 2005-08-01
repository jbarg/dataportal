#ifndef TAPE_DRV_SW_H
#define TAPE_DRV_SW_H

#include "srb.h"
#include "srbTapeMiscExtern.h"
#include "genTapeExtern.h"

/* The driver functions */

typedef struct tapeDriver {
    int (*initTape)();
    int (*openTape)();
    int (*closeTape)();
    int (*seekTape)();
    int (*writeEOF)();
} tapeDvr_t;

static tapeDvr_t tapeDvrTable[] = {
    /* generic drivers */
    {genInitTape, genOpenTape, genCloseTape, genSeekTape, genWriteEOF}
}; 

/* Number of sw tables */

int ntapeDvrEntries =
  (sizeof(tapeDvrTable) / sizeof(tapeDvr_t));

#endif	/* TAPE_DVR_SW_H */

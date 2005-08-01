/* tapeDvrSwExtern.h 
 */

#ifndef TAPE_DRV_SW_EXTERN_H
#define TAPE_DRV_SW_EXTERN_H

int swInitTape (int which, tapeDev_t *tapeDev);
int swOpenTape (int which, tapeDev_t *tapeDev, int flag);
int swSeekTape (int which, tapeDev_t *tapeDev, tapePos_t *destPos);
int swCloseTape (int which, tapeDev_t *tapeDev, int unloadFlag);
int swWriteEOF (int which, tapeDev_t *tapeDev);

#endif	/* TAPE_DRV_SW_EXTERN_H */


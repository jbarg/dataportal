/* genTapeExtern.h */

#ifndef GEN_TAPE_EXTERN_H
#define GEN_TAPE_EXTERN_H

/* prototype for genTape.c */
int genInitTape (tapeDev_t *tapeDev);
int genOpenTape (tapeDev_t *tapeDev, int flag);
int genSeekTape (tapeDev_t *tapeDev, tapePos_t *destPos);
int genCloseTape (tapeDev_t *tapeDev, int unloadFlag);
int genWriteEOF (tapeDev_t *tapeDev);
int openRewindTape (tapeDev_t *tapeDev, int flag);

#endif	/* GEN_TAPE_EXTERN_H */

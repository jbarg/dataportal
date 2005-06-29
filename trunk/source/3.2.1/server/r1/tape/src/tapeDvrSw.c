/* tapeDvrSw.c */

#include "tapeDvrSw.h"

int swInitTape (int which, tapeDev_t *tapeDev)
{
    int retVal;

    if (which < 0 || which >= ntapeDvrEntries) {
        /* elog (NOTICE, "The tape driver inx %d is not supported", which); */
        return INP_ERR_NO_SUPPORT;
    }
    retVal = (*(tapeDvrTable[which].initTape))(tapeDev);

    return (retVal);
}

int swOpenTape (int which, tapeDev_t *tapeDev, int flag)
{
    int retVal;

    if (which < 0 || which >= ntapeDvrEntries) {
        /* elog (NOTICE, "The tape driver inx %d is not supported", which); */
        return INP_ERR_NO_SUPPORT;
    }
    retVal = (*(tapeDvrTable[which].openTape))(tapeDev, flag);

    return (retVal);
}

int swSeekTape (int which, tapeDev_t *tapeDev, tapePos_t *destPos)
{
    int retVal;

    if (which < 0 || which >= ntapeDvrEntries) {
        /* elog (NOTICE, "The tape driver inx %d is not supported", which); */
        return INP_ERR_NO_SUPPORT;
    }
    retVal = (*(tapeDvrTable[which].seekTape))(tapeDev, destPos);

    return (retVal);
}

int swCloseTape (int which, tapeDev_t *tapeDev, int unloadFlag)
{
    int retVal;

    if (which < 0 || which >= ntapeDvrEntries) {
        /* elog (NOTICE, "The tape driver inx %d is not supported", which); */
        return INP_ERR_NO_SUPPORT;
    }
    retVal = (*(tapeDvrTable[which].closeTape))(tapeDev, unloadFlag);

    return (retVal);
}

int swWriteEOF (int which, tapeDev_t *tapeDev)
{
    int retVal;

    if (which < 0 || which >= ntapeDvrEntries) {
        /* elog (NOTICE, "The tape driver inx %d is not supported", which); */
        return INP_ERR_NO_SUPPORT;
    }
    retVal = (*(tapeDvrTable[which].writeEOF))(tapeDev);

    return (retVal);
}


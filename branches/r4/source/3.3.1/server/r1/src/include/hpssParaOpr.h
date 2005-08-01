#ifndef HPSS_PARA_OPR_H
#define HPSS_PARA_OPR_H
#ifdef PARA_OPR
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include "c.h"
#include "srb.h"
#include "elogExtern.h"
#include "vault.h"
#include "srbMiscExtern.h"
#include "srbStat.h"
#include "srbDirent.h"
#include "distribExtern.h"
#include "clStubExtern.h"
#include "clMiscExtern.h"
#include "exfSwExtern.h"
#include "exfFunctExtern.h"
#include "portal.h"
#include "hpssParaOprExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "objParaIoFunctExtern.h"
#ifdef HPSS
#include "hpss_api.h"
#include "u_signed64.h"
#include "mvr_protocol.h"
#ifndef NO_DCE
#include "tools.h"
#endif
/* Added by K.Ishikawa 2003-07-02 */
#include <setjmp.h>

/* Define local function prototypes */
void
moverManager(struct moverInput *myMoverInput);
void
readMover(struct moverInput *myMoverInput);
void
writeMover(struct moverInput *myMoverInput);
void readRelay(struct dBuffer *dBuf);
void writeRelay(struct dBuffer *dBuf);
void            handle_signals();
int initSession (struct session *mySession);
int initControlSocket (struct session *mySession,
struct sockaddr_in *controlSocketAddr);
int
initHpssIodForRead (IOD_t *iod,srcsinkdesc_t *src,srcsinkdesc_t *sink, 
int paraOprFd, struct session *mySession,struct sockaddr_in *controlSocketAddr);
int
initHpssIodForWrite (IOD_t *iod,srcsinkdesc_t *src,srcsinkdesc_t *sink,
int paraOprFd, struct session *mySession,struct sockaddr_in *controlSocketAddr);
int setMoverInput (struct moverInput *myMoverInput, int index, char *path,
char *resLoc, int systemType, char *clHostAddr, int clPort, int operation,
struct session *mySession);

extern char cpFunctionName[256], cpLastFileName[1024];
extern unsigned int    uiSecs;
extern jmp_buf         jenv;
extern int StorVaultMode;

void 
hpss_timeout(int sig);

#define MIN_5 5*60
#define MIN_20 20*60
#define MIN_30 30*60

#endif 	/* HPSS */
#endif	/* PARA_OPR */
#endif	/* HPSS_PARA_OPR_H */

/*
 SrbNtMain()
 */
#include <string.h>
#include "SrbNtUtil.h"
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <setjmp.h>
#include <srb.h>



/* #define OLD_SRB_SERVICE_NAME	"SRBServer" */
SERVICE_STATUS          ssStatus;
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
BOOL                    bDebug = FALSE;
int     aArgc;
char    **aArgv;


BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
                         DWORD dwWin32ExitCode,
                         DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

	if(bDebug)
		return TRUE;

    if (dwCurrentState == SERVICE_START_PENDING)
       ssStatus.dwControlsAccepted = 0;
    else
       ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    ssStatus.dwCurrentState = dwCurrentState;
    ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    ssStatus.dwWaitHint = dwWaitHint;

    if ( ( dwCurrentState == SERVICE_RUNNING ) ||
             ( dwCurrentState == SERVICE_STOPPED ) )
        ssStatus.dwCheckPoint = 0;
    else
        ssStatus.dwCheckPoint = dwCheckPoint++;

    /* Report the status of the service to the service control manager. */
    return SetServiceStatus( sshStatusHandle, &ssStatus);
}

VOID ServiceStop()
{
	(VOID)ReportStatusToSCMgr(SERVICE_STOPPED,dwErr,0);
}

VOID WINAPI service_ctrl(DWORD dwCtrlCode)
{
	// Handle the requested control code.
    switch(dwCtrlCode)
    {
        // Stop the service.
        //
        // SERVICE_STOP_PENDING should be reported before
        // setting the Stop Event - hServerStopEvent - in
        // ServiceStop().  This avoids a race condition
        // which may result in a 1053 - The Service did not respond...
        // error.
        case SERVICE_CONTROL_STOP:
            ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
            ServiceStop();
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;

    }

    ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}

extern int srbMasterMain(int,char **);
int SrbNtStartService()
{
	int LocalStatus;

	if (!ReportStatusToSCMgr(SERVICE_START_PENDING,NO_ERROR,3000))
	{
		(VOID)ReportStatusToSCMgr(SERVICE_STOPPED,dwErr,0);
		return -1;
	}


	if (!ReportStatusToSCMgr(SERVICE_RUNNING,NO_ERROR,0))
	{
		(VOID)ReportStatusToSCMgr(SERVICE_STOPPED,dwErr,0);
		return -1;
	}

	LocalStatus = srbMasterMain(aArgc,aArgv);

	fprintf(stderr,"srbMasterMain exit, status = %d\n", LocalStatus);

	return 0;
}

void SrbNtServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	char srb_service_name[1024];

	ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;

	sprintf(srb_service_name,"%s_%s",SRB_SERVICE_NAME,DefaultPort);
	sshStatusHandle = RegisterServiceCtrlHandler(srb_service_name, service_ctrl);

	if(!sshStatusHandle)
		return;


	if (!ReportStatusToSCMgr(SERVICE_START_PENDING,NO_ERROR,3000))
	{
		(VOID)ReportStatusToSCMgr(SERVICE_STOPPED,dwErr,0);
		return;
	}

	(VOID)SrbNtStartService();
}

int SrbNtRunService(int aargc, char **aargv)
{
	char srb_service_name[1024];

	SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { srb_service_name, (LPSERVICE_MAIN_FUNCTION)SrbNtServiceMain},
        { NULL, NULL }
    };

	sprintf(srb_service_name,"%s_%s",SRB_SERVICE_NAME,DefaultPort);

	aArgc = aargc;
	aArgv = aargv;

	if (!StartServiceCtrlDispatcher(dispatchTable))
		return -1;

	return 0;
}

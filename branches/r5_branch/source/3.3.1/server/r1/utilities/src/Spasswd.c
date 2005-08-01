/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"


extern char mdasCollectionName[];
extern char srbAuth[];
extern char srbHost[];


int
main(int argc, char **argv)
{
   srbConn *conn;
   int i;
   char password[MAX_TOKEN]="",input[MAX_TOKEN]="";
   char password2[MAX_TOKEN]="",input2[MAX_TOKEN]="";
   char password3[MAX_TOKEN];
   struct stat statbuf;

   i = initSrbClientEnv();
    if (i < 0)
      {printf("Spasswd Initialization Error:%i\n",i); exit(1);}
 
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }
    
#ifndef _WIN32
    if (stat ("/bin/stty", &statbuf) == 0)
        system("/bin/stty -echo");
#endif

    printf("  Type New Password:"); fflush(stdout); 

    fgets(input,MAX_TOKEN,stdin);
    strncpy(password,input,strlen(input)-1);

    printf("\nReType New Password:");fflush(stdout);
    fgets(input2,MAX_TOKEN,stdin);
    strncpy(password2,input2,strlen(input2)-1);
#ifndef _WIN32
    system("/bin/stty echo");
#endif
    printf("\n");

    if ((strlen(password) != strlen(password2)) ||
	strcmp(password,password2))
      {
	printf("Password typing error\n");
	clFinish(conn); exit(4);
      }

    /* The following few lines came from Michal Wronski */
    if (strlen(password) == 0) {
       printf("Can't set empty password\n");
       clFinish(conn);
       exit(8);
    }

    strcpy(password2,srbAuth);
    strcpy(srbAuth,password);

    i = obfGetPw(password3);        /* check if Sauth being used*/
    if (i==0) {
      i = obfSavePw(0, 0, 1, password);   /* if so, update the Sauth file */
      if (i != 0) {
	 printf("Write Error in obfSavePw:%i\n",i);
	 clFinish(conn); exit(6);
      }
    }
    else {                          /* else, update the regular file */
      i = rewriteSrbClientAuthFile(1);
      if (i != 0)  {
	 printf("Write Error in SRB Authorization Env File:%i\n",i);
	 clFinish(conn); exit(5);
      }
    }

    i = srbModifyUser(conn, MDAS_CATALOG, password,
		      "", U_CHANGE_PASSWORD);
    if (i != 0) {
	printf("Modify Password in Catalog Failed:%i\n",i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	strcpy(srbAuth,password2);
	i = rewriteSrbClientAuthFile(0);
	if (i != 0) 
	  {
	    printf("Restore Error in SRB Authorization Env File:%i\n",i);
	    printf("Please Check SRB Authorization Env File (possible location ~/.srb/.MdasAuth\n");
	    srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    clFinish(conn); exit(6);
	  }	
	printf("Password restored in SRB Authorization Env File\n");
	clFinish(conn); exit(7);
    }
    else {
      printf("Password successfully updated in Catalog\n");
    }

    clFinish(conn);
    exit(0);

}


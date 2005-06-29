/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"

#include "srbSecureComm.h"

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasCollectionName[];
extern char inCondition[];
 
void
usage()
{
  fprintf(stderr,
	  "Usage: SmodD -s|-t|-c|-g|-k|-H|-x|-p|-X|-y|-Y|-z newValue  dataName\n");
  fprintf(stderr,
	  "Usage: SmodD -L|-P| newValue additionalValue dataName\n");
  fprintf(stderr,
	  "Usage: SmodD -l newDataName newCollectionName dataName\n");
  fprintf(stderr,
	  "Usage: SmodD -T dataName\n");
  fprintf(stderr,
          "Usage: SmodD -V replicaNumber oldVersionString newVersionString dataName \n");
}

int
main(int argc, char **argv)
{
  srbConn *conn;
  char collName[MAX_TOKEN];
   char dataName[MAX_TOKEN];
   char newValue[MAX_TOKEN];
   char additionalValue[MAX_TOKEN];
   int mval = 1;
   int i;

   /*    if  (argv[mval][0] == '-') 
      {
	if (argv[mval][1] == 'C')
	  {copyNum = atoi(argv[++mval]); mval++;}
      }
   */
   if (argc == 1) {
     usage();
     exit(1);
   }

   strcpy(inCondition , "");
   if (argv[mval][0] == '-' && argv[mval][1] == 'T' && argc == 3) {
   }
   else if(argc < 4) {
     usage();
     exit(1);
   }
 if  (argv[mval][0] == '-') 
      {
	if (argv[mval][1] == 'h')
	  {
	    usage();
	    exit(0);
	  }
      }
    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}


    if (argv[mval][0] == '-')
      {
    	conn = srbConnect (srbHost, NULL, srbAuth, 
         NULL, NULL, NULL, NULL);
	if (clStatus(conn) != CLI_CONNECTION_OK) {
	  fprintf(stderr,"Connection to srbMaster failed.\n");
	  fprintf(stderr,"%s",clErrorMessage(conn));
	  srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  clFinish(conn); exit(3);
	}
	strcpy(newValue, argv[mval+1]);
	if (argc > 3)
	  strcpy(additionalValue, argv[mval+2]);
	splitpath(argv[argc-1],collName, dataName);
	
	switch (argv[mval][1])
	  {
	  case 't':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_CHANGE_TYPE);
			break;
	     case 's':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_CHANGE_SIZE);
			break;
	     case 'p':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_CHANGE_MODIFY_TIMESTAMP);
			break;
	     case 'c':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_INSERT_COMMENTS);
			break;
             case 'V':
	           if (argc != 6) {
		     usage(); clFinish(conn); exit(3);
		   }
		   sprintf(additionalValue,"%s:%s",argv[2],argv[2]);
		   if (strlen(argv[3]) > 0)
		     sprintf(newValue,"%s:%s",argv[3],argv[4]);
		   else
		     sprintf(newValue,"-1:%s",argv[4]);
                   i = srbModifyDataset(conn, MDAS_CATALOG,
                                        dataName, collName,
                                        "","",
                                        additionalValue,newValue, D_CHANGE_REPLNUM_VERNUM);
                        break;
	     case 'g':
	           if (strlen(newValue) == 0)
		     i = srbModifyDataset(conn, MDAS_CATALOG, 
					  dataName, collName,
					  "","",
					  newValue,"", D_DELETE_GUID);		     
	           else if (argc == 5)
		     i = srbModifyDataset(conn, MDAS_CATALOG, 
					  dataName, collName,
					  "","",
					  newValue,additionalValue,D_INSERT_GUID);
		   else 
		     i = srbModifyDataset(conn, MDAS_CATALOG, 
					  dataName, collName,
					  "","",
					  newValue,"", D_INSERT_GUID);		     
			break;
	     case 'k':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_INSERT_DCHECKSUM);
			break;
	     case 'T':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					"","", D_MOVE_DATA_TO_TRASH);
			break;
	     case 'H':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_INSERT_DHIDE);
			break;
	     case 'L':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,additionalValue, 
					D_INSERT_LOCK);
			break;
	     case 'P':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,additionalValue, 
					D_UPDATE_PIN);
			break;

	     case 'x':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_UPDATE_DEXPIRE_DATE);
			break;
	     case 'X':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_UPDATE_DEXPIRE_DATE_2);
			break;
	     case 'y':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_UPDATE_DENCRYPTED);
		        break;
	     case 'Y':
	           {
		      /* Encrypted version of 'y' */
#if defined(SRB_SECURE_COMM)
                      char pubKey[1000];
                      int i1, i;
                      char *cp;

                      i1 = srbSetupSessionPublicKey(conn, pubKey);
		      if (i1 < 0) {
			fprintf(stderr,"srbSetupSessionPublicKey error %d\n",i);
			clFinish(conn); exit(4);
		      }
		      i = sscInitPublicKey(pubKey);
		      if (i != SRB_E_NOERROR) {
			fprintf(stderr,"sscInitPublicKey error %d\n",i);
			pubKey[i1]='\0';
			fprintf(stderr,
			       "Server public key or message from srbSetupSessionPublicKey: %s\n",
				pubKey);
			clFinish(conn); exit(5);
		      }
		      cp = sscSetupSession();
		      i = srbSetupSession(conn, cp);
		      if (i != SRB_E_NOERROR) {
			fprintf(stderr,"srbSetupSession error %d\n",i);
			clFinish(conn); exit(6);
		      }
		      free(cp);
		      cp = sscEncryptString(newValue);
		      if (cp == NULL) {
	                fprintf(stderr,"Secure Communication setup failure\n");
			clFinish(conn); exit(7);
		      }
		      else {
			i = srbModifyDataset(conn, MDAS_CATALOG, 
					     dataName, collName,
					     "","",
					     cp,"", D_UPDATE_DENCRYPTED);
		      }
#else
		      fprintf(stderr, 
                         "Error %d SRB Secure Communication not supported\n",
		          SRB_SECURE_COMM_NO_SUPPORT);
			clFinish(conn); exit(8);
#endif
		   }
		   break;
	     case 'z':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_UPDATE_DCOMPRESSED);
			break;
	     case 'l':
	           i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,additionalValue, D_INSERT_LINK);
			break;
             default :
	                fprintf(stderr,"Unknown option \n");
			usage();
			clFinish(conn); exit(9);
	  }
	if (i < 0)
	  {
	    fprintf(stderr,"SmodD: Error in modification:\"%s/%s:%i\"\n", 
                        collName,dataName, i);
	    srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            clFinish(conn); exit(10);        /* Roman 8/1/2000 */
	  }
/*       clFinish(conn); exit(3); Roman 8/1/2000 */
      }
    else
      {
	fprintf(stderr,"No modification prescribed  \n");
	usage();

      }

    clFinish(conn);
    exit(0);
}

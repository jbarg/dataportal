#include <strstream.h>
#include <string>
#include <stdio.h>
#include <iostream.h>
#include "DLVremote_job.h"
#include <time.h>

string* inputs( string q, int& n ){
  cout << q;
  cin >> n;
  string *t= new string[n];
  char s[10];
  for ( int i=0; i<n; i++ ){
    sprintf( s, "%i)\t", i );
    cout << s;
    cin >> t[i];
  }
  return t;
}

main(){
  int i, nCmds = 8;
  string* cmds = new string[ nCmds ];
  cmds[0] = "date >> outFile1";
  cmds[1] = "sleep 40";
  cmds[2] = "cat inFile >> outFile2";
  cmds[3] = "who";
  cmds[4] = "sleep 40";
  cmds[5] = "echo $PWD >> outFile3";
  cmds[6] = "date >> outFile";
  cmds[7] = "sleep 40";
  
  int nInFiles = 1;
  string* inFiles = new string[ nInFiles ];
  inFiles[0] = "inFile";
  
  int nOutFiles = 3;
  string* outFiles = new string[ nOutFiles ];
  outFiles[0] = "outFile1";
  outFiles[1] = "outFile2";
  outFiles[2] = "outFile3";
  
  DLVjob_file_status* file_status = new DLVjob_file_status[ nOutFiles ];
  for ( i=0; i<nOutFiles; i++ ) file_status[i] = while_running;

  string jobID;
  string host = "tcsg15";
  string user = "rshtest";
  string workDir = "RemoteJobWork";
  
  DLVremote_job *testJob = new DLVremote_job( host, user, workDir,
					      cmds, nCmds,
					      inFiles, nInFiles,
					      outFiles, file_status, nOutFiles );

  cout << "Instance of Job created...\n";
  
  int choice;
  
  do {
    cout << "\n\n\nOptions:\n";
    cout << "0\tExit\n1\tDestructor\n2\tNew instance\n";
    cout << "3\tSleep\n4\tCopy files\n5\tGet jobID\n";
    cout << "6\tRun script\n7\tGet state\n8\tRecover files\n";
    cout << "9\tUpdate recoverable file status\n";
    cout << "10\tRecover a specified file\n";
    cout << "11\tKill process\n\n";
    cout.flush();
    cin >> choice;
    string jobID, q;
    switch ( choice ){
    case 1:
      delete testJob;
      break;
    case 2:
      cout << "Enter new details? (No reinstantiates using the originals): (Y/N)";
      cin >> q;
      if ( q == "N" ){
	cout << "Enter new jobID (\"N\" uses the original if I have it...)";
	cin >> q;
	jobID = ( (q=="N") ? jobID : q );
	testJob = new DLVremote_job( host, user, workDir,
				     cmds, nCmds,
				     inFiles, nInFiles,
				     outFiles, file_status, nOutFiles, q );
	break;
      }
      delete cmds;
      delete inFiles;
      delete outFiles;
      delete file_status;
      cout << "Enter the hostname.\n";
      cin >> host;
      cout << "Enter the username on the remote machine.\n";
      cin >> user;
      cout << "Enter the work directory on the remote machine.\n";
      cin >> workDir;
      cmds = inputs( "Enter number of commands\n", nCmds );
      inFiles = inputs( "Enter number of input files\n", nInFiles );
      outFiles = inputs( "Enter number of output files\n", nOutFiles );
      file_status = new DLVjob_file_status[nOutFiles];
      for ( i=0; i<nOutFiles; i++ ) file_status[i] = while_running;
      cout << "Enter the jobID or \"N\" for none.\n";
      cin >> jobID;
      if ( jobID=="N" ) jobID = "";
      cout << "Ready to instantiate new DLVremote_job.\n";
      testJob = new DLVremote_job( host, user, workDir,
				   cmds, nCmds,
				   inFiles, nInFiles,
				   outFiles, file_status, nOutFiles, jobID );
      break;
    case 3:
      testJob->sleep();
      break;
    case 4:
      testJob->copy_files();
      break;
    case 5:
      jobID = testJob->getJobID();
      cout << "getJobID returns: " << jobID << endl;
      break;
    case 6:
      testJob->run_script();
      break;
    case 7:
      cout << "State = ";
      switch ( testJob->status() ){
      case run_and_completed:
	cout << "run_and_completed\n";
	break;
      case run_and_failed:
	cout << "run_and_failed\n";
	break;
      case run_and_recovered:
	cout << "run_and_recovered\n";
	break;
      case still_running:
	cout << "still_running\n";
	break;
      case running_and_updated:
	cout << "running_and_updated\n";
	break;      
      case not_started:
	cout << "not_started\n";
	break;
      }
      break;
    case 8:
      testJob->recover_files();
      break;
    case 9:
      testJob->update_recoverable_files();
      for ( i=0; i<nOutFiles; i++ ){
	switch ( (*testJob).file_status[i] ){
	case file_has_changed:
	  cout << "File " + outFiles[i] + " has changed.";
	  break;
	case while_running:
	  cout << "File " + outFiles[i] + " has not changed.";
	  break;
	case when_completed:
	  cout << "File " + outFiles[i] + " not checked.";
	  break;
	}
	cout << endl;
      }
      break;
    case 10:
      //Recover a specified file
      int rf;
      cout << "Enter a file number to recover:\n";
      cin >> rf;
      testJob->update_file( rf );
      break;
    case 11:
      testJob->kill();
      break;
    }
  } while ( choice );
}


















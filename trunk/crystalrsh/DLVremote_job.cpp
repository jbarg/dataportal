#include "DLVremote_job.h"

// Constructor
DLVremote_job::DLVremote_job( string hn, string un, string swd, 
			      string c[], int nc, string i[], 
			      int ni, string o[], DLVjob_file_status s[],
			      int no, string jid ){
  hostName = hn;
  userName = un;
  remoteWorkDirectory = swd;
  cmds = new string[nc];
  int j;
  for ( j=0; j<nc; j++ ){
    cmds[j] = c[j];
  }
  input = new string[ni];
  for ( j=0; j<ni; j++ ){
    input[j] = i[j];
  }
  output = new string[no];
  file_status = new DLVjob_file_status[no];
  for ( j=0; j<no; j++ ){
    output[j] = o[j];
    file_status[j] = s[j];
  }

  nCmds = nc;
  ninput = ni;
  noutput = no;
  jobID = jid; //Not pid, this is just the date etc.. used for the remote tempdir.
  needs_tidy = false;
  setUpLocal_done = false;
  setUpRemote_done = false;
  status_so_far = not_started;
  empty_string = "";
  current_dir = "./";
}
  
/*********************************************************************************/
/* Destructor                                                                    */
/*********************************************************************************/
DLVremote_job::~DLVremote_job(){
  delete [] cmds;
  delete [] input;
  delete [] output;
  delete [] file_status;
}

/**********************************************************************************/
/* Put it to sleep i.e. don't delete remote dir but only local                    */
/**********************************************************************************/
void DLVremote_job::sleep(){
  if ( needs_tidy ){
    //Delete local work directory
    localDirectory.erase( localDirectory.length() - 2 );
    mySystem( "rm -r -f " + localDirectory + "*" );  
  }
}

/*********************************************************************************/
/* Call to system - trapping problems and generating the appropriate exceptions. */
/*********************************************************************************/
void DLVremote_job::mySystem( string systemString, int retry ){
  if ( retry ){
    switch( system( systemString.c_str() ) ){
    case 127:
      //Unable to get shell
      //Generate exception
      break;
    case 0:
      //No problem
      break;
    case -1:
      //  default:
      //Error - another source.
#ifdef DEBUG
      cout << "mySystem error occurred for systemString\n" << systemString << endl;
#endif
      mySystem( systemString, --retry );
      break;
    }
#ifdef DEBUG
    cout << retry << ") " << systemString << endl << flush;
#endif
  }
}

/********************************************************************************/
/* Copy the cout and cerr from the rsh command on the remote host to the work   */
/*    directory using the same names;                                           */
/*  return false if there is an error message;                                  */
/********************************************************************************/
bool DLVremote_job::isRshErr(){
  return isRshErr( cerrRemote );
}

bool DLVremote_job::isRshErr( string &ce ){
  //rm( localFile(ce) );
  rcp( remoteFile(ce), localFile(ce) );
  rerr = getStringFromFile( localFile( ce ) );
#ifdef DEBUG
  if ( !(rerr.empty()) ) cout << "Error message is: " << rerr << endl; 
#endif
  return !( rerr.empty() );
}

bool DLVremote_job::noRshOut(){
  return noRshOut( coutRemote );
}

bool DLVremote_job::noRshOut( string &co ){
  //rm( localFile(co) );
  rcp( remoteFile(co), localFile(co) );
  rout = getStringFromFile( localFile(co) );
#ifdef DEBUG
  if ( !(rout.empty()) ) cout << "Output message is: " << rout << endl; 
#endif
  return ( rout.empty() );
}

bool DLVremote_job::isLshErr(){
  return isLshErr( cerrLocal );
}

bool DLVremote_job::isLshErr( string &ce ){
  lerr = getStringFromFile( localFile( ce ) );
  return !( lerr.empty() );
}

string DLVremote_job::getStringFromFile( string fn ){
  string rs, temp;
  ifstream ifs( fn.c_str() );
  if ( ifs.fail() ){ // File not found - throw exception...
    DLVerror( "DLVremote_job::getStringFromFile", "File error" );
#ifdef DEBUG
    cout << "Problem opening file:" << fn << "\n" << flush;
    _exit( 1 );
#endif
  } else {
    while( ifs >> temp ){
      if ( !rs.empty() ) rs.append( " " );
      rs.append( temp );
    }
  }
  return rs;
}

/********************************************************************************/
/* Send the string given as a parameter as a command using rsh.                 */
/********************************************************************************/
bool DLVremote_job::rsh( string s, bool bg ){
  return rsh( s, bg, coutRemote, cerrRemote );
}

bool DLVremote_job::rsh( string s, bool bg, string &co, string &ce ){
  // The following works on unix (sh) and NT to redirect stderr.
  mySystem( rshStringRoot + "\"sh -c \'" + s +
	    " > " + remotePath( co ) + " 2> " + remotePath( ce ) +
	    ( ( bg ) ? " &" : "" ) +
	    "\'\" 2> " + localFile( cerrLocal ) );
  if ( isLshErr() ){
    DLVerror( "DLVremote_job::rsh", lerr );
    return true;
  }
  return isRshErr( ce );
}

/********************************************************************************/
/* Send the given string to a temporary file (it may include any characters.... */
/* Copy the temporary file to the remote work directory...                      */   
/* Execute the file on the remote system.                                       */
/********************************************************************************/
bool DLVremote_job::rshViaFile( string s, string wd ){
  ofstream outFile( (localFile( rshFile )).c_str() );
  if ( outFile.fail() ){
    DLVerror( "DLVremote_job::rshViaFile", "Unable to open temporary file" );
#ifdef DEBUG
    _exit( 1 );
#endif
    return true;
  }
  if ( wd.empty() ) wd = remotePath( current_dir );
  outFile << "cd " << wd << endl;
  outFile << s << endl << flush;
  outFile.close();

#ifdef DEBUG
  cout << "rshViaFile script is\n" << "cd " << wd << endl << s << endl;
#endif

  //Set the file to be executable
  if ( chmod( (localFile( rshFile )).c_str(), 0700 ) ){
    DLVerror( "DLVremote_job::rshViaFile", "Error changing the file attributes" );
    return true;
  }
  if ( rcp( localFile(rshFile), remoteFile(rshFile) ) ){
    DLVerror( "DLVremote_job::rshViaFile", "Error in rcp of temp file" );
#ifdef DEBUG
    cout << lerr << flush;
    _exit( 1 );
#endif
    return true;
  }
  return rsh( remotePath(rshFile) );
}

/********************************************************************************/
/* Perform an rcp with the files given                                          */  
/********************************************************************************/
bool DLVremote_job::rcp( string f1, string f2 ){
  // The following works on unix (sh) and NT to redirect stderr.
  mySystem( "rcp -p " + f1 + " " + f2 + " 2> " + localFile( cerrLocal ) ); 
  return isLshErr();
}

/********************************************************************************/
/* Convert a filename into the format used by the rcp command                   */
/* Assumes that we are working in this job's own directory                      */
/********************************************************************************/
string DLVremote_job::remoteFile( string &s ){ //Relative to homeDir
  string rf = userName + "@" + hostName + ":" + remoteWorkDirectory + jobID + s;
#ifdef NT
  rf = hostName + "." + userName + "." + remoteWorkDirectory + jobID + s;
#endif
  //Return actual string so that we do not have to create a temporary string in rcps
  return rf;
}

/********************************************************************************/
/* Convert a filename so that local files are put in the correct place */
/****************************************************************************/
string DLVremote_job::localFile( string &s ){ //Relative to current directory
  string rf = localDirectory + "/" + s;
#ifdef NT
  rf = localDirectory + "\\" + s;
#endif
  //Return actual string so that we do not have to create a temporary string in rcps
  return rf;
}

/********************************************************************************/
/* Get the fully qualified path name of a file on the remote machine.           */
/********************************************************************************/
string DLVremote_job::remotePath( string &s ){
  string rf = pathToHome + remoteWorkDirectory + jobID + s;
  //Return actual string so that we do not have to create a temporary string in rcps
  return rf;
}

bool DLVremote_job::setUpPath(){ //Fully qualified
  if ( remoteWorkDirectory.empty() ) remoteWorkDirectory = "."; 
  if ( remoteWorkDirectory[ remoteWorkDirectory.length()-1 ] != '/' )
    remoteWorkDirectory += "/";
  //Check that remote work directory exists
  if ( rsh( "ls " + remoteWorkDirectory ) ){
    if ( lerr.empty() ){
      //Error => does not exist
      DLVerror( "DLVremote_job::setUpPath", "Unable to find remote work directory" );
    }
    return true;
  }

  if ( rsh("pwd") ){
    if ( lerr.empty() ){
      DLVerror( "DLVremote_job::setUpPath", "Problem obtaining remote work path" );
    }
    return true;
  } 
  if ( noRshOut() ){
    DLVerror( "DLVremote_job::setUpPath", "Problem obtaining remote work path" );
#ifdef DEBUG
    cout << "Job.C:243;Maybe pwd does not work on remote system???\n";
    _exit( 0 );
#endif
    return true;
  }
  pathToHome = rout + "/";
#ifdef DEBUG
  cout << "Job.C:206;pathToHome=" << pathToHome << "\n" << flush;
#endif
  return false;
}

/********************************************************************************/
/* Generate a unique filename based on the date and time                        */
/********************************************************************************/
string DLVremote_job::getUniqueFileName(){
  // Try three times.....
  string fileName;
  for ( int attempt=0; attempt<3; attempt++ ){
    //Produce a unique string to use for a directory name.
    //Get the date in this format - unique as I am the sole remote user.
    if ( rsh( "date +%Y_%m_%d-%T" ) ){
      if ( lerr.empty() ){
	DLVerror( "DLVremote_job::getUniqueFileName", "Error creating unique dir\n" + rerr );
      }
#ifdef DEBUG
      cout << "Job.C:272;Problem in getting directory name\n" << flush;
      _exit( 1 );
#endif
    }
    // Check for cerr here I suppose....
    if ( noRshOut() ){//Problem;
      DLVerror( "DLVremote_job::getUniqueFileName", "Unable to generate unique remote filename" );
    }
    fileName = rout + "/";
#ifdef DEBUG
    cout << " Unique file/directory to be called:\n" << fileName << "\n" << flush;
#endif
    
    string rem_job_dir = remotePath( rout );
    // if directory doesn't already exist....
    if ( rsh( "if test ! -f " + rem_job_dir + " && test ! -d " + rem_job_dir +
	      "; then echo \"yes\"; fi " ) ) {
      //Error message => unable to use this filename, try again.
#ifdef DEBUG
      cout << "Job.C:115;Test whether dir already exists -> problem\n";
      _exit( 1 );
#endif
    }

    if ( !noRshOut() && rout == "yes" ){
      // create directory...
      if ( rsh( "mkdir " + rem_job_dir ) ){
	if ( lerr.empty() ){
	  DLVerror( "DLVremote_job::getUniqueFileName", "Unable to create remote temp work directory" );
	}
	fileName="";
      }
      return fileName;
    }
  }
  //Error state is reported in setUpRemote();
  fileName = "";
  return fileName;
}

/****************************************************************************/
/* Put a filename in the correct format                                     */
/****************************************************************************/
void DLVremote_job::convertString( string &cs ){
  int first = cs.find( "/" ), last = cs.find_last_of( "/" );
  if ( ( first != 0 ) && ( last > 1 ) ){
    cs = cs.substr( ++last );
#ifdef DEBUG
    cout << "First=" << first << "\nLast=" << last << "\n";
    cout << "Changing filename: " << cs << endl;
    cout.flush();
#endif
  }
}

/********************************************************************************/
/* Delete the specified file                                                    */
/********************************************************************************/
void DLVremote_job::rm( string rm_file ){
  if ( unlink( rm_file.c_str()) ){
    DLVerror( "DLVremote_job::rm", "Problem deleting local file: " + rm_file );
  }
}

bool DLVremote_job::setUpLocal(){
  if ( setUpLocal_done ) return false;
  
  //General work directory
  string wdString = "RPC";
  if ( mkdir( wdString.c_str(), 0700 ) ){
    switch( errno ){
    case EEXIST: //Already exists, no problem...
      break;
    case EFAULT: 
    case EACCES:
    case ENAMETOOLONG:
    case ENOTDIR:
    case ENOENT:
    default:
      DLVerror( "DLVremote_job::setUpLocal", "Error in making local work directory" );
#ifdef DEBUG
      _exit(1);
#endif
      return true;
    }
  }

  //Create temp local work directory file first...
  wdString.append( "/XXXXXX" );
  char *localWorkDir = new char[ wdString.length() ];
  strcpy( localWorkDir, wdString.c_str() );
  int mkstemp_int;
  errno = 0;
  mkstemp_int = mkstemp( localWorkDir );
  if ( mkstemp_int == -1 ){
    DLVerror( "DLVremote_job::setLocal", "Error in making local temporary directory" );
#ifdef DEBUG
    switch( errno ){
    case EINVAL:
      cout << "errno = EINVAL\nTempFile = " << localWorkDir << endl;
      break;
    case EEXIST:
      cout << "errno = EEXIST\nTempFile = " << localWorkDir << endl;
      break;
    default:
      cout << "mkstemp_int = " << mkstemp_int << "\nTempFile = " << localWorkDir << endl;
    }
    cout << flush;
    _exit(1);
#endif
    return true;
  }
  //Local work directory name-file successfully created
  needs_tidy = true;
  wdString = localWorkDir;
  delete localWorkDir;
#ifdef DEBUG
  cout << "Temp local work directory file is : " << wdString << endl;
#endif 

  //Now create to the temp local work directory
  wdString += ".d";
#ifdef DEBUG
  cout << "Temp local work directory is to be : " << wdString << endl;
#endif

  localDirectory = wdString;
  if( mkdir( localDirectory.c_str(), 0700 ) ){//Error occurred
    DLVerror( "DLVremote_job::setUpLocal", "Error making local temporary directory" );
#ifdef DEBUG
    cout << "Error in creating temp local work directory.\n";
    _exit(1);
#endif
    return true;
  }
  //Files
  coutLocal = "DLV_lsh.cout";
  cerrLocal = "DLV_lsh.cerr";
  coutRemote = "DLV_rsh.cout";
  cerrRemote = "DLV_rsh.cerr";
  coutScript = "DLV_rsh.script.cout";
  cerrScript = "DLV_rsh.script.cerr";
  rshFile = "DLV_rsh.wait";
  scriptFile = "DLV_rsh.script";
  pidFile = "DLV_rsh.pid";
  scriptCompletedFile = "DLV_rsh.done";
  lockFile = "DLVrsh.lock";
  time_stamp_ending = ".DLV_time_stamp";
  rshStringRoot = "rsh " + hostName + " -n -l " + userName + " ";
#ifdef DEBUG
  //Check that we have received the correct input
  cout << "Constructed with....\n";
  cout << "Workfiles...\n" << coutLocal << endl << cerrLocal << endl;
  cout << coutRemote << endl << cerrRemote << endl;
  cout << coutScript << endl << cerrScript << endl;
  cout << rshFile << endl << scriptFile << endl;
  cout << pidFile << endl << scriptCompletedFile << endl;
  cout << "Hostname = " << hostName << "\n";
  cout << "Username = " << userName << "\n";
  cout << "Work directory = " << remoteWorkDirectory << "\n";
  cout << "Commands =\n";
  int i = 0;
  while ( i < nCmds ){
    cout << cmds[i++] << "\n";
  }
  cout << "Input files =\n";
  i = 0;
  while ( i < ninput ){
    cout << input[i++] << "\n";
  }
  cout << "Output files =\n";
  i = 0;
  while ( i < noutput ){
    cout << output[i++] << "\n";
  }
#endif
  setUpLocal_done = true;
  return false;
}

/********************************************************************************/
/* Create this job's directory in the server work directory                     */
/* Attempt to place lock to prevent other instances using the same directory    */
/* The "DLVrsh.lock" file is held locally and is locked...                  */
/* i.e. chmod +r-wx DLVrsh.lock                                             */
/********************************************************************************/
bool DLVremote_job::setUpRemote(){
  if ( setUpRemote_done ) return false;
  //Setup fully qualified remote path names
  if ( setUpPath() ) return true;
  //If remote work directory does not exists already....
  if ( jobID.empty() ){ 
    //Test whether lock file already exists
    if ( access( ( localFile(lockFile) ).c_str(), F_OK ) ){
#ifdef DEBUG
      cout << "LockFile does not currently exist -> create it and set it to read-only";
#endif
      ofstream ofs( ( localFile(lockFile) ).c_str() );
      ofs << endl << flush;
      if( chmod( ( localFile(lockFile) ).c_str(), 0444 ) ){
	DLVerror( "DLVremote_job::setUpRemote", "Error changing lockfile attributes" );
	return true;
      }
      ofs.close();
    }
    
    //Try to obtain lock on remote machine.
    int failure_count = 3;
    while ( rcp( localFile(lockFile), remoteFile(lockFile) ) ){
      if ( !failure_count-- ){
      DLVerror( "DLVremote_job::setUpRemote", "Error getting lock on remote host" );
      return true;
      }
    }

    // No error message => file copied successfully => okay to continue.
    jobID = getUniqueFileName();
      
    // ...and remove lock file.
    string rm_string = "../" + lockFile;
    if ( rsh( "rm -f " + remotePath(rm_string) ) ){
      if ( lerr.empty() ){
	string error_string = "Error removing lockfile\nPlease do this manually with\nrm -f ";
	error_string += remotePath(lockFile);
	DLVerror( "DLVremote_job::setUpRemote", error_string );
      }
#ifdef DEBUG
      cout << "Job.C:103;Problem removing lockfile....\n";
      _exit( 1 );
#endif
      return true;
    }
    
    if ( jobID.empty() ){
      DLVerror( "DLVremote_job::getUniqueFileName", "Unable to generate unique remote filename" );
      return true;
    }
    
#ifdef DEBUG
    string dot = ".";
    cout << "Directory to be used is...\n" << remotePath( dot ) << "\n";
#endif
  }
  setUpRemote_done = true;
  return false;
}

// ********************************************************************************
// ********************************************************************************
// ********************************************************************************
// Non-utility routines to perform the various procedures
// ********************************************************************************
// ********************************************************************************
// ********************************************************************************


/********************************************************************************/
/** Return the ID of the job (the unique directory in which the job resides)   **/
/********************************************************************************/
string DLVremote_job::getJobID(){
  if ( jobID.empty() ){
    DLVerror( "DLVremote_job::getJobID", "Attempt to get job ID before it has been determined" );
#ifdef DEBUG
    _exit( 1 );
#endif
  }
  return jobID;
}
 
bool DLVremote_job::isChanged( int file_no ){
  //Has the file been updated?
  if ( rshViaFile( "if test `ls -l -t | grep -E '" + output[file_no] + "$|" + 
		   output[file_no] + time_stamp_ending + "$' | awk 'NR==2{print$9}' | " +
		   "grep '" + output[file_no] + time_stamp_ending + 
		   "'` ; then echo changed; fi", remotePath( current_dir ) ) ){
    //This can be triggered by an error caused by the output file not existing yet
    //Therefore do nothing!
    /*
      if ( status_so_far == still_running ){
      error_check = true;
      error_string = error_string + 
      "Error in looking for change in file " + output[file_no] + "\n";
      }
    */
  }
  if ( !noRshOut() && rout == "changed" ){
    return true;
  }
  return false;
}

bool DLVremote_job::isRunning(){
  // First attempt to generate a file if the process is still running.
  // if ( rsh( new string( "ps -h -eo pid -p" + pid  ){ 
  // Need to use rshViaFile here because of the badly nested quotes.
  if ( rshViaFile( "ps -u " + userName + " | awk '$1==" + pid +"{ print \"Running\" }'" ) ){
    //Error message generated - not "running" does not generate error -> problem
    if ( lerr.empty() ){
      DLVerror( "DLVjob_status DLVremote_job::isRunning", "Unable to determine process status" );
    }
    return false;
  }

  if ( noRshOut() ){
    //Not running
    return false;
  }

  return ( rout == "Running" );
}

/********************************************************************************/
/* Generate, send and run the remote command script                             */
/********************************************************************************/
int DLVremote_job::run_script(){ // Run 2nd
  if ( setUpLocal() || setUpRemote() ) return DLV_STAT_FAILED; 
  ofstream outFile( ( localFile(scriptFile) ).c_str() );
  if ( !outFile ){
    DLVerror( "DLVremote_job::run_script", "Unable to write script file" );
#ifdef DEBUG
    cout << "Job.C:333;Unable to write to " << scriptFile << " output file\n";
    cout.flush();
    _exit( 1 );
#endif
  }

  // Write starter to script.
  outFile << "#! /bin/sh\n";
  // Change directory to the work directory - has been setup in setUpRemote()->setUpPath()
  outFile << "cd " << remotePath( empty_string ) << endl; 
  // Write out script pid
  outFile << "echo $$ > " << pidFile << endl;
  // Script commands
  int i=0;
  while ( i < nCmds ){
    outFile << cmds[i++] << endl;
  }
  // Write file to indicate completion
  outFile << "touch " << scriptCompletedFile << endl << flush;
  outFile.close();
  // Script made.

  if ( chmod( (localFile( scriptFile )).c_str(), 0700 ) == -1 ){
    DLVerror( "DLVremote_job::run_script", "Unable to set script file excecutable" );
    return DLV_STAT_FAILED;
  }
  if ( rcp( localFile(scriptFile), remoteFile(scriptFile) ) ){
    DLVerror( "DLVremote_job::run_script", "Unable to transfer script file" );
#ifdef DEBUG
    cout << "Job.C:124;Unable to rcp script file\n" << "Error message = " << lerr << endl << flush;
    _exit( 1 );
#endif
  }
  
  //Start script as a background job
  if ( rsh( remotePath( scriptFile ) + " < /dev/null",
	    true, coutScript, cerrScript ) ){
    if ( lerr.empty() ){
      DLVerror( "DLVremote_job::run_script", "Unable to launch remote script" );
    }
    return DLV_STAT_FAILED;
  }
  return DLV_STAT_SUCCESS;
}

/******************************************************************************/
/* Copy the specified local files to the work directory on the remote machine */
/******************************************************************************/
int DLVremote_job::copy_files(){ // Run 1st
  //Change so local work directory is created and removed each time one of these 
  if ( setUpLocal() || setUpRemote() ) return DLV_STAT_FAILED; 
  string error_string = "Unable to copy file(s)";
  bool error_check = false;
  string sLocal, sRemote, time_stamp_remote;
  int i;
  for ( i=0; i<ninput; i++ ){
    sLocal = input[i];
    sRemote = sLocal;
    convertString( sRemote );
    if ( rcp( sLocal, remoteFile( sRemote ) ) ){
      error_string = error_string + sLocal + "\n";
      error_check = true;
    }
#ifdef DEBUG
    cout << "Sent file: " << sLocal << " as: " << sRemote << endl << flush;
#endif
  }
  for ( i=0; i<noutput; i++ ){
    sRemote = output[i];
    convertString( sRemote );
    sRemote = sRemote + time_stamp_ending;
    if ( rsh( "touch " + remotePath( sRemote ) ) ){
      if ( lerr.empty() ){
	error_string = error_string + time_stamp_remote + "\n";
	error_check = true;
      }
    }
  }
  if ( error_check ){ 
    DLVerror( "DLVremote_job::copy_files", error_string );
    return DLV_STAT_FAILED;
  } else {
    return DLV_STAT_SUCCESS;
  }
}

/*********************************************************************************/
/* Change the values in *file_status to the updated state of the output files.   */
/*********************************************************************************/
void DLVremote_job::update_recoverable_files(){
  switch ( status() ){
  case not_started:
    DLVerror( "DLVremote_job::update_recoverable_files", "Not started" );
    break;
  case run_and_recovered:
    DLVerror( "DLVremote_job::update_recoverable_files", "Already recovered" );
    break;
  default:
    for ( int file_no=0; file_no<noutput; file_no++ ){
      if ( ( file_status[file_no] != when_completed ) && isChanged( file_no ) ){
	file_status[file_no] = file_has_changed;
	if ( status_so_far != run_and_completed ){
	  status_so_far = running_and_updated;
	}
      }
    }
    break;
  }
}

int DLVremote_job::update_file( int f ){
  if ( setUpLocal() || setUpRemote() ) return DLV_STAT_FAILED; 
  //Do not check again whether this file has been updated, just do it.
  string s = output[f];
  convertString( s );
  if ( rcp( remoteFile( output[f] ), s ) ){
    DLVerror( "DLVremote_job::update_file", "Unable to retrieve file: " + s );
    return DLV_STAT_FAILED;
  }
  //Reset file_status;
  file_status[f] = while_running;
  //Create new touch file for future changes
  s = output[f] + time_stamp_ending;
  if ( rsh( "touch " + remotePath( s ) ) ){
    if ( lerr.empty() ){
      DLVerror( "DLVremote_job::update_file", "Unable to update timestamp for output file:\n" + s );
    }
    return DLV_STAT_WARNING;
  }
  return DLV_STAT_SUCCESS;
}

/**********************************************************************************/
/* Copy the specified files from the remote machine back to the current directory */
/**********************************************************************************/
int DLVremote_job::recover_files(){
  string error_string, localCopy;
  bool error_check = false;
  bool warning_check = false;  
  if ( setUpLocal() || setUpRemote() ) return DLV_STAT_FAILED; 
  if ( status() == still_running ){
    //Just to update status_so_far
  }
    
  for ( int file_no=0; file_no<noutput; file_no++ ){
    switch( file_status[file_no] ){
    case when_completed:
      if ( status_so_far == still_running ) break;
    case file_has_changed:
      file_status[file_no] = while_running;
    case while_running:
      //If the file has not changed since it was last transferred then there is no point 
      //in transferring it again, even if we have now finished.
      if ( isChanged( file_no ) ){
	//File has changed ==> copy across
	localCopy = output[file_no];
	convertString( localCopy );
	if ( rcp( remoteFile( output[file_no] ), localCopy ) ){
	  error_string = error_string + "Unable to retrieve file " + output[file_no] + "\n";
	  error_check = true;
	}
	file_status[file_no] = file_has_changed;
      }
    }
  }
  
  switch( status_so_far ){
  case run_and_completed:
  case run_and_failed:
    //Now copy accross cout and cerr from script and delete remote work directory
    if ( rcp( remoteFile( coutScript ), localFile( coutScript ) ) ){
      error_string = error_string + "Script standard output";
      error_check = true;
    }
    if ( isRshErr( cerrScript ) ){ //Transfering stderr here.
      DLVerror( "DLVremote_job::recover_files", "Remote script error\n" + rerr );
    }
    
    //Delete remote work directory
    if ( rsh( "rm -f -r " + remotePath( empty_string ) ) ){
      if ( lerr.empty() ){
	DLVerror( "DLVremote_job::setUpRemote", "Error removing remote work directory" );
	warning_check = true;
      } else {
	//rsh command has failed - do not set status_so_far as we don't know.
	return DLV_STAT_FAILED;
      }
    }  
    //Set this so that we don't attempt to recover the files from the non-existant directory.
    status_so_far = run_and_recovered;
  default:
    break;
  }
  
  if ( error_check ){
    DLVerror( "DLVremote_job::recover_files", error_string );
    status_so_far = run_and_failed;
  }

  return error_check ? DLV_STAT_FAILED : warning_check ? DLV_STAT_WARNING : DLV_STAT_SUCCESS;
} 

/********************************************************************************/
/* Is the remote script still running?                                          */
/********************************************************************************/
DLVjob_status DLVremote_job::status(){
  if ( status_so_far == run_and_failed || status_so_far == run_and_recovered ){
    return status_so_far;
  }
  if ( setUpLocal() || setUpRemote() ) return run_and_failed;//???Is this the correct response 

  if ( pid.empty() ){
    if (rcp ( remoteFile(pidFile), localFile(pidFile) ) ){
      //No pid file -> has not started yet
      status_so_far = not_started;
      return status_so_far;
    } else {
      pid = getStringFromFile( localFile(pidFile) );
    }
  }

  if ( isRunning() ){
    status_so_far = still_running;
    return status_so_far;
  }

  //From here it has started and is no longer running.

  //First look for a "done" file..
  if ( rcp( remoteFile(scriptCompletedFile), localFile(scriptCompletedFile) ) ){
    //Local error message -> probably there is no "done" file i.e. error
    status_so_far = run_and_failed;
    return status_so_far;
  } else {
    //No error -> done file exists
    status_so_far = run_and_completed;
    return status_so_far;
  }
}


int DLVremote_job::kill(){
  //Kill the running job
  int return_value = DLV_STAT_SUCCESS;
  status();
  if ( pid.empty() ){
    DLVerror( "DLVremote_job::kill", "No job to kill" );
    return DLV_STAT_WARNING;
  } 
  if ( rsh( "kill -9 " + pid ) ){
    if ( lerr.empty() ){
      DLVerror( "DLVremote_job::kill", "Unable to kill job" );      

    }
    return_value = DLV_STAT_FAILED;
  }
  //Recover any updated files and remove the remote work directory.
  if ( recover_files() == DLV_STAT_FAILED ) return_value = DLV_STAT_FAILED;
  return return_value;
}






















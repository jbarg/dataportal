//#include <strstream.h>
#include <string>
#include <fstream.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define DLV_STAT_FAILED -1
#define DLV_STAT_SUCCESS 1
#define DLV_STAT_WARNING 0

using namespace std;

enum DLVjob_status {
  run_and_completed, run_and_failed, run_and_recovered, still_running, running_and_updated, not_started
};

enum DLVjob_file_status {
  when_completed, while_running, file_has_changed
};

class DLVjob{
 public:
  virtual int run_script() = 0;
  virtual int copy_files() = 0;
  virtual int recover_files() = 0;
  virtual int kill() = 0;
  virtual void update_recoverable_files() = 0;
  virtual DLVjob_status status() = 0;
};

class DLVremote_job: public DLVjob{
 public:
  DLVremote_job( string, string, string, string[], int,
		 string[], int, string[], DLVjob_file_status[], int, string jid = "" );
  virtual ~DLVremote_job();
  virtual string getJobID();
  virtual int run_script();
  virtual int kill();
  virtual int copy_files();
  virtual int recover_files();
  virtual DLVjob_status status();
  virtual void update_recoverable_files();
  int update_file( int );
  //  int recover_file( int );
  void sleep();
  //Make private later
  DLVjob_file_status *file_status;

 private:
  void DLVerror( string whoAmI, string whatWentWrong ){
    cerr << "DLVerror: from: " << whoAmI << endl << whatWentWrong << endl;
  }
  bool isChanged( int );
  bool isRunning();
  bool setUpPath();
  bool setUpRemote();
  bool setUpLocal();
  void rm( string );
  void convertString( string& );
  string getUniqueFileName();
  string remotePath( string& );
  string remoteFile( string& );
  string localFile( string& );
  bool rcp( string, string );
  bool rshViaFile( string, string="" ); //Put string in tempfile - can contain spaces etc..
  bool rsh( string, bool, string&, string& );
  bool rsh( string, bool = false );
  string getStringFromFile( string );
  bool isLshErr();
  bool isLshErr( string& );
  bool noRshOut();
  bool noRshOut( string& );
  bool isRshErr();
  bool isRshErr( string& );
  void mySystem( string, int=3 );
  
  string empty_string;
  string current_dir;
  bool setUpLocal_done, setUpRemote_done, needs_tidy;
  DLVjob_status status_so_far;
  string pid, jobID, lerr, lout, rerr, rout;
  string rshStringRoot, hostName, userName;
  string localDirectory, remoteWorkDirectory, pathToHome;
  string coutLocal, coutRemote, cerrLocal, cerrRemote;
  string scriptFile, scriptCompletedFile, coutScript, cerrScript;
  string pidFile, rshFile, lockFile, time_stamp_ending;
  string *cmds, *input, *output;
  int nCmds, ninput, noutput;
};












package CeSCPortlets.GRAMJob;

import java.sql.Timestamp;


public class GRAMJobParameters {
  private String gjp_username;
  private String gjp_hostname;
  private String gjp_executable;
  private String gjp_arguments;
  private String gjp_directory;
  private String gjp_jobmanager;
  private String gjp_stdin;
  private String gjp_stdout;
  private String gjp_stderr;
  private boolean gjp_batch;

  private Timestamp gjp_ts;
  private String gjp_jobid;
  private String gjp_jobresult;
  private String gjp_batchjobstatus;
  private String gjp_joberror;


  public GRAMJobParameters() {
  }


  public String getUsername() {
    return this.gjp_username;
  }
  public String getHostname() {
    return this.gjp_hostname;
  }
  public String getExecutable() {
    return this.gjp_executable;
  }
  public String getArguments() {
    return this.gjp_arguments;
  }
  public String getDirectory() {
    return this.gjp_directory;
  }
  public String getJobmanager() {
    return this.gjp_jobmanager;
  }
  public String getStdin() {
    return this.gjp_stdin;
  }
  public String getStdout() {
    return this.gjp_stdout;
  }
  public String getStderr() {
    return this.gjp_stderr;
  }
  public boolean getBatch() {
    return this.gjp_batch;
  }
  public Timestamp getTimestamp() {
    return this.gjp_ts;
  }
  public String getJobID() {
    return this.gjp_jobid;
  }
  public String getJobResult() {
    return this.gjp_jobresult;
  }
  public String getBatchJobStatus() {
    return this.gjp_batchjobstatus;
  }
  public String getJobError() {
    return this.gjp_joberror;
  }


  public void setUsername(String username) {
    this.gjp_username = username.trim();
  }
  public void setHostname(String hostname) {
    this.gjp_hostname = hostname.trim();
  }
  public void setExecutable(String executable) {
    this.gjp_executable = executable.trim();
  }
  public void setArguments(String arguments) {
    this.gjp_arguments = arguments.trim();
  }
  public void setDirectory(String directory) {
    this.gjp_directory = directory.trim();
  }
  public void setJobmanager(String jm) {
    this.gjp_jobmanager = jm.trim();
  }
  public void setStdin(String stin) {
    this.gjp_stdin = stin.trim();
  }
  public void setStdout(String sout) {
    this.gjp_stdout = sout.trim();
  }
  public void setStderr(String serr) {
    this.gjp_stderr = serr.trim();
  }
  public void setBatch(boolean batch) {
    this.gjp_batch = batch;
  }
  public void setTimestamp(Timestamp ts) {
    this.gjp_ts = ts;
  }
  public void setJobID(String jobid) {
    this.gjp_jobid = jobid;
  }
  public void setJobResult(String result) {
    this.gjp_jobresult = result;
  }
  public void setBatchJobStatus(String bjs) {
    this.gjp_batchjobstatus = bjs.trim();
  }
  public void setJobError(String error) {
    this.gjp_joberror = error;
  }

}

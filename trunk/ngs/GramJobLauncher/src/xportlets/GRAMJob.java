package CeSCPortlets.GRAMJob;


import java.io.*;

import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;

import org.globus.gram.*;
import org.globus.util.deactivator.*;
import org.globus.io.gass.server.GassServer;


public class GRAMJob {

  private static final String msg_nooutput = "Your job does not have output.";


  public void executeGRAMJob(GRAMJobParameters params, GSSCredential proxy) {
    // submit a GRAM job
    String host = params.getHostname();
    String exec = params.getExecutable();
    String args = params.getArguments();
    String dir = params.getDirectory();
    boolean batch = params.getBatch();

    StringBuffer rsl = new StringBuffer();
    GramJob gramJob = null;
    GassServer gassServer = null;

    try {
      Gram.ping(proxy, host);

      rsl.append("&(executable=").append(exec).append(")");
      if (args!=null && !args.equals("")) {
        rsl.append("(arguments=").append(args).append(")");
      }
      if (dir!=null && !dir.equals("")) {
        rsl.append("(directory=").append(dir).append(")");
      }

      if (batch) { // batch job
        String jm = params.getJobmanager();
        String stin = params.getStdin();
        String sout = params.getStdout();
        String serr = params.getStderr();
        if (stin!=null && !stin.equals("")) {
          rsl.append("(stdin=").append(stin).append(")");
        }
        if (sout!=null && !sout.equals("")) {
          rsl.append("(stdout=").append(sout).append(")");
        }
        if (serr!=null && !serr.equals("")) {
          rsl.append("(stderr=").append(serr).append(")");
        }
        if (jm!=null && !jm.equals("")) {
          host += "/jobmanager-" + jm;
        }

        gramJob = new GramJob(proxy, rsl.toString());
//          System.out.println("Submitting a batch job to " + host + "...");
        gramJob.request(host, batch);
        params.setJobID(gramJob.getIDAsString());
//          System.out.println("JobID returned: " + jobID);

          // jobID should be saved here for future usage, e.g., check job status

/*          // check job status -- Keep in mind that once the job finishes the job
          // manager process dies and the client won't be able to connect anymore
          // to check on the status. This is a special case that you must handle
          // in your way.
          String gramJobStatus = "";
          for (int i=0;i<6;i++) {
            try {
              Thread.sleep(2000);
            } catch (Exception e) {}
            GramJob gj = new GramJob(proxy, "");
            gj.setID(jobID);
            Gram.jobStatus(gj);
            gramJobStatus = gj.getStatusAsString();
            System.out.println(gramJobStatus);
          }*/
      }
      else { // interactive/non-batch job
        gassServer = new GassServer(proxy, 0);
        String gassURL = gassServer.getURL();
        ByteArrayOutputStream stdout = new ByteArrayOutputStream();
        rsl.append("(rsl_substitution=(GLOBUSRUN_GASS_URL ")
           .append(gassURL)
           .append("))");
        rsl.append("(stdout=$(GLOBUSRUN_GASS_URL)/dev/stdout)");
        rsl.append("(stderr=$(GLOBUSRUN_GASS_URL)/dev/stderr)");
        gassServer.registerJobOutputStream("out", stdout);
        gassServer.registerJobOutputStream("err", stdout);

        gramJob = new GramJob(proxy, rsl.toString());
        gramJob.addListener(new GramJobListener() {
          public void statusChanged(GramJob job) {
//              System.out.println("  gramJob status change \n" +
//                                 "    ID     : " + job.getIDAsString() + "\n" +
//                                 "    Status : " + job.getStatusAsString());
          }
        });

        gramJob.request(host, batch); // job in interactive mode
//          System.out.println("  Job submitted: " + gramJob.getIDAsString());

        try {
          Thread.sleep(2000);
        }
        catch (Exception e) {
          params.setJobError(e.getMessage());
        }

        try {
          while (Gram.getActiveJobs()!=0) {
            Thread.sleep(2000);
          }
        }
        catch (Exception e) {
          params.setJobError(e.getMessage());
        }

        String str = stdout.toString();
        if (str==null || str.equals("")) {
          params.setJobResult(msg_nooutput);
        }
        else {
          params.setJobResult(str);
        }
      }
    }
    catch (Exception e) {
      params.setJobError(e.getMessage());
    }
    finally {
      if (gassServer!=null) {
        gassServer.shutdown();
      }
      Deactivator.deactivateAll();
    }

  }

}

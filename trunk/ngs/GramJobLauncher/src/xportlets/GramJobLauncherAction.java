/*
 * Xiaobo Yang, CCLRC e-Science Centre, 14 December 2004
 *
 */

package org.apache.jetspeed.modules.actions.xportlets;

//import org.apache.jetspeed.modules.actions.portlets.VelocityPortletAction;
import org.apache.jetspeed.portal.portlets.VelocityPortlet;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

import java.util.*; 
import java.net.*;

// Turbine classes
import org.apache.turbine.util.Log;
import org.apache.turbine.util.RunData;

// Velocity classes
import org.apache.velocity.context.Context;

// Jetspeed stuff 
import org.apache.jetspeed.om.security.JetspeedUser;
import org.apache.jetspeed.services.rundata.JetspeedRunData;
import org.apache.jetspeed.services.statemanager.SessionState;

//import xportlets.gramjoblauncher.GramJobLauncher;

// CeSCPortlets
import CeSCPortlets.GRAMJob.*;

import xportlets.proxymanager.*;

import org.ietf.jgss.GSSCredential;

import org.chefproject.actions.VelocityPortletPaneledAction;

import org.chefproject.service.GridService;
import org.chefproject.util.Menu;

import java.sql.Timestamp;
import javax.servlet.http.HttpSession;


/**

* LSA Component launcher launches a python script 
* 
* @author <a href="mailto:gkandasw@cs.indiana.edu">Gopi Kandaswamy</a>
*/

//public class GramJobLauncherAction extends VelocityPortletAction {
public class GramJobLauncherAction extends VelocityPortletPaneledAction {
    
  final private String HOSTNAME = "gjl_hostname";
  final private String JOBMANAGER = "gjl_jobmanager";
  final private String EXECUTABLE = "gjl_executable";
  final private String ARGUMENTS = "gjl_arguments";
  final private String DIRECTORY = "gjl_workingdirectory";
  final private String STDIN = "gjl_stdin";
  final private String STDOUT = "gjl_stdout";
  final private String STDERR = "gjl_stderr";
  final private String JOBTYPE = "gjl_jobtype";

  final private String JOB_INTERACTIVE = "GRAMJOB_INTERACTIVE";
  final private String JOB_BATCH = "GRAMJOB_BATCH";

//  final private String STDOUT_HOME = "GJL_OUTPUT_HOME";

  final private String JOBRESULT = "gjl_result";
  final private String JOBID = "gjl_jobid";
  final private String MSG_ALERT = "alertMessage";

  private final String MSG_NOPROXY = "Get a proxy first before submitting your job!";
//  private final String msg_notyourproxy = "Get your own proxy if you do not have one and set it as the default one before submitting your job!";
  private final String MSG_NOJOBIDRETURNED = "No JobID returned for a batch job!";
//  private final String msg_errortable = "Could not create table to save data!";
//  private final String msg_errorinsertrow = "Could not save information to the table!";

  final private String PARAMS = "GJL_PARAMETERS";

  final private String HOSTLIST = "GJL_HOSTLIST";

  private boolean b_start = false;

    static Logger logger = Logger.getRootLogger();

    
  public String buildMainPanelContext(VelocityPortlet portlet,
			Context context, RunData rundata, SessionState state) {
System.out.println("LA: buildMainContext() called");
    // put $action into context for menus, forms and links
    context.put(Menu.CONTEXT_ACTION, state.getAttribute(STATE_ACTION));

    String[] gjl_hostlist = org.apache.jetspeed.util.StringUtils.stringToArray(org.apache.jetspeed.util.PortletConfigState.getParameter(portlet,rundata, HOSTLIST, "grid-compute.leeds.ac.uk, grid-comptue.oesc.ox.ac.uk, grid-data.man.ac.uk, grid-data.rl.ac.uk"), ", ");
    context.put(HOSTLIST, gjl_hostlist);

    return buildLaunchContext(state, context);

  }

  private String buildLaunchContext(SessionState state, Context context) {
System.out.println("LA: buildLaunchContext() called");
    context.put(PARAMS, state.getAttribute(PARAMS));

    context.put(JOBTYPE, state.getAttribute(JOBTYPE));

    String result = (String)state.getAttribute(JOBRESULT);
    if (result!=null && !result.equals("")) {
System.out.println(result);
      context.put(JOBRESULT, result);
    }

    String jobid = (String)state.getAttribute(JOBID);
    if (jobid!=null && !jobid.equals("")) {
System.out.println("LALALA: " + jobid);
      context.put(JOBID, jobid);
    }

    String alert = (String)state.getAttribute(MSG_ALERT);
    if (alert!=null && !alert.equals("")) {
      context.put(MSG_ALERT, alert);
    }

    return "xportlets-gramjoblauncher";

  }

  public void doLaunch(RunData runData, Context context) {
//System.out.println("LA: doLaunch() called");
    SessionState state = ((JetspeedRunData)runData).getPortletSessionState(((JetspeedRunData)runData).getJs_peid());

    // first remove all attributes
    state.clear();

    // check Hostname and Executable
    String jobHost = runData.getParameters().getString(HOSTNAME);
    String jobExecutable = runData.getParameters().getString(EXECUTABLE);
    String missing = null;
    if ((jobHost==null || jobHost.equals("")) &&
        (jobExecutable==null || jobExecutable.equals(""))) {
      missing = "You missed both \"Hostname\" and \"Executable\"";
    }
    else if (jobHost==null || jobHost.equals("")) {
      missing = "You missed \"Hostname\"";
    }
    else if (jobExecutable==null || jobExecutable.equals("")) {
      missing = "You missed \"Executable\"";
    }

    if (missing!=null) {
      state.setAttribute(MSG_ALERT, missing);
    }
    else {
      GRAMJobParameters params = new GRAMJobParameters();

      params.setHostname(jobHost);
      params.setExecutable(jobExecutable);
      params.setArguments(runData.getParameters().getString(ARGUMENTS));
      params.setDirectory(runData.getParameters().getString(DIRECTORY));
      params.setJobmanager(runData.getParameters().getString(JOBMANAGER));
      params.setStdin(runData.getParameters().getString(STDIN));
      params.setStdout(runData.getParameters().getString(STDOUT));
      params.setStderr(runData.getParameters().getString(STDERR));
      String jobtype = runData.getParameters().getString(JOBTYPE);
      state.setAttribute(JOBTYPE, jobtype);
      if (jobtype.equalsIgnoreCase(JOB_INTERACTIVE)) {
        params.setBatch(false);
      }
      else {
        params.setBatch(true);
      }

      // submit a GRAM job
      GSSCredential proxy = GridService.getCurrentUserGSSCredential();
      if (proxy==null) {
        HttpSession session = runData.getSession();
        proxy = ProxyManager.getDefaultProxy(session);
      }

      if (proxy==null) {
        state.setAttribute(MSG_ALERT, MSG_NOPROXY);
      }
      else { // the default proxy should belong to current user
        GRAMJob gramJob = new GRAMJob();
        if (params.getBatch()) { // set timestamp and username for batch job
//          Timestamp ts = new Timestamp(Calendar.getInstance().getTimeInMillis());
          Timestamp ts = new Timestamp(System.currentTimeMillis());
          params.setTimestamp(ts);
          String username = runData.getUser().getUserName();
          params.setUsername(username);
        }
        gramJob.executeGRAMJob(params, proxy);

        if (params.getBatch()) { // batch job
          Log.info("CeSCPortlets.JobSubmission called, "
                  + runData.getUser().getUserName() + " submitted a batch job");
System.out.println("  " + runData.getUser().getUserName() + " submitted a batch job");

          if (params.getJobID()!=null && !params.getJobID().equals("")) {
//            context.put(JOBID, params.getJobID());
            state.setAttribute(JOBID, params.getJobID());
//            updateDatabase(context, params); // TODO
            Log.info("JobID: " + params.getJobID()); // display JobID
System.out.println("    JobID: " + params.getJobID());
          }
          else {
            params.setJobError(MSG_NOJOBIDRETURNED);
//            context.put(ERROR_JOB, msg_nojobidreturned); // display error message
            state.setAttribute(MSG_ALERT, MSG_NOJOBIDRETURNED);
          }
        }
        else { // interactive job
          Log.info("CeSCPortlets.JobSubmission called, "
           + runData.getUser().getUserName() + " submitted an interactive job");
          Log.info(params.getHostname() + " " + params.getExecutable() + " "
                   + params.getArguments() + " " + params.getDirectory());
System.out.println("  " + runData.getUser().getUserName() + " submitted an interactive job");
System.out.println(params.getHostname() + " " + params.getExecutable() + " "
                   + params.getArguments() + " " + params.getDirectory());
          if (params.getJobResult()!=null && !params.getJobResult().equals("")) {
//            context.put(JOBRESULT, params.getJobResult()); // display interactive job result
            state.setAttribute(JOBRESULT, params.getJobResult());
System.out.println("    Job result: " + params.getJobResult());
          }
        }

        if (params.getJobError()!=null && !params.getJobError().equals("")) {
//          context.put(ERROR_JOB, params.getJobError()); // display interactive job error, interactive job without output is included here
          state.setAttribute(MSG_ALERT, params.getJobError());
          Log.info("JobError: " + params.getJobError());
        }

        state.setAttribute(PARAMS, params);

      }
    }

//String[] ss = state.getAttributeNames();
//for (int i=0; i<ss.length; i++) {
//  System.out.println("**ss[" + i + "] = " + ss[i]);
//}

  }

}

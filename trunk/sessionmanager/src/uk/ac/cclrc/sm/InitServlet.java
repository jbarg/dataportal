/*
 * NewServlet.java
 *
 * Created on 24 November 2004, 09:48
 */

package uk.ac.cclrc.sm;

import java.io.*;
import java.net.*;
import java.util.*;

import org.apache.log4j.*;

import javax.servlet.*;
import javax.servlet.http.*;

import org.quartz.CronTrigger;
import org.quartz.JobDetail;
import org.quartz.Scheduler;
import org.quartz.SchedulerFactory;
import org.quartz.SchedulerMetaData;
import org.quartz.SimpleTrigger;
import org.quartz.helpers.TriggerUtils;

/**
 *
 * @author  gjd37
 * @version
 */
public class InitServlet extends HttpServlet {
    
    private Scheduler sched;
    private  Logger log = Logger.getLogger(this.getClass().getName());
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
        super.init(config);
        
        ServletContext sc = config.getServletContext();
        String workingDir = sc.getRealPath("");
        //set the log4j properties file for the whole web application
        PropertyConfigurator.configure(workingDir+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
        log.debug("I am LOADED from session manager!");
        this.setUpQuartzJobs();
    }
    
    /** Destroys the servlet.
     */
    public void destroy() {
        System.out.println("I am UNLOADING from session manager");
        try{
            // shutdown the housekeeping tasks.
            sched.shutdown();
        }catch(Exception e){
            System.out.println("Unable to shutdown the house keeping! "+e);
        }
    }
    
    /** Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
     * @param request servlet request
     * @param response servlet response
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        //nothing
    }
    
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    }
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    }
    
    /** Returns a short description of the servlet.
     */
    public String getServletInfo() {
        return "This uses Quatz to remove all old data in database+";
    }
    
    /**
     * Setup the housekeeping jobs to delete old files form the users download
     * directory.
     */
    private void setUpQuartzJobs(){
        try {
            
            SchedulerFactory schedFactory = new org.quartz.impl.StdSchedulerFactory();
            sched = schedFactory.getScheduler();
            // create a simple trigger that fires in 10s from now,
            // then repeats every 1 hour indefineatley.
            long startTime = System.currentTimeMillis() + 10000L;
            //long interval = (((60L*1000L)*60L)*1L);
            //evry 10 seconds
            long interval = 10000L;
            // startTime, endTime, number of repeats, interval.
            SimpleTrigger trigger = new SimpleTrigger("myTrigger",
            sched.DEFAULT_GROUP,
            new Date(startTime),
            null,
            SimpleTrigger.REPEAT_INDEFINITELY,
            interval);
            // create the job detail.
            JobDetail jobDetail = new JobDetail("sessionManagerHouseKeeping", sched.DEFAULT_GROUP, SessionManagerHouseKeeping.class);
            
            // relate the trigger with the job and schedule execution.
            Date ft = sched.scheduleJob(jobDetail, trigger);
            
            // start execution.
            sched.start();
        }catch(Exception e){
            
            log.error("Session Manager database house keeping unable to start",e);
        }
    }
    
}

/*
 * SessionManagerHouseKeeping.java
 *
 * Created on 24 November 2004, 10:07
 */

package uk.ac.cclrc.sm;

import java.util.Date;
import org.quartz.Job;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

import uk.ac.cclrc.db.*;
import uk.ac.cclrc.certificate.Certificate;
import org.apache.log4j.*;

import java.sql.*;

/**
 *
 * @author  gjd37
 */
public class SessionManagerHouseKeeping implements Job{
    
    private  Logger log = Logger.getLogger(this.getClass().getName());
    private Certificate cert;
    private String sid;
    
    /** Creates a new instance of SessionManagerHouseKeeping */
    public SessionManagerHouseKeeping() {
    }
    
    /**
     * Specify this classes job which is to check that session
     * information is deleted
     */
    public void execute(JobExecutionContext context)
    throws JobExecutionException {
        DBAccess db = null;
        try {
            db = new DBAccess("dataportalDB");
            db.connect();
            //log.info("Looking for old sessions in database");
            
            ResultSet rs = db.getData("select sid, certificate from session");
            
            //check which session ids are invalid
            while (rs.next()) {
                try{
                    this.cert = new Certificate(rs.getString("certificate"));
                    this.sid = rs.getString("sid");
                }
                catch(Exception e){
                   // System.out.println("User "+this.cert.getDName()+" has invalid certificate.  Trying to remove...");
                   log.warn("User "+this.cert.getDName()+" has invalid certificate.");
                    
                    db.updateData("delete from session where sid = '"+this.sid+"'");
                    db.updateData("delete from session_access_rights where sid '"+this.sid+"'");
                      log.warn("Removed certificate for "+this.cert.getDName()+".");
                }
                
              //  log.info("User "+this.cert.getDName()+" has a certificate");
                 //System.out.println("User "+this.cert.getDName()+" has a certificate, with lifetime "+this.cert.getLifetime());
                if(this.cert.getLifetime() == 0){
                  //  log.info("Invalid! no lifetime left.  Removing");
                  //  log.info("\n");
                    log.info("Invalid certificate. No lifetime left for "+this.cert.getDName()+".");
                 //System.out.println("\n");
                    db.updateData("delete from session where sid = '"+this.sid+"'");
                    db.updateData("delete from session_access_rights where sid '"+this.sid+"'");
                    log.info("Removed certificate for "+this.cert.getDName()+".");
                }
            }
           // log.debug("Finished checking sessions");
            
        }
        catch(Exception sql){
           // log.warn("Error with job execution",sql);
        }
        finally {
            try{
                db.disconnect();
            }
            catch(Exception ignore){}
        }
        
    }
}

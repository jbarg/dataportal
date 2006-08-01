/*
 * DataPortalConstants.java
 *
 * Created on 20 June 2006, 11:05
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

/**
 *
 * @author gjd37
 */
public class DataPortalConstants {
    
    //myproxy stuff
    public final static String MYPROXY_HOST = "myproxy.grid-support.ac.uk";
    public final static int MYPROXY_PORT = 7512;
    public final static String MYPROXY_DN = "/C=UK/O=eScience/OU=CLRC/L=DL/CN=host/myproxy.grid-support.ac.uk/E=a.j.richards@dl.ac.uk";
    
    //srb stuff
    public final static String SRB_HOST = "srb1.ngs.rl.ac.uk";
    public final static int SRB_PORT = 5544;
    
    //query cache limit
    public final static int QUERY_LIMIT = 250;
    
    
    //EJB Session names
    public final static String SESSION = "SessionEJB";
    public final static String TRANSFER = "TransferEJB";
    public final static String QUERY = "QueryEJB";
    public final static String DATA_CENTER = "DataCenterEJB";
    public final static String DATA_AUTHORISATOIN = "DataAuthorisationEJB";
    public final static String LOOKUP = "LookupEJB";
    public final static String TIMER = "TimerEJB";
    
    //EJB MDB names
    public final static String QUERY_MDB = "jms/QueryMDBQueue";
    public final static String EVENT_MDB = "jms/EventMDBQueue";
    public final static String CONNECTION_FACTORY = "MDBQueueConnectionFactory";
    
    
}

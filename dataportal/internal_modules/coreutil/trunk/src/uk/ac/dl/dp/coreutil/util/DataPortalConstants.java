/*
 * DataPortalConstants.java
 *
 * Created on 20 June 2006, 11:05
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.File;

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
    
    /**
     * Limit of the amount of queries in memory
     */
    public final static int QUERY_LIMIT = 250;
    
    /**
     * Max number of results passed to the iCAT API to return from each ICAT
     */
    public final static int MAX_RESULTS = 50;
    
    public static String MYPROXY_USERNAME = "glen";
    public static String MYPROXY_PASSWORD = "kkkkkk";
    
    //allowd imageJ file types
    public final static String[] IMAGEJ_TYPES = {"RAW","TIFF","JPEG","JPG","GIF","BMP","DICOM","FITS","PGM","ZIP","LUT","ROI","TEXT","TXT"};
    
    
    //EJB Session names
    public final static String SESSION = "SessionEJB";
    public final static String TRANSFER = "TransferEJB";
    public final static String DOWNLOAD = "DownloadEJB";
    public final static String QUERY = "QueryEJB";
    public final static String DATA_CENTER = "DataCenterEJB";
    public final static String DATA_AUTHORISATION = "DataAuthorisationEJB";
    public final static String LOOKUP = "LookupEJB";
    public final static String TIMER = "TimerEJB";
    public final static String EVENT = "EventEJB";
    public final static String ADMIN = "AdminEJB";
    
    //EJB MDB names
    public final static String QUERY_MDB = "jms/QueryMDBQueue";
    public final static String SEND_MDB = "jms/sendMDBQueue";
    public final static String LOGIN_ICAT_MDB = "jms/LoginICATMDBQueue";
    public final static String EVENT_MDB = "jms/EventMDBQueue";
    public final static String KEYWORD_MDB = "jms/KeywordMDBQueue";
    public final static String DOWNLOAD_MDB = "jms/DownloadMDBQueue";
    public final static String CONNECTION_FACTORY = "MDBQueueConnectionFactory";
    
    //location of the keyword files
    public final static String KEYWORD_LOCATION= System.getProperty("java.io.tmpdir")+File.separator;
    public final static String FILE_DOWNLOAD_LOCATION = System.getProperty("java.io.tmpdir")+File.separator+"downloads";
    public final static boolean LOCAL_EJB_REFS = false;
}

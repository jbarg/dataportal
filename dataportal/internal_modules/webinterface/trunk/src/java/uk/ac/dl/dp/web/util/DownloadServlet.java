package uk.ac.dl.dp.web.util;
import org.ietf.jgss.GSSCredential;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.dl.dp.coreutil.delegates.DownloadDelegate;
import uk.ac.dl.dp.coreutil.delegates.EventDelegate;
import uk.ac.dl.dp.coreutil.entity.DPConstants;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.util.Certificate;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.web.backingbeans.Visit;
import org.apache.log4j.*;
import java.io.*;
import java.net.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.srbapi.srb.*;

/**
 *
 * @author gjd37
 * @version
 */
public class DownloadServlet extends HttpServlet {
    
    private static Logger log = Logger.getLogger(DownloadServlet.class);
    
    private static Collection<String> downloadedFiles  = new ArrayList<String>();
    
    private String workingDir = "";
    private String context = "";
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
        super.init(config);
        ServletContext sc = config.getServletContext();
        workingDir = sc.getRealPath("");
        context = sc.getContextPath();
        
        try {
            
            boolean isSet = DownloadDelegate.getInstance().isSet();
            if(!isSet){
                log.trace("Constants not set to trying to set");
                //set the DB to vales for download
                InetAddress host = InetAddress.getLocalHost();
                
                DPConstants dpConst = new DPConstants();
                dpConst.setLoggingLevel("DEBUG");
                dpConst.setDownloadLocation(workingDir);
                dpConst.setPortNumber(8181);
                dpConst.setContextRoot(context);
                
                dpConst.setServiceName(host.getHostAddress());
                DownloadDelegate.getInstance().setConstants(dpConst);
            }
        } catch (Exception ex) {
            log.warn("Error setting up the constants",ex);
        }
    }
    
    /** Destroys the servlet.
     */
    public void destroy() {
        
    }
    
    /** Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
     * @param request servlet request
     * @param response servlet response
     */
    protected void processRequest(HttpServletRequest req, HttpServletResponse response)
    throws ServletException, IOException {
        HttpSession session = req.getSession();
        Visit visit  =  (Visit)session.getAttribute(WebConstants.SESSION_KEY);
        
        if(visit == null){
            //user timed out
            return ;
        }
        
        String ID = req.getParameter("url");
        String TYPE = req.getParameter("type");
        String FROM  = req.getParameter("from");
        
        String[] srbUrl = null;
        String name = null;
        File srbDownload = null;
        String DpId = null;
        
        
        if(FROM.equals(DownloadConstants.DATA_CENTER)){
            //request from Data Center
            if(TYPE.startsWith(DownloadConstants.DATA_FILE)){
                Url file = visit.getVisitData().getDataUrlFromCart(ID);
                DpId = file.getDpId();
                name = file.getName();
                srbUrl = new String[]{file.getUrl()};
                
               //this is when a file is a data ref
            } else if(TYPE.startsWith(DownloadConstants.DATA_FILE_AS_REF)){
                DataReference file = visit.getVisitData().getDataReferenceFromCart(ID);
                DpId = file.getDpId();
                name = file.getName();
                for(Url url : file.getUrls()){
                    srbUrl = new String[]{url.getUrl()};
                    break;
                }                
                
                //this will get DATASETS and DATASETINFOLDER types
            } else if(TYPE.startsWith(DownloadConstants.DATA_SET)){
                DataReference file = visit.getVisitData().getDataReferenceFromCart(ID);
                DpId = file.getDpId();
                name = file.getName();
                //iterate through reference and get all URLS out of it
                ArrayList<String> urls = new ArrayList<String>();
                for(Url url : file.getUrls()){
                    urls.add(url.getUrl());
                }
                
                srbUrl = urls.toArray(new String[urls.size()]);
            } else if(TYPE.equals(DownloadConstants.DOWNLOAD_MULTIPLE)){
                //multiple file download
                srbUrl = visit.getVisitData().getCartSRBURLs();
                name = DownloadConstants.DOWNLOAD_MULTIPLE;
                DpId = DownloadConstants.DOWNLOAD_MULTIPLE;
            }
            
            //do other types from DATASETS
        } else if(FROM.equals(DownloadConstants.DATA_SETS)){
            // log.debug("Download from datasets.");
            //request from Data Sets
            if(TYPE.startsWith(DownloadConstants.DATA_FILE)){
                DataFile file = visit.getVisitData().getDataFileFromSearchedData(ID);
                DpId = file.getDpId();
                name = file.getName();
                srbUrl = new String[]{file.getUri()};
                //this will get DATASETS and DATASETINFOLDER types
            } else if(TYPE.startsWith(DownloadConstants.DATA_SET)){
                // log.trace("Type Data Set");
                DataSet file = visit.getVisitData().getDataSetFromSearchedData(ID);
                DpId = file.getDpId();
                name = file.getName();
                //iterate through set and get all URLS out of it
                ArrayList<String> urls = new ArrayList<String>();
                for(DataFile datafile :visit.getVisitData().getCurrentDatafiles()){
                    if(datafile.getDataSetId().equals(file.getId())){
                        urls.add(datafile.getUri());
                    }
                }
                srbUrl = urls.toArray(new String[urls.size()]);
            } else if(TYPE.equals(DownloadConstants.DOWNLOAD_MULTIPLE)){
                //multiple file download
                srbUrl =  visit.getVisitData().getSearchedSRBURLs();
                name = DownloadConstants.DOWNLOAD_MULTIPLE;
                DpId = DownloadConstants.DOWNLOAD_MULTIPLE;
            }
        }
        
        
        //check if already started a downlaod for this ID
        if(!visit.contains(ID)){
            
            log.info("Id for download is "+ID+" type "+TYPE+" from "+FROM);
            printURLS(srbUrl);
            
            log.debug("file for download: "+srbUrl);
            
            try{
                //not download started so start one
                SRBFileManagerThread man = new SRBFileManagerThread();
                Certificate cert = new Certificate(visit.getSession().getCredential());
                GSSCredential cred = cert.getCredential();
                
                log.info("Credential information "+cred.getName());
                try {
                    man.setCredentials(Util.getSRBHost(srbUrl),Util.getSRBPort(srbUrl),cred);
                    
                    String host = Util.getSRBHost(srbUrl);
                    log.trace("Looking for SRB Server with host: "+host);
                    Collection<SrbServer> srbServers = visit.getSession().getSrbServers();
                    if(srbServers != null){
                        for(SrbServer srbServer : srbServers){
                            log.trace("Found: "+srbServer.getHostname());
                            if(srbServer.getHostname().equalsIgnoreCase(host)){
                                if(srbServer.isDefaultSet()){
                                    log.debug("Default SRB user for : "+Util.getSRBHost(srbUrl)+" is: "+srbServer.getDefaultUser());
                                    man.setDefaultUser(srbServer.getDefaultUser());
                                    man.setDefaultPass(srbServer.getDefaultPassword());
                                    man.setDefaultDomain(srbServer.getDefaultDomain());
                                } else {
                                    log.trace("SRB Server info not set: ");
                                }
                            }
                        }
                    }
                    
                    
                } catch (Exception ex) {
                    log.warn("Error getting SRB host info",ex);
                    RequestDispatcher dispatcher =
                            req.getRequestDispatcher("/protected/downloaderror.jsp?message=Invalid SRB URL Found&name="+name);
                    if (dispatcher != null) dispatcher.forward(req, response);
                    return ;
                }
                
                man.setSRBFile(srbUrl);
                
                if(TYPE.startsWith(DownloadConstants.DATA_FILE.toString()) || TYPE.startsWith(DownloadConstants.DATA_FILE_AS_REF.toString())) {
                    man.setZipeFile(false);
                } else {
                    man.setZipeFile(true);
                }
                
                man.setFolderFilesOnly(true);
                man.start();
                visit.putSrbManager(ID, man);
                //
                
                log.debug("started download: "+name);
                //removeOld();
                //log download
                Collection<String> urls = new ArrayList<String>();
                for(String url : srbUrl){
                    urls.add(url);
                }
                String detail = null;
                if(name.equals(DownloadConstants.DOWNLOAD_MULTIPLE)){
                    detail = name;
                } else detail = DpId+" "+name;
                
                EventDelegate.getInstance().sendDownloadEvent(visit.getSid(),detail,urls);
                
                
                RequestDispatcher dispatcher =
                        req.getRequestDispatcher("/protected/downloadsrb.jsp?close=0&name="+name+"&type="+TYPE+"&url="+ID);
                if (dispatcher != null) dispatcher.forward(req, response);
            } catch(Exception e){
                log.warn(e);
                visit.removeSrbManager(ID);
                RequestDispatcher dispatcher =
                        req.getRequestDispatcher("/protected/downloaderror.jsp");
                if (dispatcher != null) dispatcher.forward(req, response);
            }
        }
        
        else {
            // log.trace("SrbManager not null");
            SRBFileManagerThread man = visit.getSrbManager(ID);
            if(man.isFinished()){
                if(man.getException() == null){
                    log.trace("Download complete");
                    
                    if(TYPE.endsWith("IMAGEJ")){
                        //want to veiw data not download it
                        srbDownload = man.getFile();
                        log.info("Viewing data: "+srbDownload.getAbsolutePath());
                        log.info("Copying to: "+workingDir);
                        String date = Util.createDate();
                        String webStartFile = "";
                        File newFile = new File(workingDir+File.separator+"web-start"+File.separator+date+File.separator+srbDownload.getName());
                        try {
                            Util.copyFile(srbDownload,newFile);
                            webStartFile = Util.createWebStartFile(req,srbDownload.getName(),workingDir,context);
                        } catch (Exception ex) {
                            log.warn(ex);
                            String message = "Unable to launch requested appliacation.";
                            RequestDispatcher dispatcher =
                                    req.getRequestDispatcher("/protected/downloaderror.jsp?message="+message+"&name="+name+"&url="+ID);
                            if (dispatcher != null) dispatcher.forward(req, response);
                        }
                        
                        response.sendRedirect("../web-start/"+date+"/"+webStartFile);
                       /* RequestDispatcher dispatcher =
                                req.getRequestDispatcher("/css/myproxy.jnlp");
                        if (dispatcher != null) dispatcher.forward(req, response);*/
                    }else {
                        //down load data
                        srbDownload = man.getFile();
                        //downloadedFiles.add(srbDownload.getAbsolutePath());
                        log.trace("Downloading: "+srbDownload.getAbsolutePath()+" of TYPE:" +TYPE);
                        log.trace("setting srbman to null");
                        visit.removeSrbManager(ID);
                        
                        ServletOutputStream out = response.getOutputStream();
                        
                        //downloaded file, pump to page
                        ////My Way
                        String contenttype = getContentType(srbDownload.getAbsolutePath());
                        response.setContentType(contenttype);
                        response.setBufferSize(65536);
                        if(TYPE.startsWith(DPUrlRefType.DATA_SET.toString()) || TYPE.equals(DownloadConstants.DATA_FILE_FOLDER.toString()) || TYPE.equals(DownloadConstants.DOWNLOAD_MULTIPLE)) {
                            response.setHeader("Content-disposition","attachment; filename="+name.replaceAll(" ","_")+".zip"+"");
                        } else {
                            response.setHeader("Content-disposition","attachment; filename="+name.replaceAll(" ","_")+"");
                        }
                        
                        //my downlaod way
                        FileInputStream myFileInputStream = new FileInputStream(srbDownload);
                        BufferedInputStream myBufferedInputStream = new BufferedInputStream(myFileInputStream);
                        
                        BufferedOutputStream myBufferedOutputStream = new BufferedOutputStream(out);
                        
                        byte[] buffer = new byte[65536];
                        int c=0;
                        log.trace("nothing after this");
                        while ((c=myBufferedInputStream.read(buffer)) > -1) {
                            myBufferedOutputStream.write(buffer,0,c);
                        }
                        
                        myBufferedOutputStream.flush();
                        myBufferedOutputStream.close();
                        myBufferedInputStream.close();
                        myFileInputStream.close();
                        myFileInputStream = null;
                        
                        log.trace("deleting "+srbDownload.getAbsolutePath()+"?:"+srbDownload.delete());
                        if(TYPE.equals(DPUrlRefType.FILE.toString())) log.trace("deleting "+srbDownload.getParentFile().getAbsolutePath()+"?:"+srbDownload.getParentFile().delete());
                        
                        out.close();
                    }
                } else {
                    visit.removeSrbManager(ID);
                    String message = "";
                    if(man.getException() instanceof NoAccessToDataStorage || (man.getException().getCause() != null && man.getException().getCause() instanceof NoAccessToDataStorage)){
                        log.warn("User: "+visit.getDn()+" is not registered to SRB: "+srbUrl);
                        message = "No access to data storage.";
                    } else if(man.getException() instanceof ReadAccessException || (man.getException().getCause() != null && man.getException().getCause() instanceof ReadAccessException)){
                        log.warn("User: "+visit.getDn()+" has no read access to: "+srbUrl);
                        message = "Insufficient access rights to the data.";
                    } else {
                        log.error("User: "+visit.getDn()+" has cannot download : "+srbUrl,man.getException());
                    }
                    RequestDispatcher dispatcher =
                            req.getRequestDispatcher("/protected/downloaderror.jsp?message="+message+"&name="+name+"&url="+ID);
                    if (dispatcher != null) dispatcher.forward(req, response);
                }
            } else{
                // log.trace("/protected/downloadsrb.jsp?close=0&percentage="+man.getPercentageComplete()+"&name="+name+"&type="+TYPE+"&url="+ID);
                RequestDispatcher dispatcher =
                        req.getRequestDispatcher("/protected/downloadsrb.jsp?close=0&percentage="+man.getPercentageComplete()+"&name="+name+"&type="+TYPE+"&url="+ID);
                if (dispatcher != null) dispatcher.forward(req, response);
            }
        }
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
        return "Short description";
    }
    
    /**
     * Uses the file extension to determine the type of file.
     *
     * @param filename  name of the file whose type to determine
     * @return          the content type of the given file
     */
    public String getContentType(String filename) {
        int idx = filename.lastIndexOf(".");
        if(idx == -1)
            return new String("application/octet-stream");
        
        String ext = filename.substring(idx+1, filename.length());
        
        if(ext.equals("ps")) {
            return new String("application/postscript");
        } else if(ext.equalsIgnoreCase("pdf")) {
            return new String("application/pdf");
        } else if(ext.equalsIgnoreCase("zip")) {
            return new String("application/zip");
        } else if(ext.equalsIgnoreCase("jpg")) {
            return new String("image/jpeg");
        } else if(ext.equalsIgnoreCase("gif")) {
            return new String("image/gif");
        } else if(ext.equalsIgnoreCase("tif")) {
            return new String("image/tiff");
        } else if(ext.equalsIgnoreCase("mpg")) {
            return new String("video/mpeg");
        } else if(ext.equalsIgnoreCase("mov")) {
            return new String("video/quicktime");
        } else if(ext.equalsIgnoreCase("htm")) {
            return new String("text/html");
        } else if(ext.equalsIgnoreCase("html")) {
            return new String("text/html");
        } else if(ext.equalsIgnoreCase("xml")) {
            return new String("text/xml");
        } else if(ext.equalsIgnoreCase("doc")) {
            return new String("application/msword");
        } else if(ext.equalsIgnoreCase("txt") || ext.equalsIgnoreCase("out") || ext.equalsIgnoreCase("err")) {
            return new String("text/plain");
        }
        
        return new String("application/octet-stream");
    }
    
    private void printURLS(String[] srbUrl) {
        log.trace("URLS for download:");
        for(String url : srbUrl){
            log.trace(url);
        }
    }
    
}

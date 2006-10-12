package uk.ac.dl.dp.web.util;
/*
 * DownloadServlet.java
 *
 * Created on 16 March 2005, 10:54
 */
import java.nio.channels.FileChannel;
import org.ietf.jgss.GSSCredential;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.util.Certificate;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.web.backingbeans.Visit;
import uk.ac.dl.dp.web.util.WebConstants;
import org.apache.log4j.*;
import java.io.*;
import java.net.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
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
        
        String id = req.getParameter("url");
        String type = req.getParameter("type");
        String from = req.getParameter("from");
        String[] srbUrl = null;
        String name = null;
        File srbDownload = null;
        
        if(from.equals("DATA_CENTER")){
            if(type.startsWith("DATA_CENTER_FILE")){
                Url file = getDataUrl(id,visit);
                name = file.getName();
                srbUrl = new String[]{file.getName()};
            } else{
                DataReference file = getDataRef(id,visit);
                name = file.getName();
                srbUrl = new String[]{file.getName()};
            }
        } else if(type.equals("DOWNLOAD")){
            if(from.equals("DATA_SETS")){
                //from data ref or data sets
                srbUrl =  getDownloadSRBFiles(visit);
                name = "Multiple_DataPortal_Download";
            } else {
                //from data center
                srbUrl =  getDownloadSRBFilesRef(visit);
                name = "Multiple_DataPortal_Download";
                
            }
        }  else if(type.startsWith(DPUrlRefType.FILE.toString())){
            
            DataFile file = getDataFile(id,visit);
            name = file.getName();
            srbUrl = new String[]{file.getName()};
        } else{
            DataSet file = getDataSet(id,visit);
            name = file.getName();
            srbUrl = new String[]{file.getName()};
        }
        
        log.info("Id for download is "+id+" type "+type+" name ");
        
        if(!visit.contains(id)){
            
            log.debug("file for download: "+srbUrl);
            
            try{
                SRBFileManagerThread man = new SRBFileManagerThread();
                Certificate cert = new Certificate(visit.getSession().getCredential());
                GSSCredential cred = cert.getCredential();
                
                log.info("Credential information "+cred.getName());
                
                man.setCredentials("srb1.ngs.rl.ac.uk",5544,cred);
                //man.setCredentials("glen-drinkwater",cred);
                
                if(type.equals(DPUrlRefType.DATA_SET.toString())) {
                    man.setZipeFile(true);
                    man.setSRBFile(new String[]{"srb://dfdfd:676/ngs/home/glen-drinkwater.ngs"});
                } else if(type.equals("DOWNLOAD")){
                    
                    //test
                    Collection<String> urls = new ArrayList<String>();
                    String[] files  = { "SSHTermApplet-signed.jar","extract.png", "hourglass-a.gif","view.jsp","FluorescentCells.jpg","webui.jar"};
                    for(int i = 0; i < files.length;i++){
                        if(i >= 5) break;
                        else if(i == srbUrl.length) break;
                        else urls.add("srb://dfdfd:676/ngs/home/glen-drinkwater.ngs/"+files[i]);
                    }
                    
                    man.setSRBFile(urls.toArray(new String[urls.size()]));
                    man.setZipeFile(true);
                }else {
                    
                    man.setSRBFile(new String[]{"srb://dfdfd:676/ngs/home/glen-drinkwater.ngs/FluorescentCells.jpg"});
                    man.setZipeFile(false);
                }
                
                man.start();
                visit.putSrbManager(id, man);
                //
                
                log.debug("started download: "+name);
                //removeOld();
                
                RequestDispatcher dispatcher =
                        req.getRequestDispatcher("/protected/downloadsrb.jsp?close=0&name="+name+"&type="+type);
                if (dispatcher != null) dispatcher.forward(req, response);
            } catch(Exception e){
                log.warn(e);
                visit.removeSrbManager(id);
                RequestDispatcher dispatcher =
                        req.getRequestDispatcher("/protected/downloaderror.jsp");
                if (dispatcher != null) dispatcher.forward(req, response);
            }
        }
        
        else {
            log.trace("SrbManager not null");
            SRBFileManagerThread man = visit.getSrbManager(id);
            if(man.isFinished()){
                if(man.getException() == null){
                    log.trace("Download complete");
                    
                    if(type.endsWith("IMAGEJ")){
                        //want to veiw data not download it
                        srbDownload = man.getFile();
                        log.info("Viewing data: "+srbDownload.getAbsolutePath());
                        log.info("Copying to: "+workingDir);
                        String date = createDate();
                        String webStartFile = "";
                        File newFile = new File(workingDir+File.separator+"web-start"+File.separator+date+File.separator+srbDownload.getName());
                        try {
                            copyFile(srbDownload,newFile);
                            webStartFile = createWebStartFile(req,srbDownload.getName());
                        } catch (Exception ex) {
                            log.warn(ex);
                            String message = "Unable to launch requested appliacation.";
                            RequestDispatcher dispatcher =
                                    req.getRequestDispatcher("/protected/downloaderror.jsp?message="+message+"&name="+name);
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
                        log.trace("Downloading: "+srbDownload.getAbsolutePath());
                        log.trace("setting srbman to null");
                        visit.removeSrbManager(id);
                        
                        ServletOutputStream out = response.getOutputStream();
                        
                        //downloaded file, pump to page
                        ////My Way
                        String contenttype = getContentType(srbDownload.getAbsolutePath());
                        response.setContentType(contenttype);
                        response.setBufferSize(65536);
                        if(type.equals(DPUrlRefType.DATA_SET.toString()) || type.equals("DOWNLOAD")) {
                            response.setHeader("Content-disposition","attachment; filename="+name+".zip"+" ");
                        } else {
                            response.setHeader("Content-disposition","attachment; filename=FluorescentCells.jpg");
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
                        if(type.equals(DPUrlRefType.FILE.toString())) log.trace("deleting "+srbDownload.getParentFile().getAbsolutePath()+"?:"+srbDownload.getParentFile().delete());
                        
                        out.close();
                    }
                } else {
                    String message = "";
                    if(man.getException() instanceof NoAccessToDataStorage){
                        log.warn("User: "+visit.getDn()+" is not registered to SRB: "+srbUrl);
                        message = "No access to data storage.";
                    }else if(man.getException() instanceof ReadAccessException){
                        log.warn("User: "+visit.getDn()+" has no read access to: "+srbUrl);
                        message = "Insufficient access rights to the data.";
                    }
                    RequestDispatcher dispatcher =
                            req.getRequestDispatcher("/protected/downloaderror.jsp?message="+message+"&name="+name);
                    if (dispatcher != null) dispatcher.forward(req, response);
                }
            } else{
                log.trace("Download : "+man.getPercentageComplete());
                RequestDispatcher dispatcher =
                        req.getRequestDispatcher("/protected/downloadsrb.jsp?close=0&percentage="+man.getPercentageComplete()+"&name="+name+"&type="+type);
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
    
    private DataFile getDataFile(String param, Visit visit) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        
        for(DataFile file : visit.getVisitData().getCurrentDatafiles()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.debug("Found datafile: "+file);
                return file;
            }
        }
        return null;
    }
    
    private DataSet getDataSet(String param, Visit visit) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        
        for(DataSet file : visit.getVisitData().getCurrentDatasets()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.debug("Found dataset: "+file);
                return file;
            }
        }
        return null;
    }
    //TODO add into VistData
    private DataReference getDataRef(String param, Visit visit) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        log.trace("looking for:"+ fac+"-"+id);
        
        for(DataReference file : visit.getVisitData().getCurrentDataReferences()){
            log.trace(file.getFacility()+"-"+file.getId()+"-"+file.getName());
            if(file.getId().toString().equals(id) && file.getFacility().equals(fac) ){
                log.debug("Found dataref: "+file);
                return file;
            }
        }
        return null;
    }
    
    private Url getDataUrl(String param, Visit visit) {
        String fac = param.split("-")[0];
        String data_ref_id = param.split("-")[1];
        String url_id = param.split("-")[2];
        
        for(DataReference file : visit.getVisitData().getCurrentDataReferences()){
            if(file.getId().toString().equals(data_ref_id) && file.getFacility().equals(fac) ){
                log.debug("Found dataref: "+file);
                Collection<Url> urls = file.getUrls();
                for(Url url : urls){
                    if(url.getId().toString().equals(url_id)){
                        return url;
                    }
                }
            }
        }
        return null;
    }
    
    private void copyFile(File source, File dest) throws Exception {
        
        //NB:  NIO did not work on gen 2.  1.4.2_06 jre from sun.  Called a
        // java.io.Exception:  Illegal arguement exception.  Arguments were fine.
        //Error with NIO on gen 2??????
        
        log.warn("File names are "+source.toString() +"   and "+dest.toString());
        if(!dest.getParentFile().exists()) dest.getParentFile().mkdir();
        
        FileChannel sourceChannel = new   FileInputStream(source).getChannel();
        FileChannel destinationChannel = new   FileOutputStream(dest).getChannel();
        sourceChannel.transferTo(0, sourceChannel.size(), destinationChannel);
        // or
        //  destinationChannel.transferFrom(sourceChannel, 0, sourceChannel.size());
        sourceChannel.close();
        destinationChannel.close();
        
        /*log.trace("Copying "+source+" to "+dest);
        try {
            BufferedReader br = new BufferedReader(new FileReader(source));
            BufferedWriter bw = new BufferedWriter(new FileWriter(dest));
         
            int read = 0;
            while((read = br.read()) != -1) {
         
                bw.write(read);
            }
         
            br.close();
            bw.close();
        } catch (FileNotFoundException fnfe) {
            throw fnfe;
         
        } catch (IOException e) {
            throw e;
        }*/
        
    }
    
    private void removeOld(){
        for(String file: downloadedFiles){
            if(new File(file).exists()){
                File fileF = new File(file);
                boolean delete = fileF.delete();
                log.info("deleting old file?: "+fileF.getAbsolutePath()+" " +delete);
                if(delete) downloadedFiles.remove(file);
            }
            
        }
    }
    
    private String createDate() {
        int day = Calendar.getInstance().get(Calendar.DAY_OF_MONTH);
        int month = Calendar.getInstance().get(Calendar.MONTH);
        int year = Calendar.getInstance().get(Calendar.YEAR);
        return day+"-"+month+"-"+year;
    }
    
    private String createWebStartFile(HttpServletRequest req, String SRBFilename) throws UnknownHostException  {
        ServletRequest sreq = (ServletRequest)req;
        File file = new File(workingDir+File.separator+"web-start"+File.separator+"webstart_template.jnlp");
        File webStartDir = new File(workingDir+File.separator+"web-start"+File.separator+createDate());
        if(!webStartDir.exists()) webStartDir.mkdir();
        
        String filename = ""+Math.random()+".jnlp";
        File webStartFile = new File(webStartDir,filename);
        
        BufferedReader in = null;
        BufferedWriter out = null;
        InetAddress host = InetAddress.getLocalHost();
        String hostURL = "http://"+host.getHostAddress()+":"+sreq.getServerPort()+context+"/web-start/"+createDate();
        try {
            in = new BufferedReader(new FileReader(file));
            out = new BufferedWriter(new FileWriter(webStartFile));
            
            String str;
            while ((str = in.readLine()) != null) {
                if(str.trim().startsWith("codebase")){
                    out.write("codebase=\""+hostURL+"\"\n");
                    
                } else if(str.trim().startsWith("href")){
                    out.write("href=\""+filename+"\">\n");
                    
                } else  if(str.trim().startsWith("<application-desc")){
                    out.write(str);
                    out.write("\n<argument>"+hostURL+"/"+SRBFilename+"</argument>\n");
                    out.write("</application-desc>\n");
                    out.write("</jnlp>");
                    break;
                    
                } else out.write(str+"\n");
            }
            
        } catch (IOException e) {
            System.out.println(e);
        } finally {
            try {
                in.close();
                out.close();
                
                //deelte old date files
                File dir = new File(workingDir+File.separator+"web-start");
                File[] webStartDatefiles = dir.listFiles();
                for(File Sfile : webStartDatefiles){
                    log.trace("File: "+Sfile.getAbsolutePath());
                    if(Sfile.isDirectory()){
                        if(!Sfile.getName().equals(createDate())) {
                            log.trace("Deleting");
                            deleteDirectory(Sfile);
                        } else log.trace("Not deleting");
                    }
                }
            } catch (IOException ex) {
                log.warn("Error closing buffers",ex);
            }
        }
        return filename;
    }
    
    /**
     * Recursively deletes a directory, dangerous!
     * @param directory the directory to delete
     */
    public  boolean deleteDirectory(File directory) {
        return  deleteDirectory(directory, directory.getPath().length()); }
    
    private  boolean deleteDirectory(File directory, int pathSize) {
        if (directory != null && directory.isDirectory()) {
            // get all the files in the directory
            File[] files = directory.listFiles();
            
            for (int i = 0; i < files.length; i++) {
                File file = files[i];
                
                // do a simple check to ensure that the pathname isn't getting
                // smaller - this might save wiping an entire HDD if a hard
                // link has been setup somewhere
                if (file.getPath().length() > pathSize) {
                    // if the file is a directory, recurse
                    if (file.isDirectory()){
                        boolean success =  deleteDirectory(file, file.getPath().length());
                        if (!success) {
                            return false;
                        }
                    } else {
                        boolean success = file.delete();
                        log.trace("Trying to delete: "+file.getName()+"?: "+success);
                        if (!success) {
                            return false;
                        }
                    }
                } else log.warn("Stopped deleting "+directory.getAbsolutePath()+", detected a link to smaller path size: "+file.getPath());
            }
            
            // finally delete the parent directory
            return  directory.delete();
        }
        return directory.delete();
    }
    
    private synchronized String[] getDownloadSRBFiles(Visit visit) {
        Collection<String> urls = new ArrayList<String>();
        for(DataFile file : visit.getVisitData().getCurrentDatafiles()){
            if(file.isDownload()){
                log.debug("Found datafile to download: "+file);
                urls.add(file.getUri());
                //now set to false
                // file.setDownload(false);
            }
        }
        
        return urls.toArray(new String[urls.size()]);
    }
    
    private synchronized String[] getDownloadSRBFilesRef(Visit visit) {
        Collection<String> urls = new ArrayList<String>();
        for(DataReference file : visit.getVisitData().getCurrentDataReferences()){
            if(file.isDownload() && file.getTypeOfReference().equals(DPUrlRefType.FILE.toString())){
                for(Url url : file.getUrls()){
                    log.debug("Found dataref to download: "+file);                    
                    urls.add(url.getUrl());
                }
            } else{
                for(Url url : file.getUrls()){
                    if(url.isDownload()){
                        log.debug("Found datafile to download: "+url);
                        urls.add(url.getUrl());
                        //now set to false
                        // file.setDownload(false);
                    }
                }
            }
        }       
        
        return urls.toArray(new String[urls.size()]);
    }
    
}

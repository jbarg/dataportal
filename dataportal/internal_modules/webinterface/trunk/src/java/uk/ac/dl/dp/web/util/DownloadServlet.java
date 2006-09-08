package uk.ac.dl.dp.web.util;
/*
 * DownloadServlet.java
 *
 * Created on 16 March 2005, 10:54
 */
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
    
    /** Initializes the servlet.
     */
    public void init(ServletConfig config) throws ServletException {
        super.init(config);
        
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
        
        String id = req.getParameter("url");
        String type = req.getParameter("type");
        String from = req.getParameter("from");
        String srbUrl = null;
        String name = null;
        File srbDownload = null;
        
        if(from.equals("DATA_CENTER")){
            if(type.equals("DATA_CENTER_FILE")){
                Url file = getDataUrl(id,visit);
                name = file.getName();
                srbUrl = file.getName();
            } else{
                DataReference file = getDataRef(id,visit);
                name = file.getName();
                srbUrl = file.getName();
            }
        } else if(type.equals(DPUrlRefType.FILE.toString())){
            DataFile file = getDataFile(id,visit);
            name = file.getName();
            srbUrl = file.getName();
        } else{
            DataSet file = getDataSet(id,visit);
            name = file.getName();
            srbUrl = file.getName();
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
                } else {
                    man.setSRBFile(new String[]{"srb://dfdfd:676/ngs/home/glen-drinkwater.ngs/test/log4j.properties"});
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
                    if(type.equals(DPUrlRefType.DATA_SET.toString())) {
                        response.setHeader("Content-disposition","attachment; filename="+name+".zip"+" ");
                    } else {
                        response.setHeader("Content-disposition","attachment; filename="+name+" ");
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
        
        for(DataFile file : visit.getCurrentDatafiles()){
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
        
        for(DataSet file : visit.getCurrentDatasets()){
            if(file.getId().equals(id)&& file.getFacility().equals(fac)){
                log.debug("Found dataset: "+file);
                return file;
            }
        }
        return null;
    }
    
    private DataReference getDataRef(String param, Visit visit) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        log.trace("looking for:"+ fac+"-"+id);
        
        for(DataReference file : visit.getCurrentDataReferences()){
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
        
        for(DataReference file : visit.getCurrentDataReferences()){
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
        
                /*logger.warn("File names are "+in.toString() +"   and "+out.toString());
        FileChannel sourceChannel = new
        FileInputStream(in).getChannel();
        FileChannel destinationChannel = new
        FileOutputStream(out).getChannel();
        sourceChannel.transferTo(0, sourceChannel.size(), destinationChannel);
        // or
        //  destinationChannel.transferFrom(sourceChannel, 0, sourceChannel.size());
        sourceChannel.close();
        destinationChannel.close();*/
        log.trace("Copying "+source+" to "+dest);
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
        }
        
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
}

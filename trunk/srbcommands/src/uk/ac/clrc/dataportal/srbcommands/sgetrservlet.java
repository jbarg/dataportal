/*
 * sgetrservlet.java
 *
 * Created on 12 August 2003, 15:15
 */

package uk.ac.clrc.dataportal.srbcommands;

import javax.servlet.*;
import javax.servlet.http.*;



import java.util.*;
import java.io.*;
import java.lang.StringBuffer;
import org.apache.log4j.*;
/**
 *
 * @author  maw24
 * @version
 */
public class sgetrservlet extends HttpServlet {
    
    static Logger log = Logger.getLogger("sgetrservlet.class");
    private boolean sinitdone = false;
    private Properties props = null;
    private ServletConfig scon = null ;
    String workingDir = null;
    
    public void init(ServletConfig config) throws ServletException {
        scon = config;
        super.init(config);
        try {
            ServletContext sc = scon.getServletContext();
            workingDir = sc.getRealPath("");
            try{
                props = getProps(workingDir);
            }
            catch(Exception e){
                log.warn(e);
                throw new IOException("Unable to load properties file");
            }
            this.Sinit();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public void destroy() {
        try {
            this.Sexit();
        }
        catch (Exception e) {
            super.destroy();
        }
    }
    
    public Properties getProps(String propertiesFileName) throws Exception {
        //String propertiesFileName = "/home/tomcat4/srb.properties";
        
        try {
            log.info("Loading SRB properties from " + propertiesFileName+File.separator+"WEB-INF"+File.separator+"srb.properties");
            props = new Properties();
            FileInputStream in = new FileInputStream(propertiesFileName+File.separator+"WEB-INF"+File.separator+"srb.properties");
            props.load(in);
            in.close();
        }
        catch (Exception e) {
            throw e;
        }
        
        return props;
    }
    
    public void Sinit() throws Exception {
        // Log on to SRB - we could have made this a general purpose login so that clients could use this web service to interact with any SRB server
        // but for now just have it so that the Web Service is configured locally and connects to just one SRB server / domain
        if (! sinitdone) {
            log.info("Logging on to SRB");
            String srbHome = props.getProperty("srbHome");
            if(srbHome == null) log.info("srbHome is null");
            else log.info("Executing "+props.getProperty("srbHome") + "/Sinit");
            LaunchProcess.runCommand( props.getProperty("srbHome") + "/Sinit");
        }
        sinitdone = true;
    }
    
    public void Sexit() throws Exception {
        // For completeness include a logout method
        if (sinitdone) {
            log.info("Logged out from SRB");
            LaunchProcess.runCommand( props.getProperty("srbHome") + "/Sexit");
        }
        sinitdone = false;
    }
    
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, java.io.IOException {
        ServletOutputStream out = response.getOutputStream();
        File list = null;
        String exe = null;
        try {
            //check password
            String password = props.getProperty("srb_passwd");
             exe = request.getParameter("format");
            
            if(exe == null ) exe  ="tar";
            
            //String executable = props.getProperty("tarHome");
            //if(exe.equals("tar")) executable = props.getProperty("tarHome")+" -cvf";
            //else if(exe.equals("zip")) executable = props.getProperty("zipHome");
            
            if(!password.equals("dp()u5er4em@a^")){
                
                response.flushBuffer();
                response.setContentType("text/html");
                response.setHeader("Content-disposition",null);
                
                out.println("<html>");
                out.println("<head>");
                out.println("<title>Servlet</title>");
                out.println("</head>");
                out.println("<h1>Wrong password!</h1>");
                out.println("<body>");
                out.println("</body>");
                out.println("</html>");
                out.close();
                
                String ip = request.getRemoteAddr();
                String time = new java.util.Date().toString();
                String query = request.getQueryString();
                System.out.println("----------------");
                System.out.println("WRONG PASSWORD: ");
                System.out.println("----------------");
                System.out.println("ip address: "+ip);
                System.out.println("time: "+time);
                System.out.println("query string: "+query);
                System.out.println("----------------");
                return;
            }
            
            
            //does file /tmp/srbtemp exist
            File srbtemp = new File(props.getProperty("srbDest"));
            if(!srbtemp.exists()) srbtemp.mkdir();
            
            response.setContentType("application/download");
            
            //add not cacheing etc
            
            response.setBufferSize(65536);
            String dir = request.getParameter("dir");
            String filename = dir.substring(dir.lastIndexOf("/") + 1);
            filename = filename.replace('.','_');
            
            
            response.setHeader("Content-disposition","attachment; filename="+filename+exe+"."+exe);
            
            //this is the one that is working with the whole collection
            File srbtempdir = new File(props.getProperty("srbDest"));
            if(!srbtempdir.exists()) srbtempdir.mkdir();
            File cre = new File(props.getProperty("srbDest")+File.separator+request.getSession().getId());
            cre.mkdir();
            
            LaunchProcess.runCommand( props.getProperty("srbHome") + File.separator+"Sget -r " + dir +props.getProperty("srbDest") + File.separator+request.getSession().getId());
            
            //this is my one with the single files
            // LaunchProcess.runCommand("/bin/sh -c \""+ props.getProperty("srbHome") + "/Sls -C " + dir + " > /tmp/srbtemp/" + request.getSession().getId()+"conf\"");
            
            /*Vector files = ReaderRights.ReaderRights("glen","/tmp/srbtemp/"+request.getSession().getId()+"conf",Runtime.getRuntime().exec(props.getProperty("srbHome") + "/Sls -C " + dir ).getInputStream());
            StringBuffer filenames = new StringBuffer();
            for(int i = 0; i < files.size();i++){
                filenames.append(" "+files.get(i));
            }
             
            LaunchProcess.runCommand( props.getProperty("srbHome") + "/Sget  -r " + dir +" "+ filenames.toString() + " /tmp/srbtemp/" + request.getSession().getId());
             
             
            // Even though this works when typed into a shell it doesn't work from here!!  More work needed.....
            //LaunchProcess.runCommand( "/bin/sh -c \"cd /tmp/srbtemp/" + request.getSession().getId() + ";/usr/bin/zip -mr /tmp/srbtemp/" + request.getSession().getId() + ".zip .\"");
             */
            
            String listing = "/bin/ls -ltr "+props.getProperty("srbDest")+File.separator+"$1 | grep -v '^d' | grep -v total | awk '{print $9}' > "+props.getProperty("srbDest")+File.separator+"$1.list";
            //execute cammand.  List the files in the directory and prints it out to file
            //LaunchProcess.runCommand("ls -ltr "+props.getProperty("srbDest") +File.separator+ request.getSession().getId()+" | grep -v '^d' | grep -v total | awk '{print $9}' > "+props.getProperty("srbDest") +File.separator+ request.getSession().getId()+".list");
            String newFile = workingDir+File.separator+"run.sh";
            LaunchProcess.runCommand(newFile + " "+request.getSession().getId());
            
            
            //read in file and add all the files to a buffer
             list = new File(props.getProperty("srbDest") +File.separator+ request.getSession().getId()+".list");
            BufferedReader buff = new BufferedReader(new FileReader(list));
            String str;
            StringBuffer b = new StringBuffer();
            while((str = buff.readLine()) !=null){
                b.append(str+" ");
                
            }
            buff.close();
            // Need to fix the above command to strip off the extra path information that we don't want
            //LaunchProcess.runCommand( "/usr/bin/zip -mr /tmp/srbtemp/" + request.getSession().getId() + ".zip /tmp/srbtemp/" + request.getSession().getId());
            if(exe.equals("tar")){
                LaunchProcess.runCommand( props.getProperty("tarHome")+" -cvf "+props.getProperty("srbDest") +File.separator+ request.getSession().getId() + ".tar -C "+props.getProperty("srbDest") +File.separator+ request.getSession().getId()+" "+b.toString());
            }
            else if(exe.equals("zip")){
                //LaunchProcess.runCommand("cd"+ props.getProperty("srbDest"));
                //LaunchProcess.runCommand( props.getProperty("zipHome")+" -rvj "+ request.getSession().getId() + ".zip  "+props.getProperty("srbDest") +File.separator+ request.getSession().getId()+File.separator+"*");
                
            }
            else if(exe.equals("jar")){
                LaunchProcess.runCommand( props.getProperty("jarHome")+" -cvf "+props.getProperty("srbDest") +File.separator+ request.getSession().getId() + ".jar -C "+props.getProperty("srbDest") +File.separator+ request.getSession().getId()+" "+b.toString());
                
            }
            else{
                LaunchProcess.runCommand( props.getProperty("jarHome")+" -cvf "+props.getProperty("srbDest") +File.separator+ request.getSession().getId() + ".tar -C "+props.getProperty("srbDest") +File.separator+ request.getSession().getId()+" "+b.toString());
            }
            File myFileIn = null;
            
            myFileIn = new File(props.getProperty("srbDest")+File.separator + request.getSession().getId() + "."+exe);
            
            FileInputStream myFileInputStream = new FileInputStream(myFileIn);
            BufferedInputStream myBufferedInputStream = new BufferedInputStream(myFileInputStream);
            
            BufferedOutputStream myBufferedOutputStream = new BufferedOutputStream(out);
            
            byte[] buffer = new byte[65536];
            int c=0;
            
            while ((c=myBufferedInputStream.read(buffer)) > -1) {
                myBufferedOutputStream.write(buffer,0,c);
            }
            
            myBufferedOutputStream.flush();
            myBufferedOutputStream.close();
            myBufferedInputStream.close();
            
            out.close();
            list.delete();
            LaunchProcess.runCommand( props.getProperty("rmHome")+" -r "+props.getProperty("srbDest") +File.separator+ request.getSession().getId() + "."+exe);
            LaunchProcess.runCommand( props.getProperty("rmHome")+" -r "+props.getProperty("srbDest") +File.separator+ request.getSession().getId() );
            
        }
        catch (java.lang.Exception e) {
            
            // Better error handling is required for when things go wrong.....
            try{
                list.delete();
                LaunchProcess.runCommand( props.getProperty("rmHome")+" -r "+props.getProperty("srbDest") +File.separator+ request.getSession().getId() + "."+exe);
                LaunchProcess.runCommand( props.getProperty("rmHome")+" -r "+props.getProperty("srbDest") +File.separator+ request.getSession().getId() );
            }
            catch(Exception ignore){}
            
            response.flushBuffer();
            response.setContentType("text/html");
            response.setHeader("Content-disposition",null);
            
            out.println("<html>");
            out.println("<head>");
            out.println("<title>Servlet</title>");
            out.println("</head>");
            out.println("<h1>ERROR!</h1>");
            out.println("<body>");
            out.println("</body>");
            out.println("</html>");
            out.close();
            e.printStackTrace();
        }
    }
    
    /** Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, java.io.IOException {
        try {
            processRequest(request, response);
        }
        catch (java.lang.Exception e) {
            e.printStackTrace();
        }
    }
    
    /** Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, java.io.IOException {
        try {
            processRequest(request, response);
        }
        catch (java.lang.Exception e) {
            e.printStackTrace();
        }
    }
    
    /** Returns a short description of the servlet.
     */
    public String getServletInfo() {
        return "SRB Sget -r wrapper";
    }
    
}

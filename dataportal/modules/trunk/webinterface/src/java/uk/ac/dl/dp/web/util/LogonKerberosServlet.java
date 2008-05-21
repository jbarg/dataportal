/*
 * LogonKerberosServlet.java
 *
 * Created on 09 January 2007, 09:01
 */

package uk.ac.dl.dp.web.util;

import java.util.Properties;
import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;
import org.apache.log4j.Logger;

import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.util.DPDefaultLocation;
import uk.ac.dl.dp.web.backingbeans.Visit;

/**
 *
 * @author gjd37
 * @version
 */
public class LogonKerberosServlet extends HttpServlet {
    
    private static Logger log = Logger.getLogger(LogonKerberosServlet.class);
    
    
    // Main entry point
    
    public void doGet(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        
        // extract attributes from headers.... username
        String KRB5CCNAME = request.getHeader("krb5ccname");
        String sid = null;
        SessionDTO session = null;
        
        if(KRB5CCNAME == null){
            log.debug("krb5ccname header is null");      
            
            
            response.sendRedirect("../faces/logon.jsp");
        } else{
            try {
                
                
                SessionDelegate sd = SessionDelegate.getInstance();
                
                sid = sd.login(KRB5CCNAME);
                
                log.info("Logged in with sid "+sid);
                
                //get session info once logged in to DP
                session  = sd.getSession(sid);
                log.info("Expire time: "+session.getExpireTime());
                log.trace("User prefs: "+session.getUserPrefs().getResultsPerPage());
                
                //check user
                boolean loggedIn = checkUser(session);
                if(!loggedIn){
                    log.warn("User "+session.getDN()+" has no access to Data Portal");
                    response.sendRedirect("../faces/logon.jsp");
                    return;                   
                }
                
                //logged in ok, get session visit bean, if not there this methoid creates one and sets the returned session
                HttpSession httpSession = request.getSession();
                Visit visit = new Visit();
                visit.setLogonType("LOG_ON_KERBEROS");
                visit.setSession(session);
                
                httpSession.setAttribute(WebConstants.SESSION_KEY,visit);
                
                //logged in, return ok, return to default location
                DPDefaultLocation defaultLocation = session.getUserPrefs().getDefaultLocation();
                
                
                if(defaultLocation.toString().equals(DPDefaultLocation.BASIC_SEARCH.toString())){
                    response.sendRedirect("../faces/protected/basic_search.jsp");
                } else if(defaultLocation.toString().equals(DPDefaultLocation.BOOKMARKS.toString())){
                    response.sendRedirect("../faces/protected/bookmark.jsp");
                } else if(defaultLocation.toString().equals(DPDefaultLocation.DATA_REFERENCES.toString())){
                    response.sendRedirect("../faces/protected/data_center.jsp");
                } else if(defaultLocation.toString().equals(DPDefaultLocation.MY_DATA.toString())){
                    
                    //need to search the date and then return to mydata
                    //TODO should not be in this bean
                   /* MyDataSearchBean myDataSearchBean = new MyDataSearchBean();
                    httpSession.setAttribute("myDataSearchBean",myDataSearchBean);
                    myDataSearchBean.myDataSearchForKerberos();
                    
                    
                    //check that have some data
                    if(((Visit)httpSession.getAttribute("visit")).getVisitData().getSearchedInvestigations() == null || ((Visit)httpSession.getAttribute("visit")).getVisitData().getSearchedInvestigations().size() == 0){
                        //no data associated
                        response.sendRedirect("../faces/protected/basic_search.jsp");
                    } else response.sendRedirect("../faces/protected/investigations.jsp");*/
                    response.sendRedirect("../faces/protected/basic_search.jsp");
                } else     response.sendRedirect("../faces/protected/basic_search.jsp");
                
                
            } catch(Exception e){
                log.error("Login kerberos error, sending to normal login" ,e);
                response.sendRedirect("../faces/logon.jsp");
            }
        }
        
        // Get the output writer
        /*PrintWriter out = response.getWriter();
         
        // extract attributes from headers.... username
        String KRB5CCNAME = request.getHeader("krb5ccname");
         
        if(KRB5CCNAME == null){
            log.debug("krb5ccname header is null");
            response.sendRedirect("../faces/logon.jsp");
        } else{
            try {
         
                log.debug("krb5ccname header is "+KRB5CCNAME);
                // Send out page header
                response.setContentType("text/html");
                out.println("<html>");
                out.println("<head>");
                out.println("<title>SSO Test Page</title>");
                out.println("<style type=\"text/css\">");
                out.println(".var { font-family:Arial, Verdana, sans-serif; font-size:12px; color:black; font-weight:900; vertical-align:top }");
                out.println(".val { font-family:Arial, Verdana, sans-serif; font-size:12px; color:blue; vertical-align:top }");
                out.println("</style>");
                out.println("</head>");
         
                // start on outputting the body
                out.println("<body bgcolor=\"#EEEE33\">");
                out.println("<h1>SSO Test Page</h1>");
         
                // output table of attributes
                out.println("<table width=\"600\" align=\"center\" cellpadding=\"10\" cellspacing=\"0\" border=\"1\" bgcolor=\"#eeeeee\">");
                out.println("<tr>");
                out.println("<td colspan=\"2\" class=\"var\" style=\"font-size:16px; text-align:center\">Environment variables</td>");
                out.println("</tr>");
                out.println("<tr>");
         
                //find out file properties
                File file = new File(KRB5CCNAME);
                if(!file.exists())  log.trace(KRB5CCNAME +" does not exist");
                else {
                    log.info("FILE PERMISSIONS ON : "+KRB5CCNAME);
                    log.info(file.canRead());
                    log.info(file.canWrite());
                    log.info(file.canExecute());
                }
         
                // loop through all headers.
                for (Enumeration e =  request.getHeaderNames(); e.hasMoreElements() ;) {
         
                    String var = (String) e.nextElement();
                    log.trace(request.getHeader(var));
                    out.println("<tr>");
                    out.println("<td class=\"var\">"+var+"</td><td class=\"val\">"+request.getHeader(var)+"</td>");
                    out.println("</tr>");
                }
                String t = request.getQueryString();
                out.println("<tr>");
                out.println("<td class=\"var\"><em>QueryString</em></td><td class=\"val\">"+t+"</td>");
                out.println("</tr>");
         
                // Run klist to find out the username from the credential
                Process p = (Runtime.getRuntime()).exec("env KRB5CCNAME="+KRB5CCNAME+" /usr/kerberos/bin/klist -c");
                p.waitFor();
                BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
                BufferedReader ebr = new BufferedReader(new InputStreamReader(p.getErrorStream()));
         
                //
         
         
                // Display output and search for principal name
                String pname = null;
                String line=br.readLine();
                String returned="<H4>Output:</H4>";
                log.trace("Output of klist is:");
                while(line!=null) {
                    log.trace(line);
                    if(line.startsWith("Default principal:")) {
                        pname = line.substring(19,line.indexOf("@"));
                    }
                    returned+=line+"<br>";
                    line = br.readLine();
                }
                line=ebr.readLine();
                returned+="<h4>Errors:</h4>";
                while(line!=null) {
                    returned+=line+"<br>";
                    line = ebr.readLine();
                }
         
                out.println("<H3>klist returned:</H3>");
                out.println(returned);
         
                log.trace("Klist returned "+returned);
         
                if(pname==null) {
                    out.println("<p>Could not find principal name...</p>");
                    //copy file
                    fileCopy(KRB5CCNAME,KRB5CCNAME+Math.random());
                } else {
                    out.println("<p>Principal name is: "+pname+"</p>");
                }
         
                // Download the proxy
                GSSCredential cred = saslProxy(out, pname, KRB5CCNAME);
                if(cred==null) {
                    out.println("<H3>Could not get a GSSCredential</H3>");
                } else {
                    out.println("<H3>Got a GSSCredential</H3>");
                    out.println("<p><strong>Name: "+cred.getName()+"</strong></p>");
                }
                out.println("</body>");
                out.println("</html>");
            } catch(Exception e) {
                log.error("Error getting proxy",e);
                out.println(e.toString().replace("<","&lt;"));
                e.printStackTrace(out);
                out.println("</table></body>");
                out.println("</html>");
            }
        }*/
    }
    
   /* private static GSSCredential saslProxy(PrintWriter out, String username, String keytab) throws
            Exception {
        
        log.trace("Getting proxy now");
        
        String hostname="myproxy-sso.grid-support.ac.uk";
        String realm = "FED.CCLRC.AC.UK";
        String kdc = "FED.CCLRC.AC.UK";
        int port=7513;
        GSSCredential gsscredential = null;
        //  CertUtil.init();
        MyProxy myproxy = new MyProxy(hostname, port);
        SASLParams params = new SASLParams(username);
        params.setRealm(realm);
        params.setLifetime(12*3600);
        params.setKDC(kdc);
        params.setKeytab(keytab);
        log.trace("About to download proxy");
        gsscredential = myproxy.getSASL(null, params);
        log.trace("Got proxy");
        return gsscredential;
    }*/
    
    private boolean checkUser(SessionDTO session) {
        Properties prop = new Properties();
        File user_file = new File(System.getProperty("user.home")+File.separator+"users.properties");
        if(!user_file.exists()) return true;
        else{
            try {
                prop.load(new FileInputStream(user_file));
            } catch (IOException ex) {
                log.error(ex);
                return false;
            }
        }
        
        if(!prop.containsValue(session.getDN())){
            return false;
        } else return true;
    }
    
    
    public static boolean streamCopy(InputStream in, OutputStream out)
    throws IOException {
        byte[] buffer = new byte[10240]; // 10k buffer
        int count;
        do
        {
            count = in.read(buffer);
            if (count != -1)
                out.write(buffer, 0, count);
        } while (count != -1);
        
        return true;
    }
    
    /**
     * Copy file from one location to another
     * throws IOException
     */
    public static boolean fileCopy(String fromLocation, String toLocation)
    throws IOException {
        log.trace("Copying file from: "+fromLocation+" to: "+toLocation);
        FileInputStream fis = new FileInputStream(new File(fromLocation));
        FileOutputStream fos = new FileOutputStream(new File(toLocation));
        
        boolean b = streamCopy(fis, fos);
        
        fis.close();
        fos.close();
        
        return (b);
    }
    
}


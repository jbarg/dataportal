package org.apache.jetspeed.modules.actions.xportlets;

// TODO: GridFTPClient cleaup code. Closing connections and stuff.
// TODO: Destroy the transfer thread you have created, I think by implementing HTTPSessionBindingListener.
// TODO: If the GridFTPClient object has become disfunctional (because of server timeout) revive it.

//xportlet Stuff
import xportlets.gridftp.*;
import xportlets.proxymanager.*;

// Turbine stuff
import org.apache.turbine.util.Log;
import org.apache.turbine.util.RunData;
import org.apache.turbine.util.parser.DefaultCookieParser;
import org.apache.turbine.util.upload.FileItem;

// Velocity Stuff
import org.apache.velocity.context.Context;

// Jetspeed Stuff
import org.apache.jetspeed.portal.Portlet;
import org.apache.jetspeed.om.security.JetspeedUser;
import org.apache.jetspeed.portal.portlets.VelocityPortlet;
import org.apache.jetspeed.modules.actions.portlets.VelocityPortletAction;
import org.apache.jetspeed.services.rundata.JetspeedRunData; 

// Cog Stuff
import org.globus.ftp.*;
import org.globus.ftp.exception.*;
import org.globus.util.GlobusURL;
import org.globus.io.urlcopy.*;
import org.globus.util.GlobusURL;
import org.ietf.jgss.GSSCredential;
import javax.servlet.*;
import javax.servlet.http.*;

// GT3 beans stuff
import org.globus.ogce.beans.gt3.GridCopyBean;

import java.util.*; 
import java.net.*;
import java.io.*;

/**
 * An  class to build VelocityPortlet actions.
 * 
 * @author <a href="mailto:lifang@extreme.indiana.edu">Liang Fang</a>
 * @author <a href="mailto:nijsure@mcs.anl.gov">Sandeep Nijsure</a>
 */

public class GridFTPClientAction extends VelocityPortletAction
{
    private String HOST1 = "host1";
    private String PORT1 = "port1";
    private String HOST2 = "host2";
    private String PORT2 = "port2";
    private String SRCHOST = "srchost";
    private String SRCPORT = "srcport";
    private String DSTHOST = "dsthost";
    private String DSTPORT = "dstport";
    private String SRCPATH = "srcpath";
    private String DSTPATH = "dstpath";
    private String PATH = "path";
    private String PATH1 = "path1";
    private String PATH2 = "path2";
    private String FILELIST1 = "files1";
    private String FILELIST2 = "files2";
    private String PORTLET = "portlet";
    private String ERRMSG = "errmsg";
    private String SECONDON = "rol";
    private String FILENAME = "filename";
    private String UPLOADFILE = "uploadfilename";
    private String STATUS = "status";
    private String WARNMSG = "warnmsg";
    private String DEBUGMSG = "debugmsg";
    private String CURRDIR1 = "currdir1";
    private String CURRDIR2 = "currdir2";
    private String DOWNLOAD1 = "download1";
    private String DOWNLOAD2 = "download2";
    private String PARENTDIR1 = "parentdir1";
    private String PARENTDIR2 = "parentdir2";
    private String HOME1 = "home1";
    private String HOME2 = "home2";
    private String START = "start";

    /** 
     * Subclasses should override this method if they wish to
     * build specific content when maximized. Default behavior is
     * to do the same as normal content.
     */
    protected void buildMaximizedContext( VelocityPortlet portlet, 
                                          Context context,
                                          RunData rundata )
    {
        buildNormalContext(portlet, context, rundata); 
	
        setTemplate(rundata, "gridftp-browser");
    }
    
    /** 
     * Subclasses should override this method if they wish to
     * provide their own customization behavior.
     * Default is to use Portal base customizer action
     */
    protected void buildConfigureContext( VelocityPortlet portlet, 
                                          Context context,
                                          RunData rundata )
    {
        // buildNormalContext(portlet, context, rundata);
	context.put(HOST1, portlet.getAttribute(HOST1, "grid-data.rl.ac.uk", rundata));
	context.put(PORT1, portlet.getAttribute(PORT1, "2811", rundata));
	context.put(HOST2, portlet.getAttribute(HOST2, "grid-data.man.ac.uk", rundata));
	context.put(PORT2, portlet.getAttribute(PORT2, "2811", rundata));
	context.put(SECONDON, portlet.getAttribute(SECONDON, "local", rundata));

        setTemplate(rundata, "gridftp-customize");        
    }

    /** 
     * Subclasses must override this method to provide default behavior 
     * for the portlet action
     */
    protected void buildNormalContext( VelocityPortlet portlet, 
                                       Context context,
                                       RunData rundata )
    {
	try {
	    context.put(HOST1, portlet.getAttribute(HOST1, "grid-data.rl.ac.uk", rundata));
	    context.put(PORT1, portlet.getAttribute(PORT1, "2811", rundata));
	    context.put(HOST2, portlet.getAttribute(HOST2, "grid-data.man.ac.uk", rundata));
	    context.put(PORT2, portlet.getAttribute(PORT2, "2811", rundata));
	    context.put(SECONDON, portlet.getAttribute(SECONDON, "local", rundata));

	    String start = (String)rundata.getUser().getTemp(START, "false");
	    
	    Log.info(" {xportlet:GridFTP} In buildNormalContext Start: "+start);

	    if(start.equals("false")) {
		setTemplate(rundata, "gridftp-customize");
	    }
	    else {
		doList(rundata, context);
		setTemplate(rundata, "gridftp-browser");
	    }
	}
	catch(Exception e) {
	    PrintError(e.getMessage(), context);
	}
    }

    public void doShow_progress(RunData data, Context context) throws Exception
    {
	// May be null, that is OK..	    
	Vector copyBeanVector = (Vector)(getFromSession(data, "CopyBeanVector"));
	context.put("FileTransfer_Queue", copyBeanVector);
	
	if(copyBeanVector != null)
        {
            Vector fromURLs = new Vector();
            Vector toURLs = new Vector();
		
            for(int i=0; i< copyBeanVector.size(); i++)
	    {
                GridCopyBean gcb = (GridCopyBean)copyBeanVector.get(i);
		if(gcb.getStatus().equals("Finished"))
		    gcb.dispose();

		fromURLs.add(new GlobusURL(gcb.getFromURL())); 
	        toURLs.add(new GlobusURL(gcb.getToURL())); 
            }
		
	    context.put("FileTransfer_fromURLs", fromURLs);
	    context.put("FileTransfer_toURLs", toURLs);
        }
	setTemplate(data, "ViewTransfers");
    }
    
    public void doShow_main(RunData data, Context context) throws Exception
    {
	VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);
	buildNormalContext(portlet, context, data);
	setTemplate(data, "gridftp-browser");
    }
    
    public void doSubmit(RunData data, Context context) throws Exception
    {
	Log.info(" {xportlet:GridFTP} Doing doSubmit ... ");
        String host1 = data.getParameters().getString(HOST1);
        String port1 = data.getParameters().getString(PORT1);
	String rol = data.getParameters().getString(SECONDON);
	String host2 = data.getParameters().getString(HOST2);;
	String port2 = data.getParameters().getString(PORT2);;

	Log.info(" {xportlet:GridFTP} Host1: "+host1);
	Log.info(" {xportlet:GridFTP} Port1: "+port1);
	Log.info(" {xportlet:GridFTP} ROLE: "+rol);

	
	if(rol.equals("remote")) {
	    host2 = data.getParameters().getString(HOST2);
	    port2 = data.getParameters().getString(PORT2);

	    Log.info(" {xportlet:GridFTP} Host2: "+host2);
	    Log.info(" {xportlet:GridFTP} Port2: "+port2);
	}
	
	if(host1 == null) {
	    PrintError("You must specify a Grid FTP host name!", context);
	    // setTemplate(data, "gridftp-browser");
	    return;
	}
	
	if(port1 == null) {
	    PrintError("You must specify a host port (default port number: 7512)", context);
	    // context.put(ERRMSG, "You must specify a host port (default port number: 7512)");
	    // setTemplate(data, "gridftp-browser");
	    return;
	}	

	VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);

	data.getUser().setPerm(HOST1, host1);
	data.getUser().setPerm(PORT1, port1);
	data.getUser().setPerm(SECONDON, rol);

	// portlet.setAttribute(HOST1, host1, data);
	// portlet.setAttribute(PORT1, port1, data);
	// portlet.setAttribute(SECONDON, rol, data);

	if(rol.equals("remote")) {
	    if(host2 == null) {
		PrintError("You must specify the second Grid FTP host!", context);
		return;
	    }
	    
	    if(port2 == null){
		PrintError("You must specify the second Grid FTP's port (default port number: 7512)", context);
		return;
	    }
	    
	    data.getUser().setPerm(HOST2, host2);
	    data.getUser().setPerm(PORT2, port2);
	    // portlet.setAttribute(HOST2, host2, data);
	    // portlet.setAttribute(PORT2, port2, data);
	}
	else {
	    data.getUser().setPerm(HOST2, "");
	    data.getUser().setPerm(PORT2, "");
	    // portlet.setAttribute(HOST2, "", data);
	    // portlet.setAttribute(PORT2, "", data);
	}

	data.getUser().setTemp(START, "true");

	buildNormalContext(portlet, context, data);
    }
    
    public void doList(RunData data, Context context) throws Exception {

	GridFTPClient client1 = null;
	GridFTPClient client2 = null;
	String path = "";
	String msg = "";
	String msg2 = "";
	Log.info(" {xportlet:GridFTP} doList ... ");

	try {
	    VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);

	    // String host1 = data.getParameters().getString(HOST1);
	    // String port1 = data.getParameters().getString(PORT1);
	    // String host2 = data.getParameters().getString(HOST2);
	    // String port2 = data.getParameters().getString(PORT2);
            String host1 = (String)data.getUser().getPerm(HOST1);   
            String port1 = (String)data.getUser().getPerm(PORT1);   
            String host2 = (String)data.getUser().getPerm(HOST2);   
            String port2 = (String)data.getUser().getPerm(PORT2);   
	    
	    String path1 = data.getParameters().getString(PATH1);
	    String path2 = data.getParameters().getString(PATH2);

            Log.info(" {xportlet:GridFTP} Host1: "+ host1+":"+port1);
            Log.info(" {xportlet:GridFTP} Host2: "+ host2+":"+port2);

	    if(host1 == null) 
		host1 = (String)context.get(HOST1);
	    if(host1 == null)           
		host1 = "";
	    if(port1 == null)
		port1 = (String)context.get(PORT1);
	    if(port1 == null)
		port1 = "";
	    if(host2 == null)	    
		host2 = (String)context.get(HOST2);
	    if(host2 == null)
		host2 = "";
	    if(port2 == null)
		port2 = (String)context.get(PORT2);
	    if(port2 == null)
		port2 = "";
	    if(path1 == null) 
		path1 = "";
	    if(path2 == null)           
		path2 = "";

	    if(!host1.equals("")) {
		context.put(HOST1, host1);
		context.put(PORT1, port1);
		
		PrintDebug("Host1: " + host1+":"+port1, context);
		client1 = getConnect(data, host1, port1, "2-party", true);
		
		if(client1 == null) {
		    throw new Exception("client1 null!");
		}
		
		context.put(FILELIST1, toFileAttributes(getDirInfo(client1, path1)));

		Log.info(" {xportlet:GridFTP} Do List here (get Currdir)... " + path1);
		String currDir = client1.getCurrentDir();
		context.put(CURRDIR1, currDir);
		Log.info(" {xportlet:GridFTP} Current dir in server 1 : " + currDir + " File separator " + File.separator);

		if(path1.equals("")) {
		    Log.info(" {xportlet:GridFTP} Set home1: " + currDir);
		    // portlet.setAttribute(HOME1, currDir, data);
		    data.getUser().setPerm(HOME1, currDir);
		}

		// context.put(HOME1, portlet.getAttribute(HOME1, "/", data));
		context.put(HOME1, (String)data.getUser().getPerm(HOME1, "/"));
		
		// TODO: Take care of the File separator, depending on what the server returns.
		if(currDir.equals("/"))
		    context.put(PARENTDIR1, "");
		else 
		    context.put(PARENTDIR1, currDir.substring(0, currDir.lastIndexOf("/")));
		context.put(DOWNLOAD1, "gfd?host=" + host1 + "&port=" + port1 + "&path=" + currDir + "&filename=");
		msg = "Host1 connected to " + host1;
		msg2 = "Host2 not connected";
	    }

	    if(!host2.equals("")) {
		context.put(HOST2, host2);
		context.put(PORT2, port2);	
		
		PrintDebug("Host2: " + host2 + ":" + port2, context);
		client2 = getConnect(data, host2, port2, "2-party", true);

		if(client2 == null) {
		    throw new Exception("client2 null!");
		}

		context.put(FILELIST2, toFileAttributes(getDirInfo(client2, path2)));

		String currDir = client2.getCurrentDir();
		context.put(CURRDIR2, currDir);
		Log.info(" {xportlet:GridFTP} Current dir in server 2 : " + currDir);

		if(path2.equals("")) {
		    Log.info(" {xportlet:GridFTP} Set home2: " + currDir);
		    // portlet.setAttribute(HOME2, currDir, data);
		    data.getUser().setPerm(HOME2, currDir);
		}

		// context.put(HOME2, portlet.getAttribute(HOME2, "/", data));
		context.put(HOME2, (String)data.getUser().getPerm(HOME2, "/"));
		
		// TODO: Take care of the File separator, depending on what the server returns.
		if(currDir.equals("/"))
		    context.put(PARENTDIR2, "");
		else 
		    context.put(PARENTDIR2, currDir.substring(0, currDir.lastIndexOf("/")));
		context.put(DOWNLOAD2, "gfd?host=" + host2 + "&port=" + port2 + "&path=" + currDir + "&filename=");

		msg2 = "Host2 connected to " + host2;
	    }
//  	    context.put(STATUS, msg +"<br>\n"+ msg2);
	    PrintStatus(msg +"<br>\n"+ msg2, context);
	}
	catch(Exception e){
	    PrintError(e.getMessage(), context);
	    e.printStackTrace();
	}

    }

    public void doTransfer(RunData data, Context context) throws Exception
    {
	String[] f2trans = data.getParameters().getStrings(FILENAME);
	String srchost = data.getParameters().getString(SRCHOST);
	String srcport = data.getParameters().getString(SRCPORT);
	String dsthost = data.getParameters().getString(DSTHOST);
	String dstport = data.getParameters().getString(DSTPORT);
	String srcpath = data.getParameters().getString(SRCPATH);
	String dstpath = data.getParameters().getString(DSTPATH);
	String errmsg = ""; 
	VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);

	if(f2trans == null) {
	    PrintError("No file selected for transferring.", context);
	    return;
	}
	
	String factorylocation = "http://shakey.mcs.anl.gov:9090/ogsa/services/base/reliabletransfer/ReliableTransferFactoryService";
	String security="";
	String delegation="";
	int parallelStreams = 1;
	int tcpBuffer = 16384;
	
	Vector copyBeanVector = (Vector)(data.getUser().getTemp("CopyBeanVector"));
	if(copyBeanVector == null)
	    copyBeanVector = new Vector();
	
//  	ProxyTable proxies = new ProxyTable();
  	JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
//  	proxies.load( user );
//  	GSSCredential proxy = proxies.getDefault();

	HttpSession session = data.getSession();
	GSSCredential proxy = ProxyManager.getDefaultProxy(session);
	
	for(int i=0; i<f2trans.length; i++) 
	    {	
		PrintStatus("Transferring from " + srcpath + "/" + f2trans[i] 
			    + " to " + dstpath + "/" + f2trans[i], context); 
		String fromURL = new String("gsiftp://" + srchost + ":" + Integer.parseInt(srcport) 
					    + srcpath + "/" + f2trans[i]); 
		String toURL = new String("gsiftp://" + dsthost + ":" + Integer.parseInt(dstport) 
					  + dstpath + "/" + f2trans[i]);
		GridCopyBean rft = new GridCopyBean(factorylocation, security, delegation, proxy);
		rft.setFromURL(fromURL);
		rft.setToURL(toURL);
		rft.setParallelStreams(parallelStreams);
		rft.setTcpBuffer(tcpBuffer);
		rft.setCredential(proxy);
		rft.submitTransfer();
		copyBeanVector.add(rft);
		
	    }
	
	data.getUser().setTemp("CopyBeanVector", copyBeanVector);
	
	buildNormalContext(portlet, context, data);
    }
    
    private String getStackTrace(Exception e)
    {
	StringWriter sw = new StringWriter();
	PrintWriter pw = new PrintWriter(sw);
	e.printStackTrace(pw);
	return sw.toString();
    }
    
    public void doUrlcopy(RunData data, Context context) throws Exception
    {  
	Log.info(" {xportlet:GridFTP} doUrlCopy ... ");
	String[] f2trans = data.getParameters().getStrings(FILENAME);
        String srchost = data.getParameters().getString(SRCHOST);
        String srcport = data.getParameters().getString(SRCPORT);
        String dsthost = data.getParameters().getString(DSTHOST);
        String dstport = data.getParameters().getString(DSTPORT);
        String srcpath = data.getParameters().getString(SRCPATH);
        String dstpath = data.getParameters().getString(DSTPATH);
        String errmsg = "";
        VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);
 
        if(f2trans == null) {
            PrintError("No file selected for transferring.", context);
            return;
        }
 
        try {
//              ProxyTable proxies = new ProxyTable();
              JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
//              proxies.load( user );
//  	    GSSCredential proxy = proxies.getDefault();

            HttpSession session = data.getSession();
            GSSCredential proxy = ProxyManager.getDefaultProxy(session);
 
            if(proxy == null)
                throw new Exception("proxy null");
 
            errmsg = proxy.toString();
            UrlCopy urlCopy = new UrlCopy();
            urlCopy.setCredentials(proxy);
            urlCopy.setUseThirdPartyCopy(true);
 
            GlobusURL srcURL, dstURL;
 
            int count = 0;
 
            for(int i = 0; i < f2trans.length;i++) {
                String srcurl = "gsiftp://" + srchost + ":" + srcport + "/" + srcpath + "/" + f2trans[i];
                String dsturl = "gsiftp://" + dsthost + ":" + dstport + "/" + dstpath + "/" + f2trans[i];
                PrintDebug("SRC URL: " + srcurl, context);
                PrintDebug("DEST URL: " + dsturl, context);
                //System.out.println("SRC URL: " + srcurl);
                //System.out.println("DST URL: " + dsturl);
                srcURL = new GlobusURL(srcurl);
                dstURL = new GlobusURL(dsturl);
                try {
                    urlCopy.setSourceUrl(srcURL);
                    urlCopy.setDestinationUrl(dstURL);
                    urlCopy.copy();
                    count++;
                }
                catch(UrlCopyException e) {
                    e.printStackTrace();
                    Log.error(" {xportlet:GridFTP} Failed to transfer: " + f2trans[i]);
                    context.put(WARNMSG, e.getMessage() + "<br>Failed to transfer " + f2trans[i]);
                    errmsg += "<br>" + f2trans[i] + ": " + e.getMessage();
                }
                catch(Exception e) {
                    PrintError("Totally " + count + " file(s) of " + 
                               f2trans.length + " file(s) transferred.<br>" + 
			       "Error: " + errmsg, context);
                    throw e;
                }
            }
            if(count == 0) {
                PrintError("Totally " + count + " file(s) of " + 
                           f2trans.length + " file(s) transferred.<br>" + 
			   "Error: " + errmsg, context);
            }
            else {
		// context.put(STATUS, "Totally " + count + " file(s) of " + f2trans.length + " file(s) transferred.");
                PrintStatus("Totally " + count + " file(s) of " + 
                            f2trans.length + " file(s) transferred.", context);
            }
        } catch (Exception e) {
            e.printStackTrace();
            PrintError(e.getMessage(), context);
        }
 
	//System.out.println("leaving url copy ... ");
	buildNormalContext(portlet, context, data);
    }

    public void doUpload(RunData data, Context context) throws Exception 
    {
	GridFTPClient client = null;
	String dsthost = data.getParameters().getString(DSTHOST);
	String dstport = data.getParameters().getString(DSTPORT);
	String dstpath = data.getParameters().getString(DSTPATH);
	VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);

	try {
	    Log.info(" {xportlet:GridFTP} doUpload ...");

	    HttpServletRequest request = data.getRequest();
	    PrintDebug("<TABLE BORDER=1 ALIGN=CENTER>\n<TR BGCOLOR=#FFAD00><TH>Header Name<TH>Header Value", context);
	    Enumeration headerNames = request.getHeaderNames();
	    while(headerNames.hasMoreElements()) { 
		String headerName = (String)headerNames.nextElement(); 
		PrintDebug("<TR><TD>" + headerName, context); 
		PrintDebug(" <TD>" + request.getHeader(headerName), context); 
	    }
	    PrintDebug("</TABLE>", context);
	    String useragent = request.getHeader("User-Agent");

	    client = getConnect(data, dsthost, dstport, "2-party", true);
	    
	    if(client == null) {
		throw new Exception("client null!");
	    }
	    
	    Log.info(" {xportlet:GridFTP} doUpload Current dir: " + client.getCurrentDir());
	    FileItem file = data.getParameters().getFileItem(UPLOADFILE);
	    if(file == null) 
		throw new Exception("Upload file null");

	    Log.info(" {xportlet:GridFTP} doUpload completed.\nSize of uploaded file: " + file.getSize());

	 /*
	    File f = file.getStoreLocation();
	    boolean inMemory = file.inMemory();
	    if(inMemory) {
		System.out.println("File " + s + " in memory");
	    }
	    else
	        System.out.println("File Location: " + f.getPath());
	 */
	    String s = file.getName();
	    System.out.println("File Name: " + s);
	    String separator = "/";
	    if(useragent.indexOf("Win") >= 0) {  // the client is a Windows machine
		separator = "\\";
	    }
	    System.out.println(useragent);
	    System.out.println("Separator: " + separator);
	    int idx1 = s.lastIndexOf(separator);
	    int idx2 = s.length();
	    if(idx1 == -1) { //in case no path in the filename
		idx1 = 0;
	    }
	    else 
		idx1++;

	    System.out.println("Index1: " + idx1 + " Index2: " + idx2);
	    s = s.substring(idx1, idx2);
	    System.out.println("Without path: " + s);
	    System.out.println("Dest path: " + dstpath);

	/*
	    if(inMemory) {
		File tempf = new File("putemp");
		FileOutputStream fos = new FileOutputStream(tempf);
		InputStream in = file.getInputStream();
		int c;
		while((c = in.read()) != -1) {
		    fos.write(c);
		}
		in.close();
		fos.close();
		
		client.put(tempf, dstpath + File.separator + s, false);

		tempf.delete();
	    }
	    else
		client.put(file.getStoreLocation(), dstpath + File.separator + s, false);
	*/

	    InputStream in = file.getInputStream();
	    DataSourceStream dss = new DataSourceStream(in);
	    client.put(dstpath + File.separator + s, dss, null);
 
	    // doList(data, context);

            // context.put(STATUS, file.getName() + " uploaded.");
	    PrintStatus(file.getName() + " uploaded.", context);
	}
	catch (Exception e) {
	    e.printStackTrace();
	    Log.error(" {xportlet:GridFTP} Failed to upload file: "+e);
	    PrintError("Failed to upload file: " + e.getMessage(), context);
	}

	buildNormalContext(portlet, context, data);
    }

    public void doPerform(RunData data) throws Exception
    {
	FileItem file = data.getParameters().getFileItem("uploadfilename");         
	Log.info(" {xportlet:GridFTP} Size of uploaded file from (FileItem) file.getSize(): " + file.getSize());                    
    }
    
    public void doCustomize(RunData data, Context context) throws Exception
    {        
	doSubmit(data, context);
    }
    
    public Vector getDirInfo(GridFTPClient client, 
			     String path) throws Exception{
	Vector v = null;
	
	try {
	    String currDir = client.getCurrentDir();
	    if(path == null) 
		path = "";
            
	    if(path.equals(""))
		client.changeDir(currDir + "/" + path);
  	    else
  		client.changeDir(path);

	    currDir = client.getCurrentDir();
	    
	    v = client.list();
	}
	catch(Exception e) {
	    e.printStackTrace();
	    throw e;
	}

	return v;
    }
  
    
    public GridFTPClient getConnect(RunData data,
				    String host,
				    String port,
                                    String purpose, // 2-party or 3 party? They suggest to use 2 different clients
                                    boolean remotePassive) throws Exception {
	GridFTPClient client = null;
	
	try {
	    System.out.println("Debug 1");
	    
	    HttpSession session = data.getSession();
	    System.out.println("Debug 2");
	    
	    if(session == null)
		{
		    Log.info(" {xportlet:GridFTP} GridFTPClient SESSION NULL. host " + host + " purpose " + purpose);
		    throw new Exception("Session is null");
		}
	    
	    client = (GridFTPClient)(getFromSession(data, host + purpose));
	    
	    boolean closedConnection = false;
	    /*
	      if(client != null) 
	      client.getCurrentDir();
	      try
	      {
	      }
	      catch(IOException ioe) { closedConnection = true; }
	    */
	    
	    System.out.println("Debug 3");
	    
	    if(client == null || closedConnection == true)
		{
//  		    ProxyTable proxies = new ProxyTable();
  		    JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
//  		    System.out.println("Debug 4");
//  		    proxies.load( user );
//  		    System.out.println("Debug 5");
		    
//  		    GSSCredential proxy = proxies.getDefault();

		    GSSCredential proxy = ProxyManager.getDefaultProxy(session);
		    if (proxy == null)
			throw new Exception("Proxy is not set or is null");
		    
		    Log.info(" {xportlet:GridFTP} getConnect() FTP Host: " + 
                             host + ":" + port);

		    client = new GridFTPClient( host, Integer.parseInt(port) );
		    
		    client.authenticate( proxy );
                    Log.info(" {xportlet:GridFTP} getConnect() Authenticate successful");
		    client.setDataChannelAuthentication( DataChannelAuthentication.NONE );
                    Log.info(" {xportlet:GridFTP} getConnect() setDataChannelAuthenticate successful");
		    
		    putIntoSession(data, host + purpose, client);
                    Log.info(" {xportlet:GridFTP} getConnect() Put "+host+
                             " connection into System properties."+" session: "
			     + session.getId());
		    
		    
		    if(purpose.equals("2-party"))
                    {
		        session.setAttribute(host, client);
		        System.getProperties().put("aaa", client);
		        ServletContext sctx = data.getServletContext();
		        sctx.setAttribute("sh", client);
		        System.out.println("Put client into servletContext");
		    }
		}
	    
            // Set the modes as per the request 
            // We don't need to save this into session, 
            // since this is going to be done anyway.
	    if(purpose.equals("2-party"))
            	if(remotePassive)
		{
                    client.setPassive();
                    client.setLocalActive();
		}
           	else
		{
                    client.setLocalPassive();
                    client.setActive();
		}
	}
	catch(ProxyTableException e) {
	    Log.error(" {xportlet:GridFTP} Proxy Table Exception!");
	    throw e;
	}
	catch(Exception e) {
	    Log.error(" {xportlet:GridFTP} "+e.getMessage());
	    e.printStackTrace();
	    throw e;
	}
	
	return client;
    }

    private Object getFromSession(RunData data, String name)
    {
	return data.getUser().getTemp(name);
    }
    
    private void putIntoSession(RunData data, String name, Object obj)
    {
	if(data == null)
	    System.out.println("PutIntoSession: data is null");
	if(data.getUser() == null)
	    System.out.println("PutIntoSession: user is null");
	if(name == null)
	    System.out.println("PutIntoSession: name is null");
	if(obj == null)
	    System.out.println("PutIntoSession: obj is null");
	data.getUser().setTemp(name, obj);
    }
    
    private Vector toFileAttributes(Vector v) {
	Vector dirV = new Vector();
	
	Enumeration enum = v.elements(); 	
	while(enum.hasMoreElements()) {
	    FileInfo finfo = (FileInfo)enum.nextElement();
	    
	    if(!finfo.isSoftLink()) {
		FileAttributes fattr = new FileAttributes();
		fattr.setName(finfo.getName());
		fattr.setTime(finfo.getTime() + " " + finfo.getDate());
		
		if(finfo.isFile()) {
		    fattr.setSize(String.valueOf(finfo.getSize()));
		    fattr.setisFile("1");
		    String filename = finfo.getName();
		    String ext = "";
		    int idx = filename.lastIndexOf(".");
		    if(idx >= 0)
			ext = filename.substring(idx+1, filename.length());
		    
		    if(ext.equals("ps")) {
			fattr.setPiclink("images/xportlets/ps.gif");
		    }
		    else if(ext.equalsIgnoreCase("pdf")) {
			fattr.setPiclink("images/xportlets/pdf.gif");
		    }
		    else if(ext.equalsIgnoreCase("zip")) {
			fattr.setPiclink("images/xportlets/zip.gif");
		    }
		    else if(ext.equalsIgnoreCase("jar")) {
			fattr.setPiclink("images/xportlets/zip.gif");
		    }
		    else if(ext.equalsIgnoreCase("gz")) {
			fattr.setPiclink("images/xportlets/gz.gif");
		    }
		    else if(ext.equalsIgnoreCase("jpg")) {
			fattr.setPiclink("images/xportlets/image.gif");
		    }
		    else if(ext.equalsIgnoreCase("gif")) {
			fattr.setPiclink("images/xportlets/image.gif");
		    }
		    else if(ext.equalsIgnoreCase("tif")) {
			fattr.setPiclink("images/xportlets/image.gif");
		    }
		    else if(ext.equalsIgnoreCase("mpg")) {
			fattr.setPiclink("images/xportlets/mpg.gif");
		    }
		    else if(ext.equalsIgnoreCase("mov")) {
			fattr.setPiclink("images/xportlets/mov.gif");
		    }
		    else if(ext.equalsIgnoreCase("htm")) {
			fattr.setPiclink("images/xportlets/web.gif");
		    }
		    else if(ext.equalsIgnoreCase("txt")) {
			fattr.setPiclink("images/xportlets/text.gif");
		    }
		    else if(ext.equalsIgnoreCase("java")) {
			fattr.setPiclink("images/xportlets/java.gif");
		    }
		    else if(ext.equalsIgnoreCase("c")) {
			fattr.setPiclink("images/xportlets/c.gif");
		    }
		    else if(ext.equalsIgnoreCase("cpp")) {
			fattr.setPiclink("images/xportlets/cpp.gif");
		    }
		    else if(ext.equalsIgnoreCase("hpp")) {
			fattr.setPiclink("images/xportlets/cpp.gif");
		    }
		    else if(ext.equalsIgnoreCase("cc")) {
			fattr.setPiclink("images/xportlets/cpp.gif");
		    }
		    else if(ext.equalsIgnoreCase("h")) {
			fattr.setPiclink("images/xportlets/h.gif");
		    }
		    else if(ext.equalsIgnoreCase("class")) {
			fattr.setPiclink("images/xportlets/class.gif");
		    }
		    else if(ext.equalsIgnoreCase("o")) {
			fattr.setPiclink("images/xportlets/obj.gif");
		    }
		    else if(ext.equalsIgnoreCase("obj")) {
			fattr.setPiclink("images/xportlets/obj.gif");
		    }
		    else if(ext.equalsIgnoreCase("html")) {
			fattr.setPiclink("images/xportlets/web.gif");
		    }
		    else if(ext.equalsIgnoreCase("xml")) {
			fattr.setPiclink("images/xportlets/web.gif");
		    }
		    else if(ext.equalsIgnoreCase("doc")) {
			fattr.setPiclink("images/xportlets/msword.gif");
		    }
		    else if(ext.equalsIgnoreCase("ppt")) {
			fattr.setPiclink("images/xportlets/ppt.gif");
		    }
		    else if(ext.equalsIgnoreCase("xls")) {
			fattr.setPiclink("images/xportlets/excel.gif");
		    }
		    else 
			fattr.setPiclink("images/xportlets/file.gif");
		}
		else if(finfo.isDirectory()) {
		    fattr.setPiclink("images/xportlets/folder.gif");
		}
		else {
		    fattr.setPiclink("images/xportlets/folder.gif");
		}
		
		dirV.add(fattr);
	    }
	}	

	return dirV;
    }

    private void PrintDebug(String msg, Context context) {
	String amsg = (String)context.get(DEBUGMSG);
	if(amsg == null)
	    amsg = "";
	amsg += msg + "<br>\n";
	context.put(DEBUGMSG, amsg);
    }

    private void PrintError(String msg, Context context) {
	String amsg = (String)context.get(ERRMSG);
	if(amsg == null)
	    amsg = "";
	amsg += msg + "<br>\n";
	context.put(ERRMSG, amsg);
    }

    private void PrintStatus(String msg, Context context) {
	String amsg = (String)context.get(STATUS);
	if(amsg == null)
	    amsg = "";
	amsg += msg + "<br>\n";
	context.put(STATUS, amsg);
    }
    
}

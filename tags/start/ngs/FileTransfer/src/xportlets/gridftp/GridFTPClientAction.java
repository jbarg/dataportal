/*
 * Xiaobo Yang, CCLRC e-Science Centre, 15 December 2004
 *
 */

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
//import org.apache.jetspeed.modules.actions.portlets.VelocityPortletAction;
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

import org.chefproject.service.GridService;

import org.chefproject.actions.VelocityPortletPaneledAction;
import org.chefproject.util.Menu;
import org.apache.jetspeed.services.statemanager.SessionState;

/**
 * An  class to build VelocityPortlet actions.
 * 
 * @author <a href="mailto:lifang@extreme.indiana.edu">Liang Fang</a>
 * @author <a href="mailto:nijsure@mcs.anl.gov">Sandeep Nijsure</a>
 */

public class GridFTPClientAction extends VelocityPortletPaneledAction {
  private final String HOSTLIST = "hostlist";
  private final String DEFAULTHOST1 = "defaulthost1";
  private final String DEFAULTHOST2 = "defaulthost2";

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
//  private String ERRMSG = "errmsg";
  private String ERRMSG = "alertMessage";
  private String SECONDON = "rol";
  private String REMOTE = "remote";
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

  final private String sep = File.separator;
  private String fileName = null;
  private long fileSize = -1;

  final private String TOOLMODE = "toolmode";
  final private String BROWSE = "browse";
  final private String THIRD_PARTY_TRANSFER = "thrid_party_transfer";
  final private String DELETE = "delete";
  final private String UPLOAD = "upload";


  public String buildMainPanelContext(VelocityPortlet portlet,
			Context context, RunData rundata, SessionState state) {
    String template = null;

    // put $action into context for menus, forms and links
    context.put(Menu.CONTEXT_ACTION, state.getAttribute(STATE_ACTION));

    String toolmode = (String)state.getAttribute(TOOLMODE);
    if (toolmode==null) { // display xportlets-gridftp-customize
      String[] hostlist = org.apache.jetspeed.util.StringUtils.stringToArray(org.apache.jetspeed.util.PortletConfigState.getParameter(portlet, rundata, HOSTLIST, "grid-compute.leeds.ac.uk, grid-compute.oesc.ox.ac.uk, grid-data.man.ac.uk, grid-data.rl.ac.uk"), ", ");
      context.put(HOSTLIST, hostlist);
      context.put(DEFAULTHOST1, portlet.getAttribute(DEFAULTHOST1, "grid-compute.oesc.ox.ac.uk", rundata));
      context.put(DEFAULTHOST2, portlet.getAttribute(DEFAULTHOST2, "grid-data.man.ac.uk", rundata));
      template = "xportlets-gridftp-customize";
    }
    else if (toolmode.equals(BROWSE)) { // "Enter" clicked in xportlets-gridftp-customize
      doList(rundata, context);
      template = "xportlets-gridftp-browse";
    }
    else if (toolmode.equals(THIRD_PARTY_TRANSFER)) {
      doList(rundata, context);
      template = buildThirdpartytransferContext(state, context);
    }
    else if (toolmode.equals(DELETE)) {
      doList(rundata, context);
      template = buildDeleteContext(state, context);
    }
    else if (toolmode.equals(UPLOAD)) {
      doList(rundata, context);
      template = buildUploadContext(state, context);
    }
    else {
      System.out.println("WARRINGTON: mode = " + toolmode);
      state.setAttribute(TOOLMODE, null);
      template = "xportlets-gridftp-customize";
    }

    context.put(ERRMSG, state.getAttribute(ERRMSG));
System.out.println("A: buildMainPanelContext() called -- mode: " + toolmode);

    return template;

  }

  private String buildThirdpartytransferContext(SessionState state, Context context) {
System.out.println("B1: buildThirdpartytransferContext() called");
    String status = (String)state.getAttribute(STATUS);
    if (status!=null && !status.equals("")) {
      context.put(STATUS, status);
    }

    return "xportlets-gridftp-browse";

  }

  private String buildUploadContext(SessionState state, Context context) {
System.out.println("C1: buildUploadContext() called");
    String status = (String)state.getAttribute(STATUS);
    if (status!=null && !status.equals("")) {
      context.put(STATUS, status);
    }

    return "xportlets-gridftp-browse";

  }

  private String buildDeleteContext(SessionState state, Context context) {
System.out.println("D1: buildDeleteContext() called");
    String status = (String)state.getAttribute(STATUS);
    if (status!=null && !status.equals("")) {
      context.put(STATUS, status);
    }

    String warn = (String)state.getAttribute(WARNMSG);
    if (warn!=null && !warn.equals("")) {
      context.put(WARNMSG, warn);
    }

    return "xportlets-gridftp-browse";

  }

  /**
  * Get parameters and then get proxy from MyProxy server.  Store proxy
  * in the user's proxy table.  Each proxy is stored under their
  * hash symbol in the ProxyTable and will be available from other portlets.
  * Also, load proxies into xportlet-proxymanager.vm.
  */
  public void doBrowse(RunData runData, Context aContext) {
System.out.println("E: doBrowse() called");
    SessionState state = ((JetspeedRunData)runData).getPortletSessionState(((JetspeedRunData)runData).getJs_peid());
    state.setAttribute(TOOLMODE, BROWSE);
    state.removeAttribute(ERRMSG);

    // get parameters from xportlets-gridftp-customize template
    // SECONDON (rol) should be set
    String rol = runData.getParameters().getString(SECONDON, REMOTE);
    String host1 = runData.getParameters().getString(HOST1);
    String port1 = runData.getParameters().getString(PORT1);

    state.setAttribute(SECONDON, rol);

    runData.getUser().setPerm(SECONDON, rol);

    // check parameters for host 1
    String missing = "";
    if (host1==null || host1.equals("")) {
      missing += " " + HOST1;
    }
    if (port1==null || port1.equals("")) {
      missing += " " + PORT1;
    }
    if (missing!="") {
      state.setAttribute(ERRMSG, "Missing the following parameter(s): " +
								missing);
      return;
    }

    state.setAttribute(HOST1, host1);
    state.setAttribute(PORT1, port1);

    runData.getUser().setPerm(HOST1, host1);
    runData.getUser().setPerm(PORT1, port1);

	Log.info(" {xportlet:FileTransfer} Host1: "+host1);
	Log.info(" {xportlet:FileTransfer} Port1: "+port1);
	Log.info(" {xportlet:FileTransfer} ROLE: "+rol);

    // check parameters for host 2
    String host2 = null;
    String port2 = null;
    if (rol.equalsIgnoreCase(REMOTE)) {
      host2 = runData.getParameters().getString(HOST2);
      port2 = runData.getParameters().getString(PORT2);

	    Log.info(" {xportlet:FileTransfer} Host2: "+host2);
	    Log.info(" {xportlet:FileTransfer} Port2: "+port2);
      missing = "";
      if (host2==null || host2.equals("")) {
        missing += " " + HOST2;
      } 
      if (port2==null || port2.equals("")) {
        missing += " " + PORT2;
      }
      if (missing!="") {
        state.setAttribute(ERRMSG, "Missing the following parameter(s): " +
								missing);
        return;
      }

      state.setAttribute(HOST2, host2);
      state.setAttribute(PORT2, port2);

      runData.getUser().setPerm(HOST2, host2);
      runData.getUser().setPerm(PORT2, port2);

    }

  }
    
//  public void doList(RunData data, Context context) throws Exception {
  public void doList(RunData data, Context context) {
System.out.println("F: doList() called");
//    SessionState state = ((JetspeedRunData)data).getPortletSessionState(((JetspeedRunData)data).getJs_peid());
//    state.removeAttribute(ERRMSG);
//System.out.println("Toolmode: " + state.getAttribute(TOOLMODE));

    GridFTPClient client1 = null;
    GridFTPClient client2 = null;
    String path = "";
    String msg = "";
    String msg2 = "";
	Log.info(" {xportlet:GridFTP} doList ... ");

    try {
//      VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);

      String rol = (String)data.getUser().getPerm(SECONDON); 

      String host1 = (String)data.getUser().getPerm(HOST1);
      String port1 = (String)data.getUser().getPerm(PORT1);
      String path1 = data.getParameters().getString(PATH1);

      String host2 = null;
      String port2 = null;
      String path2 = null;
System.out.println(host1+":"+port1);
      if (rol.equalsIgnoreCase(REMOTE)) {
        host2 = (String)data.getUser().getPerm(HOST2);
        port2 = (String)data.getUser().getPerm(PORT2);
        path2 = data.getParameters().getString(PATH2);
System.out.println(host2+":"+port2);
      }


            Log.info(" {xportlet:FileTransfer} Host1: "+ host1+":"+port1);
            Log.info(" {xportlet:FileTransfer} Host2: "+ host2+":"+port2);

      if (host1==null) host1 = (String)context.get(HOST1);
      if (host1==null) host1 = "";
      if (port1==null) port1 = (String)context.get(PORT1);
      if (port1==null) port1 = "";
      if (host2==null) host2 = (String)context.get(HOST2);
      if (host2==null) host2 = "";
      if (port2==null) port2 = (String)context.get(PORT2);
      if (port2==null) port2 = "";
      if (path1==null) path1 = "";
      if (path2==null) path2 = "";

      if (!host1.equals("")) { // host 1
        context.put(HOST1, host1);
        context.put(PORT1, port1);
//        PrintDebug("Host1: " + host1+":"+port1, context);

        client1 = getConnect(data, host1, port1, "2-party", true);

//        if (client1==null) throw new Exception("client1 null!");

        context.put(FILELIST1, toFileAttributes(getDirInfo(client1, path1)));

		Log.info(" {xportlet:FileTransfer} Do List here (get Currdir)... " + path1);

        String currDir = client1.getCurrentDir();
        context.put(CURRDIR1, currDir);
//		Log.info(" {xportlet:GridFTP} Current dir in server 1 : " + currDir + " File separator " + File.separator);
		Log.info(" {xportlet:FileTransfer} Current dir in server 1 : " + currDir + " File separator " + sep);

        if (path1.equals("")) {
		    Log.info(" {xportlet:FileTransfer} Set home1: " + currDir);
		    // portlet.setAttribute(HOME1, currDir, data);
          data.getUser().setPerm(HOME1, currDir);
        }

		// context.put(HOME1, portlet.getAttribute(HOME1, "/", data));
//		context.put(HOME1, (String)data.getUser().getPerm(HOME1, "/"));
        context.put(HOME1, (String)data.getUser().getPerm(HOME1, sep));
		
		// TODO: Take care of the File separator, depending on what the server returns.
//		if(currDir.equals("/"))
        if (currDir.equals(sep)) context.put(PARENTDIR1, "");
        else context.put(PARENTDIR1, currDir.substring(0, currDir.lastIndexOf(sep)));

        context.put(DOWNLOAD1, "gfd?host=" + host1 + "&port=" + port1 + "&path=" + currDir + "&filename=");

        msg = "Host1 connected to " + host1;
        msg2 = "Host2 not connected";
      }

      if (rol.equalsIgnoreCase(REMOTE) && !host2.equals("")) {
        context.put(HOST2, host2);
        context.put(PORT2, port2);	
//        PrintDebug("Host2: " + host2 + ":" + port2, context);

        client2 = getConnect(data, host2, port2, "2-party", true);

//        if (client2==null) throw new Exception("client2 null!");

        context.put(FILELIST2, toFileAttributes(getDirInfo(client2, path2)));

        String currDir = client2.getCurrentDir();
        context.put(CURRDIR2, currDir);
		Log.info(" {xportlet:FileTransfer} Current dir in server 2 : " + currDir);

        if (path2.equals("")) {
		    Log.info(" {xportlet:FileTransfer} Set home2: " + currDir);
		    // portlet.setAttribute(HOME2, currDir, data);
          data.getUser().setPerm(HOME2, currDir);
        }

		// context.put(HOME2, portlet.getAttribute(HOME2, "/", data));
//		context.put(HOME2, (String)data.getUser().getPerm(HOME2, "/"));
        context.put(HOME2, (String)data.getUser().getPerm(HOME2, sep));
		
		// TODO: Take care of the File separator, depending on what the server returns.
//		if(currDir.equals("/"))
        if (currDir.equals(sep)) context.put(PARENTDIR2, "");
        else context.put(PARENTDIR2, currDir.substring(0, currDir.lastIndexOf(sep)));
//		    context.put(PARENTDIR2, currDir.substring(0, currDir.lastIndexOf("/")));
		    
        context.put(DOWNLOAD2, "gfd?host=" + host2 + "&port=" + port2 + "&path=" + currDir + "&filename=");

        msg2 = "Host2 connected to " + host2;
      }

      context.put(STATUS, msg +"<br>\n"+ msg2);
//      PrintStatus(msg +"<br>\n"+ msg2, context);
System.out.println("\n");
System.out.println(" {xportlet:FileTransfer} Host1: "+ host1+":"+port1);
System.out.println(" {xportlet:FileTransfer} Host2: "+ host2+":"+port2);
System.out.println(" {xportlet:FileTransfer} Path1: "+ path1);
System.out.println(" {xportlet:FileTransfer} Path2: "+ path2);
System.out.println(" {xportlet:FileTransfer} Home1: "+ data.getUser().getPerm(HOME1));
System.out.println(" {xportlet:FileTransfer} Home2: "+ data.getUser().getPerm(HOME2));
System.out.println("\n");
    }
    catch (Exception e){
//      PrintError(e.getMessage(), context);
      context.put("errmsg", e.getMessage());
//System.out.println(e.toString());
    }

  }

  //
  // Xiaobo Yang, 12 November 2004, copy file/directory
  // Method rewritten, using GridFTP 3rd-party transfer instead of UrlCopy for better performance
  // Use GridFTPClient objects stored in the session
  //
  // Method doThirdpartytransfer() starts
  public void doThirdpartytransfer(RunData data, Context context)
							throws Exception {  
System.out.println("B2: doThirdpartytransfer() called");
    SessionState state = ((JetspeedRunData)data).getPortletSessionState(((JetspeedRunData)data).getJs_peid());
    state.setAttribute(TOOLMODE, THIRD_PARTY_TRANSFER);
    state.removeAttribute(ERRMSG);
//        final String sep = "/";

	Log.info(" {xportlet:GridFTP} doUrlCopy ... ");
    String[] f2trans = data.getParameters().getStrings(FILENAME);
    String srchost = data.getParameters().getString(SRCHOST);
    String srcport = data.getParameters().getString(SRCPORT);
    String dsthost = data.getParameters().getString(DSTHOST);
    String dstport = data.getParameters().getString(DSTPORT);
    String srcpath = data.getParameters().getString(SRCPATH);
    String dstpath = data.getParameters().getString(DSTPATH);
    String errmsg = "";
//    VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);
 
    if (f2trans==null) {
      state.setAttribute(ERRMSG, "No file selected for transferring.");
      return;
    }
 
    try {
//      JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();

      GridFTPClient srcGFC = getConnect(data, srchost, srcport,
							"3rd-party", true);
      GridFTPClient destGFC = getConnect(data, dsthost, dstport,
		 					"3rd-party", false);

      int count = 0;

      for (int i=0; i<f2trans.length; i++) {
        if (checkDir(srcGFC, srcpath + sep + f2trans[i])) {
//System.out.print("LA: transferring DIRECTORY " + srcpath + sep + f2trans[i] + " on " + srchost + " to " + dstpath + sep + f2trans[i] + " on " + dsthost + "...");
          thirdPartyTransferDir2(srcGFC, srcpath + sep + f2trans[i],
			destGFC, dstpath + sep + f2trans[i], false);
//System.out.println("OK LA");
        }
        else {
//System.out.print("LALA: transferring FILE " + srcpath + sep + f2trans[i] + " on " + srchost + " to " + dstpath + sep + f2trans[i] + " on " + dsthost + "...");
//                  gfcSrc.transfer(srcpath + sep + f2trans[i], gfcDest, dstpath + sep + f2trans[i], false, null);
//System.out.println("OK LALA");
          thirdPartyFileTransfer(srcGFC, srcpath, f2trans[i], destGFC,
					dstpath, f2trans[i], false);
        }

        count++;

      }

      srcGFC.changeDir(srcpath);
      destGFC.changeDir(dstpath);

      if (count==0) {
        state.setAttribute(ERRMSG, "Totally " + count + " file(s) of " + 
				f2trans.length + " file(s) transferred.<br>");
      }
      else {
        state.setAttribute(STATUS, "Totally " + count + " file(s) of " + 
				f2trans.length + " file(s) transferred.");
      }

    }
    catch (Exception e) {
      e.printStackTrace();
      state.setAttribute(ERRMSG, e.getMessage());
    }

  }
  // Method doThirdpartyTransfer() ends

  //
  // Xiaobo Yang, 12 November 2004
  // Delete selected files/directories
  // Use GridFTPClient object stored in the session
  //
  // Method doDelete() starts
  public void doDelete(RunData data, Context context) throws Exception {
System.out.println("D2: doThirdpartytransfer() called");
    SessionState state = ((JetspeedRunData)data).getPortletSessionState(((JetspeedRunData)data).getJs_peid());
    state.setAttribute(TOOLMODE, DELETE);
    state.removeAttribute(ERRMSG);
    state.removeAttribute(WARNMSG);
//        final String sep = "/";

	Log.info(" {xportlet:GridFTP} doDelete ... ");
    String[] f2trans = data.getParameters().getStrings(FILENAME);
    String srchost = data.getParameters().getString(SRCHOST);
    String srcport = data.getParameters().getString(SRCPORT);
    String srcpath = data.getParameters().getString(SRCPATH);
    String errmsg = "";
//        VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);

    if (f2trans==null) {
//            PrintError("No file selected for deleting.", context);
      state.setAttribute(ERRMSG, "No file selected for deleting.");
      return;
    }

    try {
      GridFTPClient gridFTPClient = getConnect(data, srchost, srcport,
							"2-party", true);

      int count = 0;

      for (int i=0; i<f2trans.length; i++) {
        //
        // Xiaobo Yang, 9 November 2004, delete directory
        //
        // starts
        int i_portSrc = Integer.parseInt(srcport);
        if (checkDir(gridFTPClient, srcpath + sep + f2trans[i])) {
          try {
//System.out.print("LA: Deleting DIRECTORY " + srcpath + sep + f2trans[i] + " on " + srchost + "...");
            deleteRemoteSubDir2(gridFTPClient, srcpath, f2trans[i]);
//System.out.println("OK LA");
            count++;
          }
          catch (Exception e) {
            e.printStackTrace();
          }
          finally {
            continue;
          }
        }
        // ends

        try {
//System.out.print("LALA: Deleting FILE " + srcpath + sep + f2trans[i] + " on " + srchost + "...");
          gridFTPClient.deleteFile(srcpath + sep + f2trans[i]);
//System.out.println("OK LALA");
          count++;
        }
        catch (IOException ioe) {
          ioe.printStackTrace();
                Log.error(" {xportlet:FileTransfer} Failed to delete: " + f2trans[i]);
          context.put(WARNMSG, ioe.getMessage() + "<br>Failed to delete " + f2trans[i]);
          errmsg += "<br>" + f2trans[i] + ": " + ioe.getMessage();
        }
        catch (ServerException se) {
          se.printStackTrace();
                Log.error(" {xportlet:FileTransfer} Failed to delete: " + f2trans[i]);
//                context.put(WARNMSG, se.getMessage() + "<br>Failed to delete " + f2trans[i]);
          state.setAttribute(WARNMSG, se.getMessage() + "<br>Failed to delete " + f2trans[i]);
          errmsg += "<br>" + f2trans[i] + ": " + se.getMessage();
        }
      }

      if (count==0) {
//                PrintError("Totally " + count + " file(s) of " + 
//                           f2trans.length + " file(s) deleted.<br>" + 
//			   "Error: " + errmsg, context);
        state.setAttribute(ERRMSG, "Totally " + count + " file(s) of " + 
				f2trans.length + " file(s) deleted.<br>" + 
				"Error: " + errmsg);
      }
      else {
		// context.put(STATUS, "Totally " + count + " file(s) of " + f2trans.length + " file(s) deleted.");
//                PrintStatus("Totally " + count + " file(s) of " + 
//                            f2trans.length + " file(s) deleted.", context);
        state.setAttribute(STATUS, "Totally " + count + " file(s) of " + 
					f2trans.length + " file(s) deleted.");
      }
    }
    catch (Exception e) {
      e.printStackTrace();
//            PrintError(e.getMessage(), context);
      state.setAttribute(ERRMSG, e.getMessage());
    }

  }
  // Method doDelete() ends

  public void doUpload(RunData data, Context context) throws Exception {
System.out.println("C2: doThirdpartytransfer() called");
    SessionState state = ((JetspeedRunData)data).getPortletSessionState(((JetspeedRunData)data).getJs_peid());
    state.setAttribute(TOOLMODE, UPLOAD);
    state.removeAttribute(ERRMSG);
//	    String separator = "/";

    GridFTPClient client = null;
    String dsthost = data.getParameters().getString(DSTHOST);
    String dstport = data.getParameters().getString(DSTPORT);
    String dstpath = data.getParameters().getString(DSTPATH);
//	VelocityPortlet portlet = (VelocityPortlet)context.get(PORTLET);

    try {
	    Log.info(" {xportlet:GridFTP} doUpload ...");

      HttpServletRequest request = data.getRequest();
	    PrintDebug("<TABLE BORDER=1 ALIGN=CENTER>\n<TR BGCOLOR=#FFAD00><TH>Header Name<TH>Header Value", context);
      Enumeration headerNames = request.getHeaderNames();
      while (headerNames.hasMoreElements()) { 
        String headerName = (String)headerNames.nextElement(); 
		PrintDebug("<TR><TD>" + headerName, context); 
		PrintDebug(" <TD>" + request.getHeader(headerName), context); 
      }
	    PrintDebug("</TABLE>", context);
      String useragent = request.getHeader("User-Agent");

      client = getConnect(data, dsthost, dstport, "2-party", true);
      if (client==null) throw new Exception("client null!");

	    Log.info(" {xportlet:GridFTP} doUpload Current dir: " + client.getCurrentDir());
      FileItem file = data.getParameters().getFileItem(UPLOADFILE);
      if (file==null) throw new Exception("Upload file null");

	    Log.info(" {xportlet:GridFTP} doUpload completed.\nSize of uploaded file: " + file.getSize());

      String s = file.getName();
	    System.out.println("File Name: " + s);
      String separator = sep;
      if (useragent.indexOf("Win")>=0) {  // the client is a Windows machine
        separator = "\\";
      }
	    System.out.println(useragent);
	    System.out.println("Separator: " + separator);
      int idx1 = s.lastIndexOf(separator);
      int idx2 = s.length();
      if (idx1==-1) { //in case no path in the filename
        idx1 = 0;
      }
      else {
        idx1++;
      }

	    System.out.println("Index1: " + idx1 + " Index2: " + idx2);
      s = s.substring(idx1, idx2);
	    System.out.println("Without path: " + s);
	    System.out.println("Dest path: " + dstpath);

      InputStream in = file.getInputStream();
      DataSourceStream dss = new DataSourceStream(in);
      client.put(dstpath + sep + s, dss, null);

//	    PrintStatus(file.getName() + " uploaded.", context);
      state.setAttribute(STATUS, file.getName() + " uploaded.");
    }
    catch (Exception e) {
      e.printStackTrace();
	    Log.error(" {xportlet:GridFTP} Failed to upload file: "+e);
//	    PrintError("Failed to upload file: " + e.getMessage(), context);
      state.setAttribute(ERRMSG, "Failed to upload file: " + e.getMessage());
    }

  }

/*    public void doShow_progress(RunData data, Context context) throws Exception
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
	setTemplate(data, "xportlets-gridftp-browse");
    }

    public void doPerform(RunData data) throws Exception
    {
	FileItem file = data.getParameters().getFileItem("uploadfilename");         
	Log.info(" {xportlet:GridFTP} Size of uploaded file from (FileItem) file.getSize(): " + file.getSize());                    
    }
*/    

    private String getStackTrace(Exception e)
    {
	StringWriter sw = new StringWriter();
	PrintWriter pw = new PrintWriter(sw);
	e.printStackTrace(pw);
	return sw.toString();
    }

    public Vector getDirInfo(GridFTPClient client, 
			     String path) throws Exception{
	Vector v = null;
	
	try {
	    String currDir = client.getCurrentDir();
	    if(path == null) 
		path = "";
            
	    if(path.equals(""))
//		client.changeDir(currDir + "/" + path);
		client.changeDir(currDir + sep + path);
  	    else
  		client.changeDir(path);

	    currDir = client.getCurrentDir();
	    
            //
            // Xiaobo Yang, 10 November 2004
            // In case this method is called more than one times
            //
            client.setPassive();
            client.setLocalActive();

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
//	    System.out.println("Debug 1"); // commented by Xiaobo Yang, 12 November 2004
	    
	    HttpSession session = data.getSession();
//	    System.out.println("Debug 2"); // commented by Xiaobo Yang, 12 November 2004
	    
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
	    
//	    System.out.println("Debug 3"); // commented by Xiaobo Yang, 12 November 2004
	    
	    if(client == null || closedConnection == true)
		{
//  		    ProxyTable proxies = new ProxyTable();
  		    JetspeedUser user = ((JetspeedRunData)data).getJetspeedUser();
//  		    System.out.println("Debug 4");
//  		    proxies.load( user );
//  		    System.out.println("Debug 5");
		    
//  		    GSSCredential proxy = proxies.getDefault();

  		        //
  		  	// use gridservice first, then proxymanager to retrieve a proxy
  		  	// Xiaobo Yang, 30 November 2004
  		  	//
  		    GSSCredential proxy = GridService.getCurrentUserGSSCredential();
  		    if (proxy==null) {
  		    	proxy = ProxyManager.getDefaultProxy(session);
  		    }
  		    if (proxy==null) {
  		    	throw new Exception("Proxy is not set or is null");
  		    }
		    
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
                    //
                    // Xiaobo Yang, 8 November 2004
                    // Add directory to FILELIST
                    //
                    // starts
                    fattr.setisFile("2");
                    // ends
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

  //
  // Xiaobo Yang, 10 November 2004
  // Check whether "dir" is a directory on remote "host"
  // Returns "true" if "dir" is a directory, otherwise "false"
  //
  // Method checkDir() starts
  private boolean checkDir(GridFTPClient gridFTPClient, String dir) {
   boolean b_isDir = false;
    try {
//      gridFTPClient.setPassive();
//      gridFTPClient.setLocalActive();
      gridFTPClient.changeDir(dir);
      b_isDir = true;
    }
    catch (IOException ioe) {
      b_isDir = false;
//      ioe.printStackTrace();
    }
    catch (ServerException se) {
      b_isDir = false;
//      se.printStackTrace();
    }
    finally {
      return b_isDir;
    }

  }
  // Method checkDir() ends


  //
  // Xiaobo Yang, 10 November 2004
  // Delete directory on remote host
  //
  // Method deleteRemoteSubDir2() startsD
  private void deleteRemoteSubDir2(GridFTPClient gridFTPClient,
		String dir_parent, String dir_sub) throws Exception {
//    final String sep = "/";

    // get a list of all files & directories of dir on host
    Vector v_DirInfo = getDirInfo(gridFTPClient, dir_parent + sep + dir_sub);
    int totalNumber = v_DirInfo.size();
//System.out.println("Dir: " + dir_sub + " has " + totalNumber + " object(s)");

    if (totalNumber==0) { // dir on host is empty
      gridFTPClient.setLocalPassive();
      gridFTPClient.setActive();
      gridFTPClient.changeDir(dir_parent);
//System.out.print("Current DIR: " + gridFTPClient.getCurrentDir() + ", deleting empty DIR " + dir_sub + "...");
      gridFTPClient.deleteDir(dir_sub); // dir is empty so delete it

    }
    else { // dir_sub is not empty
      Enumeration enum = v_DirInfo.elements(); // dir is not empty
      int i_subDir = 0;
      Vector v_subDir = new Vector();
      while (enum.hasMoreElements()) {
        FileInfo finfo = (FileInfo)enum.nextElement();
        gridFTPClient.setLocalPassive();
        gridFTPClient.setActive();
        if (finfo.isDirectory()) { // sub-directory exists
          i_subDir++;
          v_subDir.add(finfo.getName());
        }
        else {
//System.out.print("Deleting FILE: " + finfo.getName() + "...");
          gridFTPClient.deleteFile(finfo.getName());
//System.out.println("OK");
        }
//      else if (finfo.isFile()) { // file
//      }
//      else if (finfo.isSoftLink()) { // softlink
//      }
//      else if (finfo.isDevice()) { // device
//      }

      }

      if (i_subDir>0) { // sub dir exists, delete
        for (int i=0; i<i_subDir; i++) {
          deleteRemoteSubDir2(gridFTPClient, dir_parent + sep + dir_sub,
					(String)v_subDir.get(i));
        }
      }

      // now no sub dir, delete dir
      gridFTPClient.setLocalPassive();
      gridFTPClient.setActive();
      gridFTPClient.changeDir(dir_parent);
//System.out.print("Current DIR: " + gridFTPClient.getCurrentDir() + ", deleting empty DIR " + dir_sub + "...");
      gridFTPClient.deleteDir(dir_sub); // dir is empty so delete it
//System.out.println("OK");
    }

  }
  // Method deleteRemoteSubDir2() ends


  //
  // Xiaobo Yang, 3 December 2004
  // 3rd party transfer file
  // if file > 1MB transfer in parallel mode, otherwise in non-parallel mode
  // Method thirdPartyFileTransfer() starts
  public void thirdPartyFileTransfer(GridFTPClient srcGFC,
		String srcDir, String srcFile, GridFTPClient destGFC,
		String destDir, String destFile, boolean b_append)
		throws Exception {
    String srcFullpathFile = srcDir + sep + srcFile;
    String destFullpathFile = destDir + sep + destFile;

    if (!srcGFC.exists(srcFullpathFile)) {
      System.out.println("ERROR: source file " + srcFullpathFile + " does not exist");
      fileSize = -1;
      return;
    }

    fileName = srcFile;
    fileSize = srcGFC.getSize(srcFullpathFile);
    System.out.println(srcFile + ": " + fileSize + " byte(s)");

    if (fileSize>1048576) { // file > 1MB transfer in parallel mode
      setParams3rdPartyParallelMode(srcGFC, destGFC);
      System.out.print("3rd-party transfer in parallel mode started...");
      long t_start = System.currentTimeMillis();
      srcGFC.extendedTransfer(srcFullpathFile, destGFC, destFullpathFile,
//						null);
						new MarkerListenerImpl());
      double dt = (double)(System.currentTimeMillis() - t_start)/1000.0;
      System.out.println("OK, " + dt + " s, " + (double)fileSize/dt + " bytes/s");
    }
    else { // file <= 1MB transfer in non-parallel mode
      setParams3rdPartyBackToNonParallelMode(srcGFC, destGFC);
      System.out.print("3rd-party transfer in non-parallel mode started...");
      srcGFC.transfer(srcFullpathFile, destGFC, destFullpathFile, b_append, null); // 3rd party transfer
      System.out.println("OK");
    }
    setParams3rdPartyBackToNonParallelMode(srcGFC, destGFC);

  }
  // Method thirdPartyFileTransfer() ends

  //
  // Xiaobo Yang, 3 December 2004
  // 3rd-party transfer, transfer directory
  //
  // Method thridPartyTransferDir2() starts
  public void thirdPartyTransferDir2(GridFTPClient srcGFC, String srcDir,
		GridFTPClient destGFC, String destDir, boolean b_append)
		throws Exception {
    if (!destGFC.exists(destDir)) { // if dir_dest does not exist
      System.out.print(destDir + " does not exist on destinaton host, creating...");
      destGFC.makeDir(destDir); // create destDir on destHost
      System.out.println("OK");
    }

    System.out.print("Changing to destination directory " + destDir + "...");
    destGFC.changeDir(destDir); // set destDir as current directory
    System.out.println("OK");

    // get a list of all files & directories of srcDir on srcHost
    System.out.println("Getting a list of source directory...");
    Vector vSrcDirInfo = getDirInfo(srcGFC, srcDir);
    System.out.println("OK");
    Enumeration enum = vSrcDirInfo.elements();
    while (enum.hasMoreElements()) {
      FileInfo finfo = (FileInfo)enum.nextElement();
      setParams3rdPartyBackToNonParallelMode(srcGFC, destGFC);

      if (finfo.isDirectory()) { // directory
        System.out.println("Transferring directory " + finfo.getName() + "...");
        thirdPartyTransferDir2(srcGFC,
		srcDir + sep +	finfo.getName(), destGFC,
		destDir + sep + finfo.getName(), b_append);
        System.out.println("OK");
      }
      else if (finfo.isFile()) { // file
        thirdPartyFileTransfer(srcGFC, srcDir, finfo.getName(), destGFC,
				destDir, finfo.getName(), b_append);
      }
//      else if (finfo.isSoftLink()) { // softlink
//      }
//      else if (finfo.isDevice()) { // device
//      }

    }

    setParams3rdPartyBackToNonParallelMode(srcGFC, destGFC);

  }
  // Method thridPartyTransferDir2() ends

  //
  // Xiaobo Yang, 3 December 2004
  // Methods setParams3rdParty(), setParams3rdPartyModeE(),
  // setParams3rdPartyParallelMode(), setParams3rdPartyBackToNonParallelMode()
  // are used to set correct server mode for file transfer using gridftp
  //
  private void setParams3rdParty(GridFTPClient client) throws Exception{
    client.setProtectionBufferSize(16384);
    client.setType(GridFTPSession.TYPE_IMAGE);
    client.setMode(GridFTPSession.MODE_STREAM);

  }

  private void setParams3rdPartyModeE(GridFTPClient client)
							throws Exception{
    client.setProtectionBufferSize(16384);
    client.setType(GridFTPSession.TYPE_IMAGE);
    client.setMode(GridFTPSession.MODE_EBLOCK);

  }

  private void setParams3rdPartyParallelMode(GridFTPClient srcGFC,
				GridFTPClient destGFC) throws Exception {
    setParams3rdPartyModeE(srcGFC);
    setParams3rdPartyModeE(destGFC);
    srcGFC.setOptions(new RetrieveOptions(5));
    HostPortList hpl = destGFC.setStripedPassive();
    srcGFC.setStripedActive(hpl);

  }

  private void setParams3rdPartyBackToNonParallelMode(GridFTPClient srcGFC,
				GridFTPClient destGFC) throws Exception {
    setParams3rdParty(srcGFC);
    setParams3rdParty(destGFC);
    destGFC.setActive(srcGFC.setPassive());
    // the line below is necessary for method list() if the transfer mode is
    // changed from parallel to non-parallel
    srcGFC.setOptions(new RetrieveOptions(1));

  }

  //
  // Xiaobo Yang, 3 December 2004
  // MarkerListener implementation for 3rd-party transfer in parallel mode
  //
  // class MarkerListenerImpl starts
  class MarkerListenerImpl implements MarkerListener{

	public ByteRangeList list;
	
	public MarkerListenerImpl() {
	    list = new ByteRangeList();
	}
	
	public void markerArrived(Marker m) {
	    if (m instanceof GridFTPRestartMarker) {
		restartMarkerArrived((GridFTPRestartMarker) m);
	    } else if (m instanceof PerfMarker) {
		perfMarkerArrived((PerfMarker) m);
	    } else {
		System.out.println("Received unsupported marker type");
	    }
	};
	
	private void restartMarkerArrived(GridFTPRestartMarker marker) {
//	    System.out.println("--> restart marker arrived:");
//	    list.merge(marker.toVector());
//	    System.out.println("Current transfer state: " + list.toFtpCmdArgument());
	}
	
	private void perfMarkerArrived(PerfMarker marker) {
//	    System.out.println("--> perf marker arrived");
//	    // time stamp
//	    System.out.println("Timestamp = " + marker.getTimeStamp());
//
//	    // stripe index
//	    if (marker.hasStripeIndex()) {
//		try {
//		    System.out.println("Stripe index =" + marker.getStripeIndex());
//		} catch (PerfMarkerException e) {
//		    System.out.println(e.toString());
//		}
//	    }else {
//		System.out.println("Stripe index: not present");
//	    }

	    // stripe bytes transferred
	    if (marker.hasStripeBytesTransferred()) {
		try {
                  long bytesTransferred = marker.getStripeBytesTransferred();
//		    System.out.println("Stripe bytes transferred = "
//				 + bytesTransferred);
                  float f = (float)bytesTransferred/(float)fileSize*(float)100.0;
                  String s = Float.toString(f);
                  s += "000"; // in case "0.0" can not get substring
                  s = s.substring(0, s.indexOf('.')+3);
                  System.out.println(s + "% transferred");
//                  PrintError("Transferring " + fileName + ": " + s
//						+"% finished", context);
		}
                catch (PerfMarkerException e) {
                  System.out.println(e.toString());
		}
            }
            else {
//		System.out.println("Stripe Bytes Transferred: not present");
	    }
	    
//	    // total stripe count
//	    if (marker.hasTotalStripeCount()) {
//		try {
//		    System.out.println("Total stripe count = " 
//				 + marker.getTotalStripeCount());
//		} catch (PerfMarkerException e) {
//		    System.out.println(e.toString());
//		}
//	    }else {
//		System.out.println("Total stripe count: not present");
//	    }

	}//PerfMarkerArrived   

  }
  //class MarkerListenerImpl ends

}

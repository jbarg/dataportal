package xportlets.gridftp;

import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

import org.globus.ftp.*;

public class GridFTPDownload extends HttpServlet {
 
    public void doGet(HttpServletRequest request,
                      HttpServletResponse response)
        throws ServletException, IOException {
	//          response.setContentType("text/html");
	ServletOutputStream out = response.getOutputStream();
	Enumeration paramNames = request.getParameterNames();
	
        String[] host = new String[1];
        String[] port = new String[1];
        String[] path = new String[1];
	String[] filename = new String[1];
        int choice = 1;
 
        while(paramNames.hasMoreElements()) {
            String paramName = (String)paramNames.nextElement();
            if(paramName.equals("host")) {
                host = request.getParameterValues(paramName);
            }
            else if(paramName.equals("port")) {
                port = request.getParameterValues(paramName);
            }
            else if(paramName.equals("path")) {
                path = request.getParameterValues(paramName);
            }
            else if(paramName.equals("filename")) {
                filename = request.getParameterValues(paramName);
            }
        }
	
	org.globus.ftp.GridFTPClient client = null;
	try {
	    client = getConnect(request.getSession(), host[0], port[0]);

	    File file = new File("/tmp/" + filename[0]);
	    //client.setType(FTPClient.BINARY);
	    client.setType(GridFTPSession.TYPE_IMAGE);
	    client.get( path[0] + "/" + filename[0], file );

            FileInputStream fis = new FileInputStream(file);
 
	    String contenttype = getContentType(filename[0]);
	    System.out.println("Content type: " + contenttype);
	    response.setContentType(contenttype);
	    response.setHeader("Content-Disposition", "attachment; filename="+filename[0]+";");
	    
            int c = 0;
            while ((c = fis.read()) != -1) {
                out.write(c);
            }
            out.flush();
            out.close();
	    file.delete();
	}
	catch(Exception e) {
	    e.printStackTrace();
	}
    }
 
    public void doPost(HttpServletRequest request,
		       HttpServletResponse response)
	throws ServletException, IOException {
	doGet(request, response);
    }
    
    public String getContentType(String filename) {
	int idx = filename.lastIndexOf(".");
	if(idx == -1)
	    return new String("application/octet-stream");

	String ext = filename.substring(idx+1, filename.length());

	if(ext.equals("ps")) {
	    return new String("application/postscript");
	}
	else if(ext.equalsIgnoreCase("pdf")) {
	    return new String("application/pdf");
	}
	else if(ext.equalsIgnoreCase("zip")) {
	    return new String("application/zip");
	}
	else if(ext.equalsIgnoreCase("jpg")) {
	    return new String("image/jpeg");
	}
	else if(ext.equalsIgnoreCase("gif")) {
	    return new String("image/gif");
	}
	else if(ext.equalsIgnoreCase("tif")) {
	    return new String("image/tiff");
	}
	else if(ext.equalsIgnoreCase("mpg")) {
	    return new String("video/mpeg");
	}
	else if(ext.equalsIgnoreCase("mov")) {
	    return new String("video/quicktime");
	}
	else if(ext.equalsIgnoreCase("htm")) {
	    return new String("text/html");
	}
	else if(ext.equalsIgnoreCase("html")) {
	    return new String("text/html");
	}
	else if(ext.equalsIgnoreCase("xml")) {
	    return new String("text/xml");
	}
	else if(ext.equalsIgnoreCase("doc")) {
	    return new String("application/msword");
	}

	return new String("application/octet-stream");
    }

    public org.globus.ftp.GridFTPClient getConnect(HttpSession session,
                                                   String host,
                                                   String port) throws Exception {
	org.globus.ftp.GridFTPClient client = null;
	
	try {
	    client = (org.globus.ftp.GridFTPClient)session.getAttribute(host);
	    System.out.println("Trying to get " + host + " connection from System properties...");	    
//  	    client = (org.globus.ftp.GridFTPClient)System.getProperties().get("aaa");
//  	    client = System.getProperties().get("aaa");
//  	    if(client == null) {
//  		ServletConfig sc = getServletConfig();
//  		ServletContext sctx = sc.getServletContext();
//  		client = (org.globus.ftp.GridFTPClient)sctx.getAttribute("sh");
//  	    }
	    if(client == null) {
		throw new Exception("No such connection in session: " + session.getId());
	    }
            client.setPassive();
            client.setLocalActive();
	}
	catch(Exception e) {
	    throw e;
	}
	
	return (org.globus.ftp.GridFTPClient)client;
	
    }
    
}

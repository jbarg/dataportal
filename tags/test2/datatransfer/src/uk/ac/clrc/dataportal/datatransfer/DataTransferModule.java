package uk.ac.clrc.dataportal.datatransfer;
/**
 * @author Andrew Woolf
 */

import org.globus.io.urlcopy.UrlCopy;
import org.globus.util.GlobusURL;
import org.globus.myproxy.MyProxy;
import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;

import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSName;
import java.io.*;
import org.gridforum.jgss.ExtendedGSSManager;
import java.net.*;
import org.ietf.jgss.GSSManager;

/**
 * This is the data transfer class for the CLRC data portal. It looks after
 * transferring a data file between specified locations using one of a
 * variety of transport protocols (FTP, HTTP, GridFTP).
 */

public class DataTransferModule {

    /**
     * This is the core method for file transfer. A GridFTP third-party transfer is
     * used to transfer a file from one URL to another. The same Globus credential
     * is used for both ends (and is retrieved from a MyProxy server). Valid forms for
     * source and destination URLs include:
     *       {gsiftp,http,https}://gridftp.host.fqdn//fully/qualified/file/path
     *       file:////fully/qualified/file/path
     * The extra forward slash in the file path is needed because of a curiosity of
     * the Globus Java CoG.
     *
     * @param fromURL The source URL
     * @param toURL The destination URL
     * @param myProxyServer The hostname of the MyProxy server to use
     * @param username The username of the MyProxy credential to retrieve
     * @param password The password of the MyProxy credential to retrieve
     */

   /* public static void transferFile(String fromURL, String toURL,
    String myProxyServer, String username, String password)
    throws Exception {
        try {

            // Get default (user) credentials for authenticating to MyProxy server
            //      GSSManager manager = ExtendedGSSManager.getInstance();
            //      GSSCredential credential = manager.createCredential(GSSCredential.INITIATE_ONLY);
            //      System.err.println("DEFAULT CREDENTIAL:"+credential.getName().toString());

            // Setup some initial stuff (this should probably come from a config file)
            // String portalCertFile = "/home/tomcat4/.globus/portalcert.pem";
            // String portalKeyFile = "/home/tomcat4/.globus/portalkey.pem";
            String portalCertFile = System.getProperty("user.home")+"/.globus/portalcert.pem";
            String portalKeyFile = System.getProperty("user.home")+"/.globus/portalkey.pem";

            // Create portal credential from cert/key files
            GlobusCredential globusCredential = new GlobusCredential(
            portalCertFile, portalKeyFile);
            GSSCredential credential2 = new GlobusGSSCredentialImpl(globusCredential,
            GSSCredential.INITIATE_AND_ACCEPT);



            // Get MyProxy credential for use with GridFTP
            MyProxy myProxy = new MyProxy(myProxyServer, 7512);
            GSSCredential myProxyCredential = myProxy.get(credential2, username, password,
            2*3600);
            System.out.println("cred ok");
            // Do GridFTP third-party transfer
            UrlCopy uc = new UrlCopy();
            uc.setSourceUrl(new GlobusURL(fromURL));
            uc.setDestinationUrl(new GlobusURL(toURL));
            uc.setUseThirdPartyCopy(true);
            //      uc.setDCAU(true);
            //      uc.setSourceAuthorization(null);
            //      uc.setDestinationAuthorization(null);
            //      uc.setAppendMode(false);
            uc.setCredentials(myProxyCredential);
            uc.copy();
        }
        catch (Exception e) {
            System.err.println("DataTransferModule.transferFile error: " +
            e.getMessage());
            throw e;
        }
    }*/

    public static String urlCopy(String fromURL, String toURL,String cert)
    throws Exception {
        try {

       /*   FileWriter file = new FileWriter("c:/v.pm");
            file.write(cert);
            file.close();
            URL url1 = new URL("file:///c:/v.pm");
            // System.out.println(url);
            URLConnection con = url1.openConnection();
            InputStream in2 = con.getInputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(in2));
            String inputLine;
            // String certt;
            StringBuffer cert1 = new StringBuffer();
            while ((inputLine = in.readLine()) != null){
                //  System.out.println(inputLine);
                cert1.append(inputLine);
                //cert1.append("\n");
                 if(!inputLine.equals("-----END CERTIFICATE-----"))  cert1.append("\n");

            }
            in.close();
            String cert3 = cert1.toString();*/
             GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(cert.getBytes()));


            //GlobusCredential globusCredential = new GlobusCredential(new FileInputStream("c:/v.pm"));
            //  GlobusCredential globusCredential = new GlobusCredential("c:/x509_gjd");

            //  GlobusCredential globusCredential = new GlobusCredential(System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
            GSSCredential credential2 = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
            //System.out.println(credential2.getRemainingLifetime());

            UrlCopy uc = new UrlCopy();
            uc.setSourceUrl(new GlobusURL(fromURL));
            uc.setDestinationUrl(new GlobusURL(toURL));
            uc.setUseThirdPartyCopy(true);
            //      uc.setDCAU(true);
            //      uc.setSourceAuthorization(null);
            //      uc.setDestinationAuthorization(null);
            //      uc.setAppendMode(false);
            uc.setCredentials(credential2);
            uc.copy();
            return "true";
        }
        catch (Exception e) {
            System.err.println("DataTransferModule.transferFile error: " +
            e);
            return e.getMessage();
            
           
        }
    }


    public static void main(String[] args){
        try{
            //   URL url1  = new URL("file:///"+System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
            URL url1 = new URL("file:///"+System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
            // System.out.println(url);
            URLConnection con = url1.openConnection();
            InputStream in2 = con.getInputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(in2));
            String inputLine;
            // String certt;
            StringBuffer cert = new StringBuffer();
            while ((inputLine = in.readLine()) != null){
                //  System.out.println(inputLine);
                cert.append(inputLine);
                cert.append("\n");
                // if(!inputLine.equals("-----END CERTIFICATE-----"))  cert.append("\n");

            }
            in.close();
            String cert3 = cert.toString();
            urlCopy("http://localhost:8080/index.jsp","gsiftp://esc.dl.ac.uk/ggg",cert3);
        }
        catch(Exception e){
            System.out.println(e);
        }

    }

}

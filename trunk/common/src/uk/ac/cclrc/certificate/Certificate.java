package uk.ac.cclrc.certificate;

// Globus certificate
import org.globus.gsi.*;
import org.globus.gsi.gssapi.*;
import org.globus.gsi.gssapi.*;
import org.ietf.jgss.*;

// Streams
import java.io.ByteArrayInputStream;

// Testing
import java.net.URL;
import java.net.URLConnection;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;

public class Certificate {
    
    private String certificate;
    public String toString() { return certificate; }
    
    public Certificate(String certificate) {
        this.certificate = certificate;
    }
    
    /** Creates a new instance of certificate from a string */
    public Certificate(java.io.InputStream certificate) throws Exception {
        BufferedReader in = new BufferedReader(new InputStreamReader(certificate));
        String inputLine;
        StringBuffer cert = new StringBuffer();
        while ((inputLine = in.readLine()) != null){
            cert.append(inputLine);
            cert.append("\n");
        }
        in.close();

        this.certificate = cert.toString();
        
    }
    
    /*
     * Creates a new instance of certificate from a file 
     * e.g. file:///E:/cog-1.1/build/cog-1.1/bin/x509up_36855.pem
     */
    public Certificate(URL url) throws Exception {
        URLConnection con = url.openConnection();
        InputStream in2 = con.getInputStream();
        BufferedReader in = new BufferedReader(new InputStreamReader(in2));
        String inputLine;
        StringBuffer cert = new StringBuffer();
        while ((inputLine = in.readLine()) != null){
            cert.append(inputLine);
            cert.append("\n");
        }
        in.close();

        this.certificate = cert.toString();
    }

    public long getLifetime() throws Exception {
        
        byte[] data = this.certificate.getBytes();
        GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
        GSSCredential cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
        
        // Get remaining lifetime in milliseconds
        long lifetimeLeft = cred.getRemainingLifetime();
        return lifetimeLeft;
    }
    
    public boolean lifetimeLeft() throws Exception {
        boolean result = false;
        // Check some lifetime left on proxy certificate
        if (getLifetime() > 0) {
            result = true;
        }
        return result;
    }
    
    public String getDName() throws Exception {
        
        byte[] data = this.certificate.getBytes();
        GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(data));
        GSSCredential cred = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
        return cred.getName().toString();
    }
    
    // Testing stub
    private static void prt(Certificate c) throws Exception {
        System.out.println("Certificate: "+c.toString());
        System.out.println("Lifetime: "+c.getLifetime());
        System.out.println("Any life left? "+c.lifetimeLeft());
        System.out.println("DN: "+c.getDName());
    }

    public static void main(String args[]) throws Exception {
        
        Certificate cert;
        
        // Name of proxy that has expired
        cert = new Certificate(new URL("file:///E:/cog-1.1/build/cog-1.1/bin/x509up_36855.pem"));
        prt(cert);
        
        // Name of current proxy file
        cert = new Certificate(new URL("file:///E:/cog-1.1/build/cog-1.1/bin/x509up_47677.pem"));
        prt(cert);
        
        // test with certificate as string
        //Certificate cert2 = new Certificate(cert.toString());
        //prt(cert2);
    }
    
}
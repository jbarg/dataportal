/*
 * TokenReader.java
 *
 * Created on 26 October 2003, 21:50
 */

package uk.ac.cclrc.authorisation.client;
import uk.ac.cclrc.authorisation.Config;
import java.io.IOException;
import java.io.PrintStream;
import java.security.*;
import java.security.cert.*;
import java.security.interfaces.*;
import org.globus.gsi.CertUtil;
import org.globus.gsi.OpenSSLKey;
import org.globus.gsi.bc.BouncyCastleOpenSSLKey;
import uk.ac.cclrc.authorisation.*;
import org.apache.log4j.*;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import java.util.*;
import java.io.*;
import org.globus.util.Base64;

public class TokenReader {
    
    private X509Certificate acServerCertificate;
    private PublicKey acServerPublicKey;
    private String messageText;
    private String certFile;
    private Properties prop;
    
    //use for Java KeyStore
    static Logger log = Logger.getLogger(TokenReader.class);
    RSAPublicKey pubKey;
    
    public TokenReader()
    throws IOException, GeneralSecurityException, InvalidKeyException {
        // certFile = System.getProperty("user.home")+File.separator+".globus"+File.separator+"portalcert.pem";
        
        //System.out.println("cert difle "+certFile );
        //certFile = "/home/asm67/project/authorisation/certificates/escvig3Cert.pem";
        // acServerCertificate = CertUtil.loadCertificate(certFile.trim());
        
        // acServerPublicKey = acServerCertificate.getPublicKey();
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        //Using SUN Java Key Store for now
        Properties prop = new Properties();
        try{
            prop.load(new FileInputStream(Config.getContextPath()+"authorisation.prop"));
        }
        catch(IOException ioe){
            log.error("Unable to find "+Config.getContextPath()+"authorisation.prop",ioe);
            throw ioe;
        }
        String filename = prop.getProperty("certificate");
        if(filename == null) throw new FileNotFoundException("null not found on the system");
        File certFile = new File(filename);
        if(!certFile.exists()) throw new FileNotFoundException(certFile.getAbsolutePath()+" not found on the system");
        InputStream inStream = new FileInputStream(certFile);
        CertificateFactory cf = CertificateFactory.getInstance("X.509");
        X509Certificate cert1 = (X509Certificate)cf.generateCertificate(inStream);
        inStream.close();
        pubKey = (RSAPublicKey)cert1.getPublicKey();
    }
    
    /** Method to verify Attribute Certificate
     ** @param String attributeCertificate
     *  @return boolean true/false
     **/
    public AttributeList getAttributes(org.w3c.dom.Element authorisationToken) throws NoSuchAlgorithmException, InvalidKeyException, SignatureException, InvalidAuthorisationTokenException, Exception {
        try {
            DOMBuilder builder = new DOMBuilder();
            Element root = builder.build(authorisationToken);
            Element acInfo = root.getChild("acInfo");
            Element signature = root.getChild("signature");
            
            if (verifyauthorisationToken(acInfo,signature)) {
                
                AttributeList al = new AttributeList(acInfo);
                
                if (al.isValid()) // check if remaining time is still present in Token
                    return al;
                else throw new InvalidAuthorisationTokenException("Token timeout, request for new token");
            } else {
                throw new InvalidAuthorisationTokenException("Signature does not match");
            }
        } catch (Exception e) {
            //log.error("Unexcepted error with getAttributes",e);
            throw e;
        }
    }
    
    /** Method to verify Attribute Certificate
     * @param String messageText
     * @param String signatureText
     *  @return boolean true/false
     **/
    public boolean verifyauthorisationToken(String messageText, String signatureText)
    throws NoSuchAlgorithmException, InvalidKeyException, SignatureException {
        Signature sigVerifier = Signature.getInstance("SHA1withRSA");
        sigVerifier.initVerify(acServerPublicKey);
        sigVerifier.update(messageText.getBytes());
        return sigVerifier.verify(signatureText.getBytes());
    }
    
    /** Method to verify Attribute Certificate
     * @param Element acInfo
     * @param Element signature
     *  @return boolean true/false
     **/
    private boolean verifyauthorisationToken(Element acInfo, Element signature)   throws NoSuchAlgorithmException, InvalidKeyException, SignatureException   {
        
        XMLOutputter outputter = new XMLOutputter();
        String messageText = outputter.outputString(acInfo);
        
        String signatureText = signature.getText();
        //decode the signiture text
        byte[] decode = org.globus.util.Base64.decode(signatureText.getBytes());
        
        
        
        Signature sigVerifier = Signature.getInstance("SHA1withRSA");
        sigVerifier.initVerify(pubKey);
        sigVerifier.update(messageText.getBytes());
        
        
        if(!sigVerifier.verify(decode)){
            
            
            return false;
        }
        else return true;
        
    }
    
}

//package src.uk.ac.cclrc.authorisation;
package uk.ac.cclrc.authorisation;


import java.io.*;
import java.security.*;
import java.security.cert.*;
import java.security.interfaces.*;
import java.io.*;
import org.jdom.*;
import org.jdom.output.*;
import org.apache.log4j.*;
import java.util.*;

public class ACGen {
    
    private X509Certificate hostCertificate;
    private PrivateKey hostPrivateKey;
    private PublicKey hostPublicKey;
    private String messageText;
    private String certFile;
    private String keyFile;
    
    //use for Java KeyStore
    RSAPrivateKey prvKey;
    RSAPublicKey pubKey;
    String algorithm;
    static Logger log = Logger.getLogger(ACGen.class);
    
    public ACGen()
    throws IOException, GeneralSecurityException, InvalidKeyException {
        // certFile = System.getProperty("user.home")+File.separator+".globus"+File.separator+"portalcert.pem";
        //keyFile = System.getProperty("user.home")+File.separator+".globus"+File.separator+"portalkey.pem";
        //openHostKey(certFile, keyFile);
        
        //Using SUN Java Key Store for now
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        loadKeyStore();
        
    }
    //not needed
    public String getACInString(String attributeString)
    throws InvalidKeyException, NoSuchAlgorithmException, SignatureException {
        messageText = attributeString;
        Signature signer = Signature.getInstance(algorithm);
        signer.initSign(prvKey);
        signer.update(messageText.getBytes());
        byte signatureBytes[] = signer.sign();
        
        //return Base64 encoding of the signature
        byte[] encode = org.globus.util.Base64.encode(signatureBytes);
        BufferedReader reader= new BufferedReader(new InputStreamReader(new ByteArrayInputStream(encode)));
        String stringEncoding = "";
        try{
            stringEncoding = reader.readLine();
            reader.close();
        }
        catch(Exception e){
            try{
                reader.close();
            }
            catch(Exception ignore){}
        }
        return stringEncoding;
    }
    
    /** Method generates signature of the message using the pvt key of server */
    public String getSignatureString(String attributeString) throws Exception{
        messageText = attributeString;
        Signature signer = Signature.getInstance(algorithm);
        signer.initSign(prvKey);
        signer.update(messageText.getBytes());
        byte signatureBytes[] = signer.sign();
        
        //return Base64 encoding of the signature
        byte[] encode = org.globus.util.Base64.encode(signatureBytes);
        BufferedReader reader= new BufferedReader(new InputStreamReader(new ByteArrayInputStream(encode)));
        String stringEncoding = "";
        try{
            stringEncoding = reader.readLine();
            reader.close();
        }
        catch(Exception e){
            try{
                
                reader.close();
            }
            catch(Exception ignore){}
        }
        return stringEncoding;
    }
    
    //use when this works
    private void openHostKey(String certFile, String keyFile)
    throws IOException, GeneralSecurityException, InvalidKeyException {
        //OpenSSLKey key = new BouncyCastleOpenSSLKey(keyFile.trim());
        
        //hostPrivateKey = key.getPrivateKey();
        //hostCertificate = CertUtil.loadCertificate(certFile.trim());
        // hostPublicKey = hostCertificate.getPublicKey();
    }
    
    public boolean verifyAttributeCertificate(String messageText, String signatureText)
    throws NoSuchAlgorithmException, InvalidKeyException, SignatureException {
        
        byte[] decode = org.globus.util.Base64.decode(signatureText.getBytes());
        Signature sigVerifier = Signature.getInstance(algorithm);
        sigVerifier.initVerify(pubKey);
        sigVerifier.update(messageText.getBytes());
        return sigVerifier.verify(decode);
    }
    
    
    private void loadKeyStore() throws IOException, GeneralSecurityException{
        
        Properties prop = new Properties();
        try{
            prop.load(new FileInputStream(Config.getContextPath()+"authorisation.prop"));
        }
        catch(IOException ioe){
            log.error("Unable to find "+Config.getContextPath()+"authorisation.prop",ioe);
            throw ioe;
        }
        
        
        //String keyStoreFileName = System.getProperty("user.home")+File.separator+".keystore";
        String keyStoreFileName = prop.getProperty("keystore");
        String keyStorePasswd = prop.getProperty("keystore_passwd");
        String keyStoreAlias = prop.getProperty("keystore_alias");
        algorithm = prop.getProperty("algorithm");
        // System.out.println(keyStoreFileName);
        if(keyStoreFileName == null || keyStoreFileName.equals("")) keyStoreFileName = System.getProperty("user.home")+File.separator+".keystore";
        if(keyStorePasswd == null || keyStorePasswd.equals("")) keyStorePasswd = "changeit";
        
        KeyStore keystore = KeyStore.getInstance("JKS");
        keystore.load(new FileInputStream(keyStoreFileName), keyStorePasswd.toCharArray());
        Key key = keystore.getKey(keyStoreAlias, keyStorePasswd.toCharArray());
        if(key == null)throw new GeneralSecurityException("No private key loaded");
        prvKey = (RSAPrivateKey)key;
        java.security.cert.Certificate cert = keystore.getCertificate(keyStoreAlias);
        if(key == null)throw new GeneralSecurityException("No certificate loaded");
        pubKey = (RSAPublicKey)cert.getPublicKey();
        
        // NDB - demo-code
        System.out.println("**********************************************");
        System.out.println("Get authorisation server's private and public keys from its keystore");
        System.out.println("//////////////////////////////////////////////");
        System.out.println( prvKey );
        System.out.println("");
        System.out.println("");
        System.out.println( pubKey );
        System.out.println("");
        System.out.println("");         
    }
    
    public static void main(String args[]) {
        try {
            ACGen acGen = new ACGen();
            
            String messageString = "This is the string to be tested...";
            String signature = acGen.getSignatureString(messageString);
            System.out.println(signature);
            
            System.out.println(acGen.verifyAttributeCertificate(messageString, signature));
        }
        catch(Exception e) {
            log.warn(e);
        }
    }
}

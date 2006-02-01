/*
 * SendCertificate.java
 *
 * Created on 23 June 2004, 01:08
 */

package uk.ac.clrc.dataportal.credentials;
import java.util.*;
import java.net.*;
import javax.xml.namespace.QName;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.client.Service;
import java.io.*;
import org.apache.axis.encoding.*;
import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.*;
import org.gridforum.jgss.*;
import org.ietf.jgss.GSSCredential;

import org.globus.gsi.*;
import org.globus.gsi.bc.*;
import java.security.cert.*;
import java.security.*;

/**
 *
 * @author  glen
 */
public class SendCertificate {
    
    private LinkedHashMap parameters;
    private ArrayList list;
    
    /** Creates a new instance of SendCertificate */
    public SendCertificate() {
        parameters = new LinkedHashMap();
        list = new ArrayList();
        System.out.println("creating sendcertt");
        //do nothing else
    }
    
    public void addParameter(QName type,Object value){
        ParameterBean bean = new ParameterBean();
        bean.setValue(value);
        bean.setType(type);
        list.add(bean);
    }
    
    public Object sendDelegatedCertificate(String url,String method,QName returnType,String sessionid, String proxy) throws Exception{
        // System.out.println("1");
        GlobusCredential cr = null;
        try{
            cr = new GlobusCredential(new ByteArrayInputStream(proxy.getBytes()));
            return  sendDelegatedCertificateImpl( url, method, returnType, sessionid, cr) ;
        }
        catch(Exception r){
            throw r;
        }
        
    }
    
    
    public Object sendDelegatedCertificate(String url,String method,QName returnType,String sessionid, File proxy) throws Exception{
        GlobusCredential cr = null;
        try{
            cr = new GlobusCredential(new FileInputStream(proxy));
            return  sendDelegatedCertificateImpl( url, method, returnType, sessionid, cr) ;
        }
        catch(Exception r){
            throw r;
        }
        
    }
    public Object sendDelegatedCertificate(String url,String method,QName returnType,String sessionid, GSSCredential proxy) throws Exception{
        
        GlobusCredential cr = null;
        try{
            
            cr = ((GlobusGSSCredentialImpl)proxy).getGlobusCredential();
                       
            return  sendDelegatedCertificateImpl( url, method, returnType, sessionid, cr) ;
        }
        catch(Exception r){
            throw r;
        }
    }
    
    public Object sendDelegatedCertificateImpl(String url,String method,QName returnType,String sessionid, GlobusCredential proxy) throws Exception{
        byte[] globuscredential = null;
        
        X509Certificate issuerCert = null;
        try{
            
            //System.out.println("2");
            
           /* File f = new File(System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
            System.out.println(f.exists());
            byte [] data = new byte[(int)f.length()];
            FileInputStream in = new FileInputStream(f);// read in the credential data
            in.read(data);
            in.close();
            ExtendedGSSManager manager = (ExtendedGSSManager)ExtendedGSSManager.getInstance();
            GSSCredential cr2 = manager.createCredential(data, ExtendedGSSCredential.IMPEXP_OPAQUE,    GSSCredential.DEFAULT_LIFETIME,  null,         GSSCredential.INITIATE_AND_ACCEPT);
            
            
            cr = ((GlobusGSSCredentialImpl)cr2).getGlobusCredential();*/
            
            issuerCert = proxy.getIdentityCertificate();
            
            Service service = new Service();
            Call call = (Call)service.createCall();
            call.setTargetEndpointAddress(url);
            call.setOperationName("signRequest");
            call.addParameter("certificate", XMLType.XSD_STRING, ParameterMode.IN);
            call.addParameter("sid", XMLType.XSD_ANYTYPE, ParameterMode.IN);
            
            call.setReturnType(XMLType.XSD_ANYTYPE);
            Object ob[] = {issuerCert.getEncoded() ,sessionid };
            globuscredential = (byte[])call.invoke(ob);
            //System.out.println(globuscredential);
        }
        catch(Exception e){
            System.out.println("signing request" +e);
        }
        
        try {
            // Load issuers private key`.
            
            PrivateKey issuerKey = proxy.getPrivateKey();
            BouncyCastleCertProcessingFactory certFactory = BouncyCastleCertProcessingFactory.getDefault();
            
            // Create delegate certificate
            X509Certificate delegateCert = certFactory.createCertificate(
            new ByteArrayInputStream( globuscredential ),
            issuerCert,
            issuerKey,
            new Long(proxy.getTimeLeft()).intValue(), // seconds ( <=0 same as issuer )
            GSIConstants.DELEGATION_FULL );
            
            System.out.println("Created user dlegation useranme "+delegateCert.getSubjectDN().toString());
            
            Service service = new Service();
            Call call = (Call)service.createCall();
            call.setTargetEndpointAddress(new URL(url));
            call.setOperationName(method);
            
            //   System.out.println("list size is "+list.size());
            Object ob[] = new Object[(list.size()+2)];
            for(int i =0; i < list.size();i++){
                ParameterBean para = (ParameterBean)list.get(i);
                call.addParameter("arg"+String.valueOf(i),para.getType() , ParameterMode.IN);
                ob[i] = para.getValue();
            }
            ob[(list.size())]=sessionid;
            ob[(list.size()+1)] = delegateCert.getEncoded();
            call.addParameter("sid", XMLType.XSD_STRING, ParameterMode.IN);
            call.addParameter("certificate", XMLType.XSD_ANYTYPE, ParameterMode.IN);
            call.setReturnType(returnType);
            
            return (Object)call.invoke(ob);
            
            /*FileWriter e1 = new FileWriter("c:/cred");
            e1.write(cred);
            e1.close();*/
        }
        catch(Exception e) {
            // System.out.println("sending sert" +e);
            e.printStackTrace();
            throw e;
        }
    }
    
}

/*
 * DelegateCredential.java
 *
 * Created on 21 September 2004, 11:08
 */

package uk.ac.clrc.dataportal.credentials;

import java.io.*;
import java.net.*;
import org.globus.gsi.*;
import org.globus.gsi.gssapi.*;
import org.globus.io.urlcopy.*;
import org.globus.util.*;
import org.ietf.jgss.*;
import org.apache.log4j.*;
import java.util.HashMap;
import org.globus.gsi.bc.*;
import org.globus.gsi.*;
import java.security.cert.*;
import java.security.*;
import java.util.*;

import org.quartz.impl.*;
import org.quartz.*;

/**
 *
 * @author  gjd37
 */
//public class DelegateCredential implements Job{

public class DelegateCredential {
    
    static Logger logger = Logger.getLogger(DataTransferModule.class);
    static HashMap map= new HashMap();
    
   /* public DelegateCredential() {
    
        try{
            System.out.println("starting quartz..");
            SchedulerFactory schedFact = new org.quartz.impl.StdSchedulerFactory();
    
            Scheduler sched = schedFact.getScheduler();
            sched.start();
    
            JobDetail jobDetail = new JobDetail("myJob", sched.DEFAULT_GROUP,  DelegateCredential.class);
            // JobDetail jobDetail2 = new JobDetail("myJob2", sched.DEFAULT_GROUP,  Another.class);
    
            long endTime = System.currentTimeMillis() + 6000000L;
    
            SimpleTrigger trigger = new SimpleTrigger("mytrigger", sched.DEFAULT_GROUP, new Date(),null, SimpleTrigger.REPEAT_INDEFINITELY, 10L*1000L);
            // CronTrigger trigger = new CronTrigger("myTrigger", sched.DEFAULT_GROUP,"myJob2",  sched.DEFAULT_GROUP,    "0 * 12 ? * WED*");
    
            sched.scheduleJob(jobDetail, trigger);
        }
        catch(Exception r){
            System.out.println(r);
        }
    
    }*/
    
   /* public void execute(JobExecutionContext context)   throws JobExecutionException {
        //remove all old requests
        // for(int i = 0 ; i < map.size();i++){
    
        //Iterator it = map.values().iterator();
        System.out.println(map.size());
        for(Iterator it = map.values().iterator(); it.hasNext();) {
    
            CertUtilBean b = (CertUtilBean)it.next();
            long g = b.getTime();
            System.out.println("time put in is "+new Date(g)+" time now is "+ new Date());
            if(g < (System.currentTimeMillis() - 10000)){
                System.out.println("removed "+b.getCert().getSubjectDN().toString());
                it.remove();
            }
            System.out.println("\n");
        }
        // }
        System.out.println("size after removal is "+map.size());
    
    }*/
    
    public byte[] signRequest(byte[] x509, String sessionid) throws Exception{
        //logger.info("SigningData request..");
        
        X509Certificate X509Cert = CertUtil.loadCertificate(new ByteArrayInputStream( x509 ));
        
        // java.security.cert.X509Certificate X509Cert = javax.security.cert.X509Certificate.getInstance(x509.getBytes());
        
        // generate new key pair
        KeyPair keyPair = CertUtil.generateKeyPair( "RSA", 512 );
        
        // get instance of Certificate Factory
        BouncyCastleCertProcessingFactory certFactory = BouncyCastleCertProcessingFactory.getDefault();
        
        // Generate Signing Request from recieved cert
        byte [] signingRequest = certFactory.createCertificateRequest( X509Cert, keyPair );
        //logger.info("Created signing resquest.");
        CertUtilBean bean = new CertUtilBean();
        bean.setCert(X509Cert);
        bean.setKeyPair(keyPair);
        bean.setTime();
       
        map.put(sessionid , bean);
        
        
        return signingRequest;
    }
    
    protected GSSCredential getCredential(String sessionid, byte[] cert)throws Exception{
        try{
            // byte[] decode = org.globus.util.Base64.decode(delegateCert.getBytes());
            //PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
            X509Certificate X509DelCert = CertUtil.loadCertificate(new ByteArrayInputStream( cert ));
            
            //get key pair from hashmap
            CertUtilBean bean = (CertUtilBean)map.get(sessionid);
            if(bean == null) throw new Exception("No signing request avaliable for session "+sessionid);
            PrivateKey delegateKey = bean.getKeyPair().getPrivate();
            
            // Create delegate credential
            GlobusCredential delegateCred = new GlobusCredential( delegateKey, new X509Certificate[] { X509DelCert, bean.getCert() } );
            //System.out.println(delegateCred.getIdentity());
            //GlobusCredential noneDelCred = delegateCred.getDefaultCredential();
            // GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(cert.getBytes()));
            GSSCredential credential2 = new GlobusGSSCredentialImpl(delegateCred, GSSCredential.INITIATE_AND_ACCEPT);
       
            map.remove(sessionid);
            return credential2;
        }
        catch(Exception t){
            map.remove(sessionid);
            throw t;
        }
    }
}


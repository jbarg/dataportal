/*
 * ACWebService.java
 *
 * Created on July 18, 2003, 12:59 PM
 */

package uk.ac.cclrc.authorisation.server;

import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;
import java.util.*;
import org.apache.log4j.*;
import uk.ac.cclrc.authorisation.Config;
/**
 * The web service class acts as an interface for external services to request for authorisation token. To request for
 * authorisation token the user has to forward his proxy certificate.
 * @author  asm67
 */
public class ACWebService {
    static Logger log = Logger.getLogger(ACWebService.class);
    /** This method is used to get authorisation Token describing the parameters of the user in XML string
     *@param String proxyCertificateInString proxyCertificate of the user in String
     *@exception java.lang.Exception
     *@return String XML string representation of the Authorizatino token for the user
     */
    public String getAuthorisationTokenInXMLString(String userCert) throws Exception {
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        
        try {
            
            ACServer acServer = new ACServer();
            
            return acServer.getAuthorisationTokenInXML(userCert);
        } catch (Exception e) {
            log.error(e);
            throw e;
        }
    }
    
    /** This method is used to get authorisation Token describing the parameters of the user in XML string
     *@param String proxyCertificateInString proxyCertificate of the user in String
     *@exception java.lang.Exception
     *@return String XML string representation of the Authorizatino token for the user
     */
    public org.w3c.dom.Element getAuthorisationTokenInDOMElement(String userCert) throws Exception {
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        
        try {
            ACServer acServer = new ACServer();
                        /* GlobusCredential globusCredential = new GlobusCredential(new ByteArrayInputStream(userCert.getBytes())
                        GSSCredential credential = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
                        userDn = credential.getName().toString();
                         */
            return acServer.getAuthorisationTokenInDOM(userCert);
        } catch (Exception e) {
            log.error(e);
            throw e;
        }
    }
}

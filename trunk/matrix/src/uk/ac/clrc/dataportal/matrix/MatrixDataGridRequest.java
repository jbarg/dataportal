/*
 * DataGridRequest.java
 *
 * Created on 18 July 2003, 12:02
 *
 * This creates a DataGridRequest object for sending to the Matrix server.
 * Currently a file is created but this will later be changed into a Web Services Client
 * that will just send the XML to the server instead of using the bundled xml sender program.
 *
 * Some parameters MUST be set as specified below as the SDSC Matrix code seems to get some
 * parameters from the wrong places!!  eg. OperationName taken from StepName
 *
 * NOTE: The Java Web Services Development Kit should be used to run this and the Matrix Server too.
 */

package uk.ac.clrc.dataportal.matrix;

import javax.xml.soap.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;

import javax.activation.*;

import java.io.*;
import java.net.URL;
import java.util.Iterator;

import edu.sdsc.matrix.srb.parser.*;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;

import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

// Just for testing....
import java.io.FileOutputStream;

/**
 *
 * @author  Mark Williams
 */

public class MatrixDataGridRequest {
    
    private String username;
    private String password;
    private String domain;
    private String home;
    private String resource;
    private String collection;
    private String host;
    private int port;
    
    private static ObjectFactory objFactory = new ObjectFactory();
    private static JAXBContext jc = null;
    private DataGridRequest dataGridRequest = null;
    
    public MatrixDataGridRequest(String host, int port, String username, String password, String domain, String collection, String resource) {
        
        setHost(host);
        setPort(port);
        setUsername(username);
        setPassword(password);
        setDomain(domain);
        setCollection(collection);
        setResource(resource);

        try {
                dataGridRequest = objFactory.createDataGridRequest();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public void setTransaction(Transaction tx) {
        this.dataGridRequest.setTransaction(tx);
    }
    
    public void setTransactionStatusQuery(TransactionStatusQuery txq) {
        this.dataGridRequest.setTransactionStatusQuery(txq);
    }
    
    public Source getSource() {
        
        ByteArrayInputStream is = null;
        
        try {
            jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
            //Unmarshaller unmarshaller = jc.createUnmarshaller();
            Marshaller marshaller = jc.createMarshaller();
            
            // First create the common elements that exist in all requests
            OrganizationInfo orgInfo = objFactory.createOrganizationInfo();
            orgInfo.setDescription("Data Portal Integration");
            orgInfo.setOrganizationName(this.domain); // THIS MUST BE THE DOMAIN!!!
            
            UserInfo userInfo = objFactory.createUserInfo();
            userInfo.setChallengeResponse(this.password);
            userInfo.setDefaultStorageResource(this.resource);
            userInfo.setEMail("dataportal@rl.ac.uk");
            userInfo.setHomeDirectory(this.home);
            userInfo.setOrganization(orgInfo);
            userInfo.setPhoneNumber(0);
            userInfo.setUserID(this.username);
            
            DocMeta docMeta = objFactory.createDocMeta();
            docMeta.setDocAuthor(this.username);
            docMeta.setDocVersion("1.0");
            docMeta.setDocumentation("Testing Matrix Server for DataPortal");
            
            // Must have this even though it doesn't actually point to anything
            FaultHandlers fh = objFactory.createFaultHandlers();
            fh.setName("FaultHandler");
            fh.setTBA("AnyFaultHandler");
            
            VirtualOrganizationInfo voInfo = objFactory.createVirtualOrganizationInfo();
            voInfo.setServer(this.host);
            voInfo.setPort(this.port);
            
            dataGridRequest.setDocMeta(docMeta);
            dataGridRequest.setGridUser(userInfo);
            //dataGridRequest.setTransaction(listOp.createListOpTransaction()); // Should be done by the client
            dataGridRequest.setVOInfo(voInfo);
            dataGridRequest.setFaultHandler(fh);
            
            // For testing purposes write out the xml file equivalent of our request
            //marshaller.marshal(dataGridRequest, new FileOutputStream("requestfile.xml"));
            
            ByteArrayOutputStream os = new ByteArrayOutputStream();
            marshaller.marshal(dataGridRequest, os);
            is = new ByteArrayInputStream(os.toByteArray());
        }
        catch (JAXBException jbe) {
            System.out.println("JAXB Exception: " + jbe.getMessage());
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        return new StreamSource(is);
    }
    
    /** Getter for property collection.
     * @return Value of property collection.
     */
    public java.lang.String getCollection() {
        return collection;
    }
    
    /** Setter for property collection.
     * @param collection New value of property collection.
     */
    public void setCollection(java.lang.String collection) {
        this.collection = collection;
    }
    
    /** Getter for property username.
     * @return Value of property username.
     */
    public java.lang.String getUsername() {
        return username;
    }
    
    /** Setter for property username.
     * @param username New value of property username.
     */
    public void setUsername(java.lang.String username) {
        this.setHome("/home/" + username + "." + this.domain);
        this.username = username;
    }
    
    /** Getter for property domain.
     * @return Value of property domain.
     */
    public java.lang.String getDomain() {
        return domain;
    }
    
    /** Setter for property domain.
     * @param domain New value of property domain.
     */
    public void setDomain(java.lang.String domain) {
        this.setHome("/home/" + this.username + "." + domain);
        this.domain = domain;
    }
    
    /** Getter for property home.
     * @return Value of property home.
     */
    public java.lang.String getHome() {
        return home;
    }
    
    /** Setter for property home.
     * @param home New value of property home.
     */
    public void setHome(java.lang.String home) {
        this.home = home;
    }
    
    /** Getter for property resource.
     * @return Value of property resource.
     */
    public java.lang.String getResource() {
        return resource;
    }
    
    /** Setter for property resource.
     * @param resource New value of property resource.
     */
    public void setResource(java.lang.String resource) {
        this.resource = resource;
    }
    
    /** Getter for property host.
     * @return Value of property host.
     */
    public java.lang.String getHost() {
        return host;
    }
    
    /** Setter for property host.
     * @param host New value of property host.
     */
    public void setHost(java.lang.String host) {
        this.host = host;
    }
    
    /** Getter for property port.
     * @return Value of property port.
     */
    public int getPort() {
        return port;
    }
    
    /** Setter for property port.
     * @param port New value of property port.
     */
    public void setPort(int port) {
        this.port = port;
    }
    
    /** Getter for property password.
     * @return Value of property password.
     */
    public java.lang.String getPassword() {
        return password;
    }
    
    /** Setter for property password.
     * @param password New value of property password.
     */
    public void setPassword(java.lang.String password) {
        this.password = password;
    }
    
}

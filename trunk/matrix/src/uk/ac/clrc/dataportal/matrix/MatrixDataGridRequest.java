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
    
    private static String USERNAME = "srbadm";                      // SRB Username
    private static String PASSWORD = "srbadm4badc";                 // SRB Password
    public static String DOMAIN = "badc";                           // SRB Domain
    public static String HOME = "/home/" + USERNAME + "." + DOMAIN; // SRB Home collection
    public static String RESOURCE = "badc-escience";                // SRB Default Resource
    public static String COLLECTION = "/home/srbadm.matrix";        // SRB Collection
    private static String HOST = "sapphire.esc.rl.ac.uk";           // SRB Server host
    private static int PORT = 5544;                                 // SRB Server port
    
    private static ObjectFactory objFactory = new ObjectFactory();
    private static JAXBContext jc = null;
    private DataGridRequest dataGridRequest = null;
    
    public MatrixDataGridRequest() {
        try {
            jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
            //Unmarshaller unmarshaller = jc.createUnmarshaller();
            Marshaller marshaller = jc.createMarshaller();
            
            dataGridRequest = objFactory.createDataGridRequest();
            
            //TransactionStatusQuery txStatusQuery = objFactory.createTransactionStatusQuery();
            //Resource res = objFactory.createResource();
            //Container container = objFactory.createContainer();
            
            
            // First create the common elements that exist in all requests
            
            OrganizationInfo orgInfo = objFactory.createOrganizationInfo();
            orgInfo.setDescription("Data Portal Integration");
            orgInfo.setOrganizationName(MatrixDataGridRequest.DOMAIN); // THIS MUST BE THE DOMAIN!!!
            
            UserInfo userInfo = objFactory.createUserInfo();
            userInfo.setChallengeResponse(MatrixDataGridRequest.PASSWORD);
            userInfo.setDefaultStorageResource(MatrixDataGridRequest.RESOURCE);
            userInfo.setEMail("dataportal@rl.ac.uk");
            userInfo.setHomeDirectory(MatrixDataGridRequest.HOME);
            userInfo.setOrganization(orgInfo);
            userInfo.setPhoneNumber(0);
            userInfo.setUserID(MatrixDataGridRequest.USERNAME);
            
            DocMeta docMeta = objFactory.createDocMeta();
            docMeta.setDocAuthor(MatrixDataGridRequest.USERNAME);
            docMeta.setDocVersion("1.0");
            docMeta.setDocumentation("Testing Matrix Server for DataPortal");
            
            // Must have this even though it doesn't actually point to anything
            FaultHandlers fh = objFactory.createFaultHandlers();
            fh.setName("FaultHandler");
            fh.setTBA("AnyFaultHandler");
            
            VirtualOrganizationInfo voInfo = objFactory.createVirtualOrganizationInfo();
            voInfo.setServer(MatrixDataGridRequest.HOST);
            voInfo.setPort(MatrixDataGridRequest.PORT);
            
            dataGridRequest.setDocMeta(docMeta);
            dataGridRequest.setGridUser(userInfo);
            //dataGridRequest.setTransaction(listOp.createListOpTransaction()); // Should be done by the client
            dataGridRequest.setVOInfo(voInfo);
            dataGridRequest.setFaultHandler(fh);
            
            // For testing purposes write out the xml file equivalent of our request
            //marshaller.marshal(dataGridRequest, new FileOutputStream("requestfile.xml"));
            
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
            Marshaller marshaller = jc.createMarshaller();
            
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
    
}

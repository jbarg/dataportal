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

public class MatrixClient {
    
    private static String ENDPOINT = "http://dmgdev1.esc.rl.ac.uk:8080/matrix/receiver";
    private static Unmarshaller unmarshaller;
    private static Marshaller marshaller;
    
    public static void main(String[] args) {
        
        
        try {
            MatrixDataGridRequest request = new MatrixDataGridRequest();
            Transaction tx = null;
            
            // Setup your transaction to perform the operation you want.... should be a parameter
            
            //MatrixDownloadOp downloadOp = new MatrixDownloadOp();
            //tx = downloadOp.createDownloadOpTransaction();
            
            //MatrixListOp listOp = new MatrixListOp();
            //tx = listOp.createListOpTransaction();
            
            // Ingest the file as /home/srbadm.matrix/ingest2.txt (regardless of what the source filename is)
            // The resource used should be changed int the MatrixDataGridRequest object for now but it should be possible to specify it for different file
            // This will have to wait until the next version!
            MatrixIngestOp ingestOp = new MatrixIngestOp();
            tx = ingestOp.createIngestOpTransaction("/home/srbadm.matrix", "ingest2.txt");
            
            // Attach transaction to request
            request.setTransaction(tx);
            
            JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
            unmarshaller = jc.createUnmarshaller();
            
            SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
            SOAPConnection connection = scf.createConnection();
            MessageFactory mf = MessageFactory.newInstance();
            SOAPMessage msg = mf.createMessage();
            SOAPPart soapPart = msg.getSOAPPart();
            
            // Figure out what the transaction was..... use the step name for this as it's set to the op name
            // Note that SDSC may fix this as some point so we might need another way to get the step name!!
            Step step = (Step) tx.getFlow().getSteps().get(0);
            String stepName = step.getStepName();
            
            if (stepName.equals("ingestOp")) {
                // Get a data source, this could be a URLDataSource too or even extended further for possibly GridFTP etc
                FileDataSource ds = new FileDataSource("ingest.txt"); // Source file.  Should be a parameter!
                DataHandler dh = new DataHandler(ds);
                
                // Attach file to be ingested to the SOAP message
                // We can attach multiple files if specified in the DataGridRequest but let's just play with one for now...
                AttachmentPart attPart = msg.createAttachmentPart(dh);
                attPart.setContentId("INGESTFILE"); // Hard coded for now but should be dynamic when ingesting multiple files.  See MatrixIngestOp.java
                msg.addAttachmentPart(attPart);
            }
            
            Source source = request.getSource();
            soapPart.setContent(source);
            
            System.out.println("Sending request to endpoint : " + MatrixClient.ENDPOINT);
            SOAPMessage reply = connection.call(msg, new URL(MatrixClient.ENDPOINT));
           
            // printReply(reply);
            
            // Get the TRANSACTION ID from the reply so we can get the status of our request
            Source replyxml = reply.getSOAPPart().getContent();
            DataGridResponse dgresponse = (DataGridResponse) unmarshaller.unmarshal(replyxml);
            TransactionAcknowledgment txnAck = dgresponse.getTransactionAcknowledgement();
            String txnId = txnAck.getTransactionID();
            
            System.out.println("TRANSACTION ID IS " + txnId);
            
            // Now create another request to get the status.....
            request = new MatrixDataGridRequest();
            MatrixStatusRequest statusRequest = new MatrixStatusRequest();
            request.setTransactionStatusQuery(statusRequest.createTransactionStatusQuery(txnId));
            
            msg = mf.createMessage();
            soapPart = msg.getSOAPPart();
            source = request.getSource();
            soapPart.setContent(source);
            
            System.out.println("Sending status query to endpoint : " + MatrixClient.ENDPOINT);
            reply = connection.call(msg, new URL(MatrixClient.ENDPOINT));
            
            if (stepName.equals("listOp")) {
                System.out.println("List Op Response");
                // We should, in theory,  be able to unmarshal this response in the same way as below but the schema file
                // bundled with their release does not include the extra elements returned by the listOp operation!
                // So just print out the reply for now to show the contents of the Container
                printReply(reply);
            } else if (stepName.equals("ingestOp")) {
                System.out.println("Ingest Op processing.....");
                printReply(reply);
            } else if (stepName.equals("downloadDataSetOp")) {
                
                // Status Codes are in edu/sdsc/matrix/srb/code/MatrixCodes.java
                int statusCode = 9000003; // This is the transaction started (but not finished) status code
                
                // Loop until the transaction has finished
                while ( statusCode == 9000003 ) {
                    
                    replyxml = reply.getSOAPPart().getContent();
                    dgresponse = (DataGridResponse) unmarshaller.unmarshal(replyxml);
                    TransactionStatusResponse txnStatus = dgresponse.getTransactionStatusResponse();
                    
                    // Because we know there was only one step in our transaction we can get the first object in the list
                    // This will need changing when multiple files support is added to our client
                    StepStatusResponse stepStatusResponse = (StepStatusResponse) txnStatus.getFlowStatusResponse().getStepStatusResponse().get(0);
                    statusCode = stepStatusResponse.getStatusCode();
                    
                    System.out.println("STATUS CODE IS " + statusCode + "\n");
                }
                
                System.out.println("Received " + reply.countAttachments() + " attachments:");
                Iterator it = reply.getAttachments();
                while(it.hasNext()) {
                    AttachmentPart attachment = (AttachmentPart)it.next();
                    System.out.println(attachment.getContentId());
                    DataHandler dh = attachment.getDataHandler();
                    FileOutputStream fout = new FileOutputStream(attachment.getContentId());
                    dh.writeTo(fout);
                    fout.close();
                }
            } else {
                System.out.println("Unknown step- response not processed.");
            }
            
            connection.close();
            
        } catch(Exception e) {
            e.printStackTrace();
        }
        
    }
    
    private static void printReply(SOAPMessage reply) throws Exception {
        TransformerFactory tFact = TransformerFactory.newInstance();
        Transformer transformer = tFact.newTransformer();
        Source src = reply.getSOAPPart().getContent();
        StreamResult result = new StreamResult( System.out );
        transformer.transform(src, result);
        System.out.println();
    }
    
}

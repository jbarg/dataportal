/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */



// default package

import java.io.*;
import javax.xml.soap.*;
import java.net.URL;
import javax.mail.internet.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import javax.activation.*;
import org.dom4j.*;
import java.util.*;
import edu.sdsc.matrix.srb.parser.*;
import javax.xml.bind.Unmarshaller;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;

// A StandAlone JAXM Client that sends a local XML file as a request

public class SRBJaxmSender {

    private static final String PROPERTIES_FILE = "examples.properties";
    private static final String SERVER_URL_PROPERTY = "server.url";
    private static final String REQUEST_FILE_PROPERTY = "request.file";
    private static final String ATTACHMENT_FILE_PROPERTY = "attachment.file";
    private static final String ATTACHMENT_ID_PROPERTY = "attachment.id";
    
	private static Unmarshaller unmarshaller;

    public static void main(String args[]) {

        try {
	    Properties prop = new Properties();
		System.out.println("PROPERTIES_FILE :"+PROPERTIES_FILE);
	    prop.load(new FileInputStream(PROPERTIES_FILE));
		System.out.println("PROPERTIES_FILE LOADED");
	    String url = prop.getProperty(SERVER_URL_PROPERTY);
            URL endpoint = new URL(url);

	    // create request
            SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
            SOAPConnection connection = scf.createConnection();
            MessageFactory mf = MessageFactory.newInstance();
            SOAPMessage msg = mf.createMessage();
            SOAPPart soapPart = msg.getSOAPPart();
            String requestFile = prop.getProperty(REQUEST_FILE_PROPERTY);
	    Source source = new StreamSource(new FileInputStream(requestFile));

            soapPart.setContent(source);

            // add an attachment
	    if(prop.getProperty(ATTACHMENT_FILE_PROPERTY) != null) {
		String attachmentFile = prop.getProperty(ATTACHMENT_FILE_PROPERTY);
		String attachmentID = prop.getProperty(ATTACHMENT_ID_PROPERTY);
                System.out.println("adding an attachment...");
                FileDataSource ds = new FileDataSource(attachmentFile);
                DataHandler dh = new DataHandler(ds);
                AttachmentPart attachment = msg.createAttachmentPart(dh);
                attachment.setContentId(attachmentID);
                msg.addAttachmentPart(attachment);
            }

	    // send out request
            System.out.println("Sending message to URL: " + endpoint);
			printReply(msg);
            SOAPMessage reply = connection.call(msg, endpoint);
            System.out.println("Received reply from: " + endpoint);

	    printReply(reply);
	    
		JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
		unmarshaller = jc.createUnmarshaller();
		Source replyxml = reply.getSOAPPart().getContent();
		DataGridResponse dgresponse = (DataGridResponse) unmarshaller.unmarshal(replyxml);
		TransactionAcknowledgment txnAck = dgresponse.getTransactionAcknowledgement();
		String txnId = txnAck.getTransactionID();
		System.out.println("\n\nYour Transaction ID : " + txnId+"\n");
	    
	    connection.close();

        } catch(FileNotFoundException e) {
	    System.err.print("Properties file jaxmsender.properties not found. ");
	    System.err.println("Please put jaxmsender.properties into the same directory as SRBJaxmSender.class");
	}
	catch(Exception e) {
            e.printStackTrace();
        }
    }

    private static void printReply(SOAPMessage reply) throws Exception {
	System.out.println("Result:");
        TransformerFactory tFact = TransformerFactory.newInstance();
        Transformer transformer = tFact.newTransformer();
	Source src = reply.getSOAPPart().getContent();
	StreamResult result = new StreamResult( System.out );
	transformer.transform(src, result);
	System.out.println();
    }
    
	private static void printReply(SOAPMessage reply,String outputFileName) throws Exception {
	
		   TransformerFactory tFact = TransformerFactory.newInstance();
		   Transformer transformer = tFact.newTransformer();
	   Source src = reply.getSOAPPart().getContent();
	   FileOutputStream tempReplyOS = new FileOutputStream(outputFileName);
	   StreamResult result1 = new StreamResult( System.out );
	   StreamResult result2 = new StreamResult( tempReplyOS );
	   transformer.transform(src, result1);
	   transformer.transform(src, result2);
	   System.out.println();
	   }
}



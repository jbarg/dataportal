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
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import edu.sdsc.matrix.srb.util.JaxbUtil;
import java.util.Iterator;
import java.util.Properties;
import edu.sdsc.matrix.srb.parser.*;

// A StandAlone JAXM Client that sends status requests

public class SRBJaxmStatusSender {

    private static final String PROPERTIES_FILE = "examples.properties";
    private static final String SERVER_URL_PROPERTY = "server.url";
    private static ObjectFactory objFactory = new ObjectFactory();
    private static BufferedReader console = new BufferedReader(new InputStreamReader(System.in));

    public static void main(String args[]) {
	
        try {
	    Properties prop = new Properties();
	    prop.load(new FileInputStream(PROPERTIES_FILE));
	    String url = prop.getProperty(SERVER_URL_PROPERTY);
            URL endpoint = new URL(url);

	    // create request
            SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
            SOAPConnection connection = scf.createConnection();
            MessageFactory mf = MessageFactory.newInstance();
            SOAPMessage msg = mf.createMessage();
            SOAPPart soapPart = msg.getSOAPPart();

	    String transactionID = getStringInput("What transaction ID do you want to query for? ");
	    Source source = createTransactionStatusQuery(transactionID);

            soapPart.setContent(source);

	    // send out request
            System.out.println("Sending message to URL: " + endpoint);
			System.out.println("\nRequest:");
			printReply(msg,"statusRequest.xml");
            SOAPMessage reply = connection.call(msg, endpoint);
            System.out.println("Received reply from: " + endpoint);

		System.out.println("\nResult:");
	    printReply(reply,"statusResult.xml");

	    System.out.println("--------");
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

	    connection.close();

        } catch(Exception e) {
            e.printStackTrace();
	}
    }

    private static void printReply(SOAPMessage reply,String fileName) throws Exception {

        TransformerFactory tFact = TransformerFactory.newInstance();
        Transformer transformer = tFact.newTransformer();
	Source src = reply.getSOAPPart().getContent();
	//StreamResult result = new StreamResult( System.out );
	FileOutputStream tempReplyOS = new FileOutputStream(fileName);
		StreamResult result1 = new StreamResult( System.out );
		StreamResult result2 = new StreamResult( tempReplyOS );
	transformer.transform(src, result1);
	transformer.transform(src, result2);
	System.out.println();
    }
    
    public static String getStringInput(String prompt) {
	System.out.print(prompt);
	System.out.flush();
	do{
	    try {
		String input = console.readLine().trim();
		System.out.println();
		System.out.flush();
		if (input.length() < 1) {
		    System.out.println("Please enter a nonempty string");
		}
		else return input;
	    }
	    catch(IOException e) {
		// do nothing
	    }
	} while(true);

    }
    
	public static int getIntInput ( int min, int max) {
	   int returnValue = 0;
	   do {
		   try{
		   System.out.flush();
		   String input = console.readLine();
		   returnValue = Integer.parseInt(input.trim());
		   if ((returnValue < min) || (returnValue > max)) throw new NumberFormatException("Out of desired input range");
		   else return returnValue;
		   }catch(Exception e){	    	
		   System.out.println("Enter a number between " + min +" and " + max);
		   }
	   } while(true);
	   }

    private static Source createTransactionStatusQuery(String transactionID) 
	throws JAXBException {
	JAXBContext jc = JAXBContext.
	    newInstance("edu.sdsc.matrix.srb.parser");
	Marshaller marshaller = jc.createMarshaller();

	DataGridRequest request = objFactory.createDataGridRequest();
	request.setDocMeta(JaxbUtil.createDocMeta());
	request.setGridUser(JaxbUtil.createUserInfo());
	request.setVOInfo(JaxbUtil.createVirtualOrganizationInfo());
	request.setTerminationHandler(JaxbUtil.createTerminationHandlers());

	TransactionStatusQuery tsq = objFactory.createTransactionStatusQuery();
	
	System.out.print("Enter number of flows for transaction:");
	int flowCount = getIntInput (0,10);
	
	for(int i=0;i<flowCount;i++)
	{ 
		String flowID = getStringInput("Enter flow ID: ");
		FlowStatusQuery fsq = objFactory.createFlowStatusQuery();
		fsq.setFlowID(flowID);
		
		// For Steps
		System.out.print("Enter number of steps for flow id "+flowID+":");
		int stepCount = getIntInput (0,10);
		for(int j=0;j<stepCount;j++)
		{ 
			String stepID = getStringInput("Enter Step ID: ");
			StepStatusQuery ssq = objFactory.createStepStatusQuery();
			ssq.setStepID(stepID);
			
			ssq.setQueryType("simple");
						
			fsq.getStepStatusQuery().add(ssq);
		}
		
		tsq.getFlowStatusQuery().add(fsq);
	}	
		
	tsq.setTransactionID(transactionID);
	//tsq.setFlowStatusQuery(fsq);
	request.setTransactionStatusQuery(tsq);

	ByteArrayOutputStream os = new ByteArrayOutputStream();
	marshaller.marshal(request, os);
	ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
	return new StreamSource(is);
    }

}




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
import javax.xml.bind.Unmarshaller;
import edu.sdsc.matrix.srb.util.JaxbUtil;
import edu.sdsc.matrix.srb.parser.*;
import java.util.Properties;



// A StandAlone JAXM Client that sends transactions

public class SRBJaxmOpSender {

    private static final String PROPERTIES_FILE = "examples.properties";
    private static final String SERVER_URL_PROPERTY = "server.url";
    private static ObjectFactory objFactory = new ObjectFactory();
    private static BufferedReader console = new BufferedReader(new InputStreamReader(System.in));
    
	private static Unmarshaller unmarshaller;

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

	    System.out.println("M A T R I X  Alpha demo");

	    //System.out.println("Please enter the URL of a Matrix server to connect to: [" + DEFAULT_SERVICE_URI + "]");
	    //String endpoint = getStringInputAllowEmpty();

	    String[] options = {"default", "enter my own"};
	    String choice = getChoice("Do you want to use the default SRB values (username, password, etc.) or enter your own?", options);

	    Source source = null;
	    if(choice.equals("default")) {
		source = createTransaction(); // use defaults
	    }
	    else {
		UserInfo ui = doGetUserInfo();
		VirtualOrganizationInfo voi = doGetVirtualOrganizationInfo();
		source = createTransaction(ui, voi);
	    }

	    soapPart.setContent(source);

	    // send out request
            System.out.println("Sending message to URL: " + endpoint);
			System.out.println("\nRequest:");
			printReply(msg,"request.xml");
            SOAPMessage reply = connection.call(msg, endpoint);
            System.out.println("Received reply from: " + endpoint);

		System.out.println("\nReply:");
	    printReply(reply,"reply.xml");
	    
		JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
		unmarshaller = jc.createUnmarshaller();
		Source replyxml = reply.getSOAPPart().getContent();
		DataGridResponse dgresponse = (DataGridResponse) unmarshaller.unmarshal(replyxml);
		TransactionAcknowledgment txnAck = dgresponse.getTransactionAcknowledgement();
		String txnId = txnAck.getTransactionID();
		
		System.out.println("\n\nYour Transaction ID : " + txnId+"\n");
		
	    
	    connection.close();

        } catch(Exception e) {
            e.printStackTrace();
	}
    }
    
    public static Operation doCreate() throws JAXBException {
	String[] options = {"container", "collection"};
	String choice = getChoice("Do you want to create a: ", options);
	String name = getStringInput("Please type the new " + choice + " name: ");

	if(choice.equals("container")) return JaxbUtil.createCreateOpForContainer(name);
	else return JaxbUtil.createCreateOpForCollection(name);

    }
    
	public static Operation doIngestMetaData() throws JAXBException {
	   String[] options = {"container", "collection","data set"};
	   String choice = getChoice("Do you want to create metadata for : ", options);
	   String name = getStringInput("Please type the " + choice + " name: ");

	   //if(choice.equals("container")) return JaxbUtil.createCreateOpForContainer(name);
	   //else return JaxbUtil.createCreateOpForCollection(name);
	   
	   Operation operation  = 	objFactory.createOperation();
	   IngestMetadataOp ingestOp	 = 	 objFactory.createIngestMetadataOp();
	   IngestMetadataParams ingestParams = objFactory.createIngestMetadataParams();
	   
	   MetaData metaDataForIngest = objFactory.createMetaData();
	   
	   SystemMetaData sysMetaData = objFactory.createSystemMetaData(); 
	   DatasetSystemMetaData dataSysMetaData = objFactory.createDatasetSystemMetaData() ;
	   AnyType anyType = objFactory.createAnyType(); 
	   anyType.getContent().add("metadata fields");	
	   //dataSysMetaData.setIsDirty("DIRTY");
	   //dataSysMetaData.setOffset(5);
	   dataSysMetaData.setSize(1107);
	   sysMetaData.setDataSetSysMeta(dataSysMetaData);
	   metaDataForIngest.setSystemMeta(sysMetaData);
	   
	   
	   // User Defined Pairs
	   AVUpair userDef1 = objFactory.createAVUpair();
	   userDef1.setAttribute("Attribute3");
	   userDef1.setValue("Attr3Value");
	   userDef1.setUnit("Attr3Unit");
	   
	   AVUpair userDef2 = objFactory.createAVUpair();
			  userDef2.setAttribute("Attribute3");
			  userDef2.setValue("Attr3Value");
			  userDef2.setUnit("Attr3Unit");
			  
		metaDataForIngest.getAVUPairs().add(userDef1);
		metaDataForIngest.getAVUPairs().add(userDef2);  
	  
	   
	   ingestParams.setMetadata(metaDataForIngest); 
	   
		StdDatagridObject stdDatagridObject = createStdDatagridObjectForCollection(name);
		ingestParams.setDataGridObject(stdDatagridObject);
		ingestOp.setStdParams(ingestParams);
		operation.setIngestMetadataOp(ingestOp);
	    return(operation);
	   

	   }
    
	public static Operation doPrepareTicket() throws JAXBException {
		UserInfo ticketUser = doGetUserInfo();
		VirtualOrganizationInfo ticketVOInfo = doGetVirtualOrganizationInfo();
		
		return JaxbUtil.createPrepareTicketOp(ticketUser,ticketVOInfo);

		}
		
	public static Operation doMetadataQuery() throws JAXBException {
			
			String queryString = getStringInput("Please type the query : ");
			return JaxbUtil.createMetatdataQueryOp(queryString);

			}	
    
 
    public static Operation doRename() throws JAXBException {
	String[] options = {"collection", "dataset"};
	String choice = getChoice("Do you want to rename a: ", options);
	
	String oldname = getStringInput("Old " + choice + " name: ");
	String newname = getStringInput("New " + choice + " name: ");
	
	if(choice.equals("collection")) {
	    return JaxbUtil.createRenameOpForCollection(oldname, newname);
	}
	else {
	    String logicalLocation = getStringInput("Logical Location: ");
	    return JaxbUtil.createRenameOpForDataSet(oldname, logicalLocation, newname);
	}
    }

    public static Operation doDelete() throws JAXBException {
	String[] options = {"collection", "dataset"};
	String choice = getChoice("Do you want to delete a: ", options);
	String name = getStringInput(choice + " name: ");
	
	if(choice.equals("collection")){
	    return JaxbUtil.createDeleteOpForCollection(name);
	}
	else {
	    String logicalLocation = getStringInput("Logical Location: ");
	    return JaxbUtil.createDeleteOpForDataSet(name, logicalLocation);
	}
    }

    public static void printOptions(String[] options) {
	for(int i=0; i<options.length; i++){
	    System.out.println("[" + (i+1) + "] " + options[i]);
	}
	System.out.println();
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

    public static String getStringInputAllowEmpty() {
	try {
	    return console.readLine().trim();
	}
	catch(IOException e) {
	    return "";
	}
    }

    public static String getStringInput(String prompt) {
	System.out.print(prompt);
	do{
	    try {
		String input = console.readLine().trim();
		System.out.println();
		if (input.length() < 1) {
		    System.out.println("Please enter a nonempty string");
		}
		else return input;;
	    }
	    catch(IOException e) {
		// do nothing
	    }
	} while(true);

    }

    public static String getChoice(String prompt, String[] options) {
	System.out.println(prompt);
	printOptions(options);
	System.out.print("Your choice: ");
	int choice = getIntInput(1, options.length);
	System.out.println();
	return options[choice-1];
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

    private static Source createTransaction() throws JAXBException {
	return createTransaction(JaxbUtil.createUserInfo(),
				 JaxbUtil.createVirtualOrganizationInfo());
       
    }

    private static Source createTransaction(UserInfo ui, VirtualOrganizationInfo voi)
	throws JAXBException {

	String[] options = {"createOp", "renameOp", "deleteOp","prepareTicketOp","metadataQuery","ingestMetaData"};
	String[] flowOptions = {"sequential", "parallel"};
	//String stepName = getChoice("Please enter operation you want to execute: ", options);

	JAXBContext jc = JAXBContext.
	    newInstance("edu.sdsc.matrix.srb.parser");
	Marshaller marshaller = jc.createMarshaller();

	DataGridRequest request = objFactory.createDataGridRequest();
	request.setDocMeta(JaxbUtil.createDocMeta());
	request.setGridUser(ui);
	
	// Test Code for Query
	voi.setPort(7573);
	ui.setDefaultStorageResource("hpss-sdsc");
	//ui.setDefaultStorageResource("test-unix");
	
	request.setVOInfo(voi);
	request.setTerminationHandler(JaxbUtil.createTerminationHandlers());

	Transaction trans = objFactory.createTransaction();
	
	System.out.print("Enter the number of flows:");
	int flowCount = getIntInput ( 1,3);
	
	for(int flowIndex=0;flowIndex<flowCount;flowIndex++)
	{
		String flowType = getChoice("Please enter type of flow: ", flowOptions);
		String stepName = getChoice("Please enter operation you want to execute: ", options);
		Flow flow = objFactory.createFlow();
		//flow.setFlowType("sequential");
		flow.setFlowType(flowType);
		//trans.setFlow(flow);
		Step step = objFactory.createStep();
	
		Operation op = null;
		if(stepName.equals("createOp")) {
		    op = doCreate();
		}
		else if(stepName.equals("deleteOp")) {
		    op = doDelete();
		}
		else if(stepName.equals("renameOp")) {
		    op = doRename();
		}
		else if(stepName.equals("prepareTicketOp")) {
			op = doPrepareTicket();
		}
		else if(stepName.equals("metadataQuery")) {
			op = doMetadataQuery();
		}
		else if(stepName.equals("ingestMetaData")) {
					op = doIngestMetaData();
				}
	
		step.setOperation(op);
		//step.setStepName(stepName);
		step.setStepID("Step-ID-0");
		flow.getSteps().add(step);
	
		// To Support Multiple Flows
		trans.getFlow().add(flow);
	}
	request.setTransaction(trans);
	
	ByteArrayOutputStream os = new ByteArrayOutputStream();
	
	marshaller.marshal(request, os);
	ByteArrayInputStream is = new ByteArrayInputStream(os.toByteArray());
	return new StreamSource(is);
    }
    

	private static UserInfo doGetUserInfo() throws JAXBException {
	    while (true){
			String[] options = {"Ticket","User Login"};
			String userType = getChoice("Please enter type of User Info: ", options);	
			if(userType.equals("User Login"))
			{
			
				String username = getStringInput("Enter SRB User Name: ");
				String password = getStringInput("Enter password: ");
				String homeDirectory = getStringInput("Enter home directory: ");
				String defaultStorageResource = getStringInput("Enter default storage resource: ");
				String organization = getStringInput("Enter organization: ");
		
				System.out.println("The srb datagrid user information to be used for the demo:");
				System.out.println("UserName = " + username);
				System.out.println("Password = " + password);
				System.out.println("Home Directory = " + homeDirectory);
				System.out.println("Default Storage Resource = " + defaultStorageResource);
				System.out.println("Organization = " + organization);
		
				String proceed = getStringInput("If the user information is correct, Enter Y [Y]");
				if (proceed.startsWith("y") || proceed.startsWith("Y")) {
				    return JaxbUtil.createUserInfo(username, 
								   password, 
								   homeDirectory, 
								   defaultStorageResource, 
								   organization);
				}
			}
			else if(userType.equals("Ticket"))
			{
				String ticketId = getStringInput("Enter Ticket ID: ");
				return JaxbUtil.createUserInfoWithTicket(ticketId);
			}
	    }
	}

	private static VirtualOrganizationInfo doGetVirtualOrganizationInfo() throws JAXBException {

	    String domainserver = getStringInput("Enter SRB domain server: ");

	    System.out.print("Enter port: ");
	    int port = getIntInput(1024, 65535);
	    System.out.println();

	    return JaxbUtil.createVirtualOrganizationInfo(domainserver, port);
	}
	
	public static StdDatagridObject createStdDatagridObjectForCollection(String collectionName)
		throws JAXBException
		{
			Collection collection = 	objFactory.createCollection();
			Collection.StdCollectionType stdCollectionType = objFactory.createCollectionStdCollectionType();
			StdDatagridObject stdDatagridObject = objFactory.createStdDatagridObject();
		
			stdCollectionType.setCollectionName(collectionName);
			collection.setStdCollection(stdCollectionType);
			stdDatagridObject.setCollectionNow(collection);
		
			return(stdDatagridObject);
		}
	
	
    }




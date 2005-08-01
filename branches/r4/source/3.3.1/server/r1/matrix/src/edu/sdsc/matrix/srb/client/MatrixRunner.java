/*
		* 	SDSC Matrix. A Gridflow Management System for data grids and digtal libraries
	*  Copyright (C) 2004 SDSC Matrix Project
	*
	* This library is free software; you can redistribute it and/or
	* modify it under the terms of the GNU Lesser General Public
	* License as published by the Free Software Foundation; either
	* version 2.1 of the License, or (at your option) any later version.
	*
	* This library is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	*	Lesser General Public License for more details.
	*
	*	You should have received a copy of the GNU Lesser General Public
	*	License along with this library; if not, write to the Free Software
	*	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	*
	* ====================================================================
	*
	* This software consists of voluntary contributions from the developers of the SDSC
	* Matrix Project. We request that all redistribution and software made using this code
	* acknowledge their use of the “SDSC Matrix project” in their end-user documentation or
	* in their website.
	*
	* SDSC Matrix Project (list of developers as of Dec 2005)
	*	Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
	* 9500 Gilman Drive, MC0505
	* San Diego Supercomputer Center
	* University of California
	* La Jolla CA 92093
	*
	* Allen Ding (alding@sdsc.edu)
	* Lucas Gilbert (iktome@sdsc.edu)
	* Arun Jagatheesan (arun@sdsc.edu)
	* Reena Mathew	(rmathew@sdsc.edu)
	* Daniel Moore (mixx@umail.ucsb.edu)
	* Erik Vandekieft (evk@sdsc.edu)
	* Jonathan Weinberg (jonw@sdsc.edu)
	*
	*/

package edu.sdsc.matrix.srb.client;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.URL;
import java.util.Iterator;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.soap.MessageFactory;
import javax.xml.soap.SOAPConnection;
import javax.xml.soap.SOAPConnectionFactory;
import javax.xml.soap.SOAPException;
import javax.xml.soap.SOAPMessage;
import javax.xml.soap.SOAPPart;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.apache.commons.cli.*;

/**
	* <p>Title: MatrixRunner </p>
	* <p>Description: An utility class that demonstrates running a datagrid request and reading
	*  the data grid response from the Matrix server. Can deal with asynchronous and synchronous requests.
		* Can also login using using both gridTickets and full loging information. It is
	* the assumption that the user will need only one instance of this class and all methods
	* are static - basing the assumption on the hypotheis that an user will be operating on only
	* one data grid at a time. But, in very rare scenaorios this might not be true when a single
	* application will have to connect as multiple users at the same time </p>
	* <p>Copyright (c) 2003</p>
	* <p>Company: SDSC </p>
	* @author Jon WeinBerg, Arun Jagatheesan
	* @version 1.0
	*/

public class MatrixRunner{

		private static Marshaller marshaller;
		public static String DEFAULT_USER_ID = "Matrix-demo";
		public static String DEFAULT_PASSWORD = "******";
		public static String DEFAULT_HOMEDIR = "/home/Matrix-demo.sdsc";
		public static String DEFAULT_ORGANIZATION = "sdsc";
		public static String DEFAULT_SRB_SERVER = "orion.sdsc.edu";
		public static String DEFAULT_DEFAULT_STORAGE_RESOURCE =
			"sfs-tape2-dtf";
		public static int DEFAULT_SRB_PORT = 7824;
		public static String DEFAULT_DGL_NAME = "lastRequest.dgl";
		public static String DEFAULT_SERVER =
			"http://appfarm1.sdsc.edu:8080/matrix/receiver";
		private static boolean changeDefaultsBasedOnInput = true;
		public static String DEFAULT_ticketID = "enterRealTicketNumberHere";
		public static CommandLine cmd = null;

		static{
				try{
						JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
						marshaller = jc.createMarshaller();

				} catch (Exception e){
						e.printStackTrace();
				}
		}

// ================== getter and setter for default login params ======================//
		public static UserInfo getDefaultUserInfo(){
				UserInfo ui = null;
				// if valid ticketID use it or else use the full information
				if ((DEFAULT_ticketID == null) || (DEFAULT_ticketID.trim().equals("")) ||
					(DEFAULT_ticketID.equalsIgnoreCase("enterRealTicketNumberHere"))){
						ui = new UserInfo(DEFAULT_USER_ID,
							new OrganizationInfo(DEFAULT_ORGANIZATION));
						ui.setChallengeResponse(DEFAULT_PASSWORD); //optional in Matrix DGR but needed for SRB
						ui.setHomeDirectory(DEFAULT_HOMEDIR); //optional in Matrix DGR but needed for SRB
						ui.setDefaultStorageResource(DEFAULT_DEFAULT_STORAGE_RESOURCE); // the resource to store data
				} else{
						ui = new UserInfo(DEFAULT_ticketID); // ticketID of earlier session
				}
				return ui;
		}

		public static void setDefaultUserInfo(UserInfo ui){
				if (ui.getGridTicket() == null){
						DEFAULT_ticketID = null;
						DEFAULT_USER_ID = ui.getUserID();
						DEFAULT_ORGANIZATION = ui.getOrganization().getOrganizationName();
						DEFAULT_PASSWORD = ui.getChallengeResponse();
						DEFAULT_HOMEDIR = ui.getHomeDirectory();
						DEFAULT_DEFAULT_STORAGE_RESOURCE = ui.getDefaultStorageResource();
				} else{
						// theoretically no null pointer exception should take place in all the gets...
						DEFAULT_ticketID = ui.getGridTicket().getTicketID().getTicketString();
				}
		}

		public static VirtualOrganizationInfo getDefaultVO(){
				return new VirtualOrganizationInfo(DEFAULT_SRB_SERVER, DEFAULT_SRB_PORT);
		}

		public static void setDefaultVO(VirtualOrganizationInfo vio){
				DEFAULT_SRB_SERVER = vio.getServer();
				DEFAULT_SRB_PORT = vio.getPort();
		}

		/**
				* Receive the SRB Mdas Env details from the command line and set the defaults
			* @param args
			* @return exit
			*/
		public static boolean processCommandLine(String className, String args[]) throws ParseException{
				return processCommandLine(className, args, false);
		}

		public static boolean processCommandLine(String className, String args[],
			boolean askForInput) throws ParseException{
				return processCommandLine(className, args, askForInput, new OptionGroup());
		}

		public static boolean processCommandLine(String className, String args[],
			boolean askForInput, OptionGroup moreOptions) throws ParseException{
				Options options = new Options();
				options.addOption("u", "user", true, "User ID");
				options.addOption("o", "org", true, "Organization or domain");
				options.addOption("p", "pass", true,
					"Password (WARN:will be echoed on screen)");
				options.addOption("s", "server", true, "SRB Server address");
				options.addOption("P", "port", true, "SRB port number");
				options.addOption("c", "collection", true, "Home collection to login to");
				options.addOption("r", "resource", true, "Default Resource");
				options.addOption("f", "file", true, "DGL file name to save to");
				options.addOption("m", "matrix", true, "Matrix Server address");
				options.addOption("t", "ticket", true,
					"Matrix Ticket-ID to use to login (will cancel other login info)");
				options.addOption("i", "interactive", false, "Interactive input processing");
				options.addOptionGroup(moreOptions);
				options.addOption("h", "help", false, "This help message");
				// command line parsing
				CommandLineParser parser = new PosixParser();
				cmd = parser.parse(options, args);
				// command line process
				if (cmd.hasOption('h')){
						HelpFormatter formatter = new HelpFormatter();
						formatter.printHelp("java " + className + "[uopsPcrfmtih]", options);
						return false;
				} else{
						if (cmd.hasOption('u')){
								DEFAULT_USER_ID = cmd.getOptionValue("u");
						}
						if (cmd.hasOption('o')){
								DEFAULT_ORGANIZATION = cmd.getOptionValue("o");
						}
						if (cmd.hasOption('p')){
								DEFAULT_PASSWORD = cmd.getOptionValue("p");
						}
						if (cmd.hasOption('s')){
								DEFAULT_SRB_SERVER = cmd.getOptionValue("s");
						}
						if (cmd.hasOption('P')){
								DEFAULT_SRB_PORT = Integer.parseInt(cmd.getOptionValue("P"));
						}
						if (cmd.hasOption('c')){
								DEFAULT_HOMEDIR = cmd.getOptionValue("c");
						}
						if (cmd.hasOption('r')){
								DEFAULT_DEFAULT_STORAGE_RESOURCE = cmd.getOptionValue("r");
						}
						if (cmd.hasOption('f')){
								DEFAULT_DGL_NAME = cmd.getOptionValue("f");
						}
						if (cmd.hasOption('m')){
								DEFAULT_SERVER = cmd.getOptionValue("m");
						}
						if (cmd.hasOption('t')){
								DEFAULT_ticketID = cmd.getOptionValue("t");
						}
						if (cmd.hasOption('i') || askForInput){
								askForInput();
						}
						return true;
				}
		}

		public static String getCmdLineValue(char key){
				return cmd.getOptionValue(key);
		}

		public static boolean hasOptionInCmdLine(char key){
				return cmd.hasOption(key);
		}

//================== methods to execute a flow ===========================//

		/**
			* Run the flow and ask for input verification from the user
			* @param flow Flow
			* @deprecated (Use startFlow with both async and sync options)
			*/
		public static String runFlow(Flow flow){
				return runFlow(flow, true);
		}

		/**
			* Run the given flow and optionally bypass the user input part
			* @param flow Flow
			* @param askForInput boolean
			* @deprecated (Use startFlow with both async and sync options)
			*/
		public static String runFlow(Flow flow, boolean askForInput){
				String matrixServer = DEFAULT_SERVER;
				return runFlow(flow, matrixServer, askForInput);
		}

		/**
				* Run this flow and go to a specific server.  Except for the server, the other
			* parameter values will be the default ones and the user will not be asked
			* for verification
			* @param flow Flow
			* @param server String
			* @deprecated (Use startFlow with both async and sync options)
			*/
		public static String runFlow(Flow flow, String server){
				return runFlow(flow, server, false);
		}

		private static void askForInput(){
				boolean askForInput = true;
				BufferedReader console = new BufferedReader(new InputStreamReader(System.
					in));
				String temp;
				if (askForInput){
						try{
								System.out.print("userid (" + DEFAULT_USER_ID + ") : ");
								DEFAULT_USER_ID = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_USER_ID :
									temp;
								System.out.print("password (" + DEFAULT_PASSWORD + ") : ");
								DEFAULT_PASSWORD = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_PASSWORD :
									temp;
								System.out.print("homedir (" + DEFAULT_HOMEDIR + ") : ");
								DEFAULT_HOMEDIR = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_HOMEDIR :
									temp;
								System.out.print("organization (" + DEFAULT_ORGANIZATION + ") : ");
								DEFAULT_ORGANIZATION = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_ORGANIZATION : temp;
								System.out.print("srbserver (" + DEFAULT_SRB_SERVER + ") : ");
								DEFAULT_SRB_SERVER = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_SRB_SERVER :
									temp;
								System.out.print("defaultStorageResource (" +
									DEFAULT_DEFAULT_STORAGE_RESOURCE + ") : ");
								DEFAULT_DEFAULT_STORAGE_RESOURCE = ((temp = console.readLine()).length() ==
									0) ?
									DEFAULT_DEFAULT_STORAGE_RESOURCE : temp;
								System.out.print("srbport (" + DEFAULT_SRB_PORT + ") : ");
								DEFAULT_SRB_PORT = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_SRB_PORT :
									Integer.parseInt(temp);
								System.out.print("matrixserver (" + DEFAULT_SERVER + ") : ");
								DEFAULT_SERVER = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_SERVER : temp;
								System.out.print("DGL File Name (" + DEFAULT_DGL_NAME + ") : ");
								DEFAULT_DGL_NAME = ((temp = console.readLine()).length() == 0) ?
									DEFAULT_DGL_NAME :
									temp;
								System.out.println("\n");
						} catch (IOException ioe){
								ioe.printStackTrace();
						}
				}
		}

		/**
			* Run this flow and go to a specific server.
			* @param flow Flow
			* @param server String
			* @param askForInput Whether to have Interactive prompts for input
			* @deprecated (Use startFlow with both async and sync options)
			*/
		private static String runFlow(Flow flow, String matrixServer,
			boolean askForInput){
				DEFAULT_SERVER = matrixServer;
				if (askForInput)
						askForInput();
				return executeFlow(flow, DEFAULT_USER_ID, DEFAULT_PASSWORD,
					DEFAULT_HOMEDIR, DEFAULT_ORGANIZATION,
					DEFAULT_SRB_SERVER, DEFAULT_DEFAULT_STORAGE_RESOURCE, DEFAULT_SRB_PORT,
					DEFAULT_SERVER, DEFAULT_DGL_NAME);
		}

		private static String executeFlow(Flow flow,
			String userid,
			String password,
			String homedir,
			String organization,
			String srbserver,
			String defaultStorageResource,
			int srbport,
			String matrixServer,
			String dglName){
				DataGridResponse dg_res_1 = executeFlow(flow, userid, password, homedir,
					organization,
					srbserver, defaultStorageResource, srbport, matrixServer, dglName,
					"asynchronous");
				/*******************************************************************
						* Data Grid Requests for Transactions can be either Synchronous or Asynchronous.
					* If the requestType is asynchronous, the server sends back an
						* acknowledgement which will contain the transaction ID which can be used to
					* query the status of the request later at any time.
					* If the requestType is synchronous, the server sends the status of the Transaction
					* once its completed.
					******************************************************************/
				System.out.println("Received Data Grid Acknowledgement from: " +
					matrixServer);
				System.out.println("Request State: " + dg_res_1.getRequestState());
				RequestAcknowledgment ack = (RequestAcknowledgment) dg_res_1.
					getRequestAcknowledgement();
				if (ack != null){ //its a status response
						String responseid = dg_res_1.getRequestAcknowledgement().getRequestID();
						System.out.println("Data Grid Transaction ID: " + responseid);
						return responseid;
				} else{
						System.out.println("\n\nResponse XML converted to viewable format:");
						FlowStatusResponse fsr = (FlowStatusResponse) dg_res_1.
							getFlowStatusResponse();
						if (fsr != null)
								printStatusResponse(fsr, 0);
						return null;
				}
		}

		private static DataGridResponse executeFlow(Flow flow,
			String userid,
			String password,
			String homedir,
			String organization,
			String srbserver,
			String defaultStorageResource,
			int srbport,
			String matrixServer,
			String dglName,
			String requestType){
				return executeFlow(flow, userid, password, homedir, organization,
					srbserver, defaultStorageResource, srbport, matrixServer, dglName,
					requestType, "");
		}

		private static DataGridResponse executeFlow(Flow flow,
			String userid,
			String password,
			String homedir,
			String organization,
			String srbserver,
			String defaultStorageResource,
			int srbport,
			String matrixServer,
			String dglName,
			String requestType,
			String ticketID){
				try{
						/*******************************************************************
							* Now that we have login and server configuration
							* information, we create UserInfo, VirtualOrganizationInfo,
							* and Transaction objects, whcih are required to create a
							* Data Grid Request (DGR) to Matrix.
							******************************************************************/
						// Both UserInfo and VirtualOrgInfo need OrgInfo so set that up first
						OrganizationInfo oi = new OrganizationInfo(organization);
						// Set Up UserInfo object
						UserInfo ui = null;
						// if valid ticketID use it or else use the full information
						System.out.println("TicketId= " + ticketID);
						if ((ticketID == null) || ticketID.trim().equals("") ||
							(ticketID.equalsIgnoreCase("enterRealTicketNumberHere"))){
								ui = new UserInfo(userid, oi);
								ui.setChallengeResponse(password); //optional in Matrix DGR but needed for SRB
								ui.setHomeDirectory(homedir); //optional in Matrix DGR but needed for SRB
								ui.setDefaultStorageResource(defaultStorageResource); // the resource to store data
						} else{
								System.out.println("Using Grid ticketID: " + ticketID + " to login ...");
								ui = new UserInfo(ticketID); // ticketID of eaerlier session
						}
						// Set up VirtualOrganizationInfo object
						VirtualOrganizationInfo voi = new VirtualOrganizationInfo(srbserver,
							srbport);
						/*******************************************************************
								* Now we can create the actual Data Grid Request to represent our Transaction.
							* We submit the request to the Matrix server
							******************************************************************/
						DataGridRequest dg_req_1 = new DataGridRequest(ui, voi, flow);
						dg_req_1.setRequestType(requestType);
						System.out.println("\nWriting Request DGL file : " + dglName + "\n");
						OutputStream os = new FileOutputStream(dglName);
						marshaller.marshal(dg_req_1, os);
						System.out.println("\nPreparing Matrix connection to: " + matrixServer +
							"\n");
						MatrixConnection mConnect = new MatrixConnection(matrixServer);
						System.out.println("Sending Data Grid request to: " + matrixServer +
							"\n");
						DataGridResponse dg_res_1 = mConnect.sendRequest(dg_req_1);
						return dg_res_1;
				} catch (SOAPException fnfe){
						System.out.println("\n\nThere was a problem. (Ok, we know this kind of message can irritate any one." +
							" Now ;) and send an email to talk2matrix@sdsc.edu for any help");
						//fnfe.printStackTrace();
				} catch (IOException ioe){
						System.out.println("Please make sure that local file names are in the correct directory and that the matrix server" +
							" above is up and running.");
						//ioe.printStackTrace();
				} catch (Exception e){
						System.out.println("\n\nThere was a problem. (Ok, we know this kind of message can irritate any one." +
							" Now ;) and send an email to talk2matrix@sdsc.edu for any help");
						e.printStackTrace();
				}
				return null;
		}

		//========================= runFlow as sync or async ===========//

		/**
			* Start this flow and go to a specific server.
			*
			* @param flow Flow
			* @param requestType Synchronous or Asynchronous
			*/
		public static DataGridResponse startFlow(Flow flow, String requestType){
				return startFlow(flow, requestType, false);
		}

		/**
				* Run this flow and go to a specific server.  Except for the server, the other
			* parameter values will be the default ones and the user will not be asked
			* for verification
			*
			* @param flow Flow to run
			* @param requestType Synchronous or Asynchronous
			* @param server String
			*/
		public static DataGridResponse startFlow(Flow flow, String requestType,
			String server){
				DEFAULT_SERVER = server;
				return startFlow(flow, requestType, false);
		}

		/**
				* Run this flow and go to a specific server.  Except for the server, the other
			* parameter values will be the default ones and the user will not be asked
			* for verification
			*
			* @param flow Flow to run
			* @param requestType Synchronous or Asynchronous
			* @param ticketID GridTicketID to use to login
			* @param server URL of the Matrix Server (e.g) http://localhost:8080/matrix/receiver
			*/
		public static DataGridResponse startFlow(Flow flow, String requestType,
			String ticketID, String server){
				DEFAULT_SERVER = server;
				DEFAULT_ticketID = ticketID;
				return startFlow(flow, requestType, false);
		}

		/**
			* Start this flow and go to server using input gathered from askForInput
			* interactively (if required)
			*
			* @param flow Flow
			* @param requestType Synchronous or Asynchronous
			* @param askForInput Whether to have Interactive prompts for input
			*/
		public static DataGridResponse startFlow(Flow flow, String requestType,
			boolean askForInput){
				return startFlow(flow, requestType, DEFAULT_ticketID, askForInput);
		}

		/**
			* Start this flow and go to server using input gathered from askForInput
			* interactively (if required)
			*
			* @param flow Flow
			* @param requestType Synchronous or Asynchronous
			* @param ticketID GridTicketID to use (if present, all other login info are ignored)
			* @param askForInput Whether to have Interactive prompts for input
			*/
		public static DataGridResponse startFlow(Flow flow, String requestType,
			String ticketID,
			boolean askForInput){
				if (askForInput)
						askForInput();
				return executeFlow(flow, DEFAULT_USER_ID, DEFAULT_PASSWORD, DEFAULT_HOMEDIR,
					DEFAULT_ORGANIZATION,
					DEFAULT_SRB_SERVER, DEFAULT_DEFAULT_STORAGE_RESOURCE, DEFAULT_SRB_PORT,
					DEFAULT_SERVER, DEFAULT_DGL_NAME, requestType, ticketID);
		}

		//========================= status checking ===========================//

		public static FlowStatusResponse getStatus(String flowId){
				return getStatus(flowId, DEFAULT_SERVER);
		}

		public static FlowStatusResponse getStatus(String flowId, String server){
				try{
						MatrixConnection mConnect = new MatrixConnection(server);
						// Both UserInfo and VirtualOrgInfo need OrgInfo so set that up first
						OrganizationInfo oi = new OrganizationInfo(DEFAULT_ORGANIZATION);
						// Set Up UserInfo object
						UserInfo ui = new UserInfo(DEFAULT_USER_ID, oi);
						ui.setChallengeResponse(DEFAULT_PASSWORD); //optional in Matrix DGR but needed for SRB
						ui.setHomeDirectory(DEFAULT_HOMEDIR); //optional in Matrix DGR but needed for SRB
						ui.setDefaultStorageResource(DEFAULT_DEFAULT_STORAGE_RESOURCE); // the resource to store data
						// Set up VirtualOrganizationInfo object
						VirtualOrganizationInfo voi = new VirtualOrganizationInfo(
							DEFAULT_SRB_SERVER, DEFAULT_SRB_PORT);
						FlowStatusQuery tsq = new FlowStatusQuery(flowId);
						DataGridRequest dgRequest = new DataGridRequest(ui, voi, tsq);
						DataGridResponse dgResponse = mConnect.sendRequest(dgRequest);
						FlowStatusResponse statusResponse = (FlowStatusResponse) dgResponse.
							getFlowStatusResponse();
						return statusResponse;
				} catch (SOAPException fnfe){
						System.out.println("\n\nThere was a problem. (Ok, we know this kind of message can irritate any one." +
							" Now ;) and send an email to talk2matrix@sdsc.edu for any help");
						//fnfe.printStackTrace();
				} catch (IOException ioe){
						System.out.println("Please make sure that local file names are in the correct directory and that the matrix server" +
							" above is up and running.");
						//ioe.printStackTrace();
				} catch (Exception e){
						System.out.println("\n\nThere was a problem. (Ok, we know this kind of message can irritate any one." +
							" Now ;) and send an email to talk2matrix@sdsc.edu for any help");
						e.printStackTrace();
				}
				return null;
		}

		//=========================== methods for sending XML files =====================//

		/**
			* Send the given file using the default parameters
			* @param xmlFile String
			*/
		public static void sendXML(String xmlFile){
				sendXML(xmlFile, DEFAULT_SERVER);
		}

		/**
			* Sends the given xmlFile to the given server, using the default values for
			* the rest of the request parameters
			* @param xmlFile String
			* @param server String
			*/
		public static void sendXML(String xmlFile, String server){
				String REQUEST_FILE = xmlFile;
				Unmarshaller unmarshaller;
				try{
						SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
						SOAPConnection connection = scf.createConnection();
						MessageFactory mf = MessageFactory.newInstance();
						SOAPMessage msg = mf.createMessage();
						SOAPPart soapPart = msg.getSOAPPart();
						Source source = new StreamSource(new FileInputStream(REQUEST_FILE));
						soapPart.setContent(source);
						System.out.println("Sending DGL message to: " + server);
						printReply(msg);
						SOAPMessage reply = connection.call(msg, new URL(server));
						for (int i = 0; i <= 4; i++){
								System.out.println(" ");
						}
						System.out.println("Received DGL Acknowledgeement from: " +
							server);
						printReply(reply);
						JAXBContext jc = JAXBContext.newInstance(
							"edu.sdsc.matrix.srb.parser");
						unmarshaller = jc.createUnmarshaller();
						Source replyxml = reply.getSOAPPart().getContent();
						DataGridResponse dgresponse = (DataGridResponse) unmarshaller.
							unmarshal(
							replyxml);
						edu.sdsc.matrix.srb.parser.RequestAcknowledgment ack = dgresponse.
							getRequestAcknowledgement();
						if (ack != null){ //its a status response
								String flowId = ack.getRequestID();
								System.out.println("\n\nYour Flow ID : " + flowId + "\n");
						}
				} catch (FileNotFoundException e){
						e.printStackTrace();
				} catch (Exception e){
						e.printStackTrace();
				}
		}

// ============================ printing output methods =======================

		public static void printStatusResponse(edu.sdsc.matrix.srb.parser.
			FlowStatusResponse fsr){
				printStatusResponse(fsr, 0);
		}

		/**
			* Prints on screen the the Response received from data grid request.
			* Generic to print Staus Response or the Acknowledgement.
			* @param dg_res_1 Data Grid Response received from the Matrix Server
				* @return transaction Id (if it is a Acknowledgement) or null (Status Response)
			*/

		public static String printStatusResponse(DataGridResponse dg_res_1){
				/*******************************************************************
						* Data Grid Requests for Transactions can be either Synchronous or Asynchronous.
					* If the requestType is asynchronous, the server sends back an
						* acknowledgement which will contain the transaction ID which can be used to
					* query the status of the request later at any time.
					* If the requestType is synchronous, the server sends the status of the Transaction
					* once its completed.
					******************************************************************/
//						System.out.println("Received Data Grid Acknowledgement from: " +
//								matrixServer);
//						System.out.println("Request State: " + dg_res_1.getRequestState());
				if (dg_res_1.getRequestAcknowledgement() != null){ //its a status response
						String responseid = dg_res_1.getRequestAcknowledgement().getRequestID();
						System.out.println("Data Grid Transaction ID: " + responseid);
						return responseid;
				} else{
						System.out.println("\n\nResponse XML converted to viewable format:");
						FlowStatusResponse fsr = (FlowStatusResponse) dg_res_1.
							getFlowStatusResponse();
						if (fsr != null)
								printStatusResponse(fsr, 0);
						return null;
				}
		}

		private static void printStatusResponse(edu.sdsc.matrix.srb.parser.
			FlowStatusResponse fsr, int level){
				for (int i = 0; i < level; i++)
						System.out.print("\t");
				System.out.println("Flow " + fsr.getID());
				if (fsr.getFlowStatusResponse().size() == 0){
						if (fsr.getStepStatusResponse().size() == 0){
								for (int i = -1; i <= level; i++)
										System.out.print("\t");
								System.out.println(("\t STEPS NOT STARTED or NOT FOUND"));
						}
						for (Iterator i2 = fsr.getStepStatusResponse().iterator();
							i2.hasNext(); ){
								edu.sdsc.matrix.srb.parser.StepStatusResponse ssr =
									(edu.sdsc.matrix.srb.parser.StepStatusResponse) i2.next();
								for (int i = 0; i <= level; i++)
										System.out.print("\t");
								System.out.println("Step:" + ssr.getID() + "\t" + ssr.getStatusCode() +
									"\t" +
									translateStatusCode(ssr.getStatusCode()) +
									" (start:" + ssr.getStartTime().getTime() +
									", end: " + ssr.getEndTime().getTime() + ")"
									);
						}
						System.out.println("\n");
				} else{
						for (Iterator i1 = fsr.getFlowStatusResponse().iterator(); i1.hasNext(); ){
								edu.sdsc.matrix.srb.parser.FlowStatusResponse f2 =
									(edu.sdsc.matrix.srb.parser.FlowStatusResponse) i1.next();
								printStatusResponse(f2, level + 1);
						}
				}
		}

		/**
			* Decipher those status codes
			*/
		private static String translateStatusCode(int statusCode){
				switch (statusCode){
						case (9000016):
								return "THREAD_NOT_FOUND_OR_NOT_STARTED";
						case (9000017):
								return "THREAD_STARTED";
						case (9000018):
								return "THREAD_COMPLETE";
						case (9000019):
								return "THREAD_ABORTED_DUE_TO_ERROR_IN_PREPROCESS";
						case (9000020):
								return "THREAD_ABORTED_DUE_TO_ERROR_IN_PROCESS";
						case (9000021):
								return "THREAD_ABORTED_DUE_TO_ERROR_IN_POSTPROCESS";
						case (9000022):
								return "THREAD_ABORTED_DUE_TO_REQUEST_FROM_USER";
						default:
								return "UNKNOWN THREAD STATE" +
									"\nPlease e-mail talk2matrix@sdsc.edu about this"; //never returned
				} //switch
		} //translateStatusCode

		private static void printReply(SOAPMessage reply) throws Exception{
				TransformerFactory tFact = TransformerFactory.newInstance();
				Transformer transformer = tFact.newTransformer();
				Source src = reply.getSOAPPart().getContent();
				StreamResult result = new StreamResult(System.out);
				transformer.transform(src, result);
				System.out.println();
		}

		private static void toLog(String msg){
				System.out.print(msg);
		}
}
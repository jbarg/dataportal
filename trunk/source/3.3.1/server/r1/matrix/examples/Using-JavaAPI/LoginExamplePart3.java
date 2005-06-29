import javax.xml.bind.Marshaller;

import edu.sdsc.matrix.srb.client.Collection;
import edu.sdsc.matrix.srb.client.CreateStep;
import edu.sdsc.matrix.srb.client.DataGridResponse;
import edu.sdsc.matrix.srb.client.MatrixClientException;
import edu.sdsc.matrix.srb.client.MatrixRunner;
import edu.sdsc.matrix.srb.client.PrepareTicketStep;
import edu.sdsc.matrix.srb.client.SequentialFlow;
import edu.sdsc.matrix.srb.client.Step;
import edu.sdsc.matrix.srb.client.*;

import org.apache.commons.cli.*;

/**
	* This class is the third of the three classes that demonstrate the GridTickets in Matrix
	*
	* This class makes a gridflow description in Data Grid Languge (DGL) to Logoff
	* or to cancel a previously prepared Grid Tiecket to the SRB data grid.
	* Once logged in, the connection (socket) is kept alive to handle further requests until
	* this explicit logout request is received. The advantage here is that individual
	* connections need not be made for the further requests. A TicketID is provided on a
	* valid login as key to be used in future. Any user with this key can use the alive
	* connection from the Matrix server. The user with the key can also cancel it at any tiem.
	* So, multiple programs could be using the same ticket. Matrix internally has the
	* capability to manage these alive connections (sockets as a connection pool). Matrix can
	* internally restart a connection once SRB servers comeback after a maintenace.
	*
	* Apart from this login/logout "session" paradigm, another pardigm that developers must
	* note is that each DGL request by itself could have multiple SRB statements executed as
	* a "batch" using a single connection. So, if all you need is to execute multiple SRB
	* statemtements, you can do that without doing a login/logout session. Both the "session"
	* concept using tickets and the "batch-ing" of certain requests have their own-advantages.
	* They can be even used together based on your user requirements. As software developers,
	* we need to use the right options for the right requirements
	*
	* 	@author Arun swaran Jagatheesan (arun@sdsc.edu)
	*/
public class LoginExamplePart3 extends SequentialFlow{

			public LoginExamplePart3(String TICKET_ID) throws MatrixClientException{

						//// Name the top flow as "outerFlow"
						// add a sequential flow by name logoutFlow as a child to the top flow.
						super("outerFlow");
						SequentialFlow logoutFlow = new SequentialFlow("LogoutFlow");
						this.addChild(logoutFlow);


						///////////////// Logout ticket /////////////

						//the prepare ticket step is again used to logout or cancel a ticket
						// (just like we used to create it). When setUsageCount is set to a negative number,
						// the ticket is cancelled.
						// The parameters are stepName, Ticket_ID, boolean "true" to cancel ticket
						PrepareTicketStep cancelTicStep = new PrepareTicketStep("cancelTicket",TICKET_ID,true);
						logoutFlow.addChild(cancelTicStep);
			}

//=========================== Main method ====================================//

			/* creates and sends a DGRequest containing a CreateDataSetFlow */

			private static Marshaller marshaller;

			public static void main(String args[]){

						try{
									// run the flow using the Matrix Runner

//         Commented Parameters that could be updated if required.
									MatrixRunner.DEFAULT_SERVER = "http://appfarm1.sdsc.edu:8080/matrix/receiver";
//									MatrixRunner.DEFAULT_SERVER = "http://localhost:8080/matrix/receiver";
//									MatrixRunner.DEFAULT_USER_ID = "Matrix-demo";
//									MatrixRunner.DEFAULT_PASSWORD = args[0];
//									MatrixRunner.DEFAULT_HOMEDIR = "/home/Matrix-demo.sdsc";
//									MatrixRunner.DEFAULT_ORGANIZATION = "sdsc";
//									MatrixRunner.DEFAULT_SRB_SERVER = "orion.sdsc.edu";
//									MatrixRunner.DEFAULT_DEFAULT_STORAGE_RESOURCE = "sfs-tape2-dtf";
//									MatrixRunner.DEFAULT_SRB_PORT = 7824;
//									MatrixRunner.DEFAULT_DGL_NAME = "LoginExample.dgl";
//									MatrixRunner.DEFAULT_ticketID = args[0];
											if (MatrixRunner.processCommandLine("LoginExamplePart2", args)){
														if (MatrixRunner.hasOptionInCmdLine('t')){
																	// Matrix Runner will run the flow as a synchronous request
																	DataGridResponse dg_res = MatrixRunner.startFlow(new
																			LoginExamplePart3(MatrixRunner.getCmdLineValue('t')), "synchronous", false);
																	// printout the response
																	System.out.println("Received Data Grid Acknowledgement from: " +
																			MatrixRunner.DEFAULT_SERVER);
																	MatrixRunner.printStatusResponse(dg_res);
														}else{
																	usage();
														}
											}
						}catch(Exception e){
									e.printStackTrace();
									System.err.println(e.getMessage());
						}
			}

			public static void usage(){
						System.out.println("Usage:");
						System.out.println("java LoginExamplePart3 -t <Ticket_ID> ");
						System.out.println(
									"use \'java LoginExamplePart3 --help\' for more options");
							System.exit(1);

			}
}

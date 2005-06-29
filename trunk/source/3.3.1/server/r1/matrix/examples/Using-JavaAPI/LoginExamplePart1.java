import javax.xml.bind.Marshaller;

import edu.sdsc.matrix.srb.client.Collection;
import edu.sdsc.matrix.srb.client.CreateStep;
import edu.sdsc.matrix.srb.client.DataGridResponse;
import edu.sdsc.matrix.srb.client.FlowStatusResponse;
import edu.sdsc.matrix.srb.client.MatrixClientException;
import edu.sdsc.matrix.srb.client.MatrixRunner;
import edu.sdsc.matrix.srb.client.PrepareTicketStep;
import edu.sdsc.matrix.srb.client.SequentialFlow;
import edu.sdsc.matrix.srb.client.Step;
import edu.sdsc.matrix.srb.client.StepStatusResponse;

import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionGroup;

/**
	* This class is the first of the three classes that demonstrate the GridTickets in Matrix
	*
	* This class makes a gridflow description in Data Grid Languge (DGL) to Login
			* to the data grid (SRB). Once logged in, the connection (socket) is kept alive
	* to handle further requests until an explicit logout request is received. The
			* advantage here is that individual connections need not be made for the further
			* requests. A TicketID is provided on a valid login as key to be used in future.
	* Any user with this key can use the alive connection from the Matrix server. So, multiple
	* programs could be using the same ticket. Matrix internally has the capability to manage
	* these alive connections (sockets as a connection pool). Matrix can internally restart a
	* connection once SRB servers comeback after a maintenace.
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
public class LoginExamplePart1 extends SequentialFlow{

			public LoginExamplePart1(String COLLECTION_NAME) throws
					MatrixClientException{
						//// Name the top flow as "outerFlow"
						// add a sequential flow by name loginFlow as a child to the top flow.
						super("outerFlow");
						SequentialFlow loginFlow = new SequentialFlow("LoginFlow");
						this.addChild(loginFlow);
						///////////////// Prepare ticket /////////////
						//the prepare ticket step prepares a session kind of stuff
						// it will return a ticket that can be used to login again.
						// The parameters are stepName, UserInfo and VOInfo
						PrepareTicketStep prepareTicStep = new PrepareTicketStep("prepareTic",
								MatrixRunner.getDefaultUserInfo(), MatrixRunner.getDefaultVO());
						// the ticket could be set valid from  a start date till end date
						// but the default is infinite
						// the ticket could also be set valid for a specifed number of login or usage
						// the default is again infinite. Here, we set the number of usage to 100
						prepareTicStep.setUsageCount(100);
						///////////////// Create collection /////////////
						// This step is added just to illustrate that multiple SRB commands can
						// be combined together as a single batch request (without the need to
						// to use multiple requests or sessions).
						Collection testDir = new Collection(COLLECTION_NAME);
						Step makeTestDirStep = new CreateStep("makeDirStep", testDir);
						// rap up
						// attach the prepareTic and makeDirStep as children to the loginFlow
						loginFlow.addChild(prepareTicStep);
						loginFlow.addChild(makeTestDirStep);
			}

//=========================== Main method ====================================//

			/* creates and sends a DGRequest containing a CreateDataSetFlow */

			private static Marshaller marshaller;

			public static void main(String args[]){
						try{
									// run the flow using the Matrix Runner
//         Commented Parameters that could be updated if required.
									MatrixRunner.DEFAULT_SERVER =
											"http://appfarm1.sdsc.edu:8080/matrix/receiver";
//											MatrixRunner.DEFAULT_SERVER = "http://localhost:8080/matrix/receiver";
//									MatrixRunner.DEFAULT_USER_ID = "Matrix-demo";
//									MatrixRunner.DEFAULT_PASSWORD = args[0];
//									MatrixRunner.DEFAULT_HOMEDIR = "/home/Matrix-demo.sdsc";
//									MatrixRunner.DEFAULT_ORGANIZATION = "sdsc";
//									MatrixRunner.DEFAULT_SRB_SERVER = "orion.sdsc.edu";
//									MatrixRunner.DEFAULT_DEFAULT_STORAGE_RESOURCE = "sfs-tape2-dtf";
//									MatrixRunner.DEFAULT_SRB_PORT = 7824;
//									MatrixRunner.DEFAULT_DGL_NAME = "LoginExample.dgl";
									// Matrix Runner will run the flow as a synchronous request
									// Will again prompt for input (as the last parameter is set to true)
									OptionGroup options = new OptionGroup();
									options.addOption(new Option("C", "newCollection", true, "New collection to create"));
									if (MatrixRunner.processCommandLine("LoginExamplePart1", args, false,
											options)){
												if (MatrixRunner.hasOptionInCmdLine('C')){
															String collectionName = MatrixRunner.getCmdLineValue('C');
															DataGridResponse dg_res = MatrixRunner.startFlow(new
																	LoginExamplePart1(collectionName), "synchronous", true);
															// printout the response
															System.out.println("Received Data Grid Acknowledgement from: " +
																	MatrixRunner.DEFAULT_SERVER);
															MatrixRunner.printStatusResponse(dg_res);
															FlowStatusResponse fsr = dg_res.getFSR();
															StepStatusResponse ssr = fsr.getSSRbyName("prepareTic");
															System.out.println("The ticketID to use for LoginExamplePart2:" +
																	ssr.getPrepareTicketString());
															System.out.println("\n You may use the following command:");
															System.out.println("java LoginExamplePart2 -t " +
																	ssr.getPrepareTicketString() + " -C " +collectionName);
												} else{
															usage();
												}
									}
						} catch (Exception e){
									e.printStackTrace();
									System.err.println(e.getMessage());
						}
			}

			public static void usage(){
						System.out.println("Usage:");
						System.out.println("java LoginExamplePart1 -C <CollectionName> ");
						System.out.println("use \'java LoginExamplePart1 --help\' for more options");
						System.exit(1);
			}
}
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
	* This class is the second part of the three classes that demonstrate the GridTickets in Matrix
	*
	* This class makes a gridflow description in Data Grid Languge (DGL) to and uses the
	* Login ticket created from Part1. It uses the connection (socket) that was  kept alive
	* from Part1 to handle further requests until an explicit logout request is received. The
			* advantage here is that individual connections need not be made for the further
	* requests. The TicketID provided from Part1 is used to validate.
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
public class LoginExamplePart2 extends SequentialFlow{

			public LoginExamplePart2(String COLLECTION_NAME) throws
					MatrixClientException{
						//////////////    Name the top flow as "outerFlow"
						// add a sequential flow by name DemoTicketUseFlow as a child to the top flow.
						super("outerFlow");
						SequentialFlow demoFlow = new SequentialFlow("DemoTicketUseFlow");
						this.addChild(demoFlow);
						//////////         Make a collection        /////////////////////////
						//Confirms we have a collection even if created the same in Part1 ///
						Collection testDir = new Collection(COLLECTION_NAME);
						Step makeTestDirStep = new CreateStep("makeTestDirStep", testDir);
						demoFlow.addChild(makeTestDirStep);
						////////// create a forloop to fill the test dir with some files //////////
						//////  The syntax used with SET and other stuff is purely DGL ////////////
						ForLoopFlow createFilesLoop = new ForLoopFlow("createFilesLoop",
								"SET i = 0", "$i < 10", "SET i = $i+1");
						// loops must only have a single flow inside as
						SequentialFlow createFilesIteration = new SequentialFlow(
								"createFilesIteration");
						// at each iteration, ingest a dataSet named filei from googele's web site
						DataSet dataSet = new DataSet(COLLECTION_NAME + "/file+$i");
						Step ingestStep = new IngestDataSetStep("ingestFile",
								"\"http://www.google.com\"", dataSet);
						// rap-up and attach all the flows and steps together
						this.addChild(createFilesLoop);
						createFilesLoop.addChild(createFilesIteration);
						createFilesIteration.addChild(ingestStep);

			}

//=========================== Main method ====================================//

			/* creates and sends a DGRequest containing a CreateDataSetFlow */

			private static Marshaller marshaller;

			public static void main(String args[]){
						String COLLECTION_NAME = "";
						try{
									// run the flow using the Matrix Runner
//         Commented Parameters that could be updated if required.
									MatrixRunner.DEFAULT_SERVER ="http://appfarm1.sdsc.edu:8080/matrix/receiver";
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
									OptionGroup options = new OptionGroup();
									options.addOption(new Option("C", "newCollection", true,"New collection to create"));
									if (MatrixRunner.processCommandLine("LoginExamplePart2", args, false,
											options)){
												if (MatrixRunner.hasOptionInCmdLine('C') &&
														MatrixRunner.hasOptionInCmdLine('t')){
															// Matrix Runner will run the flow as a synchronous request
															DataGridResponse dg_res = MatrixRunner.startFlow(new
																	LoginExamplePart2(MatrixRunner.getCmdLineValue('C')),
																	"synchronous", MatrixRunner.getCmdLineValue('t'), false);
															// printout the response
															System.out.println("Received Data Grid Acknowledgement from: " +
																	MatrixRunner.DEFAULT_SERVER);
															MatrixRunner.printStatusResponse(dg_res);
															//for the next part (ExamplePart3)
															System.out.println("\nYou may use the following command to Log out your ticket:");
															System.out.println("java LoginExamplePart3 -t " +MatrixRunner.getCmdLineValue('t'));

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
						System.out.println(
								"java LoginExamplePart2 -t <TicketValue> -C [COLLECITON_NAME]");
						System.out.println(
								"use \'java LoginExamplePart2 --help\' for more options");
						System.exit(1);
			}
}
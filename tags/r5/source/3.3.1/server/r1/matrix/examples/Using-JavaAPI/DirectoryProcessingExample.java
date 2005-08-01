import java.io.IOException;
import java.util.List;

import javax.xml.bind.Marshaller;

import edu.sdsc.matrix.srb.client.Collection;
import edu.sdsc.matrix.srb.client.CollectionList;
import edu.sdsc.matrix.srb.client.CreateStep;
import edu.sdsc.matrix.srb.client.DataSet;
import edu.sdsc.matrix.srb.client.ExecuteProcessStep;
import edu.sdsc.matrix.srb.client.FlowStatusResponse;
import edu.sdsc.matrix.srb.client.ForEachFlow;
import edu.sdsc.matrix.srb.client.ForLoopFlow;
import edu.sdsc.matrix.srb.client.GetPhysicalLocationStep;
import edu.sdsc.matrix.srb.client.IngestDataSetStep;
import edu.sdsc.matrix.srb.client.IngestMetadataStep;
import edu.sdsc.matrix.srb.client.ListStep;
import edu.sdsc.matrix.srb.client.MatrixRunner;
import edu.sdsc.matrix.srb.client.SequentialFlow;
import edu.sdsc.matrix.srb.client.Step;
import edu.sdsc.matrix.srb.client.StreamData;
import edu.sdsc.matrix.srb.client.DataGridResponse;

import org.apache.commons.cli.OptionGroup;
import org.apache.commons.cli.Option;

import edu.sdsc.grid.io.srb.SRBAccount;

/**
	* Creates a  flow that:
	* - creates a recursive collection sturcutre with a for loop
	*   /myTestDirArun/Collection_1/Collection_2/Collection_3/...
	* - Ingests files from http://www.google.com into each directory
	* - Lists the root collection
	* - For each file in the listing (from root collections)
	*    - finds physical location of the file
	*    - caluculates md5 checksum at the location (without moving the file)
	*    - ingest the md5 sum as a meta-data value
	*/
public class DirectoryProcessingExample extends SequentialFlow{

			private static final String COLLECTION_NAME_FINAL = "myTestDirArun";

			/**
				* No arg constructor - never used
				*/
			public DirectoryProcessingExample(){
						this(COLLECTION_NAME_FINAL);
			}

			public DirectoryProcessingExample(String COLLECTION_NAME){
						this(COLLECTION_NAME, 3);
			}

			public DirectoryProcessingExample(String COLLECTION_NAME, int n){
						// name this flow "outerFlow"; create a DGL variable
						super("outerFlow");
						super.addVariable("collectionNow", COLLECTION_NAME);
						this.addVariable("locationOfFile", "noValue"); //will hold the physical location later
						this.addVariable("md5Sum", "noValue"); //will hold md5Sum later

						// for loop to create recursive collections and ingest files in them //
						ForLoopFlow createCollectiionsLoop = new ForLoopFlow("createCollnLoop",
								"SET collectCount = 0", "$collectCount < " + n, "SET collectCount = $collectCount + 1");
						// loops must only have a single flow inside as
						SequentialFlow createCollnIteration = new SequentialFlow("createFilesIteration");
						SequentialFlow makeTestDirFlow = new SequentialFlow("makeTestDirFlow");
						Collection testDir = new Collection("$collectionNow");
						Step makeTestDirStep = new CreateStep("makeTestDirStep", testDir);
						///////////////////////////////////////////////////////////////////////////
						////////// create a forloop to fill the test dir with some files //////////
						ForLoopFlow createFilesLoop = new ForLoopFlow("createFilesLoop",
								"SET i = 0", "$i < 360", "SET i = $i+1");
						// loops must only have a single flow inside as
						SequentialFlow createFilesIteration = new SequentialFlow("createFilesIteration");
						createFilesIteration.addVariable("dataSetNow","empty");
						// at each iteration, ingest a dataSet named filei
						DataSet dataSet = new DataSet("$dataSetNow");
						Step ingestStep = new IngestDataSetStep("ingestFile", "\"http://www.google.com\"", dataSet);
						ingestStep.addAction(true, "SET dataSetNow = $collectionNow+\"/file\"+$i");
						createFilesLoop.addAction(false,"SET collectionNow = $collectionNow + \"/Cln_\" + $collectCount");
						/////////////////////////////////////////////////////////////////////////////
						///////////////// do some processing on the files in test dir /////////////
						SequentialFlow processFilesFlow = new SequentialFlow("processFilesFlow");
						// create a variable called "collectionList" with an initial value of "empty"
						// this variable is a string now but will later be used to hold a CollectionList
						// this is ok to do because variables are dynamically typed in DGL
						processFilesFlow.addVariable("collectionList", "empty");
						// do an ls on the directory and record results in the variable collectionList
						SequentialFlow getListingFlow = new SequentialFlow("getListingFlow");
						testDir = new Collection(COLLECTION_NAME);
						ListStep getListingStep = new ListStep("getListingStep", testDir, -1);
						getListingStep.addAction(false, "SET collectionList = getProperty(" +
								"THIS, \"result.listResult.listing\")");
						List rulesList = getListingStep.getUserDefinedRules();
						// forEach file in the collectionList, do some processing
						ForEachFlow forEach = new ForEachFlow("forEachFlow", "file",
								new CollectionList("$collectionList"));
						// For-each is a complex flow that can have only one sub-member
						// So, add a sequential flow that can have the sub-steps in our example
						SequentialFlow forEachItrflow = new SequentialFlow("forEachItrFlow");
						DataSet checkMySumFile = new DataSet("$file");
						// use the getPhysicalLocationStep and store the physical location in a variable
						Step getLocation = new GetPhysicalLocationStep("getLocation", checkMySumFile);
						getLocation.addAction(false,
								"SET locationOfFile = getProperty(THIS, " +
								"\"result.getPhysicalLocationResult.physicalLocation\")"
								);
						// execute the md5 check on the file and place the results in the variable $md5Sum
						ExecuteProcessStep executeMD5Again = new ExecuteProcessStep("executeMD5-Metadata",
								"md5");
						executeMD5Again.setStdOut(new StreamData("$md5Sum", false));
						executeMD5Again.addParameterAsExpression("$locationOfFile");
						// Ingest md5 value as a metadata for the file
						IngestMetadataStep ingestCheckSumMetadata = new IngestMetadataStep("ingestMeta",
								checkMySumFile);
						ingestCheckSumMetadata.addMetaData("md5", "$md5Sum");
						// add each step to this flow (in order
						this.addChild(createCollectiionsLoop);
						createCollectiionsLoop.addChild(createCollnIteration);
						createCollnIteration.addChild(makeTestDirFlow);
						makeTestDirFlow.addChild(makeTestDirStep);
						createCollnIteration.addChild(createFilesLoop);
						createFilesLoop.addChild(createFilesIteration);
						createFilesIteration.addChild(ingestStep);
						this.addChild(processFilesFlow);
						processFilesFlow.addChild(getListingFlow);
						getListingFlow.addChild(getListingStep);
						processFilesFlow.addChild(forEach);
						forEach.addChild(forEachItrflow);
						forEachItrflow.addChild(getLocation);
						forEachItrflow.addChild(executeMD5Again);
						forEachItrflow.addChild(ingestCheckSumMetadata);
			}

//=========================== Main method ====================================//

			/* creates and sends a DGRequest containing a CreateDataSetFlow */

			private static Marshaller marshaller;

			public static void main(String args[]){
						String thisClassName = "DirectoryProcessingExample";
						MatrixRunner.DEFAULT_SERVER = "http://appfarm1.sdsc.edu:8080/matrix/receiver";
						try{
									SRBAccount srbAccountToUse = new SRBAccount();
									MatrixRunner.DEFAULT_DEFAULT_STORAGE_RESOURCE = srbAccountToUse.getDefaultStorageResource();
									MatrixRunner.DEFAULT_ORGANIZATION = srbAccountToUse.getDomainName();
									MatrixRunner.DEFAULT_HOMEDIR = srbAccountToUse.getHomeDirectory();
									MatrixRunner.DEFAULT_SRB_SERVER = srbAccountToUse.getHost();
									MatrixRunner.DEFAULT_SRB_PORT = srbAccountToUse.getPort();
									MatrixRunner.DEFAULT_USER_ID = srbAccountToUse.getUserName();
						} catch (IOException ioe){
									System.err.println("SRB Profile not found (MdasEnv and MdasAuth files).");
									System.err.print("You will need to enter your login info manually");
						}
						OptionGroup options = new OptionGroup();
						try{
									options.addOption(new Option("C", "MakeMD5", true, "Name of the new collection"));
									options.addOption(new Option("n", "Number", true,
											"Number of collections to create inside"));
									if (MatrixRunner.processCommandLine(thisClassName, args, false, options)){
												if (MatrixRunner.hasOptionInCmdLine('C')){
															String collectionName = MatrixRunner.getCmdLineValue('C');
															int iterations = 9;
															if (MatrixRunner.hasOptionInCmdLine('n'))
																		iterations = Integer.parseInt(MatrixRunner.getCmdLineValue('n'));
															DataGridResponse dg_res = MatrixRunner.startFlow(new
																	DirectoryProcessingExample(collectionName, iterations), "asynchronous", true);
															// printout the response
															System.out.println("Received Data Grid Acknowledgement from: " +
																	MatrixRunner.DEFAULT_SERVER);
															MatrixRunner.printStatusResponse(dg_res);
												} else{
															usage(thisClassName);
												}
									}
						} catch (Exception e){
									e.printStackTrace();
									System.err.println(e.getMessage());
						}

			}

			public void usage(){
						usage(this.getClass().getName());
			}

			public static void usage(String className){
						System.out.println("Usage:");
						System.out.println("java " + className + " -C <CollectionName> ");
						System.out.println("use \'java " + className + " --help\' for more options");
						System.exit(1);
			}

}
import java.util.List;

import javax.xml.bind.Marshaller;

import edu.sdsc.matrix.srb.client.Collection;
import edu.sdsc.matrix.srb.client.CollectionList;
import edu.sdsc.matrix.srb.client.CreateStep;
import edu.sdsc.matrix.srb.client.DataSet;
import edu.sdsc.matrix.srb.client.FlowStatusResponse;
import edu.sdsc.matrix.srb.client.ForEachFlow;
import edu.sdsc.matrix.srb.client.ForLoopFlow;
import edu.sdsc.matrix.srb.client.IngestDataSetStep;
import edu.sdsc.matrix.srb.client.IngestMetadataStep;
import edu.sdsc.matrix.srb.client.ListStep;
import edu.sdsc.matrix.srb.client.MatrixRunner;
import edu.sdsc.matrix.srb.client.SequentialFlow;
import edu.sdsc.matrix.srb.client.Step;
import edu.sdsc.matrix.srb.client.SwitchFlow;
import edu.sdsc.matrix.srb.client.TCondition;
import edu.sdsc.matrix.srb.client.*;

import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionGroup;

/**
	* Creates a Matrix datagrid flow to calculate md5 checksum for datasets in a
	* collection recursively. This class prepares the matrix data gridflow that based
	* on the Data Grid Language (DGL). DGL has flows and steps that are executed in a datagrid.
	*
	* @author Arun swaran Jagatheesan 1/29/05
	*/

/* Developer log
	* original code 1/29/05 Arun
	* formatting, comments 1/31/05 Arun
	*/
public class RecDirProcessing extends SequentialFlow{

			private static final String COLLECTION_NAME_FINAL = "myTestDirArun";

			/**
				* No arg constructor - never used
				*/
			public RecDirProcessing(){
						this(COLLECTION_NAME_FINAL);
			}

			/**
				* Constructor that actually creates the gridflow
				* @param COLLECTION_NAME Name of the collection in which to recursively create MD5
				*/
			public RecDirProcessing(String COLLECTION_NAME){
						// name this flow "outerFlow" and prepare DGL variables
						super("outerFlow");
						this.addVariable("locationOfFile", "noValue"); //will hold the physical location later
						this.addVariable("md5Sum", "noValue"); //will hold md5Sum later
						Collection testDir1 = new Collection(COLLECTION_NAME);
						/////////////////////////////////////////////////////////////////////////////
						///////////////// do processing on the files in test dir /////////////
						SequentialFlow processFilesFlow = new SequentialFlow("processFilesFlow");
						// create a variable called "collectionList" with an initial value of "empty"
						// this variable is a string now but will later be used to hold a CollectionList
						// this is ok to do because variables are dynamically typed in DGL
						processFilesFlow.addVariable("collectionList", "empty");
						// do an ls on the directory and record results in the variable collectionList
						SequentialFlow getListingFlow = new SequentialFlow("getListingFlow");
						ListStep getListingStep = new ListStep("getListingStep", testDir1, -1);
						// store the listing in a variable "collectionList"
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
						Step getLocation = new GetPhysicalLocationStep("getLocation",checkMySumFile);
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
						// add each step or flow to this flow (in order)
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
						// run the flow and remember the transactionId
						MatrixRunner.DEFAULT_SERVER = "http://appfarm1.sdsc.edu:8080/matrix/receiver";
						OptionGroup options = new OptionGroup();
						try{
									options.addOption(new Option("C", "newCollection", true, "New collection to create"));
									if (MatrixRunner.processCommandLine("LoginExamplePart1", args, false, options)){
												if (MatrixRunner.hasOptionInCmdLine('C')){
															String collectionName = MatrixRunner.getCmdLineValue('C');
															DataGridResponse dg_res = MatrixRunner.startFlow(new
																	RecDirProcessing(collectionName), "asynchronous", true);
															// printout the response
															System.out.println("Received Data Grid Acknowledgement from: " +
																	MatrixRunner.DEFAULT_SERVER);
															MatrixRunner.printStatusResponse(dg_res);
												} else{
															usage("RecDirProcessing");
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
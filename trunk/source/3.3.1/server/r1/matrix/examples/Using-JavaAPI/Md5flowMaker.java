import javax.xml.bind.Marshaller;

import edu.sdsc.matrix.srb.client.Collection;
import edu.sdsc.matrix.srb.client.CollectionList;
import edu.sdsc.matrix.srb.client.DataSet;
import edu.sdsc.matrix.srb.client.ExecuteProcessStep;
import edu.sdsc.matrix.srb.client.FlowStatusResponse;
import edu.sdsc.matrix.srb.client.ForEachFlow;
import edu.sdsc.matrix.srb.client.GetPhysicalLocationStep;
import edu.sdsc.matrix.srb.client.IngestMetadataStep;
import edu.sdsc.matrix.srb.client.ListStep;
import edu.sdsc.matrix.srb.client.MatrixRunner;
import edu.sdsc.matrix.srb.client.SequentialFlow;
import edu.sdsc.matrix.srb.client.Step;
import edu.sdsc.matrix.srb.client.StreamData;
import edu.sdsc.matrix.srb.client.DataGridResponse;

/**
	* This class makes a gridflow description in Data Grid Languge (DGL).
	*
	* The gridflow would iterate over contents of a collection and produce md5
			* checksums for each file. The sub-collections inside a collection are not used
	* - not recursive.
	*
	* This is just for demonstration purposes of the Matrix API - A
			* stealth technology undertaken as part of SRB Java development.  Since there no
	* good documentation of Matrix, feel free to ping the Matrix team using srb-chat@sdsc.edu
	* for more details.
	*
	* 	@author Arun swaran Jagatheesan (arun@sdsc.edu)
	*/
public class Md5flowMaker extends SequentialFlow{

			private static final String COLLECTION_NAME = "libExample3";


			public Md5flowMaker(){
						//// create the flow with some variables /////
						super("outerFlow");
						this.addVariable("locationOfFile", "noValue"); //will hold the physical location later
						this.addVariable("md5Sum", "noValue"); //will hold md5Sum later
						this.addVariable("collectionNow", COLLECTION_NAME);
						///////////////// do some processing on the files in test dir /////////////
						SequentialFlow processFilesFlow = new SequentialFlow("processFilesFlow");
						// create a variable called "collectionList" with an initial value of "empty"
						// this variable is a string now but will later be used to hold a CollectionList
						// this is ok to do because variables are dynamically typed in DGL
						processFilesFlow.addVariable("collectionList", "empty");
						// do an ls on the directory and record results in the variable collectionList
						SequentialFlow getListingFlow = new SequentialFlow("getListingFlow");
						Collection testDir = new Collection("$collectionNow");
						ListStep getListingStep = new ListStep("getListingStep", testDir);
						// once we get the listing assign it to the variable collectionList
						// This is done using Event Control Action/Rules - similar to database triggers
						getListingStep.addAction(false, "SET collectionList = getProperty(" +
								"THIS, \"result.listResult.listing\")");
						// forEach file in the collectionList, do some processing
						ForEachFlow forEach = new ForEachFlow("forEachFlow", "file",
								new CollectionList("$collectionList"));
						// For-each is a complex flow that can have only one sub-member
						// So, add a sequential flow that can have the sub-steps in our example
						SequentialFlow forEachItrflow = new SequentialFlow("forEachItrFlow");
						DataSet checkMySumFile = new DataSet("$collectionNow" + "+\"/\"+" + "$file");
						Step getLocation = new GetPhysicalLocationStep("getLocation",
								checkMySumFile);
						getLocation.addAction(false,
								"SET locationOfFile = getProperty(THIS, " +
								"\"result.getPhysicalLocationResult.physicalLocation\")"
								);

						// execute the md5 check on the file and place the results in the variable $md5Sum
						ExecuteProcessStep executeMD5Again = new ExecuteProcessStep(
								"executeMD5-Metadata", "md5");
						executeMD5Again.setStdOut(new StreamData("$md5Sum", false));
						executeMD5Again.addParameterAsExpression("$locationOfFile");
						IngestMetadataStep ingestCheckSumMetadata = new IngestMetadataStep(
								"ingestMeta", checkMySumFile);
						ingestCheckSumMetadata.addMetaData("md5_Sum", "$md5Sum");
						// add each step to this flow (in order
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
						// run the flow using the Matrix Runner
						//MatrixRunner.DEFAULT_SERVER = "http://appfarm1.sdsc.edu:8080/matrix/receiver";
						MatrixRunner.DEFAULT_SERVER = "http://localhost:8080/matrix/receiver";
						MatrixRunner.DEFAULT_USER_ID = "Matrix-demo";
						MatrixRunner.DEFAULT_PASSWORD = "******";
						MatrixRunner.DEFAULT_HOMEDIR = "/home/Matrix-demo.sdsc";
						MatrixRunner.DEFAULT_ORGANIZATION = "sdsc";
						MatrixRunner.DEFAULT_SRB_SERVER = "orion.sdsc.edu";
						MatrixRunner.DEFAULT_DEFAULT_STORAGE_RESOURCE = "sfs-tape2-dtf";
						MatrixRunner.DEFAULT_SRB_PORT = 7824;
						MatrixRunner.DEFAULT_DGL_NAME = "libMD5Request.dgl";
						// Matrix Runner will run the flow as a synchronous request
						// Will again prompt for input (as the last parameter is set to true)
						DataGridResponse dg_res = MatrixRunner.startFlow(new Md5flowMaker(),
								"synchronous", true);
						// printout the response
						System.out.println("Received Data Grid Acknowledgement from: " +
								MatrixRunner.DEFAULT_SERVER);
						System.out.println("Request State: " + dg_res.getRequestState());
						MatrixRunner.printStatusResponse(dg_res);
			}

}
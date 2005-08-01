import edu.sdsc.matrix.srb.client.*;
import javax.xml.bind.*;

/**
 * Class is a sequential flow that creates a collection and then a dataSet in
 * that collection
 */
public class ProxyCommandExample extends SequentialFlow
{
  private static final String COLLECTION_NAME = "myTestDir";
  private static final String OUTPUT_NAME = "outputFile";
  private static final String CHECK_SUM_NAME = "checkMySum";

  public ProxyCommandExample()
  {
    //// create the flow with some variables /////
    super("outerFlow");
    this.addVariable("locationOfFile", "initialValue");
    this.addVariable("md5Sum", "noValue");

    // create a test directory
    Collection testDir = new Collection(COLLECTION_NAME);
    Step makeCollection = new CreateStep("makeTestDirectory", testDir);

    // ingest some file whose sum we will check
    DataSet checkMySumFile = new DataSet(COLLECTION_NAME+"/"+CHECK_SUM_NAME);
    Step ingestStep = new IngestDataSetStep("ingestFile", "\"http://www.google.com\"", checkMySumFile);

    // get the physical location of the file that we want to check and set the
    // variable "locationOfFile" equal to the return value
    Step getLocation = new GetPhysicalLocationStep("getLocation", checkMySumFile);
    getLocation.addAction(false, "SET locationOfFile = getProperty(\"THIS\", "+
                          "\"result.getPhysicalLocationResult.physicalLocation\")"
                          );

    // store the physical location of the file as a piece of metadata
    IngestMetadataStep ingestMetadata = new IngestMetadataStep("ingestMeta", testDir);
    ingestMetadata.addMetaData("physicalLocation", "$locationOfFile");

    // execute the md5 check on the file and place the results into an output file
    ExecuteProcessStep executeMD5 = new ExecuteProcessStep("executeStep1", "md5");
    DataSet outputFile = new DataSet(COLLECTION_NAME+"/"+OUTPUT_NAME);
    executeMD5.setStdOut(outputFile);
    executeMD5.addParameterAsExpression("$locationOfFile");

    // execute the md5 check on the file and place the results in the variable $md5Sum
    ExecuteProcessStep executeMD5Again = new ExecuteProcessStep("executeStep2", "md5");
    executeMD5Again.setStdOut(new StreamData("$md5Sum", false));
    executeMD5Again.addParameterAsExpression("$locationOfFile");

    IngestMetadataStep ingestChecksumMetadata = new IngestMetadataStep("ingestMeta", testDir);
    ingestChecksumMetadata.addMetaData("md5_Sum", "$md5Sum");

    // add each step to this flow (in order
    this.addChild(makeCollection);
    this.addChild(ingestStep);
    this.addChild(getLocation);
    this.addChild(ingestMetadata);
    this.addChild(executeMD5);
    this.addChild(executeMD5Again);
    this.addChild(ingestChecksumMetadata);

    //fin
  }

//=========================== Main method ====================================//

  /* creates and sends a DGRequest containing a CreateDataSetFlow */

  private static Marshaller marshaller;

  public static void main(String args[])
  {
    // run the flow and remember the transactionId
    String transactionId = MatrixRunner.runFlow(new ProxyCommandExample());

    // give the flow some time to execute
    System.out.print("sleeping 15 seconds before asking for status...");
    try
    {
      for(int i = 0; i < 15; i++)
      {
        Thread.sleep(1000);
        System.out.print(i+1+" ");
      }
    }
    catch(Exception e){e.printStackTrace();}
    System.out.println("");

    // get the status from the server and print it to the console
    FlowStatusResponse status = MatrixRunner.getStatus(transactionId);
    MatrixRunner.printStatusResponse(status);
  }

}

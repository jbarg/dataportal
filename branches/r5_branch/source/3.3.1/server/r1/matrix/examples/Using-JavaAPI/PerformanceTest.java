import edu.sdsc.matrix.srb.client.*;
import javax.xml.bind.*;

/**
 * Class is a sequential flow that creates a collection and then a dataSet in
 * that collection
 */
public class PerformanceTest extends SequentialFlow
{
  private static final String COLLECTION_NAME = "myTestDir";

  public PerformanceTest()
  {
    // name this flow "outerFlow"
    super("outerFlow");
    this.addVariable("collectionList", "no_value");

    // do an ls on the directory and record results in the variable collectionList
    SequentialFlow getListingFlow = new SequentialFlow("getListingFlow");
    Collection testDir = new Collection(COLLECTION_NAME);
    ListStep getListingStep = new ListStep("getListingStep", testDir);
    getListingStep.addAction(false, "SET collectionList = getProperty("+
    "THIS, \"result.listResult.listing\")");

    // forEach file in the collectionList, do some processing
    ForEachFlow forEach = new ForEachFlow("forEachFlow", "file", new CollectionList("$collectionList"));
    forEach.setParallelIncrement(20);

    SequentialFlow forEachIteration = new SequentialFlow("ForEachIteration");
    forEachIteration.addVariable("locationOfFile", "no_value");
    forEachIteration.addVariable("md5Sum", "no_value");

    // get the physical location of the file that we want to check and set the
    // variable "locationOfFile" equal to the return value
    DataSet checkMySumFile = new DataSet(COLLECTION_NAME+"/$file");
    Step getLocation = new GetPhysicalLocationStep("getLocation", checkMySumFile);
    getLocation.addAction(false, "SET locationOfFile = getProperty(THIS, "+
                          "\"result.getPhysicalLocationResult.physicalLocation\")"
                          );

    // execute the md5 check on the file and place the results in the variable $md5Sum
    ExecuteProcessStep executeMD5 = new ExecuteProcessStep("executeMD5", "md5");
    executeMD5.setStdOut(new StreamData("$md5Sum", false));
    executeMD5.addParameterAsExpression("$locationOfFile");

    IngestMetadataStep ingestChecksumMetadata = new IngestMetadataStep("ingestMeta", checkMySumFile);
    ingestChecksumMetadata.addMetaData("$file", "$md5Sum");

    // add each step to this flow (in order)
    this.addChild(getListingFlow);
      getListingFlow.addChild(getListingStep);
    this.addChild(forEach);
      forEach.addChild(forEachIteration);
        forEachIteration.addChild(getLocation);
        forEachIteration.addChild(executeMD5);
        forEachIteration.addChild(ingestChecksumMetadata);
  }


//=========================== Main method ====================================//

  /* creates and sends a DGRequest containing a CreateDataSetFlow */

  private static Marshaller marshaller;

  public static void main(String args[])
  {
    System.out.println("outgoing: "+new java.util.Date(System.currentTimeMillis()));
    String transactionId = MatrixRunner.runFlow(new PerformanceTest(), "http://appfarm1.sdsc.edu:8080/matrix/receiver");
    System.out.println("got response: "+new java.util.Date(System.currentTimeMillis()));

//
//    System.out.print("sleeping 200 seconds before asking for status...");
//    try
//    {
//      for(int i = 0; i < 200; i++)
//      {
//        Thread.sleep(1000);
//        System.out.print(i+1+" ");
//      }
//    }
//    catch(Exception e){e.printStackTrace();}
//
//    System.out.println("");
// String transactionId = "1092769132144";
 //   FlowStatusResponse status = MatrixRunner.getStatus(transactionId, "http://appfarm1.sdsc.edu:8080/matrix/receiver");
  //  MatrixRunner.printStatusResponse(status);


  //, "http://appfarm1.sdsc.edu:8080/matrix/receiver");

 //  MatrixRunner.run(new Test(), false);
 //  MatrixRunner.checkStatus("1091754107811");

  }
}

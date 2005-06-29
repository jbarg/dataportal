import edu.sdsc.matrix.srb.client.*;
import javax.xml.bind.*;

/**
 * Class is a sequential flow that creates a collection and then a dataSet in
 * that collection
 */
public class AllStepsDemo extends SequentialFlow
{
  private static final String COLLECTION_NAME = "myCollection";
  private static final String DATASET_NAME = "myDataSet";

  public AllStepsDemo()
  {
    // name this flow "outerFlow"
    super("outerFlow");

    // create a collection
    Collection collection = new Collection(COLLECTION_NAME);
    Step makeCollection = new CreateStep("makeCollection", collection);

    // ingest a dataSet into the collection
    DataSet dataSet = new DataSet(COLLECTION_NAME+"/"+DATASET_NAME);
    Step ingestStep = new IngestDataSetStep("ingestFile", "\"http://www.google.com\"", dataSet);

    // make a copy of the original dataSet
    DataSet copy = new DataSet(COLLECTION_NAME+"/copyOf"+DATASET_NAME);
    Step makeCopy = new CopyStep("copy", dataSet, copy);

    // change the permission of the copy
    UserInfo user = getUserInfo();
    Step changePermission = new ChangePermissionStep("changePermission", copy, user, "rw" );

    // delete the original dataset
    Step deleteOriginal = new DeleteStep("deleteOriginal", dataSet);

    // rename the dataSet
    Step renameCopy = new RenameStep("rename", copy, "renamedCopy");

    // make a step that adds metadata
    IngestMetadataStep ingestMetadata = new IngestMetadataStep("ingestMeta", collection);
    ingestMetadata.addMetaData("myKey", "myValue");

    // add each step to this flow (in order
    this.addChild(makeCollection);
    this.addChild(ingestStep);
    this.addChild(makeCopy);
  //  this.addChild(changePermission);
    this.addChild(deleteOriginal);
    this.addChild(renameCopy);
    this.addChild(ingestMetadata);
  }



//================= utility =====================//

  public static UserInfo getUserInfo()
  {
    String userid = "matrix-demo";
    String password = "******";
    String homedir = "/home/Matrix-demo.sdsc";
    String organization = "sdsc";
    String defaultStorageResource = "sdsc-unix";

    OrganizationInfo oi = new OrganizationInfo(organization);

    // Set Up UserInfo object
    UserInfo ui = new UserInfo(userid, oi);
    ui.setChallengeResponse(password); //optional in Matrix DGR but needed for SRB
    ui.setHomeDirectory(homedir); //optional in Matrix DGR but needed for SRB
    ui.setDefaultStorageResource(defaultStorageResource); // the resource to store data

    return ui;
  }





//=========================== Main method ====================================//

  /* creates and sends a DGRequest containing a CreateDataSetFlow */

  private static Marshaller marshaller;

  public static void main(String args[])
  {
    // run the flow and remember the transactionId
    String transactionId = MatrixRunner.runFlow(new AllStepsDemo());

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

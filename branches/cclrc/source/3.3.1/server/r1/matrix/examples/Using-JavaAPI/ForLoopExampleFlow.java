import edu.sdsc.matrix.srb.client.*;
import javax.xml.bind.*;

/**
 * Class is a forLoop flow that iterates from i = 0 to 2 and at each iteration:
 * 1) creates a uniquely named collection
 * 2) creates a uniquely named dataSet in that collection
 */

  public class ForLoopExampleFlow extends ForLoopFlow
  {
    private static final String COLLECTION_NAME = "myCollection";
    private static final String DATASET_NAME = "myDataSet";

    public ForLoopExampleFlow()
    {
	 // name this flow "outerForLoop"
	 super("outerForLoop","SET i = 0", "$i < 10", "SET i = $i+1");

      SequentialFlow innerSequential = new SequentialFlow("innerSequential");

	 // make a step that creates the collection
	 Collection collection = new Collection(COLLECTION_NAME+"+$i");
	 Step makeCollection = new CreateStep("myFirstStep", collection);

	 // make a step that creates the dataSet
	 DataSet dataSet = new DataSet(COLLECTION_NAME + "+$i+\"/\"+" + DATASET_NAME+"+$i");
	 Step makeDataSet = new CreateStep("mySecondStep", dataSet);

	 // add each step to the sequential flow (in order)
	 innerSequential.addChild(makeCollection);
	 innerSequential.addChild(makeDataSet);

      // add the sequential flow to this forLoop flow
      this.addChild(innerSequential);
    }



  //==================== utility methods =======================//
  private static UserInfo getUserInfo()
  {
    String userid = "Matrix-demo";
    String password = "******";
    String homedir = "/home/Matrix-demo.sdsc";
    String defaultStorageResource = "sdsc-unix";
    String organization = "sdsc";

    // Set Up UserInfo object
    OrganizationInfo oi = new OrganizationInfo(organization);
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
    String transactionId = MatrixRunner.runFlow(new ForLoopExampleFlow());

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

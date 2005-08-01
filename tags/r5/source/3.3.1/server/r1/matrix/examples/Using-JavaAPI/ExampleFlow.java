import edu.sdsc.matrix.srb.client.*;
import javax.xml.bind.*;

/**
 * Class is a sequential flow that creates a collection and then a dataSet in
 * that collection
 */
public class ExampleFlow extends SequentialFlow
{
  private static final String COLLECTION_NAME = "myCollection";
  private static final String DATASET_NAME = "myDataSet";

  public ExampleFlow()
  {
    // name this flow "outerFlow"
    super("outerFlow");

    // make a step that creates the collection
    Collection collection = new Collection(COLLECTION_NAME);
    Step makeCollection = new CreateStep("myFirstStep", collection);

    // make a step that creates the dataSet
    DataSet dataSet = new DataSet(COLLECTION_NAME+"/"+DATASET_NAME);
    Step makeDataSet = new CreateStep("mySecondStep", dataSet);

    // add each step to this flow (in order
    this.addChild(makeCollection);
    this.addChild(makeDataSet);

    //fin
  }







//=========================== Main method ====================================//

  /* creates and sends a DGRequest containing a CreateDataSetFlow */

  private static Marshaller marshaller;

  public static void main(String args[])
  {
    // run the flow and remember the transactionId
    String transactionId = MatrixRunner.runFlow(new ExampleFlow());

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

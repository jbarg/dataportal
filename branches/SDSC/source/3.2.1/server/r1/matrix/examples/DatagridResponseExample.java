/*
 * DatagridResponseExample.java
 * Matrix Client API DatagridResponse test example.
 *
 * Created on June 30, 2003, 5:26 AM
 */

import edu.sdsc.matrix.srb.client.*;
import javax.xml.soap.*;
import javax.xml.bind.*;
import java.io.*;
import java.util.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;

/**
 * A Class containing a short example on how to use the matrix client API to call
 * a matrix server and create a collection.
 * Look in "docs/Client-Readme" for more in depth explanations.
 * @author  Allen Ding - SDSC
 */
public class DatagridResponseExample {
    private static BufferedReader console = new BufferedReader(new InputStreamReader(System.in));

    public static void main(String args[]) {
        try {
            /*****************************************************
             * Here are some default values for the user.
             ****************************************************/
            String userid = "Matrix-demo";
            String password = "******";
            String homedir = "/home/Matrix-demo.sdsc";
            String organization = "sdsc";
            String srbserver = "srb.sdsc.edu";
            int srbport = 6824;
            // If the below is not available, you can change it to
            // http://arun-pc.sdsc.edu:8080/matrix/receiver
            String matrixserver = "http://localhost:8080/matrix/receiver";
            
            /**********************************************************
             * Now that we have login and server configuration
             * information, set up the required DocMeta, UserInfo, 
             * VirtualOrganizationInfo, and TerminationHandler objects.
             *
             * We will create the Transaction object later.
             *********************************************************/
            // Both UserInfo and VirtualOrgInfo need OrgInfo so set that up first
            OrganizationInfo oi = new OrganizationInfo(organization);
            
            // Set Up UserInfo object
            UserInfo ui = new UserInfo(userid, oi);
            ui.setChallengeResponse(password);          //  stuff that is optional,
            ui.setHomeDirectory(homedir);               //  but is probably needed
                                                        //  most of the time.
            
            ui.setDefaultStorageResource("sdsc-unix");  // the resource to store in

            // Set up VirtualOrganizationInfo object
            VirtualOrganizationInfo voi = new VirtualOrganizationInfo(srbserver, srbport);
            
            // Set up DocMeta with some description that doesn't matter
            DocMeta dm = new DocMeta("Charkueyteow");
            
            // Set up a Dummy TerminationHandler which doesn't do anything for now
            // but is part of the DGL schema.
            TerminationHandler th = new TerminationHandler("MyTH", "tba");


            /***************************************************
             * We need to create a transaction status query with
             * the given Transaction ID.
             **************************************************/
            // Prompt the user for the parameters of the DataGridRequest
            print("Enter your transaction ID: ");
            String transactionID = console.readLine();
            print("");
            
            TransactionStatusQuery tsq = new TransactionStatusQuery(transactionID);
            DataGridRequest dgRequest = new DataGridRequest(dm, ui, voi, tsq, th);
            

                MatrixConnection mConnect = new MatrixConnection(matrixserver);
                DataGridResponse dgres = mConnect.sendRequest(dgRequest);
                
                TransactionStatusResponse tsr = (TransactionStatusResponse)dgres.getTransactionStatusResponse();

                for(Iterator iterator = tsr.getFlowStatusResponse().iterator(); iterator.hasNext();) {
                    FlowStatusResponse fsr = (FlowStatusResponse)iterator.next();
                    
                    print("Flow " + fsr.getFlowID());
                    
                    if(fsr.getStepStatusResponse().size() == 0)
                        print("\t\t NOT STARTED");
                    
                    for(Iterator i2 = fsr.getStepStatusResponse().iterator(); i2.hasNext();) {
                        StepStatusResponse ssr = (StepStatusResponse)i2.next();
                        
                        print("\t\t" + ssr.getStepID() + 
                              "\t\t" + ssr.getStatusCode() + 
                              "\t\t" + translateStatusCode(ssr.getStatusCode()));
                    }
                    
                    print("\n");
                }     

            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void print(String s) {
        System.out.println(s);
    }
    
    public static String translateStatusCode(int statusCode) {
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.FLOW_ABORTED_DUE_TO_ERROR)
            return "FLOW_ABORTED_DUE_TO_ERROR";
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.FLOW_ABORTED_DUE_TO_REQUEST_FROM_USER)
            return "FLOW_ABORTED_DUE_TO_REQUEST_FROM_USER";
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.FLOW_COMPLETE)
            return "FLOW_COMPLETE";
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.FLOW_NOT_FOUND_OR_NOT_STARTED)
            return "FLOW_NOT_FOUND";
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.FLOW_STARTED)
            return "FLOW_STARTED";              
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.STEP_ABORTED_DUE_TO_ERROR)
            return "STEP_ABORTED_DUE_TO_ERROR";
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.STEP_ABORTED_DUE_TO_REQUEST_FROM_USER)
            return "STEP_ABORTED_DUE_TO_REQUEST_FROM_USER";
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.STEP_COMPLETE)
            return "STEP_COMPLETE";        
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.STEP_NOT_FOUND_OR_NOT_STARTED)
            return "STEP_NOT_FOUND";         
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.STEP_STARTED)
            return "STEP_STARTED";        
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.TRANSACTION_ABORTED_DUE_TO_ERROR)
            return "TRANSACTION_ABORTED_DUE_TO_ERROR";        
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.TRANSACTION_ABORTED_DUE_TO_REQUEST_FROM_USER)
            return "TRANSACTION_ABORTED_DUE_TO_REQUEST_FROM_USER";        
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.TRANSACTION_COMPLETE)
            return "TRANSACTION_COMPLETE";        
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.TRANSACTION_NOT_FOUND_OR_NOT_STARTED)
            return "TRANSACTION_NOT_FOUND";        
        if(statusCode == edu.sdsc.matrix.srb.core.MatrixCodes.TRANSACTION_STARTED)
            return "TRANSACTION_STARTED";
        return "UNKNOWNSTATUSCODE";
    }    

}
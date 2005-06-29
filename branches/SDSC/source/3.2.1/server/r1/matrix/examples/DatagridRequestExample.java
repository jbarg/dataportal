/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */


/*
 * DataGridRequestExample.java
 * Large Matrix Client API DGRequest test example.
 *
 * Created on June 30, 2003, 5:26 AM
 */

import edu.sdsc.matrix.srb.client.*;
import javax.xml.soap.*;
import javax.xml.bind.*;
import java.io.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import java.net.*;

/**
 * A Class containing an example on how to use the matrix client API to call
 * a matrix server with a transaction with many flows.
 * Look in "docs/Client-Readme" and the client API javadoc for more in depth explanations.
 *
 * @author  Allen Ding - SDSC
 */
public class DatagridRequestExample {
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
             * Now we get to creating the Transaction part of a 
             * DataGridRequest.
             *
             * This example creates a transaction with 3 flows:
             *
             * Flow1 (sequential)
             * - Create collection 'ClientTestCollection'
             * - Create container 'ClientTestContainer'
             * - Rename a nonexistent dataset (to demonstrate error reporting)
             * - Prepare ticket ID
             *
             * Flow2 (parallel)
             * - Ingest HTTP URL
             * - Ingest FTP URL
             * - Ingest local file
             * - Ingest HTTP URL
             *
             * Flow3 (parallel)
             * - Delete flow2 FTP ingested dataset
             * - List operation
             * - XQuery on container for size
             **************************************************/
            
            // Setup some names for the urls and datasets in the transaction
            String NewCollectionName = "ClientTestCollection";
            String NewContainerName = "ClientTestContainer";
            String NonExistentDataset = "IDONTEXISTXYZABC";
            
            String HttpURL1 = "http://apache.webmeta.com/jakarta/cactus/binaries/jakarta-cactus-13-1.5-beta1.zip";
            DataSet HttpURL1dataset = new DataSet("ClientTestcactus.tar.gz");
            HttpURL1dataset.setLogicalLocation(homedir);
            
            String HttpURL2 = "http://apache.oregonstate.edu/jakarta/james/binaries/james-2.1.zip";
            DataSet HttpURL2dataset = new DataSet("ClientTestjames.zip");
            HttpURL2dataset.setLogicalLocation(homedir);
            
            String FtpURL1 = "ftp://ftp.oregonstate.edu/pub/apache/jakarta/velocity-tools/binaries/velocity-tools-1.0.zip";
            DataSet FtpURL1dataset = new DataSet("ClientTestvelocity.zip");
            FtpURL1dataset.setLogicalLocation(homedir);
            
            String localfile = "dummyfile.txt";
            DataSet localfiledataset = new DataSet("ClientTestdummyfile.txt");
            localfiledataset.setLogicalLocation(homedir);
            
            String XQueryString = "for $coll in collection(\"/home/Matrix-demo.sdsc\")" +
                                  "return <AVU>" +
                                  "<NAME>{$coll/USER_DEFINED_ATTRIBUTES_NAMES}</NAME>" +
                                  "<VALUE>{$coll/USER_DEFINED_ATTRIBUTES_VALUES}</VALUE>" + 
                                  "<UNIT>{$coll/USER_DEFINED_ATTRIBUTES_UNITS}</UNIT>" + 
                                  "</AVU>;";

            /*************************************
             * Now Create flow 1
             ************************************/
            Flow flow_one = new Flow("sequential");
            
            // Make an Operation for creating a collection
            Collection collection = new Collection(NewCollectionName);
            Operation createcollectionop = new Operation(new CreateOp(collection));
            flow_one.addStep(new Step("Step 1_1", createcollectionop));

            // Make an Operation for creating a container
            Container container = new Container(NewContainerName);
            Operation createcontainerop = new Operation(new CreateOp(container));
            flow_one.addStep(new Step("Step 1_2", createcontainerop));
            
            // Make an Operation to rename a non-existent data set
            DataSet dataset = new DataSet(NonExistentDataset);
            Operation renameop = new Operation(new RenameOp(dataset, "I_WAS_RENAMED"));
            flow_one.addStep(new Step("Step 1_3", renameop));
            
            // Make an Operation to prepare a Ticket ID using ourself as the provider
            Operation prepareop = new Operation(new PrepareTicketOp(ui));
            flow_one.addStep(new Step("Step 1_4", prepareop));
            

            /*************************************
             * Now Create flow 2
             ************************************/
            Flow flow_two = new Flow("parallel");
            
            // Make an Operation to ingest a HTTP URL
            IngestDataSetOp idsop1 = new IngestDataSetOp(new URL(HttpURL1), HttpURL1dataset);
            flow_two.addStep(new Step("Step 2_1", new Operation(idsop1)));
            
            // Make an Operation to ingest an FTP URL
            IngestDataSetOp idsop2 = new IngestDataSetOp(new URL(FtpURL1), FtpURL1dataset);
            flow_two.addStep(new Step("Step 2_2", new Operation(idsop2)));
            
            // Make an Operation to ingest a local file
            IngestDataSetOp idsop3 = new IngestDataSetOp("MyMimeId", localfiledataset, localfile);
            flow_two.addStep(new Step("Step 2_3", new Operation(idsop3)));

            // Make an Operation to ingest a HTTP URL
            IngestDataSetOp idsop4 = new IngestDataSetOp(new URL(HttpURL2), HttpURL2dataset);
            flow_two.addStep(new Step("Step 2_4", new Operation(idsop1)));


            /*************************************
             * Now Create flow 3
             ************************************/
            Flow flow_three = new Flow("parallel");
            
            // Make an Operation to delete the dataset in step 2_2
            // DeleteParams client API is a little incomplete, hence the tedious way
            // of creating it
            DeleteOp deleteop = new DeleteOp(new DeleteParams(new StdDatagridObject(FtpURL1dataset)));
            flow_three.addStep(new Step("Step 3_1", new Operation(deleteop)));
            
            // Make an Operation to get a listing of a collection
            ListOp listop = new ListOp(new Collection(homedir));
            flow_three.addStep(new Step("Step 3_2", new Operation(listop)));
            
            // Make and Operation with an XQuery metadata query
            // MetadataQuery client API is a little incomplete, hence the tedious way
            // of creating it
            MetadataQueryOp mdqop = new MetadataQueryOp(new MetadataQueryParams(XQueryString));
            flow_three.addStep(new Step("Step 3_3", new Operation(mdqop)));
            
            /***************************************************
             * Now we can create the actual Transaction and also
             * the DataGridRequest.
             **************************************************/
            Transaction transaction = new Transaction();
            transaction.addFlow(flow_one);
            transaction.addFlow(flow_two);
            transaction.addFlow(flow_three);
            
            DataGridRequest dgreq = new DataGridRequest(dm, ui, voi, transaction, th);

            /*******************************************************************
             * Now that we have a DataGridRequest, we can make a connection to a
             * Matrix server to send the request.
             ******************************************************************/
            print("Sending request to: " + matrixserver + "\n");
            MatrixConnection mConnect = new MatrixConnection(matrixserver);
            
            /*************************************************************
             * Send the request and get back a DataGridResponse which will
             * contain the transaction ID which can be used to query the
             * status of the request later.
             ************************************************************/
            DataGridResponse dgres = mConnect.sendRequest(dgreq);

            /*****************************************************************
             * Output some information about the DataGridResponse we received.
             ****************************************************************/
            System.out.println("Request State: " + dgres.getRequestState());
            System.out.println("Trans ID: " + dgres.getTransactionID());
            
        } catch (SOAPException fnfe) {
            print("\n\nThere was a problem");
            print("Please make sure that dummyfile.txt is in the current directory and that the matrix server" +
                  " above is up and running.");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void print(String s) {
        System.out.println(s);
    }

}